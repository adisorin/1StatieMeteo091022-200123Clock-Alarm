/////////// STAMET23 STICK USB ////////////////Ultima modificare 01-06-23 Buton alarmă ///////////////////////////
#include <Wire.h>
#include <SPI.h>
#include <MCUFRIEND_kbv.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


tmElements_t tm;



// 
#define DHTPIN A7// what digital pin we're connected to
#define ONE_WIRE_BUS A8
#define Arduino Genuino Mega or Mega 2560
#define math
#define RawValue
#define RawHigh
#define RawLow
#define ReferenceHigh
#define ReferenceLow
#define RawRange
#define ReferenceRange
#define CorrectedValue
#define sensors DallasTemperature
#define DHTTYPE DHT11// DHT 11
//#define DHTTYPE DHT22// DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21// DHT 21 (AM2301)

// **** Define Name of Color
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0XFFE0
//#define PINK   0xE5F7

// **** Meter colour schemes
#define WHITE   0xFFFF
#define CLEAR   0X0000
#define TURQUOISE   0x17F2
#define CHARCOAL    0x4A69
#define RED2RED 0
#define RED8RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5


#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930

#define BAR_MINX 180
#define BAR_MINY 125
#define BAR_HEIGHT 250
#define BAR_WIDTH 30 

///////////////////////////////////////////////////////////////////////////////////////////////////////////

////
int currentPage = 0;  // 0 = Home screen, 1 = Temperature Control parameters, 2 = Water control parameters
extern const uint16_t SoilMoisture48[];

//Initialize width and height variable of the TFT Touch Screen
uint16_t Width;
uint16_t Height;

////
MCUFRIEND_kbv tft;// TFT ID=0x9486..87 , ID=0x7523
uint32_t runTime = -99999;       // time for next update
int reading = 0;                 // Value to be displayed
int d = 0;                       // Variable used for the sinewave test waveform
boolean alert = 0;
int32_t ramp = 1;
int tesmod = 0;

//////Sensor DS18B20;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHTPIN, DHTTYPE);

int buzzer = 22; //buzzer to arduino pin 22

const int buttonPin = 23;    // the number of the pushbutton pin
const int buttonPinAL = 31;
int buttonState = 0;         // variable for reading the pushbutton status
int buttonStateAL = 0;
int RELAY_RH = 24;
int RELAY_TH = 25;
int LDRpin = A9;
char lightString [4];
String str;
int lightIntensity = 0;

float ah;//absolute humidity = water vapor density in g/m*3
float rh;// relative humidity in %
float temp;//temperature in celsius
float tempC;//temperature ds18b20
float td; //dew point in celsius




//char TempCelcius;
double dewPoint(double celsius, double humidity) {
double RATIO = 373.15 / (273.15 + celsius);
double RHS = -7.90298 * (RATIO - 1);
//double temp = 0;
RHS += 5.02808 * log10(RATIO);
RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO ))) - 1);
RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1);
RHS += log10(1013.246);
double VP = pow(10, RHS - 3) * humidity;
double T = log(VP / 0.61078);// temp var
return (241.88 * T) / (17.558 - T);
}

int contor = 0;////Contor buzzer rh==95

/////////////////////////////////// PRIM SCREEN LOAD ON SOUND BIP ////////////////////////////////

void loadingScreen(){  // Function that draw a loading screen on the TFT Touch Screen with a loading bar and the version of the software
//
tft.setTextSize(4);
tft.setTextColor(TURQUOISE);
tft.setCursor(120,80);
tft.print("Loading...");
for(int i = 0; i < 250;i++){//Loding bar
tft.fillRect(BAR_MINY - 10, BAR_MINX, i, 10, BLUE);
delay(3);
}
tone(buzzer, 3000); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
noTone(buzzer);// Stop sound...
delay(25);
tone(buzzer, 3000); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
noTone(buzzer);// Stop sound...
delay(10);
tone(buzzer, 3200); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
noTone(buzzer);     // Stop sound...
delay(1000);        // ...for 1sec
delay(10);
tone(buzzer, 4200); // Send 1KHz sound signal...
delay(300);        // ...for 1 sec
noTone(buzzer);     // Stop sound...
delay(600);        // ...for 1sec
tone(buzzer, 5200); // Send 1KHz sound signal...
delay(300);
noTone(buzzer);     // Stop sound...
delay(300);
tone(buzzer, 3200); // Send 1KHz sound signal...
delay(300);        // ...for 1 sec
noTone(buzzer);     // Stop sound...
delay(20);        // ...for 1sec
for (int i = 0; i < 250; i++){  // Loading bar

delay(1);
}

}
//////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {


