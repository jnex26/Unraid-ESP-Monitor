#include <Adafruit_GFX.h>      // Core graphics library
#include <Adafruit_ST7796S.h>  // Hardware-specific library for ST7789
//#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <String.h>
#include <WiFiClientSecure.h>
#include <UnraidGraph.h>
#include <creds.h> 


// Large S3 Thingy
//#define TFT_MOSI 22
#define TFT_GRND     //black
#define TFT_3V3      // red
#define TFT_SCLK 14  //Grey
#define TFT_MOSI 13  //Purple
#define TFT_RST 8    //Blue
#define TFT_DC 16    // Green
#define TFT_CS 15    //Yellow
#define TFT_BL 6     //Backlight

//GND Black
//VCC White
//SCL Grey
//SDA Purple
//RST BLue
//DC Green
//CS Yellow
//BL Orange
//SDA-0 Red

// GND
// 3V
// GP13 - Grey
// GP12 - Purple
// GP11 - BLue
// GP10 - GReen
// GP9 - Yellow
// GP8 -  Orange
//


//#define TFT_MISO 19
//#define TFT_MOSI 12 //
//#define TFT_SCLK 13 // (SCK)
//#define TFT_CS   9  // Chip select control pin
//#define TFT_DC    10  // Data Command control pin
//#define TFT_RST   11  // Reset pin (could connect to RST pin)
//#define TFT_BL 8 // LED




DynamicJsonDocument LastKnown(10240);

unsigned int rgbColour[3];
int rgbinc = 1;
int rgbdec = 0;
int part = 0;
int positionx = 490;
int width = 480;
int height = 320;
int linepos = 0;

unsigned long startMillis;
unsigned long currentMillis;
unsigned long lastMillisLogo;
unsigned long lastMillisMemory;
unsigned long lastMillisArray;
unsigned long lastMillisParity;
unsigned long lastMillisDisks;
unsigned long lastMillisCDisks;
unsigned long lastMillisContainer;

