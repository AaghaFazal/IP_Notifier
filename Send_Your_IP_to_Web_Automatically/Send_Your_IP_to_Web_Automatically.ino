#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecureBearSSL.h>  // Include BearSSL for secure connections
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>  // Include HTTPClient library

// Replace with your server URL (HTTPS)
const char* serverUrl = "https://noorkyp23.000webhostapp.com/update-ip.php";

// Root certificate for api.ipify.org
const char* fingerprint = "D8:C8:48:53:32:4E:5A:62:2A:92:26:A0:5D:9E:65:16:5A:6E:F8:EC"; // Change this if needed

ESP8266WebServer server(80);

const int buttonPin = 0; // GPIO0 (Flash button)
const int ledPin = 2; // GPIO2 (Onboard LED, active low)
bool wasConnected = false;
bool isAPMode = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Turn off LED initially

  Serial.begin(115200);
  delay(10);

  pinMode(buttonPin, INPUT);

  // Load Wi-Fi credentials from EEPROM
  EEPROM.begin(512);
  String ssid = getValue(0);
  String password = getValue(32);

  SerialPrintln("Loaded Wi-Fi credentials:");
  SerialPrint("SSID: ");
  SerialPrintln(ssid);
  SerialPrint("Password: ");
  SerialPrintln(password);

  if (ssid.length() > 0 && password.length() > 0) {
    SerialPrint("Connecting to ");
    SerialPrintln(ssid);
    WiFi.begin(ssid.c_str(), password.c_str());

    int attemptCount = 0;
    while (WiFi.status() != WL_CONNECTED && attemptCount < 20) {
      delay(1000);
      SerialPrint(".");
      attemptCount++;
    }
    SerialPrintln("");

    if (WiFi.status() == WL_CONNECTED) {
      SerialPrintln("WiFi connected");
      SerialPrint("IP address: ");
      SerialPrintln(WiFi.localIP().toString());

      // Send IP address on initial connection
      sendIPAddress();
      wasConnected = true;
    } else {
      SerialPrintln("Failed to connect to WiFi.");
      startAPMode();
    }
  } else {
    SerialPrintln("No Wi-Fi credentials found, starting AP mode.");
    startAPMode();
  }

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSaveCredentials);
  server.begin();
}

void loop() {
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    if (!wasConnected) {
      SerialPrintln("WiFi reconnected");
      SerialPrint("IP address: ");
      SerialPrintln(WiFi.localIP().toString());
      sendIPAddress();
      wasConnected = true;
      isAPMode = false; // Stop blinking when reconnected
    }
  } else {
    if (wasConnected) {
      SerialPrintln("WiFi disconnected, trying to reconnect...");
      String ssid = getValue(0);
      String password = getValue(32);
      WiFi.begin(ssid.c_str(), password.c_str());
      wasConnected = false;
    }
  }

  if (digitalRead(buttonPin) == LOW) {
    delay(2000); // Debounce delay
    if (digitalRead(buttonPin) == LOW) {
      SerialPrintln("Button pressed, starting AP mode.");
      startAPMode();
    }
  }

  if (isAPMode) {
    // Blink LED continuously in AP mode
    digitalWrite(ledPin, LOW);  // Turn on LED
    delay(50);
    digitalWrite(ledPin, HIGH); // Turn off LED
    delay(50);
  } else {
    delay(10000);  // Adjust as necessary for normal operation
  }
}

