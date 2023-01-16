/*==============================================
* Annoying Youtube Sub counter by Just Barran
* Like and Subscribe @justbarran 
* Youtub, Instagram, Facebook, Tictok 
* https://www.youtube.com/c/justbarran
*===============================================
*/


/* ===================================================================
* Getting Started with Seeed Studio XIAO ESP32C3
* https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/
* ===================================================================*/

/*==============================================*/
/*===============Enter info here ===============*/
/*==============================================*/

char ssid[] = "your ssid ";     
char password[] = "wifi password";
#define API_KEY "google api key" //https://developers.google.com/youtube/v3/getting-started
#define CHANNEL_ID "yt channel id you want to track"  //https://support.google.com/youtube/answer/3250431?hl=en

/*==============================================*/
/*===================== SETTINGS ===============*/
/*==============================================*/

#define BUTTON D1
#define SERVO_PIN D2
#define WS2812B_PIN D3
#define MAX_DIN D10
#define MAX_CS D9
#define MAX_CLK D8

#define SERVO_RESET 60
#define SERVO_RANGE 10
#define SERVO_Delay 100

#define NUM_LEDS    13
#define BRIGHTNESS  255 //0 - 255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define LED_INTENSITY 10 //0 - 15

#define BUTTON_WAIT 250
#define API_REQUEST_PERIOD 30000 //ms mean time between api requests, there is a daily 10k qouta so it should be more than 10 seconds. 
/*==============================================*/
/*===================== LIBRARIES ==============*/
/*==============================================*/

#include <WiFi.h>
#include <HTTPClient.h>
#include "LedController.hpp"    /* https://github.com/noah1510/LedController  v2.0.1 */
#include <ESP32Servo.h>         /*https://madhephaestus.github.io/ESP32Servo/annotated.html v0.12.0 */
#include <ArduinoJson.h>        /* https://arduinojson.org/?utm_source=meta&utm_medium=library.properties v6.20.0 */
#include <Adafruit_NeoPixel.h>  /* https://github.com/adafruit/Adafruit_NeoPixel v1.10.6 */

/*==============================================*/
/*===================== CODE BELOW ==============*/
/*==============================================*/

/*
 Now we need a LedController Variable to work with.
 We have only a single MAX72XX so the Dimensions are 1,1.
 */
LedController<1,1> lc;
Servo myservo;  // create servo object to control a servo
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(NUM_LEDS,WS2812B_PIN, NEO_GRB + NEO_KHZ800);

unsigned long pixelPrevious = 0;        // Previous Pixel Millis
unsigned long patternPrevious = 0;      // Previous Pattern Millis
int           patternCurrent = 0;       // Current Pattern Number
int           patternInterval = 5000;   // Pattern Interval (ms)
int           pixelInterval = 50;       // Pixel Interval (ms)
int           pixelQueue = 0;           // Pattern Pixel Queue
int           pixelCycle = 0;           // Pattern Pixel Cycle
uint16_t      pixelCurrent = 0;         // Pattern Current Pixel Number
uint16_t      pixelNumber = NUM_LEDS;  // Total Number of Pixels


static HTTPClient http;
static int httpError;



struct Stats {
  long viewCount;
  long subCount;
  long videoCount;
};

static Stats myStats;

static const char* rootca="-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";


bool getNrSubscriptions(HTTPClient *http, int *httpError, Stats *stats, const char* id, const char *key, const char *rootca){
  
  static int count;
  String request = "https://www.googleapis.com/youtube/v3/channels?part=statistics&id="+String(id)+"&key="+String(key);
  http->setTimeout(15000);
  http->begin(request.c_str(), rootca);
  (*httpError) = http->GET();
  
  if ((*httpError) > 0){ 
    StaticJsonDocument<1024> doc;

    String res = http->getString();
    DeserializationError error = deserializeJson(doc, res.c_str());

    if (!error) {
       stats->subCount = doc["items"][0]["statistics"]["subscriberCount"].as<int>();
       stats->viewCount = doc["items"][0]["statistics"]["viewCount"].as<int>();
       stats->videoCount = doc["items"][0]["statistics"]["videoCount"].as<int>();
       return 1;
    }    
  }
  return 0;
}

