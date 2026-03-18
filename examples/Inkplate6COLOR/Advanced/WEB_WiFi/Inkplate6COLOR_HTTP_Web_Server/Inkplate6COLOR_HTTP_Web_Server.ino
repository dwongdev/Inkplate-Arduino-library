/**
 **************************************************
 * @file        Inkplate6COLOR_HTTP_Web_Server.ino
 * @brief       Runs Inkplate 6COLOR as a simple Wi-Fi access point and HTTP web
 *              server for sending text to the display.
 *
 * @details     This example demonstrates how to use Inkplate 6COLOR as a
 *              standalone HTTP web server. The board creates its own Wi-Fi
 *              access point, hosts a simple web page, and accepts user text
 *              entered from a phone, tablet, or computer browser. When text is
 *              submitted through the page, the sketch updates the e-paper
 *              display and shows the received message.
 *
 *              This workflow is useful for quick user interaction without an
 *              external router, cloud service, or companion app. A client
 *              device connects directly to the Inkplate access point, opens the
 *              IP address shown on the display, and interacts with the hosted
 *              web interface.
 *
 *              The example uses plain HTTP on port 80 and is designed as a
 *              simple demonstration of local browser-to-display communication.
 *              It can be extended into richer local dashboards, message boards,
 *              control panels, and custom interfaces.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      Phone / tablet / computer with Wi-Fi and web browser
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Change the AP SSID and password in the sketch if needed
 * - Serial settings: not used in this example
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6COLOR.
 * 2) After boot, Inkplate starts a Wi-Fi access point using the configured
 *    SSID and password.
 * 3) On the display, note the network name, password, and local server IP.
 * 4) Connect a phone, tablet, or computer to the Inkplate Wi-Fi network.
 * 5) Open a browser and navigate to the IP address shown on the display.
 * 6) Enter text in the web form and submit it.
 * 7) The submitted text is shown on the Inkplate display.
 *
 * Expected output:
 * - Display: Wi-Fi SSID, password, local server URL, and the most recently
 *   submitted user text.
 * - Browser: A simple web page with a text input field for sending text to the
 *   display.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode with full refreshes.
 * - This example uses plain HTTP only. It is intended for local/demo use and
 *   does not provide HTTPS or authentication beyond Wi-Fi access control.
 * - Because the board runs as an access point, Internet access is not required
 *   for basic operation.
 * - Long submitted text may wrap across multiple lines depending on available
 *   display space and current text settings.
 * - Frequent full color refreshes are slower than monochrome partial-update
 *   workflows on supported boards.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2022-12-02
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "htmlCode.h"   //Include .h file where we stored out html code of our web page
#include <WebServer.h>  //Include ESP32 library for Web server
#include <WiFi.h>       //Include ESP32 WiFi library
#include <WiFiClient.h> //Include ESP32 WiFi library for AP
#include <uri/UriBraces.h>

#define ssid "Inkplate 6Color"
#define pass "Soldered"

Inkplate display;     // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
WebServer server(80); // Create Web server on port 80 (HTTP port number)

IPAddress serverIP;
String txt;

void setup()
{
    display.begin();                      // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();               // Clear frame buffer of display
    display.display();                    // Put clear image on display
    display.setTextSize(2);               // Scale text to be two times bigger then original (5x7 px)
    display.setTextColor(INKPLATE_BLACK); // Set text color to black and background color to white
    display.setTextWrap(true);            // If text does not fit on screen, send it to new line

    WiFi.begin();            // Init. WiFi library
    WiFi.mode(WIFI_AP);      // Set WiFi to Access point mode
    WiFi.softAP(ssid, pass); // Set SSID (WiFi name) and password for Access point

    serverIP = WiFi.softAPIP(); // Get the server IP address

    server.on("/", handleRoot); // If you open homepage, go to handle root function
    server.on(UriBraces("/string/{}"),
              handleString); // If you send some text to Inkplate, go to handleString function. Note that {} brackets at
                             // the end of address. That means that web address has some arguments (our text!).
    server.begin();          // Start the web server
    updatePaper();
}

void loop()
{
    server.handleClient(); // You have to constantly read if there is any new client connected to web server
}

void updateHTML()
{ // This function will send response to client and send HTML code of our web page
    server.send(200, "text/html", s);
}

void handleRoot()
{ // This function will send response to client if client open a root (homepage) of our web page
    updateHTML();
}

void handleString()
{ // This function will send response to client, send HTML code of web page, get the text from argument sent in web page
    // address and refresh screen with new text
    txt = server.arg(0);
    updateHTML();
    updatePaper();
}

void updatePaper()
{                              // This function updates screen with new data (text)
    display.clearDisplay();    // Clear everything from epaper frame buffer
    display.setCursor(20, 40); // Print out instruction on how to connect to Inkplate WiFi and how to open a web page
    display.print("Connect to ");
    display.print(ssid);
    display.println(" WiFi with pass: ");
    display.setCursor(240, 100);
    display.println(pass);
    display.setCursor(100, 150);
    display.print("Open Your web browser and open");
    display.setCursor(240, 210);
    display.print("http://");
    display.print(serverIP);
    display.println('/');
    display.println();
    display.fillRect(10, 240, 780, 4, BLACK);
    display.println("User text:"); // Print out what user typed in web page
    display.print(txt);
    display.display(); // Send everything to screen (refresh the screen)
}
