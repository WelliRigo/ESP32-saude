#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define BLYNK_TEMPLATE_ID "TMPLiZqSmQ_x"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "ap_L0wyBSQHlRLxXaLAT-3SQNQ5UPQG8"
#define BLYNK_PRINT Serial
#include <Blynk.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_MLX90614.h>

PulseOximeter pox;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// The default I2C ports are D22 (SCL) and D21 (SDA)
// MAX30100 should be connected to the default
// GY906 should be connected to the bellow 
#define SDA_2 33
#define SCL_2 32
#define REPORTING_PERIOD_MS 1000

char auth[] = ""; // You should get Auth Token in the Blynk App.
char ssid[] = ""; // Your WiFi network name
char pass[] = ""; // Your WiFi network password

float BPM, SpO2, temperature;
uint32_t tsLastReport = 0;
 
void onBeatDetected()
{
    Serial.println("Beat Detected!");
}
 
void setup()
{
    Serial.begin(115200);
    Wire.begin();
    Wire1.begin(SDA_2, SCL_2);
    
    pinMode(19, OUTPUT);
    pinMode(26, OUTPUT);
    digitalWrite(26, HIGH);
    Blynk.begin(auth, ssid, pass);
 
    Serial.print("Initializing Pulse Oximeter..");
 
    if (!pox.begin())
    {
        Serial.println("FAILED");
        // for(;;);
    }
    else
    {
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }
 
    // The default current for the IR LED is 50mA and it could be changed by uncommenting the following line.
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    Serial.print("Initializing GY906 Termometer");

    if (!mlx.begin(0x5A, &Wire1))
    {
        Serial.println("FAILED GY906");
        // for(;;);
    }
    else
    {
         Serial.println("SUCCESS GY906");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }
 
}
 
void loop()
{

    pox.update();
    Blynk.run();
 
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    float temperature = mlx.readObjectTempC();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.print(" bpm / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");
        Serial.print(temperature);
        Serial.println(" °C");
 
        Blynk.virtualWrite(V3, BPM);
        Blynk.virtualWrite(V4, SpO2);
        Blynk.virtualWrite(V6, temperature);
 
        tsLastReport = millis();
    }
}
