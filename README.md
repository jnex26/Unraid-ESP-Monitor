# Unraid Monitor 

So this is a native unraid monitor I'll walk you though build and design. 

![The Monitor](https://github.com/jnex26/Unraid-ESP-Monitor/blob/main/media/20250731_133638.jpg)

# BOM 

| Item | Link | Notes |
|---------------|-----------|-------------|
| ST7796S Screen (480x320) |  https://www.aliexpress.com/item/1005006175220737.html?spm=a2g0o.order_list.order_list_main.65.88591802qyroqp |  This is the screen above, with some work you should be able to get it working most SPI screens but you may need to adjust the organisation to showv everything  | 
| ESP32 S3 | https://www.aliexpress.com/item/1005005051294262.html?spm=a2g0o.order_list.order_list_main.83.88591802qyroqp | so i'm using is an 'expensive' S3, however I have another working one with the ESP32D, i've tried with a micro but it gets MEGA hot |

# Other Things needed . 

Prototype board and wires to connect them together, I've not done a Gerber yet, but it is really simple wiring, bascially A-2-B, some soldering skills are needed if your not a good solderer or don't want to try, ping me on reddit (same name) I'm in the UK and I can put a communicator board together and ship it out, I'm also going to do a gerber of a PCB, warning on the S3 it is MEGA wide which can make it awkward to solder inplace. 

Some Good music ( I can recomend GHOST ) , a bit of paitence and a computer and a unraid instance. 

# Wiring up. 

##  Display

They are normally in this order. 

|Screen Port| Wire Colour|
|---------|--------|
|GND|Black 
|VCC|Red
|SCL|Grey
|SDA|Purple
|RST|BLue
|DC|Green
|CS|Yellow
|BL|Orange
|SDA-0|Not Needed

## For S3 - 

Other Boards are avalible this is what I'm currently using I have a wiring diagram for a ESP32D as well 

|S3 GPIO Pins| Wire Colour|
|---------|--------|
|Ground   | black
|3v3      | Red|
|14  |Grey|
|13  |Purple|
|8    |Blue|
|16    | Green|
|15    |Yellow|
|6     |Backlight|

# Build environment. 

I've built this with the Ardino IDE 

https://www.arduino.cc/en/software/

# ArduinoIDE 

## Setting up the IDE for Builds 

Add the following URL to additional boards manager url file->prefrences

http://arduino.esp8266.com/stable/package_esp8266com_index.json

Let it refresh. 

On the Left of the IDE select boards manager you want to install "esp32 by Esprissif Systems" this will take a while and ask for your input multiple times. 

Once you have boards loaded, for S3 your going to want to configure the board and connection, plug the board into the USB port, I've found it is better to use one directly on the motherboard rather than a hub or monitor, once connected at the drop down box at the top of the IDE select your COM port, it should automatically default to ESP dev board, if you have a non-generic board click Tools -> Board -> ESP32 -> ESP32S3 Dev Module ( for S3 if your using a non-S3 board either select your board type or generic Dev board ( don't worry it will drop out if you have the wrong board type ) 

NOTE:- if the COM port is connecting and disconnecting it maybe the bootloader running on the ESP32, try installing https://github.com/espressif/esptool and then flashing a blank sketch, or running burn bootloader 

My S3 Board config is as follows. 

* USB CDC On Boot: Disabled 
* Core DEbug Level: Error 
* PSRAM: OPI PSRAM

the Rest are default... 

Flash a blank sketch then select Tools -> Serial Monitor press the reset button and you should see stuff scroll through the screen ( you have a working esp32 and you can flash ) 

## Setting up Build Env

### built in libaries. 

Once you have a stable ESP32 connected to your machine and wired up to your system your going to need to install the packages and setup a few libraries, on the left you have the Libraries manager search for ST7789 and the top should be "Adafruit ST7735 and ST7789 Libary by Adafruit" click install do this for all the packages listed below. 

* Adafruit ST7735 and ST7789 Libary by Adafruit (V1.11.0)
* Arduinojson by Benoit Blanchon (V7.4.2)
* Adafruit GFX Library (1.12.1) 

if will ask to install multiple other packages accept these. 

### My Libaries. 

I've written most of the logic as libaries so your going to need git to pull my repos. 

