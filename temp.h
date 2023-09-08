// Code for Sensors of Plant Module

#include <Arduino.h>
#include <Adafruit_BME280.h>  // temp/humid sensor
#include <Adafruit_Sensor.h>

// ----- TEMP/HUMID SENSOR
#define SEALEVELPRESSURE_HPA (1013.25) 



// -------------------- FUNCTIONS -----------------------

int temp_sensor(Adafruit_BME280& bme) {
  int offset = -5;
  float temperatureC = bme.readTemperature(); // read temperature from BME280 sensor
  int temperature = temperatureC * 9/5 + 32; // convert to Fahrenheit

  // Serial.print("Temperature: ");
  // Serial.print(temperature);
  // Serial.println(" F");

  return temperature + offset;

}

int humid_sensor(Adafruit_BME280& bme) {
  int offset = 5;
  int humidity = bme.readHumidity(); // read humidity from BME280 sensor

  // Serial.print("Humidity: ");
  // Serial.print(humidity);
  // Serial.println(" %");

  return humidity + offset;

}

int press_sensor(Adafruit_BME280& bme) {
  int pressure = bme.readPressure() / 100.0F; // read pressure from BME280 sensor
  // int pressure = map(pressure, 300, 1100, 0, 100); // map pressure to percentage value

  // Serial.print("Pressure: ");
  // Serial.print(pressure);
  // Serial.println(" hPa");

  return pressure;

}



