#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"
#include "WiFi.h"
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include "esp_sleep.h"

// Inkplate object initialization
// using 3 bit = grayscale
Inkplate display(INKPLATE_3BIT);

const char ssid[] = "";    // Your WiFi SSID
const char *password = ""; // Your WiFi password

JSONVar myDataset;

// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000
// Time ESP32 will go to sleep (in seconds)
#define TIME_TO_SLEEP 600
// web root
#define WEB_ROOT "http://your-inkpage-server"
// image IMAGE_ROOT
#define IMAGE_ROOT "frontpages/"
// JSON that determines page orientation, frontpages and their duration to show
#define JSONFILE "selected.json"

// screen rotation
#define LANDSCAPE 4
#define PORTRAIT 3

void fetchAndParseJson()
{
  if ((WiFi.status() == WL_CONNECTED))
  { // Check WiFi connection status

    HTTPClient http; // Declare an object of class HTTPClient

    // Specify request destination
    String jsonurl = WEB_ROOT IMAGE_ROOT JSONFILE;

    http.begin(jsonurl);       // request destination
    int httpCode = http.GET(); // Send the request

    if (httpCode > 0)
    { // Check the returning code

      String payload = http.getString();        // Get the request response payload
      JSONVar parsedJson = JSON.parse(payload); // Parse JSON data

      if (JSON.typeof(parsedJson) == "undefined")
      {
        Serial.println("Parsing input failed!");
        return;
      }

      myDataset = parsedJson; // Store the parsed JSON in the global variable
      Serial.println(myDataset);
    }
    else
    {
      Serial.println("Error on HTTP request");
    }

    http.end(); // Free resources
  }
  else
  {
    Serial.println("WiFi Not Connected");
  }
}

void setup()
{
  // setup code that runs once
  Serial.begin(115200);
  while (!Serial)
  {
    ; // Wait for Serial to initialize
  }
  Serial.println("Serial initialized.");

  display.begin();        // Init Inkplate library
  display.clearDisplay(); // Clear frame buffer of display
  display.display();      // Put clear image on display

  Serial.println("Starting WiFi...");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");

  Serial.println("Setup complete.");
}

void loop()
{
  // main code to run repeatedly

  Serial.println("Fetching and parsing JSON...");
  fetchAndParseJson(); // Fetch and parse the JSON data, may have changed

  // default
  int rotation = LANDSCAPE;

  if (JSON.typeof(myDataset) != "undefined")
  {

    if (myDataset.hasOwnProperty("orientation") && String((const char *)myDataset["orientation"]) == "portrait")
    {
      rotation = PORTRAIT; // Set rotation for portrait mode
      Serial.println("Setting orientation to portrait mode");
      display.setRotation(rotation);
    }
    else
    {
      rotation = LANDSCAPE;
      Serial.println("Setting orientation to landscape mode");
      display.setRotation(rotation);
    }

    for (int i = 0; i < myDataset.keys().length(); i++)
    {

      display.clearDisplay();

      String key = myDataset.keys()[i];
      if (key == "orientation")
        continue;

      JSONVar valueVar = myDataset[key];
      String valueStr = (const char *)valueVar; // Convert JSONVar to String
      int duration = valueStr.toInt();
      Serial.println(String(i) + ": Display duration " + String(duration) + " minute(s) for newspaper " + String(key));

      String img_url = WEB_ROOT IMAGE_ROOT;
      if (rotation % 2 == 0)
      {
        img_url = img_url + "1200_" + key + ".png";
      }
      else
      {
        img_url = img_url + "825_" + key + ".png";
      }
      Serial.println("Attempting to display: " + img_url);
      delay(5000);

      if (display.drawImage(img_url, 0, 0, true, false))
      {
        Serial.println("Image displayed successfully.");
        display.display();
        Serial.println("Waiting for " + String(duration) + " minutes.");

        // log divider
        /* ----------------------- */
        int count = 50;
        for (int i = 0; i < count; i++)
        {
          Serial.print("-");
        };
        Serial.println();
        /* ----------------------- */

        int wait_time = duration * 60000;
        delay(wait_time);
      }
      else
      {
        Serial.println("Failed to display image. Moving on.");
      }
    }
  }
  else
  {
    Serial.println("No JSON data to iterate.");
  }
}
