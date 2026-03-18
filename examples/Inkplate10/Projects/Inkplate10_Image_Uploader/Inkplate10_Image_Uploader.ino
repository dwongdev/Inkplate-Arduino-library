/**
 **************************************************
 * @file        Inkplate10_Image_Uploader.ino
 * @brief       Local web image uploader and display example for Soldered Inkplate 10.
 *
 * @details     Demonstrates how to host a small web app directly on Inkplate 10
 *              (ESP32) that lets the user upload an image via a browser and then
 *              display it on the e-paper screen. The Inkplate starts a WiFi Access
 *              Point (AP), serves an HTML upload page, accepts an uploaded JPEG
 *              into a RAM buffer, provides a preview endpoint, and renders the
 *              uploaded image to the display in 3-bit (grayscale) mode.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      WiFi-capable device with a web browser (PC, laptop, smartphone)
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate10
 * - Inkplate runs as WiFi Access Point (AP+STA mode)
 * - Set AP SSID and password in the sketch (ap_ssid, ap_password)
 * - HTML page is provided via src/html.h (INDEX_HTML)
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 10.
 * 2) On your phone/PC, connect to the Inkplate WiFi network (SSID/password shown on display).
 * 3) Open the IP address shown on the Inkplate screen in a web browser.
 * 4) Upload an image using the web page form.
 * 5) The uploaded image is displayed on the e-paper screen.
 *
 * Expected output:
 * - Inkplate display shows AP credentials and the local IP address.
 * - Web page allows uploading an image and previewing the last upload.
 * - Uploaded JPEG is rendered on the Inkplate display after upload completes.
 *
 * Notes:
 * - Uploaded image data is stored in RAM; large uploads may fail due to memory limits.
 * - This example renders the image in 3-bit (grayscale) mode.
 * - Endpoints used:
 *   - "/"          Serves upload page
 *   - "/upload"    Receives file upload (HTTP POST)
 *   - "/preview"   Shows browser preview of last uploaded image
 *   - "/image.jpg" Serves the uploaded JPEG from RAM
 *
 * Docs:
 * - Inkplate:   https://soldered.com/documentation/inkplate
 * - Project:    https://soldered.com/documentation/inkplate/projects/image-uploader
 *
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/

// Ensure correct board is selected
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
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
      imageBufCapacity = 1200 * 825;
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
  display.setTextSize(4);
  display.setTextColor(BLACK);
  display.clearDisplay();
  display.setCursor(10, 10);
  display.print("Welcome to the Inkplate Image Uploader example.");
  display.setTextSize(2);
  display.setCursor(10, 50);
  display.print("Connect to WiFi:");
  display.setCursor(30, 70);
  display.print("SSID: " + String(ap_ssid));
  display.setCursor(30, 90);
  display.print("Password: " + String(ap_password));
  display.setCursor(10, 110);
  display.print("Then visit:");
  display.setCursor(10, 130);
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
