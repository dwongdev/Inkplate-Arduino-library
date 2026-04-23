/**
 **************************************************
 * @file        Inkplate6COLOR_Gallery.ino
 * @brief       Displays a random image from the root of the microSD card, then
 *              enters deep sleep on Inkplate 6COLOR.
 *
 * @details     This example scans the root directory of the inserted microSD
 *              card, collects supported image filenames, randomly selects one
 *              image, renders it on the Inkplate 6COLOR display, and then puts
 *              the ESP32 into timed deep sleep.
 *
 *              Supported files in this example are discovered by filename
 *              extension and include BMP, JPG, and PNG entries found in the
 *              root folder. After one image is displayed, the board sleeps for
 *              a fixed interval and restarts from setup() on wake-up. This
 *              makes it suitable for low-power gallery/signage workflows where
 *              the displayed content only changes occasionally.
 *
 *              Because deep sleep resets the ESP32, all runtime logic is placed
 *              in setup(). The SD card is also put into sleep mode before the
 *              MCU enters deep sleep to reduce power consumption.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      microSD card with image files in the root directory
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Serial Monitor: 115200 baud
 * - Format the microSD card as FAT/FAT32
 * - Copy supported image files (.bmp, .jpg, .png) to the root directory
 * - Adjust DELAY_MS in the sketch to change the sleep/display interval
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Format a microSD card and copy supported image files to its root folder.
 * 2) Insert the card into Inkplate 6COLOR.
 * 3) Upload the sketch and open Serial Monitor at 115200 baud if you want to
 *    see detected filenames and the selected image.
 * 4) On boot, the sketch scans the SD card root and builds a list of image
 *    files.
 * 5) One image is chosen at random, displayed on the screen, and its filename
 *    is printed to Serial.
 * 6) The SD card is put to sleep and the ESP32 enters timed deep sleep.
 * 7) After wake-up, the ESP32 restarts and the process repeats with another
 *    random image selection.
 *
 * Expected output:
 * - Display: One randomly selected image from the SD card root directory.
 * - Serial: Detected image filenames and the filename of the image chosen for
 *   display.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode with full refresh.
 * - Deep sleep restarts the ESP32, so all application logic must remain in
 *   setup(); loop() should stay empty.
 * - Image decode success depends on file format support, image dimensions, and
 *   available RAM. BMP is usually the safest format for embedded workflows.
 * - Large or unsupported JPG/PNG files may fail to decode or may require more
 *   memory than available.
 * - The example searches only the SD card root directory, not subfolders.
 * - Frequent full color refreshes are slower and more power-hungry than
 *   monochrome partial-update workflows on supported boards.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2021-07-08
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;          // Create SdFile object used for accessing files on SD card

#define DELAY_MS 5 * 60 * 1000

// Here we store possible image paths
String files[100];
int fileCount = 0;

// Generic helper function to see if null terminated c string ends with other string
// https://stackoverflow.com/questions/744766/how-to-compare-ends-of-strings-in-c
int endsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void setup()
{
    // Initialize serial
    Serial.begin(115200);

    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        // Temporary buffer for file name
        char a[50];
        FsFile root = display.getSdFat().open("/");         // Currently open file and the directory we want images from e.g. root /
        FsFile f;
        while (f.openNext(&root, O_RDONLY) && f.getName(a, 40)) // Iterate over all files in directory
        {
            if (f.isFile() && !f.isHidden() && a[0] != '.' &&                        // Check if current file isn't a directory
                (endsWith(a, ".bmp") || endsWith(a, ".jpg") || endsWith(a, ".png"))) // Check if the file is an image
                Serial.println(files[fileCount++] = String(a));                      // Print the file name and add it to all files
            f.close();                                                               // Close the file
        }

        Serial.println(); // Blank print to separate all files from the chosen one

        if (fileCount != 0) // If there we're any files choose one and draw it
        {
            String s = files[random(0, fileCount)];
            display.image.draw(s, 0, 0);
            Serial.println("Drawn image: ");
            Serial.println(s);
            display.display();
        }
    }
    else
    {
        Serial.println("failed opening sd");
    }

    // Go to sleep for DELAY_MS
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);

    // Put SD card into deep sleep
    display.sdCardSleep();

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}
