Annoying Youtube Sub counter by Just Barran
* Like and Subscribe @justbarran 
* Youtub, Instagram, Facebook, Tictok 
* https://www.youtube.com/c/justbarran

Youtube video: https://youtu.be/Xyod39A879Y

Some of the code was take from @CallousCoder on YT

So credit goes to him as well, Please check out and support his channel.

Video: https://www.youtube.com/watch?v=56j4wTS5Bmo

Full Instructables: 

Code V2: https://github.com/justbarran/YoutubeSubCounter

3D Files: https://www.thingiverse.com/thing:5786711

I am using Arduino 2.03 at the time of this Instructables:

Getting Started with Seeed Studio XIAO ESP32C3: 

https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/

You would need to add the ESP32 board to the Arduino IDE if you don't have it already. 

Needed Arduino Libraries that need to be added via Arduino library manager: 

https://github.com/noah1510/LedController  v2.0.1

https://madhephaestus.github.io/ESP32Servo/annotated.html v0.12.0 

https://arduinojson.org/?utm_source=meta&utm_medium=library.properties v6.20.0 

 https://github.com/adafruit/Adafruit_NeoPixel v1.10.6 



The only change to the code needed is:

char ssid[] = "your ssid ";     
char password[] = "wifi password";
#define API_KEY "google api key" //https://developers.google.com/youtube/v3/getting-started
#define CHANNEL_ID "yt channel id you want to track"  //https://support.google.com/youtube/answer/3250431?hl=en


To get your youtube Id check: https://support.google.com/youtube/answer/3250431?hl=en

To get a google API key Follow the steps: https://developers.google.com/youtube/v3/getting-started

OPTIONALLY you can adjust setting like the brightness of the LED strip via

#define BRIGHTNESS  255 //0 - 255

The value here 255 is the brightest setting and 0 is off.

and the 7-segment displays 

#define LED_INTENSITY 10 //0 - 15

The value here is 15 is the brightest setting and 0 is off. 

Additional lighting patterns could be added and colour settings in the LEDs state machines and updated counts.
