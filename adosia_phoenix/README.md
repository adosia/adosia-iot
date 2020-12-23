# Adosia Phoenix Connect Script for SPACE IO Boards

Flash the [adosia_phoenix.ino Phoenix Connect script](https://github.com/adosia/adosia-iot/blob/master/adosia_phoenix/adosia_phoenix.ino) to your ESP8266-based **WEMOS D1 R2** or **WEMOS D1 Mini** WiFi baseboards to automatically download the latest Adosia binary to ready your own WiFi hardware for connecting to the Adosia IoT Platform.

You can also use the [Phoenix Connect script](https://github.com/adosia/adosia-iot/blob/master/adosia_phoenix/adosia_phoenix.ino) to reconnect an existing previously connected device which now may be unable to connect.

Alternatively, you can purchase IoT hardware pre-flashed with the Adosia binary from the [Adosia IoT Store](https://adosia.io).


Please support the Adosia IoT open hardware initiative by sending any amount of Ada to the following Cardano address:
**addr1qx0w683ua2hua30jhan3xfn5f473pjgh4vgxpx7mnuwfxddyah4yv7wh5dktyxs4cf7gf72pqa39twrp3qs56v0e6y4qupjq4t**


# Steps to Download the Latest Adosia Binary

1. Copy and paste the **[adosia_phoenix.ino](https://github.com/adosia/adosia-iot/blob/master/adosia_phoenix/adosia_phoenix.ino) sketch** into your **Arduino IDE**

2. Set your IDE settings to **Board** *LOLIN(WEMOS) D1 R2 & mini*, **Flash Size** *4M (3M spiffs)*, and **Upload Speed** of *115200 baud*

3. Make sure **line 11** has the correct **iot_prefix** for the IO board you are using (default is **space_v2-2_fwv**)

4. Update **lines 14 and 15** respectively with the **Network SSID** and **Password** for your WiFi Network (*WiFi login credentials are never transmitted to Adosia servers*)

5. Prepare the Arduino IDE - under Preferences set the Additional Boards Manager URLs to: **https://github.com/esp8266/Arduino/releases/download/2.6.1/package_esp8266com_index.json**

6. Use the Arduino IDE to upload the sketch to your IoT WiFi hardware
	- under **Tools > Erase Flash:**, first try flashing the board with the setting **Only Sketch** selected
	- if the above does not work you may need to set **Erase Flash** to **Sketch + WiFi Settings**
	- in extreme scenarios you may want to erase **All Flash Contents** - this will create a new device key

7. When the upload completes, reset your board (recycle power) so the OTA update will take effect and download the latest binary from Adosia servers


If using an Adosia SPACE IO board v2.3, the onboard LED will blink green 10 times once the baseboard successfully connects to your WiFi network, and will blink blue 6 times once the latest Adosia binary has been successfully downloaded.  The board should now be ready to connect to the Adosia IoT Platform just as if you purchased it from the [Adosia IoT Store](https://adosia.io)

***if the Arduino IDE doesn't recognize your board, you may need to [download the CH340 driver](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)***



# iot_prefix value to set based on Adosia IO Board

 - SPACE (or no IO board) => update iot_prefix on line 10 to **space_v2-2_fwv**
