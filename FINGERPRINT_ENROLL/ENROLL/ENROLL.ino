
#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include <Preferences.h>

// =====================================
// ESP32 UART2 CONNECTION
// =====================================

#define FINGER_RX 16
#define FINGER_TX 17

#define TOTAL_MEMBERS 3

HardwareSerial FingerSerial(2);
Adafruit_Fingerprint finger(&FingerSerial);

Preferences preferences;

int memberCount = 0;
bool usedID[4] = {false, false, false, false};

// =====================================
// READ SERIAL INPUT
// =====================================

String readInput(const char* message)
{
  Serial.println(message);

  while (Serial.available())
  {
    Serial.read();
  }

  while (true)
  {
    if (Serial.available())
    {
      String input = Serial.readStringUntil('\n');
      input.trim();

      if (input.length() > 0)
      {
        return input;
      }
    }

    delay(10);
  }
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
    {
      break;
    }

    delay(100);
  }

  delay(500);

  Serial.println("Ready for next scan.");
}

// =====================================
// CAPTURE FINGERPRINT
// =====================================

bool captureFingerprint(uint8_t slot, const char* message)
{
  Serial.println(message);

  while (true)
  {
    uint8_t result = finger.getImage();

    if (result == FINGERPRINT_OK)
    {
      break;
    }

    if (result != FINGERPRINT_NOFINGER)
    {
      Serial.print("Image capture error: ");
      Serial.println(result);
    }

    delay(100);
  }

  uint8_t result = finger.image2Tz(slot);

  if (result != FINGERPRINT_OK)
  {
    Serial.print("Image conversion failed. Error: ");
    Serial.println(result);

    return false;
  }

  Serial.println("Image captured and converted.");

  return true;
}

// =====================================
// ENROLL FINGERPRINT
// =====================================

bool enrollFingerprint(uint16_t id)
{
  Serial.println();
  Serial.println("Starting fingerprint enrollment...");

  // FIRST SCAN
  while (!captureFingerprint(
      1, "FIRST SCAN: Place your finger"))
  {
    waitForFingerRemoval();
    Serial.println("Try first scan again.");
  }

  waitForFingerRemoval();

  // SECOND SCAN
  while (!captureFingerprint(
      2, "SECOND SCAN: Place the SAME finger"))
  {
    waitForFingerRemoval();
    Serial.println("Try second scan again.");
  }

  // CREATE MODEL
  Serial.println();
  Serial.println("Creating fingerprint model...");

  uint8_t result = finger.createModel();

  if (result != FINGERPRINT_OK)
  {
    Serial.print("Model creation failed. Error: ");
    Serial.println(result);

    if (result == FINGERPRINT_ENROLLMISMATCH)
    {
      Serial.println("Both fingerprint scans do not match.");
    }

    return false;
  }

  Serial.println("Fingerprint model created.");

  // STORE MODEL IN SENSOR
  Serial.print("Saving fingerprint at ID: ");
  Serial.println(id);

  result = finger.storeModel(id);

  if (result != FINGERPRINT_OK)
  {
    Serial.print("Fingerprint storage failed. Error: ");
    Serial.println(result);

    if (result == 12)
    {
      Serial.println("DBRANGEFAIL: Sensor rejected this storage location.");
      Serial.println("Check sensor protocol and storage capacity.");
    }

    return false;
  }

  Serial.println("FINGERPRINT SAVED SUCCESSFULLY!");

  return true;
}

// =====================================
// SAVE NAME IN ESP32 MEMORY
// =====================================

bool saveMemberName(uint16_t id, String name)
{
  String key = "name_" + String(id);

  size_t written = preferences.putString(
    key.c_str(),
    name
  );

  return written > 0;
}

