#include <DHT11.h>
// #define SET_POINT 35  // 设定的温度
#define HEATER_PIN 10  // 加热器连接的GPIO口
#define BEEPER_PIN 13  // 蜂鸣器连接的GPIO口
DHT11 dht11(18);

bool firstReach = true;
bool nextReach = false;
bool heaterOn = false;
bool reachedSetPoint = false;
unsigned long startTime = 0;
int SET_POINT = 30;

void beep(int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(BEEPER_PIN, HIGH);
        delay(100);
        digitalWrite(BEEPER_PIN, LOW);
        delay(100);
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(HEATER_PIN, OUTPUT);
    pinMode(BEEPER_PIN, OUTPUT);
    Serial.println("请输入设定温度值：");

    while (Serial.available() <= 0) {
        delay(100);  // 等待用户输入
    }
    SET_POINT = Serial.parseInt();  // 读取用户输入的设定温度值
    Serial.print("设定温度值已设置为：");
    Serial.println(SET_POINT);

    beep(2);  // 加热器开启时，蜂鸣器响两声
    digitalWrite(HEATER_PIN, HIGH);
}

void loop() {
    int temperature = 0;
    int humidity = 0;

    int result = dht11.readTemperatureHumidity(temperature, humidity);

    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");

        if (firstReach && temperature >= SET_POINT - 10) {
            digitalWrite(HEATER_PIN, LOW);
            firstReach = false;
        } else if (!nextReach && !firstReach && temperature >= SET_POINT) {
            reachedSetPoint = true;
            Serial.println("已达到设定温度");
        }

        if ((reachedSetPoint || nextReach) && temperature < SET_POINT) {
            Serial.println("正在间断式加热");
            if (!heaterOn) {
                beep(2);
                digitalWrite(HEATER_PIN, HIGH);
                heaterOn = true;
                startTime = millis();
            } else if (millis() - startTime >= 5000) {
                beep(3);
                digitalWrite(HEATER_PIN, LOW);
                heaterOn = false;
                delay(5000);
            }
        }else if (reachedSetPoint && temperature > SET_POINT) {
            nextReach = true;
            reachedSetPoint = false;
        }else if (nextReach && temperature > SET_POINT) {
            digitalWrite(HEATER_PIN, LOW);
            // beep(3);
            Serial.println("温度超过设定值,等待冷却");
        }else if (temperature == SET_POINT){
            digitalWrite(HEATER_PIN, LOW);
            Serial.println("当前温度恒定");
        }
    }
    else {
        Serial.println(DHT11::getErrorString(result));
    }
}