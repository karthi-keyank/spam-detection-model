#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ==================================================
// 1. WiFi Credentials
// ==================================================
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ==================================================
// 2. Flask Server URL
// ==================================================
const char* SERVER_URL = "http://10.155.30.124:5000/predict";

// ==================================================
// 3. Hardware Pins
// ==================================================
const int BUZZER_PIN = 25;   // Active buzzer
const int LED_PIN    = 26;   // LED

// ==================================================
// 4. Hard-coded message (EDIT HERE ONLY)
// ==================================================
String messageToCheck =
  "Hi, you have been selected for a special reward today, please check the details.";

// ==================================================
// 5. Timing control
// ==================================================
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL = 10000; // 10 seconds

// ==================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Setup pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // --------------------------------------------------
  // Connect to WiFi
  // --------------------------------------------------
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("\nSystem ready. Sending messages periodically...\n");
}

// ==================================================
void loop() {

  unsigned long currentTime = millis();

  // Send message every SEND_INTERVAL milliseconds
  if (currentTime - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = currentTime;
    sendMessage(messageToCheck);
  }
}

// ==================================================
// Send message to Flask server
// ==================================================
void sendMessage(String message) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"message\":\"" + message + "\"}";

  Serial.println("Sending message:");
  Serial.println(message);

  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("Server response:");
    Serial.println(response);

    handleResponse(response);
  } else {
    Serial.print("HTTP request failed, code: ");
    Serial.println(httpCode);
  }

  http.end();
}

// ==================================================
// Handle ML server response
// ==================================================
void handleResponse(String response) {

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, response);

  if (error) {
    Serial.println("JSON parsing failed");
    return;
  }

  const char* label = doc["label"];

  Serial.print("Detected label: ");
  Serial.println(label);

  if (strcmp(label, "not_spam") == 0) {
    signalAlert(1);
  }
  else if (strcmp(label, "suspicious") == 0) {
    signalAlert(2);
  }
  else if (strcmp(label, "spam") == 0) {
    signalAlert(3);
  }
}

// ==================================================
// Beep + Blink logic
// ==================================================
void signalAlert(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    delay(200);

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}
