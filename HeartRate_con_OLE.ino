/*
  Optical Heart Rate Detection (PBA Algorithm) using the MAX30105 Breakout
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 2nd, 2016
  https://github.com/sparkfun/MAX30105_Breakout

  This is a demo to show the reading of heart rate or beats per minute (BPM) using
  a Penpheral Beat Amplitude (PBA) algorithm.

  It is best to attach the sensor to your finger using a rubber band or other tightening
  device. Humans are generally bad at applying constant pressure to a thing. When you
  press your finger against the sensor it varies enough to cause the blood in your
  finger to flow differently which causes the sensor readings to go wonky.

  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/

#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1

MAX30105 particleSensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

//Pantalla
int TextSize=1; //tamaño del texto a imprimir
bool TextInverted=false; //false=Texto normal, true=Texto invertido
String Texto; //Texto a imprimir
int TextX; //Ubicacion en X del texto
int TextY; //Ubicacion en Y del texto
bool ClearScreen=false; //Limpiar pantalla
String Linea1;
String Linea2;



void setup()
{
    //Iniciand la pantalla
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
  Serial.print("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }
  //Serial.print("Pantalla");
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display(); //Requerido por Adafruit Copywrite
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  //Impresion de prueba
  display.clearDisplay();
  PrintText(1,4,3,"Iniciando...");  
  //display.drawRect(1, 1, 126,31, WHITE);
  display.display();
  delay(2000);
 
  //Serial.begin(115200);
  //Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  //Serial.println("Place your index finger on the sensor with steady pressure.");
  display.clearDisplay();
  PrintText(1,4,3,"Coloque el dedo...");  
  //display.drawRect(1, 1, 126,31, WHITE);
  display.display();
  delay(2000);
  
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop()
{
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

/*
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
  */

  if(irValue <50000)
  {
    Linea1="BPM = ";
    Linea2="BPMAvb= " ;
  }
  else
  {
    Linea1="BPM   = " + String (int(beatsPerMinute));
    Linea2="BPMAvb= " + String(beatAvg);
  }

  
  display.clearDisplay();
  PrintText(1,4,3,Linea1);  
  PrintText(1,4,18, Linea2);
  //display.drawRect(1, 1, 126,31, WHITE);
  display.display();


  
  
}

////FUNCIONES//////////////////////////

//Imprimir Text en pantalla
void PrintText(int Size,int X, int Y,String Texto)
{
  display.setTextSize(Size);
  display.setTextColor(WHITE);
  display.setCursor(X,Y);
  display.println(Texto);
  //if(d){
    //display.display();
  
}