// initialize the pushbutton pin as an input:
pinMode(buttonPin, INPUT);
pinMode(buttonPinAL, INPUT);
// initialize the LED pin as an output:
pinMode(RELAY_RH, OUTPUT);
pinMode(RELAY_TH, OUTPUT);
pinMode(buzzer, OUTPUT);
Serial.begin(9600);
//    Serial.println("DHTxx test!");

dht.begin();
sensors.begin();
/////////////////////////////////////////////////////////////////////////////
while (!Serial);// wait for serial
//    Serial.println("DS1307RTC Read Data Time");
//    Serial.println("----------------------------------------------------");
/////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////

sensors.setResolution(12);
//  float RawHigh0 = 99.6;
//  float RawLow0 = 0.5;
//  float ReferenceHigh0 = 99.9;
//  float ReferenceLow0 = 0;
//  float RawRange0 = 99.6 - 0.5;
//  float ReferenceRange0 = 99.9 - 0;

tft.reset();//Reset LCD to begin
tft.begin(0x9487);//0x9486..87
(void)tft.readID(); // citește ID-ul, dar ignoră valoarea

tft.setRotation(3);// Set Rotation at 0 degress (default)
tft.fillScreen(CLEAR);//Set Background Color with BLACK
//    tft.fillScreen(5);
////////////////////////////
// delay(10);

loadingScreen();  // Call the function Loading Screen 
Width = tft.width();
Height = tft.height();

///////////////////////
tft.fillScreen(CLEAR);


/////////Linii de incadrare-------