// =====================================
// SETUP
// =====================================

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(30000);

  FingerSerial.begin(
    57600,
    SERIAL_8N1,
    FINGER_RX,
    FINGER_TX
  );

  finger.begin(57600);

  // ESP32 permanent storage
  preferences.begin("attendance", false);

  Serial.println();
  Serial.println("================================");
  Serial.println("    3 MEMBER ENROLLMENT SYSTEM");
  Serial.println("================================");

  // SENSOR CHECK
  if (!finger.verifyPassword())
  {
    Serial.println("Fingerprint Sensor NOT FOUND!");
    Serial.println("Check RX, TX, VCC and GND.");

    while (true)
    {
      delay(1000);
    }
  }

  Serial.println("Fingerprint Sensor Found!");

  // READ SENSOR CAPACITY
  uint8_t result = finger.getParameters();

  if (result != FINGERPRINT_OK)
  {
    Serial.print("Sensor parameter error: ");
    Serial.println(result);

    while (true)
    {
      delay(1000);
    }
  }

  Serial.print("Sensor capacity: ");
  Serial.println(finger.capacity);

  Serial.println();
  Serial.println("Available IDs: 1, 2, 3");
  Serial.println("Ready to register 3 members.");
}

// =====================================
// MAIN LOOP
// =====================================

void loop()
{
  // FINISH AFTER THREE MEMBERS
  if (memberCount >= TOTAL_MEMBERS)
  {
    Serial.println();
    Serial.println("================================");
    Serial.println("            THANK YOU");
    Serial.println("   3 MEMBERS SAVED SUCCESSFULLY");
    Serial.println("================================");

    while (true)
    {
      delay(1000);
    }
  }

  Serial.println();
  Serial.println("--------------------------------");
  Serial.print("REGISTER MEMBER ");
  Serial.print(memberCount + 1);
  Serial.println(" OF 3");
  Serial.println("--------------------------------");

  // =================================
  // STEP 1: GET STUDENT ID
  // =================================

  uint16_t id = 0;

  while (true)
  {
    String input = readInput(
      "Enter Student ID (1 to 3):"
    );

    bool valid = true;

    for (unsigned int i = 0; i < input.length(); i++)
    {
      if (!isDigit(input[i]))
      {
        valid = false;
        break;
      }
    }

    if (!valid)
    {
      Serial.println("Enter numbers only!");
      continue;
    }

    int enteredID = input.toInt();

    if (enteredID < 1 || enteredID > 3)
    {
      Serial.println("Invalid ID! Enter 1, 2 or 3 only.");
      continue;
    }

    id = enteredID;

    if (usedID[id])
    {
      Serial.println("This ID is already used in this session.");
      continue;
    }

    break;
  }

  Serial.print("Selected ID: ");
  Serial.println(id);

  // =================================
  // STEP 2: ENROLL FINGERPRINT
  // =================================

  if (!enrollFingerprint(id))
  {
    Serial.println();
    Serial.println("Member enrollment failed.");
    Serial.println("Restart and check the sensor.");

    while (true)
    {
      delay(1000);
    }
  }

  // =================================
  // STEP 3: GET STUDENT NAME
  // =================================

  String name;

  while (true)
  {
    name = readInput("Enter Student Name:");

    name.toUpperCase();

    if (name.length() > 0)
    {
      break;
    }

    Serial.println("Name cannot be empty.");
  }

  // =================================
  // STEP 4: SAVE NAME
  // =================================

  if (!saveMemberName(id, name))
  {
    Serial.println("Name storage failed!");
    Serial.println("Fingerprint is already saved in sensor.");

    while (true)
    {
      delay(1000);
    }
  }

  usedID[id] = true;
  memberCount++;

  // =================================
  // DISPLAY SUCCESS
  // =================================

  Serial.println();
  Serial.println("================================");
  Serial.println("FINGERPRINT SAVED SUCCESSFULLY!");
  Serial.println("--------------------------------");

  Serial.print("Saved ID: ");
  Serial.println(id);

  Serial.print("Saved Name: ");
  Serial.println(name);

  Serial.println("Fingerprint: SAVED");

  Serial.print("Members saved: ");
  Serial.print(memberCount);
  Serial.println("/3");

  Serial.println("================================");

  if (memberCount < TOTAL_MEMBERS)
  {
    Serial.println();
    Serial.println("Next member enrollment starting...");
    delay(1500);
  }
}