void sendIPAddress() {
  SerialPrintln("Attempting to send IP address...");

  // Use BearSSL for secure connections
  BearSSL::WiFiClientSecure client;

  // Optionally, disable certificate verification for testing
  client.setInsecure();
  
  // Alternatively, use setFingerprint if you have the fingerprint
  // client.setFingerprint(fingerprint);

  SerialPrintln("Connecting to api.ipify.org...");
  
  // Connect to api.ipify.org HTTPS server
  if (!client.connect("api.ipify.org", 443)) {
    SerialPrintln("Failed to connect to api.ipify.org");
    return;
  }

  SerialPrintln("Connected to api.ipify.org, sending GET request...");
  
  // Send GET request to api.ipify.org
  client.print("GET / HTTP/1.1\r\n");
  client.print("Host: api.ipify.org\r\n");
  client.print("Connection: close\r\n\r\n");

  // Check if connected
  if (!client.connected()) {
    SerialPrintln("Connection to api.ipify.org failed");
    client.stop();
    return;
  }

  // Wait for response
  while (client.connected() && !client.available()) {
    delay(50);
  }

  // Read response
  String ipAddress = "";
  while (client.available()) {
    char c = client.read();
    ipAddress += c;
  }

  // Remove HTTP headers
  ipAddress = ipAddress.substring(ipAddress.indexOf("\r\n\r\n") + 4);
  SerialPrint("Public IP address: ");
  SerialPrintln(ipAddress);

  // Send IP address to your server
  HTTPClient http;
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String postData = "ip=" + ipAddress;

  SerialPrintln("Sending IP address to server...");
  
  int httpCode = http.POST(postData);

  if (httpCode > 0) {
    String payload = http.getString();
    SerialPrint("Server response: ");
    SerialPrintln(payload);
  } else {
    SerialPrint("Error sending IP to server: ");
    SerialPrintln(String(httpCode));
  }

  http.end();

  // Disconnect from api.ipify.org
  client.stop();
  SerialPrintln("Disconnected from api.ipify.org");
}

void handleRoot() {
  String page = "<html><body><h1>Enter Wi-Fi Credentials</h1>";
  page += "<form action=\"/save\" method=\"post\">";
  page += "SSID: <input type=\"text\" name=\"ssid\"><br>";
  page += "Password: <input type=\"text\" name=\"password\"><br>";
  page += "<input type=\"submit\" value=\"Save\">";
  page += "</form></body></html>";

  server.send(200, "text/html", page);
}

void handleSaveCredentials() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  SerialPrintln("Saving Wi-Fi credentials:");
  SerialPrintln("SSID: " + ssid);
  SerialPrintln("Password: " + password);

  saveValue(0, ssid);
  saveValue(32, password);

  server.send(200, "text/html", "<html><body><h1>Credentials Saved. Restarting...</h1></body></html>");
  delay(2000);
  ESP.restart();
}

void saveValue(int startAddress, String value) {
  for (int i = 0; i < value.length(); ++i) {
    EEPROM.write(startAddress + i, value[i]);
  }
  EEPROM.write(startAddress + value.length(), 0); // Null terminator
  EEPROM.commit();
}

String getValue(int startAddress) {
  char data[32];
  int len = 0;
  unsigned char k;
  k = EEPROM.read(startAddress);
  while (k != 0 && len < 32) {
    k = EEPROM.read(startAddress + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

void startAPMode() {
  WiFi.softAP("IP_Notifier");

  IPAddress IP = WiFi.softAPIP();
  SerialPrint("AP IP address: ");
  SerialPrintln(IP.toString());

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSaveCredentials);
  server.begin();
  
  isAPMode = true; // Set AP mode flag to true to start blinking
}

// Custom Serial print functions to handle LED blinking
void SerialPrint(const String& message) {
  digitalWrite(ledPin, LOW);  // Turn on LED
  delay(100);                 // Wait for 100 milliseconds
  Serial.print(message);
  digitalWrite(ledPin, HIGH); // Turn off LED
  delay(100);                 // Wait for 100 milliseconds
}

void SerialPrintln(const String& message) {
  digitalWrite(ledPin, LOW);  // Turn on LED
  delay(100);                 // Wait for 100 milliseconds
  Serial.println(message);
  digitalWrite(ledPin, HIGH); // Turn off LED
  delay(100);                 // Wait for 100 milliseconds
}

void clearEEPROM() {
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}
