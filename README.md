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

USB CDC On Boot: Disabled 
Core DEbug Level: Error 
PSRAM: OPI PSRAM

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


