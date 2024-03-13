#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <Wire.h>

#define DHTTYPE DHT11
#define DHTPIN 5

struct WeatherTuple {
	int temperature;
	int temperature_f;
	int humidity;
	int pressure;
};

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

/**
 * Gets the pressure, humidity, and temperature from BMP180 and DHT11 sensor.
 * @param dht DHT11 sensor object
 * @param bmp BMP180 sensor object
 * @param unit Unit of the temperature, O for SI units, 1 for CGS units
 * @return A WeatherTuple that contains temperature, humidity, and pressure
*/
WeatherTuple get_weather(DHT dht, Adafruit_BMP085 bmp);

void setup() {
	Wire.begin();
	Serial.begin(9600);


	dht.begin();
	if (!bmp.begin()) {
		Serial.println("Could not find a valid BMP085 sensor, check wiring!");
		while (1) {}
	}
}

void loop() {
	WeatherTuple weather;
	weather = get_weather(dht, bmp);

	Serial.println("Temperature: " + String(weather.temperature) + "C, " + String(weather.temperature_f) + "F");
	Serial.println("Humidity: " + String(weather.humidity) + "%");
	Serial.println("Pressure: " + String(weather.pressure) + "hPa");
}

WeatherTuple get_weather(DHT dht, Adafruit_BMP085 bmp) {
	WeatherTuple weather;
	int humidity, temperature_f, temperature_c, pressure, heat_index;
	
	temperature_c = bmp.readTemperature();
	temperature_f = (temperature_c * (9.0/5.0)) + 32.0;
	humidity = dht.readHumidity();
	pressure = bmp.readPressure()*0.01;

	if (isnan(humidity) || isnan(temperature_c)) {
		Serial.println(F("Failed to read from DHT sensor!"));

		weather.temperature = 0;
		weather.temperature_f = 0;
		weather.humidity = 0;
		weather.pressure = 0;

		return weather;
	}

	weather.temperature = temperature_c;
	weather.temperature_f = temperature_f;
	weather.humidity = humidity;
	weather.pressure = pressure;

	return weather;
}
