#include <ThingSpeak.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>


// Define connections to sensor
#define TRIGPIN 18
#define ECHOPIN 19

// Floats to calculate distance
float duration, distance;

//Hotspot
const char *ssid = "Flashlight";
const char *pass = "wifirajelas";

int buzzer = 4;
int led = 2;
int countsensor = 0;

WiFiClient client;

//thingspeak settings
unsigned long channel1 = 1; //your channel ID number
unsigned long channel2 = 2; //your channel ID number

const char *apiKey = "3WPSBVM7LW88W1UK"; //your channel write API Key

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

float temperatureC;

Adafruit_BMP280 bmp; //BMP280 connect to ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)

void initBME(){
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void setup() {
  // Set up serial monitor
  Serial.begin(115200);
  initBME();

  // Set pinmodes for sensor connections
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  //connect to WiFi
  Serial.print("Connecting to: "); 
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected\n");
  ThingSpeak.begin(client); //initialize ThingSpeak
  
}

void loop() {

  // Set the trigger pin LOW for 2uS
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);

  // Set the trigger pin HIGH for 20us to send pulse
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(20);

  // Return the trigger pin to LOW
  digitalWrite(TRIGPIN, LOW);

  // Measure the width of the incoming pulse
  duration = pulseIn(ECHOPIN, HIGH);

  // Determine distance from duration
  // Use 343 metres per second as speed of sound
  // Divide by 1000 as we want millimeters
  // Get a new temperature reading
    temperatureC = bmp.readTemperature();
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureC);

  distance = (duration / 20) * 0.343 ;// Bawah air (duration/2)*1.498  atau 1.481

  // Print result to serial monitor
  
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println(" cm");

 
  if (distance <= 21){
    countsensor++;
    Serial.println(countsensor);
    if (countsensor >= 4){
      tone(buzzer, 1000); 
      digitalWrite(led, HIGH);
      delay(1000);
      countsensor=0;
     }else {
      //countsensor=0;
      noTone(buzzer);
      digitalWrite(led, LOW);
      delay(50);
      }
      //countsensor =0;
   }else {
      noTone(buzzer);
      digitalWrite(led, LOW);
      delay(50);
    }
  //ThingSpeak.setField(1, distance);
  //ThingSpeak.setField(2, temperatureC);
  
  int x = ThingSpeak.writeField(channel1,1,distance, apiKey);
  int y = ThingSpeak.writeField(channel2,2,temperatureC, apiKey);
  
  if (x == 200 || y == 200) {
    //Serial.println("Data " + String(distance));
    Serial.println("distance: "); Serial.println(distance);
    Serial.println("temperature: "); Serial.println(temperatureC);
    Serial.println("Update to ThingSpeak Successful.");
  }
  else {
    Serial.println("Data " + String(distance));
    Serial.println("Problem updating data. HTTP error code " + String(x));
  }

  Serial.println("Waiting...\n");
  delay(200);
 
}
