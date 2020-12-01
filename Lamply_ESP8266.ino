#include <Coordinates.h>
#include <Adafruit_NeoPixel.h>
//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>


class LedColor {
  public:
    int red = 0;
    int green = 0;
    int blue = 0;
    LedColor(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    void setRGB(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    int R() {
      return red;
    }
    int G() {
      return green;
    }
    int B() {
      return blue;
    }
};




#define pinDIN 2
#define pocetLED 10
Adafruit_NeoPixel rgbWS = Adafruit_NeoPixel(pocetLED, pinDIN, NEO_GRB + NEO_KHZ800);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "0OmH5xkOvFwPe7Izf_qVXOaMjP0mpilg";

unsigned long previousTime = millis();
int red = 0;
int green = 0;
int blue = 0;
int fps = 0;

bool afterboot = true;
bool gettingSleep = false;
bool gameplay = true;
bool controll = false;
bool bubbles = false;
bool bubblesStart = true;
bool bubblesOn = true;
bool lightUp = false;


int cykle = 0;
int pointer = 0;
int n = 0;
int genArray = 99;
int  bubblesCount = 0;
int bubblesNum [10];

int randR = 0;
int randG = 0;
int randB = 0;

int bubbleTime = 50;

LedColor gameplayColor(196, 167, 51);

LedColor controllColor(196, 98, 0); //(255, 0, 102);

LedColor IntroA(70, 37, 0);
LedColor IntroB(18, 10, 0);
LedColor IntroC(18, 10, 0);
LedColor IntroD(0, 0, 0);
LedColor IntroE(0, 0, 0); //(36, 19, 0);
LedColor IntroF(0, 0, 0); //(70, 37, 0);

Coordinates point = Coordinates();

LedColor LEDAnimationGame [] = {IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB, IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB,
                                IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB, IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB,
                                IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB, IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB,
                                IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB, IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB,
                                IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB, IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB
                               };

LedColor LEDAnimation [] = {IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB, IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB};
LedColor TouchAnimation [] = {IntroA, IntroB, IntroC, IntroD, IntroE, IntroF, IntroE, IntroD, IntroC, IntroB};

int v0 = 1;
int v1 = 1;
int v2 = 0;
int v3 = 0;

WiFiManager wifiManager;

BLYNK_WRITE(V0) // brightness
{
  v0 = param.asInt();
}
BLYNK_WRITE(V1) // V5 is the number of Virtual Pin
{
  //v1 = param.asInt();
  switch (param.asInt()) {
    case 1: {
        // Serial.println("1");
        lightUp = false;
        controll = false;
        gameplay = true;
        bubbles = false;
        break;
      }
    case 2: {
        // Serial.println("2");
        lightUp = false;
        controll = true;
        gameplay = false;
        bubbles = false;
        break;
      }
    case 3: {
        // Serial.println("3");
        lightUp = false;
        controll = false;
        gameplay = false;
        bubbles = true;
        bubblesStart = true;
        break;
      }
    case 4: {
        controll = false;
        gameplay = false;
        bubbles = false;
        bubblesStart = false;
        lightUp = true;
        break;
      }
    case 5: {
        wifiManager.resetSettings();
        break;
      }
  }
}
BLYNK_WRITE(V2) // V5 is the number of Virtual Pin
{
  v2 = param.asInt();
}
BLYNK_WRITE(V3) // V5 is the number of Virtual Pin
{
  v3 = param.asInt();
}

void setup()
{
  EEPROM.begin(512);
  Serial.begin(9600);
  WiFiManager wifiManager;
  //wifiManager.resetSettings();    //Uncomment this to wipe WiFi settings from EEPROM on boot.  Comment out and recompile/upload after 1 boot cycle.
  wifiManager.autoConnect("Blynk lamp");
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output, I like blinkies.
  Blynk.begin(auth, WiFi.SSID().c_str(), WiFi.psk().c_str());
  rgbWS.begin();
  for (int i = 0; i < 10; i++) {
    nastavRGB(0, 0, 0, i);
  }
  for (int i = 0; i < 100; i++) {
    LEDAnimationGame[i].setRGB(0, 0, 0);
  }
}




void nextFrame() {
  for (int i = 0; i < 10; i++) {
    int p = i + pointer;
    if (p > 19) p = p - 20;
    nastavRGB(LEDAnimation[p].R(), LEDAnimation[p].G(), LEDAnimation[p].B(), i);
    //Serial.println(p);
  }
  pointer++;
  if (pointer > 19) {
    pointer = 0;
  }
}

void giveMeGameplay() {
  //Serial.println("SPOUSTIM GAMEPLAY");
  while (genArray > 5) {
    int r = random (5, 50);
    if (r % 2 == 0) r += 1;
    if ((genArray - r) > 5) genArray = genArray - r;
    else {
      r = genArray - 1;
      if (r % 2 == 0) r += 1;
      genArray = 0;
    }
    /* Serial.print("r: ");
      Serial.println(r);
      Serial.print("genArray: ");
      Serial.println(genArray);*/
    LEDAnimationGame[genArray + 1 + (r - 1) / 2] = gameplayColor;
    int part = (r - 1) / 2 + 1;
    for (int i = 0; i < (r - 1) / 2; i++) {
      LEDAnimationGame[genArray + 1 + i ].setRGB(gameplayColor.R() / part * (i + 1), gameplayColor.G() / part * (i + 1), gameplayColor.B() / part * (i + 1)); //setRGB(gameplayColor.R()*(i+1)/((r-1)/2+1), gameplayColor.G()*(i+1)/((r-1)/2), gameplayColor.B()*(i+1)/((r-1)/2));
      LEDAnimationGame[genArray + r  - i].setRGB(gameplayColor.R() / part * (i + 1), gameplayColor.G() / part * (i + 1), gameplayColor.B() / part * (i + 1)); //setRGB(gameplayColor.R()*(i+1)/((r-1)/2), gameplayColor.G()*(i+1)/((r-1)/2), gameplayColor.B()*(i+1)/((r-1)/2));
    }

  }
  // Serial.println("Pole animaci:");
  for (int i = 0; i < 100; i++) {
    /*   Serial.print(99 - i);
       Serial.print(".  ");
       Serial.print(LEDAnimationGame[i].R());
       Serial.print("   ");
       Serial.print(LEDAnimationGame[i].G());
       Serial.print("   ");
       Serial.println(LEDAnimationGame[i].B());*/
  }
  //delay(10000);
  genArray = 100;
}

void game() {
  if (cykle == 0) {
    giveMeGameplay();
    cykle = 1;
    pointer = 0;
  }
  if (pointer == 10 && n < 10) {
    for (int i = 0; i < 10; i++) {
      LEDAnimation[i] = LEDAnimationGame[i + n * 10]; //nastavRGB(LEDAnimationGame[i+n*10].R(),LEDAnimationGame[i+n*10].G(),LEDAnimationGame[i+n*10].B(), i);
      /*Serial.print("Nastavuju LEDAnimation: ");
        Serial.print(i);
        Serial.print(" na LEDAnimationGame: ");
        Serial.println(i + n * 10);*/
    }
    if (gameplay) n++;
  }
  if ((pointer == 0 || pointer == 20) && n < 10) {
    for (int i = 0; i < 10; i++) {
      LEDAnimation[i + 10] = LEDAnimationGame[i + n * 10]; //nastavRGB(LEDAnimationGame[i+n*10].R(),LEDAnimationGame[i+n*10].G(),LEDAnimationGame[i+n*10].B(), i);
      /*Serial.print("Nastavuju LEDAnimation: ");
        Serial.print(i + 10);
        Serial.print(" na LEDAnimationGame: ");
        Serial.println(i + n * 10);*/
    }
    if (gameplay) n++;
  }
  if (n > 9) n = 0;
  if (cykle > 10) {
    n = 0;
    cykle = 0;
    gettingSleep = true;
  }
}


void intro () {
  //rgbWS.setBrightness(255);
  //nextFrame();
  if (cykle >= 5) {
    afterboot = false;
    gettingSleep = true;
    cykle = 0;
  }


}

void sleep () {
  for (int i = 0; i < 20; i++) {
    int r = LEDAnimation[i].R() / 1.3;
    int g = LEDAnimation[i].G() / 1.3;
    int b = LEDAnimation[i].B() / 1.3;
    LEDAnimation[i].setRGB(r, g, b);
  }
  if (cykle >= 2) {
    //Serial.println("USINAM");
    gettingSleep = false;
    cykle = 0;
    //gameplay = true;
    for (int i = 0; i < 20; i++) {
      LEDAnimation[i].setRGB(0, 0, 0);
    }
  }

}


void tick() {
  fps++;
  if (fps == 25) {
    fps = 1;
    cykle++;
  }
  nextFrame();
}

void mixMe() {
  for (int i = 0; i < 10; i++) {
    int a = random(10);
    int b = random(10);
    int pom = bubblesNum[a];
    bubblesNum[a] = bubblesNum[b];
    bubblesNum[b] = pom;
  }
}
void doBubble() {
  if (bubblesOn) {
    if (TouchAnimation[bubblesNum[bubblesCount]].R() < randR && TouchAnimation[bubblesNum[bubblesCount]].G() < randG && TouchAnimation[bubblesNum[bubblesCount]].B() < randB) {
      float newR = round(TouchAnimation[bubblesNum[bubblesCount]].R() + randR / 24.0);
      float newG = round(TouchAnimation[bubblesNum[bubblesCount]].G() + randG / 24.0);
      float newB = round(TouchAnimation[bubblesNum[bubblesCount]].B() + randB / 24.0);
      if (newR > randR || newG > randG || newB > randB){newR = randR; newG = randG; newB = randB;}
      TouchAnimation[bubblesNum[bubblesCount]].setRGB((int)newR, (int)newG, (int)newB);
    }
    else if (bubblesCount >= 9) {
      bubblesCount = 0;
      bubblesOn = false;
    }
    else {
      bubblesCount++;
    }
  }
  if (bubblesOn == false) {
    if (TouchAnimation[bubblesNum[bubblesCount]].R() > 0 && TouchAnimation[bubblesNum[bubblesCount]].G() > 0 && TouchAnimation[bubblesNum[bubblesCount]].B() > 0) {
      float newR = TouchAnimation[bubblesNum[bubblesCount]].R() - randR / 24;
      float newG = TouchAnimation[bubblesNum[bubblesCount]].G() - randG / 24;
      float newB = TouchAnimation[bubblesNum[bubblesCount]].B() - randB / 24;
      if (newR <= 0 || newG <= 0 || newB <= 0){newR = 0; newG = 0; newB = 0;}
      TouchAnimation[bubblesNum[bubblesCount]].setRGB((int)newR, (int)newG, (int)newB);
    }
    else if (bubblesCount >= 9) {
      bubblesCount = 0;
      bubblesStart = true;
    }
    else {
      bubblesCount++;
    }
  }
}

void loop()
{
  unsigned long currentTime = millis();
  Blynk.run();  // Commented out for a short time.
  if (currentTime - previousTime > 41 * 2 && controll == false) { //41
    if (afterboot) {
      intro();
      tick();
    }
    else if (gettingSleep) {
      sleep();
      tick();
    }
    else if (gameplay) {
      game();
      tick();
    }
    else if (bubbles) {
      if (bubblesStart) {
        for (int i = 0; i < 10; i++) {
          TouchAnimation[i].setRGB(0, 0, 0);
          nastavRGB(TouchAnimation[i].R(), TouchAnimation[i].G(), TouchAnimation[i].B(), i);
          bubblesNum[i] = i;
        }
        //Serial.println("Bubbles START");
        randR = random(24, 255);
        randG = random(24, 255);
        randB = random(24, 255);
        for (int i = 0; i < 10; i++) {
          mixMe();
        }
        bubblesOn = true;
        bubblesStart = false;
      }
      else {
        doBubble();
        for (int i = 0; i < 10; i++) {
          nastavRGB(TouchAnimation[i].R(), TouchAnimation[i].G(), TouchAnimation[i].B(), i);
         /* Serial.print(i);
          Serial.print(". :");
          Serial.print(TouchAnimation[i].R());
          Serial.print(" ");
          Serial.print(TouchAnimation[i].G());
          Serial.print(" ");
          Serial.println(TouchAnimation[i].B());*/
        }
      }
    }
    else if (lightUp) {
      for (int i = 0; i < 10; i++) {
        nastavRGB(255, 255, 255, i);
      }
    }
    previousTime = currentTime;
  }




  if (controll) { //&& gameplay == false && bubbles == false) {
    int vir3 = v3 - 500;
    int vir2 = v2 - 500;
    point.fromCartesian(vir2, vir3);
    double c = point.getR() / 500;
    double degree = point.getAngle() * 57296 / 1000;
    touch(degree, c * c * c);
    /*Serial.print(" c: ");
      Serial.print(c);
      Serial.print(" degree: ");
      Serial.println(degree* 57296 / 1000);*/
  }

}
void nastavRGB (int r, int g, int b, int cislo) {
  // vytvoření proměnné pro ukládání barev
  uint32_t barva;
  // načtení barvy do proměnné
  barva = rgbWS.Color(r / v0, g / v0, b / v0);
  // nastavení barvy pro danou LED diodu,
  // číslo má pořadí od nuly
  rgbWS.setPixelColor(cislo, barva);
  // aktualizace barev na všech modulech
  rgbWS.show();
}

void touch(double degree, double c) {
  for (int i = 0; i < 10; i++) {
    int r = TouchAnimation[i].R() / 1.01;
    int g = TouchAnimation[i].G() / 1.011;
    int b = TouchAnimation[i].B() / 1.011;
    TouchAnimation[i].setRGB(r, g, b);
    if (degree > 0 && degree < 36)  TouchAnimation[5] = controllColor;
    if (degree > 35 && degree < 72) TouchAnimation[6] = controllColor;
    if (degree > 71 && degree < 108) TouchAnimation[7] = controllColor;
    if (degree > 107 && degree < 144) TouchAnimation[8] = controllColor;
    if (degree > 143 && degree < 180) TouchAnimation[9] = controllColor;
    if (degree > 179 && degree < 216) TouchAnimation[0] = controllColor;
    if (degree > 215 && degree < 252) TouchAnimation[1] = controllColor;
    if (degree > 251 && degree < 288) TouchAnimation[2] = controllColor;
    if (degree > 287 && degree < 324) TouchAnimation[3] = controllColor;
    if (degree > 323 && degree < 361) TouchAnimation[4] = controllColor;

    nastavRGB(TouchAnimation[i].R()*c, TouchAnimation[i].G()*c, TouchAnimation[i].B()*c, i);
  }


}
