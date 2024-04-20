#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define DHTPIN 5
#define DHTTYPE DHT11
#define SERVICE_UUID "1c06642a-a760-4520-a8ec-226f71406555"
#define CHARACTERISTIC_UUID "b2cad3a9-824d-4ad6-a2ca-9544852e1612"

Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);

bool device_connected = false;
BLEServer *p_server;
BLECharacteristic *p_characteristic;

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *p_server) {
        device_connected = true;
        Serial.println("Connected");
    }

    void onDisconnect(BLEServer *p_server) {
        device_connected = false;
        p_server->startAdvertising();
        Serial.println("Disconnected");
    }
};

void setup() {
    Serial.begin(115200);

    BLEDevice::init("WeatherWhisper");

    p_server = BLEDevice::createServer();
    p_server->setCallbacks(new ServerCallbacks());

    BLEService *p_service = p_server->createService(SERVICE_UUID);
    p_characteristic = p_service->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );
    p_service->start();

    BLEAdvertising *p_advertising = BLEDevice::getAdvertising();
    p_advertising->addServiceUUID(SERVICE_UUID);
    p_advertising->setScanResponse(true);
    p_advertising->setMinPreferred(0x06);
    p_advertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
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

    if (device_connected) {
        String data = String(bmp_temperature) + "," + String(bmp_pressure) + "," + String(dht_humidity);
        p_characteristic->setValue(data.c_str());
        p_characteristic->notify();
    }

    delay(2000);
}
