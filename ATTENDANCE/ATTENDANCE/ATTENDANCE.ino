#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

// =====================================
// WIFI
// =====================================
const char* ssid = "iot";
const char* password = "12345678";

// =====================================
// GOOGLE APPS SCRIPT
// =====================================
const char* scriptURL =
"https://script.google.com/macros/s/AKfycbwzWnEBZxs9XZgmU3MeTnSenufr_YnZnv_H6_c3URTuaTmZErji-I5UhGLKV7ZH1tiKww/exec";

// =====================================
// FINGERPRINT SENSOR
// =====================================
#define FINGER_RX 16
#define FINGER_TX 17

HardwareSerial FingerSerial(2);
Adafruit_Fingerprint finger(&FingerSerial);

// =====================================
// RTC AND LCD
// =====================================
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================
// RTC SETTING
// =====================================
// IMPORTANT:
// First upload: set this to 1.
// After checking the correct RTC time, change it to 0
// and upload again. Do not keep it as 1.
#define SET_RTC_TIME_ONCE 1

// =====================================
// STUDENT ID / NAME
// =====================================
const char* getStudentName(uint8_t id)
{
  switch (id)
  {
    case 1: return "SHASTHA";
    case 2: return "MALADEVI";
    case 3: return "DINESH";
    default: return "UNKNOWN";
  }
}

// =====================================
// URL ENCODE
// =====================================
String urlEncode(String value)
{
  String encoded = "";
  char hex[4];

  for (unsigned int i = 0; i < value.length(); i++)
  {
    uint8_t c = (uint8_t)value[i];

    if ((c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') ||
        c == '-' || c == '_' ||
        c == '.' || c == '~')
    {
      encoded += (char)c;
    }
    else
    {
      sprintf(hex, "%%%02X", c);
      encoded += hex;
    }
  }

  return encoded;
}

// =====================================
// WAIT FOR FINGER REMOVAL
// =====================================
void waitForFingerRemoval()
{
  Serial.println("Remove finger completely...");

  while (true)
  {
    uint8_t result = finger.getImage();

    if (result == FINGERPRINT_NOFINGER)
      break;

    delay(100);
  }

  delay(500);
  Serial.println("Ready for next scan.");
}

// =====================================
// WIFI CONNECTION
// =====================================
void connectWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  lcd.clear();
  lcd.print("Connecting WiFi");

  Serial.print("Connecting to WiFi");

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < 20000)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  lcd.clear();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");

    lcd.setCursor(0, 1);
    lcd.print("Google Sheets");

    delay(1500);
  }
  else
  {
    Serial.println("WiFi Connection Failed");

    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");

    lcd.setCursor(0, 1);
    lcd.print("Check Network");

    delay(2000);
  }
}

// =====================================
// STANDBY SCREEN
// =====================================
void showStandby()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place Finger");

  lcd.setCursor(0, 1);
  lcd.print("For Attendance");
}

// =====================================
// PRINT RTC DATE/TIME
// =====================================
void printRTCDateTime()
{
  DateTime now = rtc.now();

  Serial.println("===== RTC DATE/TIME =====");

  Serial.printf("Date: %02d-%02d-%04d\n",
                now.day(),
                now.month(),
                now.year());

  Serial.printf("Time: %02d:%02d:%02d\n",
                now.hour(),
                now.minute(),
                now.second());

  Serial.println("=========================");
}

// =====================================
// SEND ATTENDANCE TO GOOGLE SHEETS
// =====================================
String sendAttendance(uint8_t id,
                      const char* name,
                      DateTime now)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected. Reconnecting...");

    WiFi.reconnect();

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - startTime < 10000)
    {
      delay(250);
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi reconnection failed.");
      return "ERROR";
    }
  }

  char dateStr[11];
  char timeStr[9];

  // DD-MM-YYYY
  snprintf(dateStr, sizeof(dateStr),
           "%02d-%02d-%04d",
           now.day(), now.month(), now.year());

  // HH:MM:SS, 24-hour format
  snprintf(timeStr, sizeof(timeStr),
           "%02d:%02d:%02d",
           now.hour(), now.minute(), now.second());

  String url = String(scriptURL);

  url += "?id=";
  url += String(id);

  url += "&name=";
  url += urlEncode(String(name));

  url += "&date=";
  url += urlEncode(String(dateStr));

  url += "&time=";
  url += urlEncode(String(timeStr));

  Serial.println("Sending attendance...");
  Serial.print("Date sent: ");
  Serial.println(dateStr);
  Serial.print("Time sent: ");
  Serial.println(timeStr);
  Serial.print("Request URL: ");
  Serial.println(url);

  WiFiClientSecure client;
  // Prototype only. Production should validate TLS certificates.
  client.setInsecure();

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(15000);

  if (!http.begin(client, url))
  {
    Serial.println("HTTP Begin Failed");
    return "ERROR";
  }

  int httpCode = http.GET();
  String response = "";

  if (httpCode > 0)
  {
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    response = http.getString();
    response.trim();

    Serial.print("Google Sheets Response: ");
    Serial.println(response);
  }
  else
  {
    Serial.print("HTTP Request Failed: ");
    Serial.println(http.errorToString(httpCode));
    response = "ERROR";
  }

  http.end();
  return response;
}