struct SpiRamAllocator {
  void* allocate(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};
using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;
SpiRamJsonDocument statusTracker(8096);

// constructor for data object named tft

Adafruit_ST7796S tft = Adafruit_ST7796S(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

UnraidGraph unraid ;

void setup(void) {
  unraid.debug(false);
  unraid.begin(unraidapi,unraidapiey);
  rgbColour[0] = 255;
  rgbColour[1] = 0;
  rgbColour[2] = 0;
  startMillis = millis();
  Serial.begin(115200);
  Serial.print(F("Hello! ST77xx TFT Test"));

  pinMode(TFT_BL, OUTPUT);                // TTGO T-Display enable Backlight pin 4
  digitalWrite(TFT_BL, HIGH);             // T-Display turn on Backlight
  tft.init(height, width, 0, 0, ST7796S_BGR);  // Initialize ST7789 240x135
  tft.setTextSize(2);
  Serial.println(F("Initialized"));
  tft.setSPISpeed(16000000);
  tft.invertDisplay(true);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  Serial.print("Connecting to :");
  //  Serial.println(ssid[defaultwifi]);
  if (psramInit()) {
    Serial.println("PSRAM is available");
  } else {
    Serial.println("PSRAM is not available");
    return;  // Exit if PSRAM is not available
  }
  Serial.println("1");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    Serial.print(".");
    delay(1000);
  }
  clearScreen();
  Serial.println("2");
  Serial.println("Connected");
  Serial.println();
  delay(1000);
  clearScreen();

  //JsonDocument tempdoc;

  //tempdoc = unraid.getUnraidMemory();

  //tft.println("");
  //String testdocument;
  //serializeJson(tempdoc,Serial);
  //tft.println(testdocument);
 
  //tft.println("12345678901234567890123456789012345678901234567890");
  drawDiskBox();
  drawCacheBox();
  systemStatBox();
  titleScreen("Unraid Monitor");
  
}


void loop() {

  Smartdelay();
}


void titleScreen(String title) {
  int center=title.length();
  center=(center/2)*12;
  int screencenter=width/2;
  int textcenter=screencenter-center;
  tft.setCursor(textcenter, 0);
  tft.println(title);
}

void drawMemoryGraph() {
  JsonDocument tempdoc;
  tempdoc = unraid.getUnraidMemory();
   String max = tempdoc["data"]["info"]["memory"]["max"];
   String used = tempdoc["data"]["info"]["memory"]["used"];
   float _max = (max.toFloat())/1024;
   float _used = (used.toFloat())/1024;
   if (_max == 0 || _used == 0 ) {
      Serial.println("no writing or calc");
   } else {
   int currentPercentage = (_used/_max)*100;
   drawTitlePercentBox(0,270,width,30,"Memory Usage",currentPercentage,returnCol(255,255,0),2);
}
}

void drawArrayGraph() {
  JsonDocument tempdoc;
  tempdoc = unraid.getUnraidArrayCapacity();
   //serializeJsonPretty(tempdoc["data"],Serial);
   String max = tempdoc["data"]["array"]["capacity"]["kilobytes"]["total"];
   String used = tempdoc["data"]["array"]["capacity"]["kilobytes"]["free"];
   float _max = (max.toFloat())/1024;
   float _used = (used.toFloat())/1024;
   _used = (_max - _used); //I've got the Free need to get the used for calcualte to work.
   if (_max == 0 || _used == 0 ) {
      Serial.println("no writing or calc");
   } else {
   int currentPercentage = (_used/_max)*100;
   
   drawTitlePercentBox(0,220,width,30,"Array Usage ",currentPercentage,returnCol(0,0,255),2);
 //  tft.fillRoundRect(0,279,barsize,29,8,returnCol(0,0,255));
}
}

void drawParity() {
  JsonDocument tempdoc;
  short parityid[2];
  short titlebox=3;
  tempdoc = unraid.getUnraidArrayParity();
  // serializeJsonPretty(tempdoc["data"]["array"]["parities"],Serial);
   for (int i=0; i<=1; i++) {
       String status = tempdoc["data"]["array"]["parities"][i]["status"];
       if (!(status == "null")) {
       titlebox = drawTitleBoxString(titlebox+5,47,String("Parity "+String(i)),status,returnCol(255,0,255),false);
       }
   }
}

void drawArrayDisks() {

  JsonDocument tempdoc;
  short parityid[2];
  short titlebox=0;
  int capture=1 ;
  //unraid.debug(true);
  tempdoc = unraid.getUnraidArrayDisksCapacity();
  //unraid.debug(false);
 // serializeJsonPretty(tempdoc,Serial);

 
   for (int i=0; i<=10; i++) {
       String status = tempdoc["data"]["array"]["disks"][i]["status"];
       //serializeJsonPretty(tempdoc["data"],Serial);
       if (!(status == "null")) {
       // serializeJsonPretty(tempdoc["data"]["array"]["parities"],Serial);
        float _Size = tempdoc["data"]["array"]["disks"][i]["fsSize"];
        float _Used = tempdoc["data"]["array"]["disks"][i]["fsUsed"];
        String _id = tempdoc["data"]["array"]["disks"][i]["idx"];
        int percentused = (_Used/_Size)*100;
    //    Serial.println(_id);
    //    Serial.println(_Size);
     //   Serial.println(_Used);
      //  Serial.println(percentused);
      //  Serial.println("-----------------");
        uint16_t colour;
        if (status=="DISK_OK") {
          colour = returnCol(0,255,0);
        } else {
          colour = returnCol(255,0,0);
        }
        capture = drawVertPercentBox(capture+5,100,20,100,percentused,_id,colour);
       //titlebox = drawTitleBoxString(titlebox+5,30,String("Parity "+String(i)),status,returnCol(255,0,255),false);
       }
   }
}

//************************************************************

void drawCacheDisks() {

  JsonDocument tempdoc;
  short parityid[2];
  short titlebox=0;
  int capture=40 ;
  //unraid.debug(true);
  tempdoc = unraid.getUnraidCacheCapacity();
  //unraid.debug(false);
 // serializeJsonPretty(tempdoc,Serial);

 
   for (int i=0; i<=1; i++) {
       String status = tempdoc["data"]["array"]["caches"][i]["status"];
       //serializeJsonPretty(tempdoc["data"],Serial);
       if (status != "null" && tempdoc["data"]["array"]["caches"][i]["fsSize"] != "null") {
       // serializeJsonPretty(tempdoc["data"]["array"]["parities"],Serial);
        float _Size = tempdoc["data"]["array"]["caches"][i]["fsSize"];
        float _Used = tempdoc["data"]["array"]["caches"][i]["fsUsed"];
        String _id = tempdoc["data"]["array"]["caches"][i]["idx"];
        String _name = tempdoc["data"]["array"]["caches"][i]["name"];
        int percentused = (_Used/_Size)*100;
    //    Serial.println(_id);
    //    Serial.println(_Size);
     //   Serial.println(_Used);
      //  Serial.println(percentused);
      //  Serial.println("-----------------");
        uint16_t colour;
        if (status=="DISK_OK") {
          colour = returnCol(0,255,0);
        } else {
          colour = returnCol(255,0,0);
        }
       // capture = drawVertPercentBox(capture+5,100,20,100,percentused,_id,colour);
        tft.setTextSize(1);
        drawTitlePercentBox(270,capture,205,20,String(_name),percentused,RGBPercent(percentused),1);
        capture=capture+34;
        tft.setTextSize(2);
       //titlebox = drawTitleBoxString(titlebox+5,30,String("Parity "+String(i)),status,returnCol(255,0,255),false);
       }
   }
}
//**************************************************************
void Smartdelay() {
  int Logo = 10;
  int Memory = 10000;
  int Array = 40000;
  int Parity = 60000;
  int Disks = 60000;
  int CDisks = 60000;
  int Containers = 60000;
  if (millis() - lastMillisLogo >= Logo) {
    tft.drawFastHLine(linepos, 16, 3, returnCol(rgbColour[0], rgbColour[1], rgbColour[2]));
    linepos++;
    if (linepos > 480) { linepos=0;}
    //drawPartHex(459, 25, 25, returnCol(rgbColour[0], rgbColour[1], rgbColour[2]), part);
    part++;
    if (part == 7) { part = 1; }
    // increment
    rgbColour[rgbinc]++;
    rgbColour[rgbdec]--;
    if (rgbColour[rgbinc] >= 255) {
      rgbinc++;
      rgbdec++;
      if (rgbinc == 3) { rgbinc = 0; }
      if (rgbdec == 3) { rgbdec = 0; }
    }
    lastMillisLogo = millis();
  }
  if (millis() - lastMillisMemory >= Memory) {
    drawMemoryGraph();
    lastMillisMemory = millis();
  }
  if (millis() - lastMillisArray >= Array) {
    drawArrayGraph();
    lastMillisArray = millis();
  }
  if (millis() - lastMillisParity >= Parity) {
    drawParity();
    lastMillisParity = millis();
  }
  if (millis() - lastMillisDisks >= Disks) {
    drawArrayDisks();
    lastMillisDisks = millis();
  }
  if (millis() - lastMillisCDisks >= CDisks) {
    drawCacheDisks();
    lastMillisCDisks = millis();
  }
  if (millis() - lastMillisContainer >= Containers) {
    drawContainers();
    lastMillisContainer = millis();
  }

}

uint16_t returnCol(int r, int g, int b) {
  return tft.color565(r, g, b);
}

uint16_t RGBPercent(short percent) {
  int maxRGBval=255;
  int r=0;
  int g=255;
  return returnCol(((maxRGBval/100)*percent),g-((maxRGBval/100)*percent),0);
}


void drawDiskBox() {
    tft.drawRoundRect(0, 35, 255, 180, 8, returnCol(255,255,255));
    tft.drawFastHLine(5, 35, 120, returnCol(0, 0, 0));
  tft.setCursor(10, 26);
  tft.print("Array Monitor");
}

void drawCacheBox() {
    tft.drawRoundRect(260, 35, 220, 80, 8, returnCol(255,255,255));
    tft.drawFastHLine(265, 35, 120, returnCol(0, 0, 0));
  tft.setCursor(271, 26);
  tft.print("Cache Monitor");
}

void systemStatBox() {
    tft.drawRoundRect(260, 125, 220, 90, 8, returnCol(255,255,255));
    tft.drawFastHLine(265, 125, 120, returnCol(0, 0, 0));
    tft.setCursor(271, 116);
    tft.print("Containers");
}


void drawContainers() { 
  JsonDocument tempdoc;
  unraid.debug(true);
  int endBoundryx=480;
  int endBoundryy=215;
  int startBoundryx=273;
  int startBoundryy=140;
  bool oddEven=false;
  int c;
  uint16_t colour;
  tempdoc = unraid.getUnraidContainers();
  String doc ;
  int r=10;
  int b=0;
  int x=startBoundryx+r;
  int y=startBoundryy+r;
  int hexcount = tempdoc["data"]["docker"]["containers"].size();
  for (int i=0;i<=tempdoc["data"]["docker"]["containers"].size();i++) {
    Serial.println(i);
    serializeJson(tempdoc["data"]["docker"]["containers"][i],Serial);
    if (String(tempdoc["data"]["docker"]["containers"][i]["state"]) == "RUNNING") {
      colour = returnCol(0,255,0);
    } else {
       colour = returnCol(255,0,0);
    }
    drawHex(x,y,r,colour);
    x=x+(r*2)+5+b;
    if (x>=endBoundryx) {
      if (oddEven) {
        x=startBoundryx+r;
        y=y+(r*2)+2;
        oddEven=false;
      } else {
        x=startBoundryx-1;
        y=y+(r*2)+2;
        oddEven=true;
      }
    }

  }
 // serializeJson(tempdoc["data"]["docker"]["containers"],doc);
   // for(String __id: doc){
  //  c++;
  //  Serial.println(c);
  //  Serial.println(__id);
  //  }
  unraid.debug(false);
}


void drawTitlePercentBox(int x, int y, int xSize, int ySize, String heading,int percentage, uint16_t colour,int fontsize) {
  int cutbox = heading.length() * 12;
  int base;
  int padding;
  String head2 = heading;
  //Serial.println(heading.length());
  //tft.drawRect(x,y+7,boxsize,y+38,colour);
  float onePercent = (xSize-4)/100;
  int barsize = percentage*onePercent;
  //Serial.println(barsize);
  head2.replace(" ","_");
  statusTracker[head2]["previous"]=statusTracker[head2]["current"];
  statusTracker[head2]["current"]=percentage;
  if (statusTracker[head2]["current"] < statusTracker[head2]["previous"] ) {
    int erase = statusTracker[head2]["previous"];
    int erasebarsize = percentage*onePercent;
    tft.fillRoundRect(x+2,y+ 18,erasebarsize, ySize-4,8 ,returnCol(0,0,0));
  }
  tft.drawRoundRect(x, y + 16, xSize, ySize, 8, colour);

  tft.fillRoundRect(x+2,y+ 18,barsize, ySize-4,8 ,colour);
 // tft.drawFastHLine(x + 5, 16, cutbox, returnCol(0, 0, 0));
  if (fontsize == 1) {
     padding = 7;
  } else {
     padding = -2;
  }
  tft.setCursor(x+13 ,y+padding);
  tft.print(heading);
}

int drawVertPercentBox(int x, int y, int xSize, int ySize,int percentage,String heading, uint16_t colour) {
  int cutbox = heading.length() * 12;
 // Serial.println("---------------");
 
  float ySizeFloat = ySize -4;
//  Serial.println(ySizeFloat);
  float onePercent = (ySizeFloat)/100;
  int barsize = (percentage*onePercent);
//   Serial.println(barsize);
  if  (barsize <= 0) {
    barsize=5;
  }
  int yStart=y+10;
  int maxRGBval=255;
  int r=0;
  int g=255;
  String head2 = heading;
  head2.replace(" ","_");
  statusTracker[head2]["previous"]=statusTracker[head2]["current"];
  statusTracker[head2]["current"]=percentage;
  if (statusTracker[head2]["current"] < statusTracker[head2]["previous"] ) {
    int erase = statusTracker[head2]["previous"];
    int erasebarsize = percentage*onePercent;
  //  tft.fillRoundRect(x+2,y+ 18,xSize-4, erasebarsize,8 ,returnCol(0,0,0));
  }
  tft.drawRoundRect(x, yStart, xSize, ySize, 8, colour);
  int topOfBar = (yStart+2)+((ySizeFloat-(barsize)));
  tft.fillRoundRect(x+2,topOfBar,(xSize-4),barsize,3 ,returnCol(((maxRGBval/100)*percentage),g-((maxRGBval/100)*percentage),0));
 // tft.fillRoundRect(100, 135, 30,  65, 8, returnCol(255,0,255));
 
 // drawVertPercentBox(100,100,30,100,65,returnCol(0,255,255));
 // tft.drawFastHLine(x + 5, 16, cutbox, returnCol(0, 0, 0));
  if (heading.length() == 1) {
    tft.setCursor(x+8,y);
  } else {
    tft.setCursor(x+4,y);
  }
  tft.setTextSize(1);
  tft.print(heading);
  tft.setTextSize(2);
  return (x+xSize);
}



long stringToLong(String s)
{
    char arr[12];
    s.toCharArray(arr, sizeof(arr));
    return atol(arr);
}

int drawTitleBox(int x, int y, String heading, float value, uint16_t colour, String unit, bool buffer) {
  int cutbox = heading.length() * 12;
  int base;
  //Serial.println(heading.length());
  int boxsize = 55;
  if (heading.length() >= 3) {
    boxsize = (heading.length() * 12) + 20;
  }
  if (buffer) { boxsize = boxsize + 20; }
  //tft.drawRect(x,y+7,boxsize,y+38,colour);
  tft.drawRoundRect(x, y + 7, boxsize, y + 38, 8, colour);
  tft.drawFastHLine(x + 5, 7, cutbox, returnCol(0, 0, 0));
  tft.setCursor(x + 10, y);
  tft.print(heading);
  tft.setCursor(x + 7, y + 20);

  if ((value - int(value)) > 0) {
    // needs to be a float
    //Serial.println(value);
    tft.print(String(value));
  } else {
    //Serial.println(String(int(value)));
    // turn it in to a int
    tft.print(int(value));
  }
  if (unit != "") {
    tft.print(unit);
  }
  if (value < 100) {
    tft.print(" ");
  }
  return (x + boxsize);
}


int drawTitleBoxString(int x, int y, String heading, String value, uint16_t colour, bool buffer) {
  int cutbox = heading.length() * 12;
  int base;
  //Serial.println(heading.length());
  int boxsize = 55;
  if (heading.length() >= 3) {
    boxsize = (heading.length() * 12) + 20;
  }
  if (buffer) { boxsize = boxsize + 20; }
  //tft.drawRect(x,y+7,boxsize,y+38,colour);
  tft.drawRoundRect(x, y + 7, boxsize,38, 8, colour);
  tft.drawFastHLine(x + 5, y + 7, cutbox, returnCol(0, 0, 0));
  tft.setCursor(x + 10, y);
  tft.print(heading);
  tft.setCursor(x + 7, y + 20);
    tft.print(String(value));
  return (x + boxsize);
}


void drawPartHex(int x, int y, int radius, uint16_t colour, int section) {
  int x1, y1, x2, y2;
  double th, startAngle;

  int sides = 6;
  int angle = 0;
  // we work in radians here
  startAngle = PI / 180 * (angle - 90.0);  // make 0 degrees straight up
  th = startAngle;
  x2 = x + round(cos(th) * radius);  // get first vertex
  y2 = y + round(sin(th) * radius);

  for (uint8_t i = 1; i <= sides; i++) {
    x1 = x2;                                 // old vertex is...
    y1 = y2;                                 // ...the new startpoint
    th = startAngle + (i * TWO_PI) / sides;  // angle of next vertex in radians
    x2 = x + round(cos(th) * radius);        // get next vertex
    y2 = y + round(sin(th) * radius);
    if (i == section) {
      tft.fillTriangle(x, y, x1, y1, x2, y2, colour);
    }
  }
}

void drawHex(int x, int y, int radius, uint16_t colour) {
  int x1, y1, x2, y2;
  double th, startAngle;

  int sides = 6;
  int angle = 0;
  // we work in radians here
  startAngle = PI / 180 * (angle - 90.0);  // make 0 degrees straight up
  th = startAngle;
  x2 = x + round(cos(th) * radius);  // get first vertex
  y2 = y + round(sin(th) * radius);

  for (uint8_t i = 1; i <= sides; i++) {
    x1 = x2;                                 // old vertex is...
    y1 = y2;                                 // ...the new startpoint
    th = startAngle + (i * TWO_PI) / sides;  // angle of next vertex in radians
    x2 = x + round(cos(th) * radius);        // get next vertex
    y2 = y + round(sin(th) * radius);

    tft.fillTriangle(x, y, x1, y1, x2, y2, colour);
  }
}



void clearScreen() {
  tft.fillScreen(ST77XX_BLACK);
}
