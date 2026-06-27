#include <FastLED.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LED_PIN 5
#define NUM_LEDS 8
#define BUZZER_PIN 9  
#define RELAY_PIN 7
#define DHTPIN 2         // DHT11 data pin
#define DHTTYPE DHT11    // Sensor type
#define LCD_I2C_ADDRESS 0x27  // Common address (might be 0x3F)
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, 16, 2);  // 16x2 display
const int ldrpin = A1;

CRGB leds[NUM_LEDS];

DHT dht(DHTPIN, DHTTYPE);  // Initialize DHT sensor

const unsigned long mosquitoFreqs[] = {25000, 28000, 30000}; 
const int numFrequencies = sizeof(mosquitoFreqs) / sizeof(mosquitoFreqs[0]);

bool relaystate = true;

int getMosquitoActivityLevel(float temp, float humidity) {
  // Activity level 0: Low mosquito activity (long delays)
  // Activity level 1: Moderate mosquito activity (medium delays)
  // Activity level 2: High mosquito activity (short delays)
  // Ideal mosquito conditions: 20-30°C and >60% humidity
  if (temp >= 20 && temp <= 30 && humidity >= 60) {
    return 2;  // High activity
  } 
  else if ((temp >= 18 && temp < 20) || (temp > 30 && temp <= 32) || (humidity >= 50 && humidity < 60)) {
    return 1;  // Moderate activity
  }
    return 0;  // Low activity
  }

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  relaystate = true;
  
  dht.begin();  // Start DHT sensor
  Serial.begin(9600);
  Serial.println("System Initialized");
  lcd.init();         // Initialize LCD
  lcd.backlight();    // Turn on backlight
  lcd.print("Starting...");
  delay(1000);
  lcd.clear();
}

void loop() {
  // Your existing LDR and LED/buzzer logic
  int ldrValue = analogRead(ldrpin);
  Serial.print("LDR value: ");
  Serial.println(ldrValue);
  delay(500);

  if (ldrValue > 500) {
    LED();
  }
  else{
    turnOffLEDs();
    delay(1000);
    Buzzerfreq();
  }
  // NEW: Added DHT11 readings (every 2 seconds)
  readDHTSensor(); 
}

void LED() {
  leds[0] = CRGB(0, 0, 50);
  FastLED.show();
  delay(500);  
  leds[1] = CRGB(0, 0, 50);
  FastLED.show();
  delay(500);  
  leds[2] = CRGB(0, 0, 50);
  FastLED.show();
  delay(500);
  leds[3] = CRGB(0, 0, 50);
  FastLED.show();
  delay(500);
  leds[4] = CRGB(0, 0, 50);
  FastLED.show();
  delay(500);
  leds[5] = CRGB(0, 0, 50);
  FastLED.show();
  delay(500);
  leds[6] = CRGB(0, 0, 50);
  FastLED.show();
  delay(500);
  leds[7] = CRGB(0, 0, 50);
  FastLED.show();
  delay(500);
}

void turnOffLEDs() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void Buzzerfreq() {
  // Get current environmental conditions
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  int activityLevel = 0;
  if (!isnan(h) && !isnan(t)) {
    activityLevel = getMosquitoActivityLevel(t, h);
  }

  // Base delay between tones (in ms)
  unsigned long baseDelay = 10000;
  
  // Adjust delay based on mosquito activity level
  switch(activityLevel) {
    case 2:  // High activity - more frequent buzzing
      baseDelay = 2000;
      break;
    case 1:  // Moderate activity
      baseDelay = 5000;
      break;
    case 0:  // Low activity
    default:
      baseDelay = 10000;
      break;
  }

  for (int i = 0; i < numFrequencies; i++) {
    unsigned long freq = mosquitoFreqs[i];
    Serial.print("Generating frequency: ");
    Serial.print(freq);
    Serial.print(" Hz with delay ");
    Serial.print(baseDelay);
    Serial.println(" ms");
    
    tone(BUZZER_PIN, freq);
    delay(500);
    noTone(BUZZER_PIN);
    delay(baseDelay);
  }
}


void readDHTSensor() {
  static unsigned long lastReadTime = 0;
  const unsigned long readInterval = 2000;
  
  if (millis() - lastReadTime >= readInterval) {
    lastReadTime = millis();
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    if (isnan(h) || isnan(t)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DHT11 Error!");
      return;
    }

    // Update LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(t, 1);  // 1 decimal place
    lcd.write(223);   // Degree symbol
    lcd.print("C");
    
    lcd.setCursor(0, 1);
    lcd.print("H:");
    lcd.print(h, 1);  // 1 decimal place
    lcd.print("%");
  }
}