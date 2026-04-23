/**
 **************************************************
 * @file        Inkplate4TEMPERA_Image_Uploader.ino
 * @brief       Host a WiFi web page to upload a JPEG image and display it on
 *              Inkplate 4 TEMPERA in 3-bit grayscale.
 *
 * @details     This example starts a simple HTTP web server on the Inkplate 4
 *              TEMPERA and exposes a small web app that lets you upload an
 *              image from a phone or PC browser. The device creates its own
 *              WiFi Access Point (AP) so you can connect directly without an
 *              existing router.
 *
 *              When a file is uploaded via HTTP POST, the sketch stores the
 *              received JPEG bytes in RAM and serves them back at /image.jpg
 *              for preview. After an upload completes, the image is rendered on
 *              the e-paper display using drawJpegFromBuffer().
 *
 *              The display is operated in 3-bit grayscale mode (INKPLATE_3BIT),
 *              which supports 8 intensity levels. Image resizing/fit behavior
 *              depends on the Inkplate JPEG draw function parameters; large
 *              images may be scaled to fit the screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      WiFi-capable phone/PC with a browser (connects to Inkplate AP)
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 4 TEMPERA
 * - Serial Monitor: 115200 baud (recommended for upload/debug logs)
 * - WiFi credentials / API keys / timezone:
 *   - Configure AP SSID/password (ap_ssid / ap_password).
 *   - Password must be at least 8 characters for WPA2 AP mode.
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 4 TEMPERA.
 * 2) On the e-paper screen, note the AP SSID, password, and IP address.
 * 3) On your phone/PC, connect to the shown WiFi network (AP mode).
 * 4) Open a browser and navigate to the displayed IP address.
 * 5) Upload a JPEG image through the web page; after upload, the image is
 *    rendered on the e-paper display. You can also open /preview to view the
 *    last uploaded image in the browser.
 *
 * Expected output:
 * - E-paper: First shows connection instructions (SSID/password/IP). After an
 *   upload, displays the uploaded image in 3-bit grayscale.
 * - Browser: Upload UI at '/', preview page at '/preview', image bytes at
 *   '/image.jpg'.
 * - Serial: Upload progress, buffer allocation size, bytes received/sent.
 *
 * Notes:
 * - Display mode is 3-bit grayscale (8 levels). Partial update is not available
 *   in grayscale mode; image rendering uses full refresh (display.display()).
 * - RAM usage: the uploaded image is stored fully in RAM. Large uploads can
 *   fail due to limited heap memory; keep images reasonably sized.
 * - The buffer capacity is derived from the HTTP Content-Length header; if the
 *   header is missing/invalid, a fallback buffer size is used.
 * - This sketch accepts and renders JPEG data as provided. For production use,
 *   consider validating file type/size and adding upload size limits/timeouts.
 * - AP+STA mode is enabled; this example primarily uses AP mode for direct
 *   connections.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/

// Ensure correct board is selected
#if !defined(ARDUINO_INKPLATE4TEMPERA)
#error "Wrong board selection for this example, please select e-radionica Inkplate4TEMPERA or Soldered Inkplate4TEMPERA in the boards menu."
#endif

#include <WiFi.h>           // WiFi support
#include <WebServer.h>      // Built-in web server
#include <HTTPClient.h>     // HTTP client for image upload
#include <Inkplate.h>       // Inkplate display library
#include "src/html.h"           // HTML page definitions (INDEX_HTML)

// Wi-Fi Access Point credentials
const char* ap_ssid     = "InkplateImage"; // SSID for AP mode
const char* ap_password = "inkplate";       // Password (min 8 chars)

// Initialize Inkplate (3-bit grayscale mode by default)
Inkplate display(INKPLATE_3BIT);

// Create a web server object on port 80
WebServer server(80);

// Buffer to hold the latest uploaded image in RAM
uint8_t* imageBuf         = nullptr;  // pointer to JPEG data
size_t   imageLen         = 0;        // actual bytes received
size_t   imageBufCapacity = 0;        // allocated buffer size

// Flag to indicate a completed upload
bool imageUploaded = false;

// Serve the main HTML page when root is accessed
void handleIndex() {
  server.send_P(200, "text/html", INDEX_HTML);
}

// Handle file upload in three stages: START, WRITE, END
void handleUpload() {
  HTTPUpload& u = server.upload();

  if (u.status == UPLOAD_FILE_START) {
    // Beginning of upload: allocate buffer based on Content-Length header
    imageLen = 0;
    if (imageBuf) {
      free(imageBuf);
      imageBuf = nullptr;
    }
    imageBufCapacity = server.header("Content-Length").toInt();
    if (imageBufCapacity <= 0) {
      // Fallback capacity if header is missing or invalid (~1200×825 pixels)
      imageBufCapacity = 600 * 600;
    }
    imageBuf = (uint8_t*) malloc(imageBufCapacity);
    Serial.printf("Upload start, buffer capacity = %u bytes\n",
                  (unsigned)imageBufCapacity);
  }
  else if (u.status == UPLOAD_FILE_WRITE) {
    // Middle of upload: append received chunk into buffer
    if (imageBuf && (imageLen + u.currentSize <= imageBufCapacity)) {
      memcpy(imageBuf + imageLen, u.buf, u.currentSize);
      imageLen += u.currentSize;
      Serial.printf("  received %u bytes (total %u)\n",
                    (unsigned)u.currentSize, (unsigned)imageLen);
    }
  }
  else if (u.status == UPLOAD_FILE_END) {
    // End of upload: mark flag to display the image
    Serial.printf("Upload complete, final size = %u bytes\n",
                  (unsigned)imageLen);
    imageUploaded = true;
  }
}

// After upload ends, reload the page so the user sees the result
void handleUploadComplete() {
  server.send(200, "text/html");
}

// Show a preview page with the last uploaded image
void handlePreview() {
  String html = "<html><body><h3>Preview:</h3>"
    "<img src=\"/image.jpg?ts=" + String(millis()) +
    "\" style=\"max-width:100%;\">"
    "</body></html>";
  server.send(200, "text/html", html);
}

// Serve the JPEG image bytes directly from RAM
void handleImage() {
  if (imageBuf && imageLen > 0) {
    // Send appropriate headers for JPEG
    server.sendHeader("Content-Type",   "image/jpeg");
    server.sendHeader("Content-Length", String(imageLen));
    server.send(200, "image/jpeg", "");  // end headers

    // Write image data to client
    WiFiClient client = server.client();
    size_t sent = client.write(imageBuf, imageLen);
    Serial.printf("Sent %u of %u bytes\n", sent, (unsigned)imageLen);
  }
  else {
    // No image available yet
    server.send(404, "text/plain", "No image uploaded");
  }
}

// Display the buffered image on the e-ink display
void showImageBuffer() {
  if (!imageBuf || imageLen == 0) return;

  display.clearDisplay();                     // clear existing content
  display.selectDisplayMode(INKPLATE_3BIT);      // ensure correct mode
  // Draw JPEG from RAM: full-screen, no dithering
  display.image.drawJpegFromBuffer(imageBuf, imageLen, 0, 0, true, false);
  display.display();                          // push to panel
}

// Helper: calculate scaled dimensions by the larger original side
void getScaledByLargerSide(int origW, int origH, int maxW, int maxH, int &outW, int &outH) {
  float ratio = (origW >= origH) ? (float(maxW) / origW) : (float(maxH) / origH);
  outW = round(origW * ratio);
  outH = round(origH * ratio);
}

void setup() {
  Serial.begin(115200);             // initialize serial for debug

  // Initialize the display
  display.begin();
  display.clearDisplay();

  // Start Wi-Fi in AP+STA mode
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP IP: "); Serial.println(apIP);

  // Show instructions on the e-ink screen
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.clearDisplay();
  display.setCursor(10, 20);
  display.print("Welcome to the Inkplate Image Uploader example.");
  display.setCursor(10, 60);
  display.print("Connect to WiFi:");
  display.setCursor(30, 80);
  display.print("SSID: " + String(ap_ssid));
  display.setCursor(30, 100);
  display.print("Password: " + String(ap_password));
  display.setCursor(10, 120);
  display.print("Then visit:");
  display.setCursor(10, 140);
  display.print(apIP.toString());
  display.display();

  // Define web routes and handlers
  server.on("/",            HTTP_GET,  handleIndex);
  server.on("/upload",      HTTP_POST, handleIndex, handleUpload);
  server.on("/preview",     HTTP_GET,  handlePreview);
  server.on("/image.jpg",   HTTP_GET,  handleImage);

  server.begin();             // start the HTTP server
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();      // handle incoming HTTP requests

  // If an image was uploaded, display it once
  if (imageUploaded) {
    imageUploaded = false;    // reset the flag
    delay(100);               // small delay to finish POST
    showImageBuffer();        // render image on e-ink
  }
}
