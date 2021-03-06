# Adosia Phoenix Connect Script for SPACE IO Boards

Flash the [adosia_phoenix.ino Phoenix Connect script](https://github.com/adosia/adosia-iot/blob/master/adosia_phoenix/adosia_phoenix.ino) to your ESP8266-based **LOLIN(WEMOS) D1 R2** or **LOLIN(WEMOS) D1 Mini** WiFi baseboards to automatically download the latest Adosia binary to ready your own WiFi hardware for connecting to the Adosia IoT Platform.

You can also use the [Phoenix Connect script](https://github.com/adosia/adosia-iot/blob/master/adosia_phoenix/adosia_phoenix.ino) to reconnect an existing previously connected device which now may be unable to connect.

Alternatively, you can purchase IoT hardware pre-flashed with the Adosia binary from the [Adosia IoT Store](https://adosia.io).


Please support the Adosia IoT open hardware initiative by sending any amount of Ada to the following Cardano address:
**addr1qx0w683ua2hua30jhan3xfn5f473pjgh4vgxpx7mnuwfxddyah4yv7wh5dktyxs4cf7gf72pqa39twrp3qs56v0e6y4qupjq4t**


# Steps to Download the Latest Adosia Binary

1. Visit the **[Arduino website](https://www.arduino.cc/en/software)** and download the Arduino IDE software appropriate for your operating system:

<img src='./images/download_arduino_ide.png' />

2. Open the Arduino IDE and open a new sketch (select **File > New**). Next copy and paste the raw **[adosia_phoenix.ino](https://raw.githubusercontent.com/adosia/adosia-iot/master/adosia_phoenix/adosia_phoenix.ino) sketch** into your **Arduino IDE**

<img src='./images/paste_phoenix_sketch.png' />

3. Prepare the Arduino IDE - under **Arduino > Preferences** set the Additional Boards Manager URLs to:
**https://github.com/esp8266/Arduino/releases/download/2.6.1/package_esp8266com_index.json**

<img src='./images/prepare_ide.png' />

4. Under **Tools**, set your IDE settings to the following:
	- **Board:** *LOLIN(WEMOS) D1 R2 & mini*
	- **Flash Size:** *4MB (FS:2MB OTA:!~019KB)*
	- **Upload Speed:** *115200 baud*
	- **Erase Flash:** *Sketch + WiFi Settings*
	- **SSL Support:** *All SSL ciphers (most compatible)*
	- **Port:** */dev/cu.usbserial...*
	
<img src='./images/ide_settings.png' />

5. Update **lines 14 and 15** respectively with the **Network SSID** and **Password** for *your WiFi Network* (*WiFi login credentials are never transmitted to Adosia servers*), and make sure **line 11** has the correct **iot_prefix** for the IO board you are using (default is **space_v2-2_fwv**)

<img src='./images/wifi_settings.png' />

6. Use the Arduino IDE to upload the sketch to your IoT WiFi hardware:
	- if flashing an off-the-shelf baseboard (purchased from a 3rd-party) for the first time, set **Erase Flash** to *All Flash Contents*
	- in extreme scenarios you may need to set **Erase Flash** to *All Flash Contents*
	- erasing the flash will force a new device key to be created if one already exists
	- erasing the flash will remove any previously stored private cryptographic keys / wallets
	- ***make sure to back up any Cardano private wallet keys stored on this device BEFORE erasing the device flash!!!***
	

<img src='./images/upload_sketch.png' />

7. When the upload completes, your board should automatically reset. If the board does not reset manually reset your board (recycle power) to force the OTA update process to begin downloading the latest binary from Adosia servers

If using an Adosia SPACE IO board v2.3, the onboard LED will blink green 10 times once the baseboard successfully connects to your WiFi network, and will blink blue 6 times once the latest Adosia binary has been successfully downloaded. The board should now be ready to connect to the Adosia IoT Platform just as if you purchased it from the [Adosia IoT Store](https://adosia.io)

***if the Arduino IDE doesn't recognize your board, you may need to [download the CH340 driver](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)***

8. Now you can proceed to [Connect your device to the Adosia IoT platform](https://www.youtube.com/watch?v=2M2ukKbx40A)


# iot_prefix value to set based on Adosia IO Board

 - SPACE (all versions, or no IO board) => update iot_prefix on line 11 to **space_v2-2_fwv**


# Troubleshooting

With the release of Mac OS X Big Sur the upload tool throws an error when attempting to flash the board:

 - root cause is **upload.py** uses *esptool* which uses *pyserial* and *pyserial* is not fully functional on Big Sur
 
 - the workaround is to edit ~/Library/Arduino15/packages/esp8266/hardware/esp8266/2.6.1/tools/esptool/esptool.py (the full path of the file to edit is: /Users/<username>/Library/Arduino15/packages/esp8266/hardware/esp8266/2.6.1/tools/esptool/esptool.py)
 
 - within that file comment out a block of code by adding **#** to the start of each line:
 
 <img src='./images/code_block.png' />

 - visit [this post](https://forum.arduino.cc/index.php?topic=702144.0) on the Arduino forum for more details