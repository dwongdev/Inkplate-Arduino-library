/**
 **************************************************
 * @file        Inkplate6PLUS_News_API.ino
 * @brief       Fetch top news headlines from NewsAPI.org over WiFi and render
 *              them on Inkplate 6PLUS, then deep-sleep between updates.
 *
 * @details     This example connects Inkplate 6PLUS to WiFi, synchronizes time
 *              via NTP, fetches news data from NewsAPI.org using an API key, and
 *              displays multiple headline + description entries formatted into
 *              text boxes on the e-paper display.
 *
 *              The layout includes a "World News" title, a date row, a "Last
 *              update" timestamp, and a list of news items. The display runs in
 *              1-bit (BW) mode (INKPLATE_1BIT), and the sketch performs a full
 *              refresh after drawing all elements.
 *
 *              After updating the screen, the ESP32 enters deep sleep for a
 *              fixed interval (DELAY_MS). Deep sleep restarts the ESP32 on wake,
 *              so setup() runs again each cycle (WiFi + NTP + API fetch + draw).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      WiFi access, NewsAPI.org API key
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate6PLUS
 * - Serial settings: 115200 baud (recommended for debugging)
 * - WiFi credentials: set ssid / pass
 * - NewsAPI key: set api_key_news (https://newsapi.org/)
 * - Timezone: set timeZone (UTC offset hours)
 * - Update interval: adjust DELAY_MS (default: 1 hour)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6PLUS/quick-start-guide/
 *
 * How to use:
 * 1) Create a NewsAPI.org account and generate an API key.
 * 2) Enter your WiFi SSID/password, NewsAPI key, and timeZone in the sketch.
 * 3) Upload the sketch to Inkplate 6PLUS.
 * 4) After boot, the device will connect, sync time, fetch news, and render the
 *    page. It will then deep-sleep until the next scheduled refresh.
 *
 * Expected output:
 * - Display: "World News" page with current date/time and several news items
 *   (title + description). If the fetch fails, an error message is shown.
 * - Serial Monitor: Current time printout and basic status/debug output.
 *
 * Notes:
 * - Display mode is 1-bit (BW). This example uses a full refresh after drawing.
 * - Deep sleep restarts the ESP32 on wake; the displayed content is refreshed
 *   only when the device wakes and reruns setup().
 * - API/network reliability: handle rate limits and connectivity issues. If the
 *   API call fails, the sketch shows an on-screen error before sleeping.
 * - Long titles/descriptions are wrapped into text boxes; very verbose feeds may
 *   be truncated by the chosen font size and box dimensions.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025-04-30
 * @license     GNU GPL V3
 **************************************************/

// Ensure the correct board is selected in the Arduino IDE
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6PLUS or Soldered Inkplate 6PLUS in the boards menu."
#endif

// ---------- CHANGE HERE -------------
// Adjust your timezone (e.g., 2 means UTC+2)
int timeZone = 2;

// WiFi credentials (replace with your WiFi network details)
char ssid[] = "YourWiFiSSID";       // Replace with your WiFi SSID
char pass[] = "YourWiFiPassword";   // Replace with your WiFi password

// News API key (get one from https://newsapi.org/)
char api_key_news[] = "YourNewsAPIKey"; // Replace with your News API key
// ------------------------------------

// Include necessary libraries
#include "Inkplate.h"
#include "src/Network.h"

// Include necessary libraries for fonts
#include "Fonts/Inter12pt7b.h"
#include "Fonts/GT_Pressura16pt7b.h"
#include "Fonts/FreeSerifItalic24pt7b.h"

// Create network and display objects
NetworkFunctions network;
Inkplate inkplate(INKPLATE_1BIT);

// Constants for delays and refreshes
#define DELAY_MS (uint32_t)60 * 60 * 1000 // Delay between API calls (1 hour)
#define DELAY_WIFI_RETRY_SECONDS 10       // Delay for WiFi retry
#define DELAY_API_RETRY_SECONDS 10        // Delay for API retry

// Function declarations
void setTime();
void drawNews(struct news *entities);

