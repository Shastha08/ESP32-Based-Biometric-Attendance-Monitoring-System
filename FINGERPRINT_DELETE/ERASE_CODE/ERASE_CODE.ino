#include <Adafruit_Fingerprint.h>

#define FINGER_RX 16
#define FINGER_TX 17

HardwareSerial FingerSerial(2);
Adafruit_Fingerprint finger(&FingerSerial);

void setup()
{
  Serial.begin(115200);

  FingerSerial.begin(
    57600,
    SERIAL_8N1,
    FINGER_RX,
    FINGER_TX
  );

  finger.begin(57600);

  Serial.println();
  Serial.println("================================");
  Serial.println(" FINGERPRINT DELETE SYSTEM");
  Serial.println("================================");

  if (!finger.verifyPassword())
  {
    Serial.println("Fingerprint Sensor NOT FOUND!");
    return;
  }

  Serial.println("Fingerprint Sensor Found!");

  // Delete ID 1 - SHASTHA
  Serial.println();
  Serial.println("Deleting ID 1 - SHASTHA");

  uint8_t result1 = finger.deleteModel(1);

  if (result1 == FINGERPRINT_OK)
    Serial.println("SHASTHA fingerprint deleted.");
  else if (result1 == FINGERPRINT_NOTFOUND)
    Serial.println("ID 1 fingerprint not found.");
  else
  {
    Serial.print("ID 1 delete error: ");
    Serial.println(result1);
  }

  delay(500);

  // Delete ID 2 - MALADEVI
  Serial.println();
  Serial.println("Deleting ID 2 - MALADEVI");

  uint8_t result2 = finger.deleteModel(2);

  if (result2 == FINGERPRINT_OK)
    Serial.println("MALADEVI fingerprint deleted.");
  else if (result2 == FINGERPRINT_NOTFOUND)
    Serial.println("ID 2 fingerprint not found.");
  else
  {
    Serial.print("ID 2 delete error: ");
    Serial.println(result2);
  }

  delay(500);

  // Delete ID 3 - DINESH
  Serial.println();
  Serial.println("Deleting ID 3 - DINESH");

  uint8_t result3 = finger.deleteModel(3);

  if (result3 == FINGERPRINT_OK)
    Serial.println("DINESH fingerprint deleted.");
  else if (result3 == FINGERPRINT_NOTFOUND)
    Serial.println("ID 3 fingerprint not found.");
  else
  {
    Serial.print("ID 3 delete error: ");
    Serial.println(result3);
  }

  Serial.println();
  Serial.println("================================");
  Serial.println(" SELECTED FINGERPRINT DELETE DONE");
  Serial.println("================================");
}

void loop()
{
}