unsigned long api_lasttime;   //last time api request has been done
unsigned long subs_water_mark = 0; 
unsigned long present_subs = 0; 
unsigned long button_time_last = 0;

byte start_up_flag =  0;
byte button_state_last =  HIGH;
byte bell_ring_flag =  0;
byte leds_flag =  0;

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

void setup() {
  pinMode(BUTTON,INPUT);
  pinMode(WS2812B_PIN,OUTPUT);
  pinMode(SERVO_PIN,OUTPUT);
  pinMode(MAX_DIN,OUTPUT);
  pinMode(MAX_CS,OUTPUT);
  pinMode(MAX_CLK,OUTPUT);

  Serial.begin(115200);
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  // Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);// Standard 50hz servo
  myservo.attach(SERVO_PIN, 1000, 2000);   // attaches the servo on pin 18 to the servo object
                                         // using SG90 servo min/max of 500us and 2400us
                                         // for MG995 large servo, use 1000us and 2000us,
                                         // which are the defaults, so this line could be
                                         // "myservo.attach(servoPin);"

  //Here a new LedController object is created without hardware SPI.
  lc=LedController<1,1>(MAX_DIN,MAX_CLK,MAX_CS);
  /* Set the brightness to a medium values 0 - 15 */
  lc.setIntensity(LED_INTENSITY);
  /* and clear the display */
  lc.clearMatrix();
  PrintNumber(888888U);
  /* Explicitly set the ESP32 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  delay(1000);  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  byte waiting = 0;
  while (WiFi.status() != WL_CONNECTED){
    waiting ++;
    if(waiting>5)
    {
      waiting = 0;
    }
    PrintLoading(5-waiting);
    Serial.print(".");
    delay(500);
  }  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");

  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)

  patternCurrent=1;
  leds_flag=1;

  PrintHello();
  PrintWorld();
}


void loop() {

  unsigned long currentMillis = millis();                     //  Update current time

  byte button_state = digitalRead(BUTTON);
  if((button_state == LOW) && (button_state_last == HIGH) && ((currentMillis - button_time_last) > BUTTON_WAIT))
  {    
    button_time_last = currentMillis;
    if(bell_ring_flag == 1)
    {
      subs_water_mark = myStats.subCount;
      bell_ring_flag = 0;
    }
    else 
    {
      patternCurrent++;                                         //  Advance to next pattern
      if(patternCurrent > 5)
      {
        leds_flag = 0;
        patternCurrent = 0;
        strip.clear();
        strip.show();
        lc.clearMatrix();
      }  
      else
      {
        leds_flag = 1;
        strip.setBrightness(BRIGHTNESS);
        lc.setIntensity(LED_INTENSITY);
        PrintNumber(myStats.subCount);
      }    
    }
  }  
  button_state_last = button_state;

  if (WiFi.status() == WL_CONNECTED) {
    if ((millis() - api_lasttime) > API_REQUEST_PERIOD) {      
      if(getNrSubscriptions(&http, &httpError, &myStats, CHANNEL_ID, API_KEY, rootca)==1)
      {
        Serial.println("---------Stats---------");
        Serial.println("Just Barran");
        Serial.print("Subscriber Count: ");
        Serial.println(myStats.subCount);
        Serial.print("View Count: ");
        Serial.println(myStats.viewCount);
        Serial.print("Video Count: ");
        Serial.println(myStats.videoCount);
        Serial.println("------------------------");   
        if(leds_flag>0)
        {
          PrintNumber(myStats.subCount);
        }           
        if(start_up_flag == 0)
        {
          subs_water_mark = myStats.subCount;
          start_up_flag = 1;
        }
      }
      else
      {
        Serial.println("---------No Stats---------");
        PrintError(1);
      }
      api_lasttime = millis();
    }
  }

  if(bell_ring_flag == 1)
  {
    ringBell();
  }
  else if(myStats.subCount > subs_water_mark )
  {
    PrintNumber(myStats.subCount);
    bell_ring_flag = 1;
    leds_flag = 1;
    patternCurrent = 1;
  }

  if(leds_flag == 1)
  {
    if(currentMillis - pixelPrevious >= pixelInterval) 
    {        //  Check for expired time
      pixelPrevious = currentMillis;                            //  Run current frame
      switch (patternCurrent) {
      case 1:
        rainbow(15); // Flowing rainbow cycle along the whole strip
        break;     
      case 2:
        colorWipe(strip.Color(0, 0, 255), 50); // Blue
        break;
      case 3:
        colorWipe(strip.Color(0, 255, 0), 50); // Green
        break;  
      case 4:
        colorWipe(strip.Color(255, 0, 0), 50); // Red
      break;  
      case 5:
        /* turn off leds */
        strip.clear();
        strip.show();
      break;       
      case 6:
        /* Add another pathern */
      break;    
      default:
       /* turn off everything */
        strip.clear();
        strip.show();
        lc.clearMatrix();
      break;
    }
    }
  }
}

