#include <WiFi.h>
#include <FirebaseESP32.h>

// Information de connection
const char* ssid = "Gnet";
const char* password = "1234@1234";

// Declaration PIN
const int soilMoisturePin = 33;
const int relay = 2;

// Information Firebase
#define FIREBASE_HOST "mit-project-c4bd4-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "kV9RQLooHVkVCWu50ZxQteyRUMicX1YWuC3ADQ1U"
FirebaseData firebaseData;

void AutoWateringMode(int sensorValue, int relay, int target)
{
  if (sensorValue < target) {
    digitalWrite(relay, LOW);
    delay(5000);
    digitalWrite(relay, HIGH);
    delay(5000);
    Serial.println("The Pump is ON");
    Firebase.setString(firebaseData, "/IOT/Status", "'The Pump is ON --- Auto Mode'");
  }
  else {
    digitalWrite(relay, HIGH);
    Serial.println("The Pump is OFF");
    Firebase.setString(firebaseData, "/IOT/Status", "'The Pump is OFF --- Auto Mode'");
  }
}

void ManuelMode(int sensorValue, int relay)
{
  digitalWrite(relay, LOW);
  Serial.println("The Pump is ON");
  Firebase.setString(firebaseData, "/IOT/Status", "'The Pump is ON --- Manual Mode'");
}

void Status_Info(String msg)
{
  Firebase.setString(firebaseData, "/IOT/Status", msg + "'");
  delay(200);
  Firebase.setString(firebaseData, "/IOT/Status", msg + ".'");
  delay(200);
  Firebase.setString(firebaseData, "/IOT/Status", msg + "..'");
  delay(200);
  Firebase.setString(firebaseData, "/IOT/Status", msg + "...'");
  delay(200);
}

void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

String power,autoWatering,manuelWatering ;
int target;


void loop() {
  Firebase.getString(firebaseData, "/IOT/On", &power);
  Firebase.getString(firebaseData, "/IOT/AutoWater", &autoWatering);
  Firebase.getString(firebaseData, "/IOT/NormalWater", &manuelWatering);
  Firebase.getInt(firebaseData, "/IOT/Target", &target);

  if (power == "1") {
    int sensorValue = analogRead(soilMoisturePin);
    sensorValue = (sensorValue * 100) / 4095;
    Serial.println(sensorValue);
    Firebase.setInt(firebaseData, "/IOT/Humidity", sensorValue);

    if (sensorValue < 100) {
      if (autoWatering == "1") {
        AutoWateringMode(sensorValue, relay, target);
      }
      if (manuelWatering == "1") {
        ManuelMode(sensorValue, relay);
      }
    }
    else {
      digitalWrite(relay, HIGH);
      Status_Info("'Verify the sensor position");
    }
  }
  else {
    digitalWrite(relay, HIGH);
    Status_Info("'Device is OFF");
  }
  delay(100);
}
