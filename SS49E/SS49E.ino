

/*

semenenko_myk@ukr.net

1. SS49E linear magnet sensor
2. Arduino nano


*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_APDS9960.h"


#define SCREEN_WIDTH 128 // ORelay display width, in pixels
#define SCREEN_HEIGHT 32 // ORelay display height, in pixels
#define SSD1306_ADDRESS 0x3C // default 0x78
#define SSD1306_ADDRESS_S 0x3D // second 0x7A

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define ORelay_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define Hall_Sensor_Pin A0
#define SENSITIVITY 2.5  // Sensitivity of the SS49E sensor in mV/G
#define MAX_MV_VOLTAGE ( 5000.0 )
#define MAX_ADC_CODE ( 1023 )
#define ZERO_ADC_MAGNET_CODE ( MAX_ADC_CODE ) / ( 2 )
#define ADC_CONSTANT ( MAX_MV_VOLTAGE / MAX_ADC_CODE )
#define ADC_MAGNET_DELTA ( 10 )

void displayPrint(void);
void ss49eGet(void);

int rawCode;
float rawValue;
float magneticFlux;

const char poleNoth = 'N';
const char poleSourth = 'S';
const char zeroField = '0';
const char* currentSymbol = &zeroField;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, ORelay_RESET);




void setup() {
	
  Serial.begin(9600);
  Wire.begin();
  
   if(!display.begin(SSD1306_SWITCHCAPVCC, SSD1306_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  else {
    Serial.println(F("SSD1306 allocation success"));
  }
  
  pinMode(Hall_Sensor_Pin,INPUT);
  
  delay(2000);
  
  display.clearDisplay();
  display.setCursor(0,0);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("SS49E magnet sensor");
  display.display();
  
  delay(5000);
}

void loop() {
  
  ss49eGet();
  
  delay(1000);
}

void ss49eGet() {
  
  rawCode = analogRead(Hall_Sensor_Pin);
  rawValue = rawCode * ADC_CONSTANT;
  magneticFlux =  rawValue / SENSITIVITY * 0.1; // mV
  
  if (rawCode > ZERO_ADC_MAGNET_CODE + ADC_MAGNET_DELTA) {
    currentSymbol = &poleNoth;
  }
  else {
    if (rawCode < ZERO_ADC_MAGNET_CODE - ADC_MAGNET_DELTA) {
      currentSymbol = &poleSourth;
    }
    else {
      currentSymbol = &zeroField;
      magneticFlux = 0;
    }
  }
   
  Serial.println("Raw Value = " + String(rawCode) + ", Voltage: " + String(rawValue) + " mV, Magnet: " + String(magneticFlux) + " mT, Pole: " + String(*currentSymbol));
  
  displayPrint();
}

void displayPrint() {
  
  display.clearDisplay();
  
  display.setCursor(0,0);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("V, mV: "+String(rawValue));

  display.setCursor(0,10);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("mT: "+String(magneticFlux));

  switch(*currentSymbol){
    case poleNoth:
        display.setCursor(0,20);  
    break;

    case poleSourth:
        display.setCursor(100,20); 
    break;

    case zeroField:
        display.setCursor(64,20);
    break;

    default:
        display.setCursor(64,20);
    break;
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print(String(*currentSymbol));

        
  display.display(); 
}
