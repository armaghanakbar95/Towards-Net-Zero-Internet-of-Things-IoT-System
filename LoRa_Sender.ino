#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>

// Sensor Pins
#define DHT_Pin 4   // DHT11 data pin

#define LDR_Pin 2   // LDR digital input pin
DHTesp dht;

//dht.setup(DHT_Pin, DHTesp::DHT11);

// LoRa pins
#define SS    18    // LoRa SS
#define RST   14    // LoRa reset
#define DIO0  26    // LoRa DIO0 (IRQ)

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//Creating global function Screen display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int counter = 1;



// Setup function
void setup() {
  // Serial begin for debugging
  Serial.begin(9600);

  while (!Serial);
  // Temperature Sensor Setup
  dht.setup(DHT_Pin, DHTesp::DHT11);

  // OLED setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }


  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("LoRa Sender");
  display.display();

  // LoRa init
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(868E6)) {  // Eu region
    Serial.println("LoRa init failed!");
    display.println("LoRa Failed");
    display.display();
    while (true);
  }


  //LoRa Configuration
  LoRa.enableCrc();   //adding CRC to packet
  LoRa.setSpreadingFactor(7);   //Spreading factor setting
  LoRa.setSignalBandwidth(125000);  // Bandwidth Setting
  
  Serial.println("LoRa Sender Ready");
  display.println("LoRa Ready");
  display.display();
}

//function to get temperature values
float Get_Temperature(){
  float temp = dht.getTemperature();  // Read temperature in Celsius
    if (isnan(temp)) {
      Serial.println("Failed to read Temperature from DHT11!");
      return -1000;  // Indicate failure
    }
  return temp;
}


//function to get humidity values
float Get_Humidity(){
  float humidity = dht.getHumidity();  //Read Humidity
    if (isnan(humidity)) {
      Serial.println("Failed to read Humidity from DHT11!");
      return -1000;  // Indicate failure
    }
  return humidity;
}


// Function to get LDR sensor Values
int Get_LightLevel(){

  int LDR_Value = digitalRead(LDR_Pin);
  return LDR_Value;
}

void loop() {
  //String message = "Hello #" + String(counter++);

  float Temperature = Get_Temperature();
  float Humidity = Get_Humidity();
  int Light = Get_LightLevel();

  //Message parts

  String Message_T = "NBT: " + String(Temperature, 1);
  String Message_H = "NBH: " + String(Humidity, 1);
  String Message_L = "NBL: " + String(Light);
  String Counter = "Counter: " + String(counter++);
  
  //final String Formation
  String Message = Message_T + " " + Message_H + " " + Message_L;

  // Send the loRa Packet
  LoRa.beginPacket();
  LoRa.print(Message);
  LoRa.endPacket();

  // Print to Serial
  Serial.print("Sent: ");
  Serial.println(Message);

  // Show on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("LoRa Sender Ready");    
  display.println("Sent:");
  //display.setTextSize(1);
  display.setCursor(0, 20);
  display.println(Message_T + " C");
  display.println(Message_H + " %");

  if (Light == LOW){
    display.println(Message_L + " - Bright");
  }
  else{
    display.println(Message_L + " - Dark");
  }
  
  display.display();

  delay(5000); // Wait 5 seconds
}
