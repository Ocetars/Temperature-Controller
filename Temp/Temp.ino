#include <DHT11.h>
#define SET_POINT 40  // 设定的温度
#define KP 1.0  // 比例系数
#define KI 0.0  // 积分系数
#define HEATER_PIN 3  // 加热器连接的GPIO口

// Create an instance of the DHT11 class.
// - For Arduino: Connect the sensor to Digital I/O Pin 2.
// - For ESP32: Connect the sensor to pin GPIO2 or P2.
// - For ESP8266: Connect the sensor to GPIO2 or D4.
DHT11 dht11(2);
double integral = 0;

void setup() {
    // Initialize serial communication to allow debugging and data readout.
    // Using a baud rate of 9600 bps.
    Serial.begin(9600);
    pinMode(HEATER_PIN, OUTPUT);
    
    // Uncomment the line below to set a custom delay between sensor readings (in milliseconds).
    // dht11.setDelay(500); // Set this to the desired delay. Default is 500ms.
}

void loop() {
    int temperature = 0;
    int humidity = 0;

    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");
        // Serial.print(" °C\tHumidity: ");
        // Serial.print(humidity);
        // Serial.println(" %");
        double error = SET_POINT - temperature;
        double proportional = KP * error;
        integral += error;
        double integralTerm = KI * integral;
        double output = proportional + integralTerm;

        if (output > 0) {
            digitalWrite(HEATER_PIN, HIGH);
        } else {
            digitalWrite(HEATER_PIN, LOW);
        }
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }
}