tft.fillRect(0,317,477,4,BLUE);//INCEPUT,V,H,GROSIME,CULOARE LINIA DE JOS 7
//tft.fillRect(0,280,477,2,YELLOW);//INCEPUT,V,H,GROSIME,CULOARE LINIA 6
tft.fillRect(0,245,477,2,BLUE);//INCEPUT,V,H,GROSIME,CULOARE LINIA 5
tft.fillRect(0,210,477,2,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
tft.fillRect(36,162,477,2,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
tft.fillRect(395,162,2,50,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
tft.fillRect(0,162,84,2,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
tft.fillRect(0,9,477,4,BLUE);//INCEPUT,V,H,GROSIME,CULOARE FINAL LINIA DE SUS 1
tft.fillRect(290,210,2,320,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
tft.fillRect(476,10,4,320,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
tft.fillRect(0,10,4,320,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
tft.fillRect(190,210,2,320,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
tft.fillRect(84,162,2,50,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
tft.fillRect(255,10,4,155,BLUE);//INCEPUT,V,H,GROSIME,CULOARE
    
///////////////////////////////////////

tft.setCursor(35, 20);//H, V
tft.setTextSize(3);
tft.setTextColor(GREEN,CLEAR);
tft.print("Temperature");

tft.setCursor(240, 50);//H, V
tft.setTextSize(2);
tft.setTextColor(GREEN,CLEAR);
tft.print("C");

tft.setCursor(178, 250);//H, V
tft.setTextSize(2);
tft.setTextColor(WHITE,CLEAR);
tft.print("C");

tft.setCursor(120, 0);//H, V
tft.setTextSize(1);
tft.setTextColor(MAGENTA,CLEAR);
tft.print("WEATHER STATION                                     AdiSorin");

tft.setCursor(410, 190);//H, V
tft.setTextSize(2);
tft.setTextColor(CYAN,CLEAR);
tft.print("75~81");

tft.setCursor(409, 170);//H, V
tft.setTextSize(2);
tft.setTextColor(CYAN,CLEAR);
tft.print("Set:");

tft.setCursor(298, 20);// H, V
tft.setTextSize(3);
tft.setTextColor(CYAN,CLEAR);
tft.print("Humidity");

tft.setCursor(200, 290);
tft.setTextSize(2);
tft.setTextColor(CYAN,CLEAR);
tft.print("ml / m3");

tft.setCursor(13, 190);// H, V
tft.setTextSize(2);
tft.setTextColor(GREEN,CLEAR);
tft.print("19~21");

tft.setCursor(15, 170);// H, V
tft.setTextSize(2);
tft.setTextColor(GREEN,CLEAR);
tft.print("Set:");


//////////// light intensity//////////

tft.setCursor(305, 218);// H, V
tft.setTextSize(3);
tft.setTextColor(YELLOW,CLEAR);
tft.print("Ext.Light");

////////////////////////////////////////    
tft.setCursor(27, 218);// H, V
tft.setTextSize(3);
tft.setTextColor(WHITE,CLEAR);
tft.print("Ext.Temp");


tft.setCursor(225, 218);// H, V
tft.setTextSize(3);
tft.setTextColor(BLUE,CLEAR);
tft.print("AH");

/////////////////////////////



}
////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
float rh = dht.readHumidity(true);
float temp = dht.readTemperature(true);
rh = dht.readHumidity();
temp = dht.readTemperature();
sensors.requestTemperatures();
tempC = sensors.getTempCByIndex(0);
str = String(lightIntensity);//+"%";
str.toCharArray(lightString,4);



////////////////////////////////CLOCK/////////CLOCK//////////CLOCK//////////////////////////////////////

if (RTC.read(tm)) {
tft.setCursor(95, 170);
tft.setTextSize(5);
tft.setTextColor(WHITE,CLEAR);

if (tm.Hour < 10) {
tft.print('0');
}
tft.print(tm.Hour);
tft.print(':');

if (tm.Minute < 10) {
tft.print('0');
}
tft.print(tm.Minute);
tft.print(':');

if (tm.Second < 10) {
tft.print('0');
}
tft.print(tm.Second);
}

////////////////Day//////Day//////Day/////////
if (RTC.read(tm)) {
tft.setCursor(367, 170);
tft.setTextSize(2);
tft.setTextColor(GREEN,CLEAR);
if (tm.Day < 10) {
tft.print('0');
}
tft.print( tm.Day);
}
///////////////Month//////Month//////Month///////
if (RTC.read(tm)) {
tft.setCursor(367, 190);
tft.setTextSize(2);
tft.setTextColor(GREEN,CLEAR);
if (tm.Month < 10) {
tft.print('0');
}
tft.print( tm.Month);
}
//////////////////////////////////////////////////
tft.setCursor(350, 170);
tft.setTextSize(2);
tft.setTextColor(GREEN,CLEAR);
tft.print('Z');

tft.setCursor(350, 190);
tft.setTextSize(2);
tft.setTextColor(GREEN,CLEAR);
tft.print('L');

///////BIP////BIP////BIP////BIP////BIP////BIP////BIP////BIP////BIP////BIP////BIP////BIP////

if (tm.Hour >= 6 && tm.Hour <= 23 && tm.Minute == 0 && tm.Second <= 1) {
tone(buzzer, 3000); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
noTone(buzzer);// Stop sound...
delay(25);
tone(buzzer, 3000); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
noTone(buzzer);// Stop sound...
delay(10);
tone(buzzer, 3200); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
noTone(buzzer);     // Stop sound...
delay(1200);        // ...for 1sec
}

///////////////////// CLOCK///ALARM///CLOCK///ALARM/////////////11111111111//////////////////////////////////////


if (tm.Hour == 4 && tm.Minute == 0 && tm.Second <= 10 && buttonStateAL == HIGH ) {

tft.setCursor(95, 170);
tft.setTextSize(5);
tft.setTextColor(RED,CLEAR);

if (tm.Hour < 10) {
tft.print('0');
}
tft.print(tm.Hour);
tft.print(':');

if (tm.Minute < 10) {
tft.print('0');
}
tft.print(tm.Minute);
tft.print(':');

if (tm.Second < 10) {
tft.print('0');
}
tft.print(tm.Second);

tone(buzzer, 1000); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
tone(buzzer, 700); // Send 1KHz sound signal...
delay(1000);        // ...for 1 sec
tone(buzzer, 100); // Send 1KHz sound signal...
delay(1000);        // ...for 1 sec
tone(buzzer, 75); // Send 1KHz sound signal...
delay(1000);  
tone(buzzer, 700); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
tone(buzzer, 1000); // Send 1KHz sound signal...
delay(1000);        // ...for 1 sec
noTone(buzzer);     // Stop sound...
delay(1000);        // ...for 1sec

}
///////////////////// CLOCK///ALARM///CLOCK///ALARM///////////222222222222////////////////////////////////////////

if (tm.Hour == 6 && tm.Minute == 45 && tm.Second <= 10 && buttonStateAL == HIGH ) {

tft.setCursor(95, 170);
tft.setTextSize(5);
tft.setTextColor(RED,CLEAR);

if (tm.Hour < 10) {
tft.print('0');
}
tft.print(tm.Hour);
tft.print(':');

if (tm.Minute < 10) {
tft.print('0');
}
tft.print(tm.Minute);
tft.print(':');

if ( tm.Second < 10) {
tft.print('0');
}
tft.print(tm.Second);

tone(buzzer, 1000); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
tone(buzzer, 700); // Send 1KHz sound signal...
delay(1000);        // ...for 1 sec
tone(buzzer, 100); // Send 1KHz sound signal...
delay(1000);        // ...for 1 sec
tone(buzzer, 75); // Send 1KHz sound signal...
delay(1000);  
tone(buzzer, 700); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
tone(buzzer, 1000); // Send 1KHz sound signal...
delay(1000);        // ...for 1 sec
noTone(buzzer);     // Stop sound...
delay(1000);        // ...for 1sec
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








////////////////////////////////CLOCK/////////CLOCK//////////CLOCK//////////////////////////////////////2222222222222222






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//Indicator stare CLOCK ALARM
buttonStateAL = digitalRead(buttonPinAL);
if (buttonStateAL == HIGH) {
tft.setCursor(245, 170);
tft.setTextSize(5);
tft.setTextColor(RED,CLEAR);
tft.print(':');
tft.setCursor(155, 170);
tft.setTextSize(5);
tft.setTextColor(RED,CLEAR);
tft.print(':');
tesmod = 0;
delay(1);
}





////////////////////CALIBRARE ”SENSOR  DS18B20” //////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
float RawValue0 = tempC;                                        //                     //
// float RawHigh0 = 99.6;                                       //                     //
// float RawLow0 = 0.5;                                         //                     //
// float ReferenceHigh0 = 99.9;                                 // Calibrare in doua   //
// float ReferenceLow0 = 0;                                     //       puncte        //
// float RawRange0 = 99.6 - 0.5;                                //                     //
// float ReferenceRange0 = 99.9 - 0;;                           //                     //
tempC = (((RawValue0 - 1.9) * 99.9) / 99.6) + 0;                //                     //
//////////////////////////////////////////////////////////////////////////////////////////
//  tempC = tempC * 0.5;  // setResolution 9 bit                                        //
//  tempC = tempC * 0.25;  // setResolution 10 bit                                      //
//  tempC = tempC * 0.125;  // setResolution 11 bit                                     //
//  tempC = tempC *(- 0.6625);  // setResolution 12 bit factory default state.          //
//////////////////////////////////////////////////////////////////////////////////////////

if (isnan(rh)) {
//  Serial.println("Failed rh. to read Humidity DHT sensor!");
return;
}

if (isnan(temp)) {
//  Serial.println("Failed temp. to read Humidity DHT sensor!");
return;
}

if (isnan(tempC)) {
//  Serial.println("Failed to read DS18B20 ExTemp!");
return;

}
//  delay(1);
  
ah = (6.112 * pow(2.71828, ((17.67 * temp) / (243.5 + temp))) * rh * 2.1674) / (273.15 + temp);

//  delay(1);
////////////////////////////////////Humidity from DHT///////////////////////////////////////////

if(rh<0) {
tft.setCursor(280, 70);// Print corect the Humidity High ”!”
tft.setTextSize(10);
tft.setTextColor(RED,CLEAR);
tft.print(' ',0);
tesmod = 0;
return;
}

if(rh<=49) {
tft.setCursor(280, 70);// Print corect the Humidity LOW ”!”
tft.setTextSize(10);
tft.setTextColor(WHITE,CLEAR);
tft.print(rh,0);
tft.print('!',0);
tesmod = 0;
}

if(rh>=50 && rh<=95) {
tft.setCursor(280, 70);// Print corect the Humidity LOW ”!”
tft.setTextSize(10);
tft.setTextColor(CYAN,CLEAR);
tft.print(rh,0);
tft.print("%");
tesmod = 0;
}

if(rh>=75 && rh<=95) {
tft.setCursor(280, 70);// Print > the Humidity Medium
tft.setTextSize(10);
tft.setTextColor(BLUE,CLEAR);
tft.print(rh,0);
tft.print("%");
tesmod = 0;
}

if (rh >= 90 && rh <= 95) {
tft.setCursor(280, 70);// Print corect the Humidity High ”!”
tft.setTextSize(10);
tft.setTextColor(RED,CLEAR);
tft.print(rh,0);
tft.print("!");
tesmod = 0;
}

if(rh>95) {
tft.setCursor(280, 70);// Print corect the Humidity High ”!”
tft.setTextSize(10);
tft.setTextColor(CLEAR,CLEAR);
tft.print(" ");
tesmod = 0;
return;
}

if(rh==95 and tm.Hour > 10  and tm.Hour < 21) {
if (contor < 1){
for(int i=0; i<1; i++){
tone(buzzer, 100); // Send 1KHz sound signal...
delay(1000);        // ...for 1 sec
tone(buzzer, 75); // Send 1KHz sound signal...
delay(1000);  
tone(buzzer, 50); // Send 1KHz sound signal...
delay(100);        // ...for 1 sec
noTone(buzzer);     // Stop sound...
delay(1000);        // ...for 1sec
}
contor += 1;
  
tft.setCursor(457, 20);// Print corect the Humidity High ”!”
tft.setTextSize(2);
tft.setTextColor(WHITE,CLEAR);
tft.print(contor);
tesmod = 0;
//  delay(4000);
}
}
else {
contor = 0;
}
/////////////////////////////////////////////PIN-OUT Humidity DHT ////////////////////////////////

// turn LED on:
if (rh>81 && rh<=95) {
digitalWrite(RELAY_RH,0);//OUT ON
//  Serial.println("pin24 ON");
}

//Button off RELAY RH

buttonState = digitalRead(buttonPin);
if (buttonState == HIGH && rh<81) {
Serial.println("pin24 ON-RH-B7");
// turn LED on:
digitalWrite(RELAY_RH,1);//OUT OFF
}

// turn LED off:
if (rh<=75 && rh>=1) {
digitalWrite(RELAY_RH,1);//OUT OFF
//  Serial.println("pin24 OFF");
}

//Indicator stare OUT RH

if (bitRead(PORTA,2) == LOW) {//PIN24 PORTA,2 bitRead (ledPin)
//  Serial.println("pin24 ON-RH-B7");
tft.setCursor(275, 43);
tft.setTextSize(2);
tft.setTextColor(RED,CLEAR);
tft.print("*              *");
tesmod = 0;
delay(1);
}

if (bitRead(PORTA,2) == HIGH) {//PIN24 PORTA,2 bitRead (ledPin)
//Serial.println("pin24 OFF-RH-B7");
tft.setCursor(275, 43);
tft.setTextSize(2);
tft.setTextColor(CYAN,CLEAR);
tft.print("*              *");
tesmod = 0;
delay(1);
}
/////////////////////////////////////Temperature from DHT//////////////////////////////////////////

if (temp<=19.0 && temp>=1) {
tft.setCursor(10, 70);// Print temperature from DHT sensor Low ”!”
tft.setTextSize(10);
tft.setTextColor(WHITE,CLEAR);
tft.print(temp,1);
tesmod = 0;
}

if (temp>=1.0 && temp<=50) {
tft.setCursor(10, 70);// Print temperature from DHT High ”!”
tft.setTextSize(10);
tft.setTextColor(GREEN,CLEAR);
tft.print(temp,1);
tesmod = 0;
}

if (temp>=32.0 && temp<=50) {
tft.setCursor(10, 70);// Print temperature from DHT High ”!”
tft.setTextSize(10);
tft.setTextColor(RED,CLEAR);
tft.print(temp,1);
tesmod = 0;
}


////////////////////////////////////////Indicator stare CLOCK puncte (: :) / Secunde//////////////////////////////

//Indicator stare CLOCK puncte (:) / Secunde
buttonStateAL = digitalRead(buttonPinAL);
if (buttonStateAL == LOW) {
tft.setCursor(245, 170);
tft.setTextSize(5);
tft.setTextColor(BLUE,CLEAR);
tft.print(':');
tft.setCursor(155, 170);
tft.setTextSize(5);
tft.setTextColor(BLUE,CLEAR);
tft.print(':');
tesmod = 0;
delay(1);
}

/////////////////////////////////////////PIN-OUT Temperature DHT///////////////////////////////////

if (temp<=19.0 && tempC<=15) {  //Intern AND Extern
digitalWrite(RELAY_TH,0);//OUT ON
//  Serial.println("pin25 ON-TH");
delay(1);
}

if (temp>=21.0) {
digitalWrite(RELAY_TH,1);//OUT OFF
//  Serial.println("pin25 OFF-TH");
delay(1);
}

//Indicator stare OUT TH

if (bitRead(PORTA,3) == LOW) {//PIN25 PORTA,3 bitRead (ledPin)
//  Serial.println("pin25 ON-TH-B6");
tft.setCursor(153, 70);//Indicator ridm date
tft.setTextSize(3);
tft.setTextColor(RED,CLEAR);
tft.print("*");
tesmod = 0;
delay(1);
}

if (bitRead(PORTA,3) == HIGH) {//PIN25 PORTA,3 bitRead (ledPin)
//  Serial.println("pin25 OFF-TH-B6");
tft.setCursor(153, 70);//Indicator ridm date
tft.setTextSize(3);
tft.setTextColor(WHITE,CLEAR);
tft.print("*");
tesmod = 0;
delay(1);
}

/////////////////////////////////////////////Ext.Temp./////////////////////////////////////////////
if (tempC<33) {
tft.setCursor(27, 218);// H, V
tft.setTextSize(3);
tft.setTextColor(WHITE,CLEAR);
tft.print("Ext.Temp");
}

if (tempC>33) {
tft.setCursor(27, 218);// H, V
tft.setTextSize(3);
tft.setTextColor(RED,CLEAR);
tft.print("Ext.Temp");
}

if (tempC<5) {
tft.setCursor(27, 218);// H, V
tft.setTextSize(3);
tft.setTextColor(BLUE,CLEAR);
tft.print("Ext.Temp");
}

if (tempC<=5 && tempC>-9) {
tft.setCursor(6, 260);// Print temperature from DS18B20 ExTemp.
tft.setTextSize(6);
tft.setTextColor(CYAN,CLEAR);
tft.print(tempC,1);
tft.print('*');
tesmod = 0;
}

if (tempC>=1 && tempC<10) {
tft.setCursor(6, 260);// Print temperature from DS18B20 Ext.Temp.
tft.setTextSize(6);
tft.setTextColor(WHITE,CLEAR);
tft.print(tempC,1);
tft.print(" ");
tesmod = 0;
}

if (tempC>=6 && tempC<=60) {
tft.setCursor(6, 260);// Print temperature from DS18B20 Ext.Temp.
tft.setTextSize(6);
tft.setTextColor(WHITE,CLEAR);
tft.print(tempC,1);
//  tft.print(' ');
tesmod = 0;
}

if (tempC>=32 && tempC<=35) {
tft.setCursor(6, 260);// Print temperature from DS18B20 Ext.Temp.
tft.setTextSize(6);
tft.setTextColor(RED,CLEAR);
tft.print(tempC,1);
tft.print(' ');
tesmod = 0;
}

if (tempC>=36 && tempC<=60) {
tft.setCursor(6, 260);// Print temperature from DS18B20 Ext.Temp.
tft.setTextSize(6);
tft.setTextColor(RED,CLEAR);
tft.print(tempC,1);
tft.print('!');
tesmod = 0;
}
//////////////////////////////////Ext.light intensity///////////////////////////////////////////////


lightIntensity = analogRead(LDRpin);



if (lightIntensity>2 && tm.Hour >=6 && tm.Hour <= 22) {  
lightIntensity = map(lightIntensity, 925, 50, 0, 98);//analog conversion lightIntensity 500...790,50,0,98
}

if (lightIntensity<1) {
lightIntensity = map(lightIntensity, 0, 0, 0, 0);// ptint lightIntensity ZERO ABSOLUT
}

////////////////////////////Humidity of cable and light sensor compensation//////////////////////////

if (lightIntensity > 2 && tm.Hour <= 6) {
lightIntensity = map(lightIntensity, 0, 0, 0, 0);// ptint lightIntensity ZERO ABSOLUT
}

if (lightIntensity>2 && tm.Hour >= 21 && tm.Hour <= 23) {
lightIntensity = map(lightIntensity, 0, 0, 0, 0);// ptint lightIntensity ZERO ABSOLUT
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
if (lightIntensity<=9) {
tft.setCursor(315, 260);// Print the light intensity in percentage
tft.setTextSize(6);
tft.setTextColor(YELLOW,CLEAR);
tft.print(lightIntensity);
tft.print("% ");
// tft.print(' ');
tesmod = 0;
}

if (lightIntensity>=10) {
tft.setCursor(315, 260);// Print the light intensity in percentage
tft.setTextSize(6);
tft.setTextColor(YELLOW,CLEAR);
tft.print(lightIntensity);
tft.print("% ");
tesmod = 4;
}

if (lightIntensity>=100) {
tft.setCursor(315, 260);// Print the light intensity in percentage
tft.setTextSize(6);
tft.setTextColor(YELLOW,CLEAR);
tft.print(lightIntensity);
tft.print("%");
//  tft.print(' ');
tesmod = 0;
}

/////////////////////////////////////RITM DE DATE///////////////////////////////////////////////////
//  tft.setCursor(153, 70);//Indicator ridm date
//  tft.setTextSize(3);
//  tft.setTextColor(WHITE,CLEAR);
//  tft.print("*");
//  tesmod = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////
if (tempC<=5 && tempC>-9) {
tft.setCursor(150, 260);// Print FULG DE NEA from DS18B20 ExTemp.
tft.setTextSize(6);
tft.setTextColor(CYAN,CLEAR);
tft.print('*');
tesmod = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

if (ah>=5.0 && ah<=35) {
tft.setCursor(205, 260);//AH 
tft.setTextSize(3);
tft.setTextColor(CYAN,CLEAR);
tft.print(ah,1);
tesmod = 0;
}

if (ah<=12.0 && ah<=35) {
tft.setCursor(205, 260);//AH 
tft.setTextSize(3);
tft.setTextColor(WHITE,CLEAR);
tft.print(ah,1);
tesmod = 0;
}

if (ah>=18.5 && ah<=35) {
tft.setCursor(205, 260);//AH 
tft.setTextSize(3);
tft.setTextColor(BLUE,CLEAR);
tft.print(ah,1);
tesmod = 0;
}
if (ah>=20.5 && ah<=35) {
tft.setCursor(205, 260);//AH 
tft.setTextSize(3);
tft.setTextColor(RED,CLEAR);
tft.print(ah,1);
tesmod = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////
if (tempC<=5 && tempC>-9) {
tft.setCursor(150, 260);// Print FULG DE NEA from DS18B20 ExTemp.
tft.setTextSize(6);
tft.setTextColor(CYAN,CLEAR);
tft.print(' ');
tesmod = 0;
}


/////////////////////////////////////Serial.print/////////////////////////////////////////////////////////
    
  //Serial.print(tm.Hour);
  //Serial.print(":");
  //Serial.print(tm.Minute);
  //Serial.println(" ");
  //Serial.print("Temperature:   ");
  //Serial.print(temp, 1);
  //Serial.println(" °C ");
  //Serial.print("Humidity:      ");
  //Serial.print(rh, 0);
  //Serial.println("% ");
  //Serial.print("LightRezistor: ");
  //Serial.print(lightString);
  //Serial.println("");
  //Serial.print("DS18B20 Temp: ");
  //Serial.print(tempC, 2);
  //Serial.println(" °C ");
  //Serial.println();

}
//end program...
