#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <BluetoothSerial.h>

#define DHTPIN 5
#define DHTTYPE DHT11

Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);
BluetoothSerial SerialBT;

void setup() {
    Serial.begin(115200);
    SerialBT.begin("ESP32test");
    
    if (!bmp.begin()) {
        Serial.println(F("Could not find a valid BMP085 sensor, check wiring!"));
        while (1);
    }

    dht.begin();
}

void loop() {
    float bmp_temperature = bmp.readTemperature();
    float bmp_pressure = bmp.readPressure() / 100.0;
    float dht_humidity = dht.readHumidity();

    SerialBT.println(bmp_temperature);
    SerialBT.println(bmp_pressure);
    SerialBT.println(dht_humidity);

    delay(2000);
}
