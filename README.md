# 🔐 IoT-Based Fingerprint Attendance Management System using ESP32, R307S, DS3231, and Google Sheets

This project demonstrates an **IoT-based biometric attendance management system** using an ESP32 microcontroller and R307S fingerprint sensor. It identifies registered users through fingerprint authentication and records their attendance with real-time date and time using the DS3231 RTC module. Attendance details are automatically uploaded to Google Sheets through Wi-Fi for online monitoring and record management.

## 📌 Features

- Fingerprint enrollment for 3 members
- UART communication with R307S fingerprint sensor
- Unique ID-based fingerprint identification
- Automatic IN and OUT attendance tracking
- Real-time date and time using DS3231 RTC
- Displays student name and attendance status on 16x2 I2C LCD
- Wi-Fi connectivity using ESP32
- Automatic attendance data upload to Google Sheets
- Prevents repeated scanning until the finger is removed
- Daily attendance tracking with IN, OUT, and COMPLETED status

## 🧰 Hardware Used

- ESP32 Development Board
- R307S Fingerprint Sensor
- DS3231 RTC Module
- 16x2 I2C LCD Display
- Wi-Fi Network
- Jumper Wires
- Power Supply

## 🔌 Pin Connections

### ESP32 – R307S Fingerprint Sensor

| Component | ESP32 Pin | Description |
|---|---|---|
| Fingerprint VCC | VIN / 5V | Power Supply |
| Fingerprint GND | GND | Ground |
| Fingerprint TX | GPIO16 | UART2 RX |
| Fingerprint RX | GPIO17 | UART2 TX |

### ESP32 – DS3231 RTC Module

| Component | ESP32 Pin | Description |
|---|---|---|
| RTC VCC | 3.3V | Power Supply |
| RTC GND | GND | Ground |
| RTC SDA | GPIO21 | I2C Data |
| RTC SCL | GPIO22 | I2C Clock |

### ESP32 – 16x2 I2C LCD

| Component | ESP32 Pin | Description |
|---|---|---|
| LCD VCC | 5V | Power Supply |
| LCD GND | GND | Ground |
| LCD SDA | GPIO21 | I2C Data |
| LCD SCL | GPIO22 | I2C Clock |

**Note:** DS3231 and LCD share the same I2C bus. Ensure ESP32 GPIO pins are not exposed to 5V through I2C pull-ups; use suitable 3.3V pull-ups or a level shifter if required. Verify the fingerprint sensor's rated supply voltage before connecting.

## 👥 Registered Members

| Fingerprint ID | Student Name |
|---|---|
| 1 | SHASTHA |
| 2 | MALADEVI |
| 3 | DINESH |

## 🧠 Code Overview

- Initializes UART2 communication with the R307S fingerprint sensor at 57600 baud.
- Enrolls fingerprints and stores unique fingerprint IDs.
- Identifies registered members using fingerprint matching.
- Reads current date and time from the DS3231 RTC module.
- Connects ESP32 to Wi-Fi.
- Sends student ID, name, date, and time to Google Sheets using HTTP GET requests.
- Receives the attendance response from Google Apps Script.
- Displays the attendance result on the LCD.

## 📊 Attendance Working

- First successful scan → IN
- Second successful scan → OUT
- Further scan on the same day → COMPLETED
- New day → Attendance cycle starts again.

## 📄 Sample Output on Serial Monitor

```text
================================
3 MEMBER ATTENDANCE SYSTEM
================================

Fingerprint Sensor Found!
WiFi Connected

ID 1: SHASTHA
ID 2: MALADEVI
ID 3: DINESH

Attendance System Ready

Fingerprint Detected
Image Converted

Matched ID: 2
Student Name: MALADEVI

Date: DD-MM-YYYY
Time: HH:MM:SS

Sending attendance to Google Sheets...
Google Sheets Response: IN

IN Attendance Saved
```

## 📋 Google Sheets Attendance Format

| ID | Name | Date | Time | Punch |
|---|---|---|---|---|
| 1 | SHASTHA | DD-MM-YYYY | HH:MM:SS | IN |
| 2 | MALADEVI | DD-MM-YYYY | HH:MM:SS | IN |
| 3 | DINESH | DD-MM-YYYY | HH:MM:SS | IN |

## 🛠️ How to Use

1. Connect the R307S fingerprint sensor, DS3231 RTC, and LCD to ESP32 according to the pin mapping.
2. Install the required Arduino libraries.
3. Enroll three members with unique fingerprint IDs.
4. Configure Wi-Fi credentials in the ESP32 program.
5. Deploy the Google Apps Script Web App and configure its URL in the code.
6. Upload the attendance program to ESP32.
7. Place a registered finger on the sensor.
8. The system identifies the member and records IN/OUT attendance automatically.
9. View attendance records in Google Sheets.

## 📁 Files Included

- `enrollment.ino` – Three-member fingerprint enrollment code
- `attendance.ino` – Fingerprint-based attendance and Google Sheets integration
- `delete_fingerprint.ino` – Delete selected fingerprint IDs
- `README.md` – Project overview and instructions

## 💻 Technologies Used

- Embedded C++
- ESP32
- Arduino IDE
- UART Communication
- I2C Communication
- Wi-Fi
- Google Apps Script
- Google Sheets

## 🎯 Applications

- Educational Institutions
- Colleges and Schools
- Office Attendance Management
- Employee Biometric Attendance
- Small Organization Attendance Systems
<img width="1920" height="1080" alt="Screenshot (383)" src="https://github.com/user-attachments/assets/f7b49466-f334-4e4a-846a-2f8b8187d8de" />