// =====================================
// SHOW ATTENDANCE RESULT
// =====================================
void showResult(const char* name, String response)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(name);

  lcd.setCursor(0, 1);

  if (response == "IN")
  {
    lcd.print("IN MARKED");
    Serial.println("IN Attendance Saved");
  }
  else if (response == "OUT")
  {
    lcd.print("OUT MARKED");
    Serial.println("OUT Attendance Saved");
  }
  else if (response == "COMPLETED")
  {
    lcd.print("ATTENDANCE DONE");
    Serial.println("Daily Attendance Completed");
  }
  else
  {
    lcd.print("SAVE FAILED");
    Serial.println("Attendance Save Failed");
  }

  delay(2500);
}

// =====================================
// SETUP
// =====================================
void setup()
{
  Serial.begin(115200);

  // I2C: SDA=GPIO21, SCL=GPIO22
  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.print("Fingerprint");
  lcd.setCursor(0, 1);
  lcd.print("Attendance");
  delay(1500);

  // RTC
  if (!rtc.begin())
  {
    Serial.println("DS3231 NOT FOUND");
    lcd.clear();
    lcd.print("RTC NOT FOUND");

    while (true)
      delay(1000);
  }

  Serial.println("DS3231 Found");

#if SET_RTC_TIME_ONCE == 1
  // Sets RTC using computer time when this sketch was compiled.
  // After first successful time setting, change the setting above to 0.
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("RTC set from compile time.");
#endif

  printRTCDateTime();

  // Fingerprint UART2
  FingerSerial.begin(
    57600,
    SERIAL_8N1,
    FINGER_RX,
    FINGER_TX
  );

  finger.begin(57600);

  if (!finger.verifyPassword())
  {
    Serial.println("Fingerprint Sensor NOT FOUND");

    lcd.clear();
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("Sensor Error");

    while (true)
      delay(1000);
  }

  Serial.println("Fingerprint Sensor Found!");

  connectWiFi();
  showStandby();

  Serial.println("================================");
  Serial.println("3 MEMBER ATTENDANCE SYSTEM");
  Serial.println("ID 1: SHASTHA");
  Serial.println("ID 2: MALADEVI");
  Serial.println("ID 3: DINESH");
  Serial.println("Attendance System Ready");
}

// =====================================
// MAIN LOOP
// =====================================
void loop()
{
  uint8_t p = finger.getImage();

  if (p != FINGERPRINT_OK)
    return;

  Serial.println();
  Serial.println("Fingerprint Detected");

  p = finger.image2Tz();

  if (p != FINGERPRINT_OK)
  {
    Serial.print("Image Conversion Failed. Code: ");
    Serial.println(p);

    lcd.clear();
    lcd.print("Scan Failed");
    delay(1500);

    showStandby();
    waitForFingerRemoval();
    return;
  }

  Serial.println("Image Converted");

  p = finger.fingerSearch();

  if (p == FINGERPRINT_NOTFOUND)
  {
    Serial.println("Fingerprint Not Registered");

    lcd.clear();
    lcd.print("Unknown Finger");
    lcd.setCursor(0, 1);
    lcd.print("Not Registered");

    delay(2000);
    showStandby();
    waitForFingerRemoval();
    return;
  }

  if (p != FINGERPRINT_OK)
  {
    Serial.print("Fingerprint Search Error: ");
    Serial.println(p);

    showStandby();
    waitForFingerRemoval();
    return;
  }

  // Matched fingerprint
  uint8_t id = finger.fingerID;
  const char* name = getStudentName(id);

  Serial.println("--------------------");
  Serial.print("Matched ID: ");
  Serial.println(id);
  Serial.print("Student Name: ");
  Serial.println(name);

  if (strcmp(name, "UNKNOWN") == 0)
  {
    Serial.println("Unknown Student");

    lcd.clear();
    lcd.print("Unknown Student");
    delay(2000);

    showStandby();
    waitForFingerRemoval();
    return;
  }

  // Read actual RTC date/time
  DateTime now = rtc.now();

  Serial.printf("Date: %02d-%02d-%04d\n",
                now.day(), now.month(), now.year());

  Serial.printf("Time: %02d:%02d:%02d\n",
                now.hour(), now.minute(), now.second());

  // Send attendance to Google Sheets
  String response = sendAttendance(id, name, now);

  showResult(name, response);

  Serial.print("Attendance Response: ");
  Serial.println(response);
  Serial.println("--------------------");

  showStandby();
  waitForFingerRemoval();
}