void setup()
{
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Initialize the display
    inkplate.begin();
    inkplate.setTextWrap(false);

    // Connect to WiFi
    network.setCredentials(ssid, pass, api_key_news);
    network.setTimeZone(timeZone);
    network.begin();

    // Set the current time
    setTime();

    // Fetch news data and display it
    struct news *entities = network.getData(inkplate);
    if (entities != nullptr)
    {
        drawNews(entities);
    }
    else
    {
        // Display an error message if fetching news fails
        inkplate.clearDisplay();
        inkplate.setCursor(50, 230);
        inkplate.setTextSize(2);
        inkplate.println(F("Failed to fetch news"));
        inkplate.display();
    }

    // Update the display
    inkplate.display();

    // Enter deep sleep until the next update
    esp_sleep_enable_timer_wakeup(1000 * DELAY_MS);
    esp_deep_sleep_start();
}

// Function to draw news items on the display
void drawNews(struct news *entities)
{
    inkplate.setRotation(3); // Set display to landscape mode

    // Display the title "World News"
    inkplate.setFont(&FreeSerifItalic24pt7b);
    int textWidth = strlen("World News") * 10; // Adjust width calculation for smaller screen
    int centerX = (inkplate.width() - textWidth) / 2;
    inkplate.setCursor(centerX - 60, 40); // Adjust Y position for smaller screen
    inkplate.print("World News");

    // Draw a dividing line below the title
    int xStart = inkplate.width() * 0.05; // Adjust margins for smaller screen
    int xEnd = inkplate.width() * 0.95;
    for (int lineY = 60; lineY < 63; lineY++) // Adjust Y position for the line
    {
        inkplate.drawLine(xStart, lineY, xEnd, lineY, BLACK);
    }

    // Display the current date and time
    struct tm timeInfo;
    time_t nowSec;
    inkplate.getNTPEpoch(&nowSec);
    while (nowSec < 8 * 3600 * 2)
    {
        delay(500);
        yield();
        nowSec = time(nullptr);
    }
    gmtime_r(&nowSec, &timeInfo);

    // Prepare date and time strings
    char dateStr[20];
    char updateStr[20];
    sprintf(dateStr, "Date : %02d.%02d.%04d",
            timeInfo.tm_mday,
            timeInfo.tm_mon + 1,
            timeInfo.tm_year + 1900);

    sprintf(updateStr, "Last update : %02d:%02d",
            timeInfo.tm_hour,
            timeInfo.tm_min);

    // Choose font
    inkplate.setFont(&Inter12pt7b);

    // Y position for the row
    int yPos = 83;

    // Print date left-aligned
    inkplate.setCursor(35, yPos); // Adjusted for smaller screen
    inkplate.print(dateStr);

    // Calculate width of 'Last update' string for right alignment
    int updateStrWidth = strlen(updateStr) * 12; // Adjust 12 for your font's avg char width
    int xRight = inkplate.width() - updateStrWidth - 28; // Adjusted margin for smaller screen

    // Print 'Last update' right-aligned
    inkplate.setCursor(xRight, yPos);
    inkplate.print(updateStr);

    // Draw a line below the date
    for (int lineY = 93; lineY < 96; lineY++) // Adjust Y position for the line
    {
        inkplate.drawLine(xStart, lineY, xEnd, lineY, BLACK);
    }

    // Render news items
    int startY = 140; // Adjust starting Y position for news items
    int boxHeight = 120; // Reduce box height for smaller screen
    int boxSpacing = 10; // Reduce spacing between boxes
    int leftMargin = 25; // Adjust margins
    int rightMargin = 250;
    int maxBoxes = (inkplate.height() - startY) / (boxHeight + boxSpacing);

    for (int i = 0; i < maxBoxes && entities[i].title != nullptr && entities[i].description != nullptr; i++)
    {
        int y0 = startY + i * (boxHeight + boxSpacing);
        int y1 = y0 + boxHeight;

        // Draw the title
        inkplate.drawTextBox(leftMargin, y0, inkplate.width() - rightMargin, y0 + 70, entities[i].title, 1, &GT_Pressura16pt7b, 26, false, 12);

        // Draw the description
        inkplate.drawTextBox(leftMargin, y0 + 65, inkplate.width() - rightMargin - 50, y1, entities[i].description, 1, &Inter12pt7b, 20, false, 10);
    }
}

void setTime()
{
    struct tm timeInfo;
    time_t nowSec;
    inkplate.getNTPEpoch(&nowSec);
    while (nowSec < 8 * 3600 * 2)
    {
        delay(500);
        yield();
        nowSec = time(nullptr);
    }
    gmtime_r(&nowSec, &timeInfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeInfo));
}

void loop()
{
    // Not used
}