void PrintHello()
{
  lc.setChar(0,5,'J',false);
  lc.setChar(0,4,'U',false);
  lc.setChar(0,3,'S',false);
  lc.setChar(0,2,'T',false);
  lc.setChar(0,1,' ',false);
  lc.setChar(0,0,' ',false);
  delay(1000);
  lc.clearMatrix();
}
void PrintWorld()
{
  lc.setChar(0,5,'B',false);
  lc.setChar(0,4,'A',false);
  lc.setChar(0,3,'R',false);
  lc.setChar(0,2,'R',false);
  lc.setChar(0,1,'A',false);
  lc.setChar(0,0,'N',false);
  delay(1000);
  lc.clearMatrix();
}
void PrintLoading(byte digit)
{
  lc.clearMatrix();
  lc.setRow(0,digit,B00000001);  
}

void PrintError(byte code)
{
  lc.setChar(0,5,'e',false);
  lc.setChar(0,4,'r',false);
  lc.setChar(0,3,'r',false);
  lc.setChar(0,2,' ',false);
  lc.setChar(0,1,' ',false);
  lc.setDigit(0,0,code,false);
  delay(1000);
  lc.clearMatrix();
}

void PrintNumber(long subs)
{
  byte ones =     (byte)(subs%10); 
  byte tens =     (byte)((subs/10)%10); 
  byte hunds =    (byte)((subs/100)%10);  
  byte thou =     (byte)((subs/1000)%10); 
  byte tenthou =  (byte)((subs/10000)%10);  
  byte hundthou = (byte)((subs/10000)%10); 
  lc.clearMatrix();
  lc.setDigit(0,0,ones,false);
  if(subs >= 10)
  {
    lc.setDigit(0,1,tens,false);
  }
  if(subs >= 100)
  {
    lc.setDigit(0,2,hunds,false);
  }
  if(subs >= 1000)
  {
    lc.setDigit(0,3,thou,false);
  }
  if(subs >= 10000)
  {
    lc.setDigit(0,4,tenthou,false);
  }
  if(subs >= 100000)
  {
    lc.setDigit(0,5,hundthou,false);
  }  
}


void ringBell(){
  myservo.write(SERVO_RESET+SERVO_RANGE); 
  delay(SERVO_Delay);
  myservo.write(SERVO_RESET); 
  delay(SERVO_Delay);
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(uint8_t wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   
  for(uint16_t i=0; i < pixelNumber; i++) {
    strip.setPixelColor(i, Wheel((i + pixelCycle) & 255)); //  Update delay time  
  }
  strip.show();                             //  Update strip to match
  pixelCycle++;                             //  Advance current cycle
  if(pixelCycle >= 256)
    pixelCycle = 0;                         //  Loop the cycle back to the begining
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   //  Update delay time
  strip.setPixelColor(pixelCurrent, color); //  Set pixel's color (in RAM)
  strip.show();                             //  Update strip to match
  pixelCurrent++;                           //  Advance current pixel
  if(pixelCurrent >= pixelNumber)           //  Loop the pattern from the first LED
    pixelCurrent = 0;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