find your Arduino libaries folder ( under windows it is normally username\documents\Arduino\Libraries I've found this easiest using git command line tool https://git-scm.com/book/en/v2/Getting-Started-The-Command-Line

get Explorer into the libaries folder, when there click the path and type CMD [ENTER] <-- press enter don't type enter, it will open a cmd prompt, type the following commands that are in the code block, in the first section I talk about removing .git this is a precaution to prevent you accidentaly sharing your wifi passwords and API key with the entire world, important to remember the . at hte end of the git clones. 

```
mkdir creds
cd creds
https://github.com/jnex26/jnex26-creds.h.git .
del .git
```
Select y to delete .git 
```
cd ..
mkdir Unraid-GraphQL-api
cd Unraid-GraphQL-api
git clone https://github.com/jnex26/Unraid-GraphQL-api.git .
```

Once you have set this up you can clone this repo pick a directory that isn't in the labraries folder and run the following command. 

```
git clone https://github.com/jnex26/Unraid-ESP-Monitor.git .
```

Setting up your connection, I've not setup any specifc wifi config for this, so you need to edit a file. 

in the creds file.. 
```
#ifndef STASSID                       
//                                    
#define STASSID "<SSID>"            //SSID 
#define STAPSK  "<Wifi Password>"        // ... and set your WIFI password
#define UNRAIDAPI "<Unraid APi Key>"
#define UNRAIDURL "<Full Insecure URL for your unraid GrapQL Instance>"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* unraidapiey = UNRAIDAPI;
const char* unraidapi = UNRAIDURL;
```

update all the feilds in the creds.h file just leave the "'s  you will also need a API key from unraid. 

### Creating the unraid api key. 

SSH into your unraid instance. ( I use putty ) and login as your privilidged user, now I only have a single unraid instance so I don't know if you need to enable the sandbox with "unraid-api developer" but i'm going to assume that is a no. 

Run the below command your going to need to create a key with admin readonly. 
```
unraid-api apikey --create
```
you will asked to Enter a Name and Description for the API key, make something up, this isn't important and I'm wasting chars on this run on scentence.. 

when asked to pick roles, navigate to ADMIN press space to higlight then enter, now I select all entries with a READ but I may not need them all, press enter and you should given an API key, copy and paste this into the creds.h file. 

your unraid url will be 
```
http://<unraid ip address>/GraphQL 
```
# Test the Build 

Open up the Unraid_Monitor.ino project click Sketch -> Verify/Compile, it should build and complete. 

when that is done, you can click upload to push it to the ESP32, the screen should inintally go multicoloured and then phase to black, once that is done you can monitor the Serial monitor as it will tell you if it connecting to the Wifi, on S3 boards there is normally two USB ports pick the one that says USB. 

Now I built this to be modular, and it's not really human readable atm. 

# Configuring 

## Refresh Timings. 

Search for Smartdelay this is the timing for refreshing things in milliseconds 

  int Logo = 10;  <- the logo bar, this is the phasing bar at the top of the screen. 
  int Memory = 10000; <- This refreshs the memory Stats 
  int Array = 40000; <- Does the Array Capacity 
  int Parity = 60000; <- Builds the Parity Windows 
  int Disks = 60000; <- this does the Disks Bar 
  int CDisks = 60000; <- this does the cache monitor bar 
  int Containers = 60000; <- this does docker ( NOTE :- this can and does time out )

Changing the number of Array Disks

this is built to be scalable, but you will need to resize some of the windows to make them work. 

look for -> drawArrayDisks 

This line below change i=10 to however many disk you have. 
```
   for (int i=0; i<=10; i++) {
```
this line below dictates the settings works like this 

```
drawVertPercentBox(capture+5,100,20,100,percentused,_id,colour);
```

* capture <- the furthest X location of the last graph,
* (first 100) <- This is the top left
* (20) <- Width in Pixels
* (second 100) <- the Height
* percentused <- the Disks Percentage used
* heading <- The Disk ID
* colour <- This is the outer ring on this specific graph it goes red if a disk has an issue. 

to define the colours, please use the following if this does the detection and you can change the colours by altering the RGB values. (returnCol(R,G,B)) 
```
        if (status=="DISK_OK") {
          colour = returnCol(0,255,0);
        } else {
          colour = returnCol(255,0,0);
        }
```

## Hex Graph 

Look for -> drawContainers; 

these are the Box boundries, but it does not draw the box, just constrains the drawing.  
```
 int endBoundryx=480;
  int endBoundryy=215;
  int startBoundryx=273;
  int startBoundryy=140;
```
Radius of the Hexs ( Obvious lower values you can fit more in the box ) B is a buffer if you need it. 
```
 int r=10;
 int b=0;
```


Feel free to modify just credit me please if you get value from this. 




