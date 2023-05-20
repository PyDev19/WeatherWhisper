#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

#define BMP_I2C 0x77
#define LCD_12C 0x27
#define DHTTYPE DHT11
#define DHTPIN 5

struct WeatherTuple {
	int temperature;
	int temperature_f;
	int humidity;
	int pressure;
};

byte degree_symbol[8] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;
LiquidCrystal_I2C lcd(LCD_12C, 20, 2);
ezButton unitchange_button(12);

int unit = 0;

/**
 * Gets the pressure, humidity, and temperature from BMP180 and DHT11 sensor.
 * @param dht DHT11 sensor object
 * @param bmp BMP180 sensor object
 * @param unit Unit of the temperature, O for SI units, 1 for CGS units
 * @return A WeatherTuple that contains temperature, humidity, and pressure
*/
WeatherTuple get_weather(DHT dht, Adafruit_BMP085 bmp, int unit);

void setup() {
	Wire.begin();
	Serial.begin(9600);

	unitchange_button.setDebounceTime(50);

	dht.begin();
	lcd.begin(20, 2);
	if (!bmp.begin()) {
		Serial.println("Could not find a valid BMP085 sensor, check wiring!");
		while (1) {}
  	}

	lcd.backlight();
	lcd.createChar(1, degree_symbol);
}

void loop() {
	unitchange_button.loop();
	if (unitchange_button.isPressed()) {
		switch (unit) {
			case 0:
				unit = 1;
				break;
			case 1:
				unit = 0;
				break;
		}
	}

	WeatherTuple weather;
	weather = get_weather(dht, bmp, unit);

	// Display the data on the LCD display
	lcd.setCursor(0, 0);
	lcd.print("Temp: ");
	switch (unit) {
		case 0:
			lcd.print(weather.temperature);
			lcd.write(1);
			lcd.print("C");
			break;
		case 1:
			lcd.print(weather.temperature_f);
			lcd.write(1);
			lcd.print("F");
			break;
	}
	
	lcd.setCursor(0, 1);
	lcd.print("Humidity: ");
	lcd.print(weather.humidity);
	lcd.print("%");
}

WeatherTuple get_weather(DHT dht, Adafruit_BMP085 bmp, int unit) {
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
