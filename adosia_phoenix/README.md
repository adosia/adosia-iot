# Adosia Phoenix Connect Script for SPACE IO Boards

Flash the [adosia_phoenix.ino](https://github.com/adosia/adosia-iot/blob/master/adosia_phoenix/adosia_phoenix.ino) script to your ESP8266-based **WEMOS D1 R2** or **WEMOS D1 Mini** WiFi baseboards to automatically download the latest Adosia binary to ready your own WiFi hardware for connecting to the Adosia IoT Platform.

Alternatively, you can purchase IoT hardware pre-flashed with the Adosia binary from the [Adosia IoT Store](https://adosia.io)

You can support Adosia open hardware development by sending any amount of ADA to the following Cardano address:
**DdzFFzCqrhssyL8yoWBsM1NbhozhNu7yZ2vM355AauDt5MrC3kmzaJC8iwE8VC6MB4QmJ2p9diiGaM5wFYruE5gtsizFV4SyfQjUjyDX**


# Steps to Download the Latest Adosia Binary

1. Copy and paste the **[adosia_phoenix.ino](https://github.com/adosia/adosia-iot/blob/master/adosia_phoenix/adosia_phoenix.ino) sketch** into your **Arduino IDE**

2. Set your IDE settings to **Board** *WeMos D1 R2 & mini*, **Flash Size** *4M (3M spiffs)*, and **Upload Speed** of *115200 baud*

3. Make sure **line 7** has the correct **iot_prefix** for the IO board you are using (default is **space_v2-2_fwv**)

4. Update **lines 13 and 14** respectively with the **Network SSID** and **Password** for your WiFi Network (*WiFi login credentials are never transmitted to Adosia servers*)

5. Use the Arduino IDE to upload the sketch to your IoT WiFi hardware - you may need to set "Erase Flash" to "All Flash Contents"

6. When the upload completes, reset your board (recycle power) so the OTA update will take effect and download the latest binary from Adosia servers


If using an Adosia SPACE IO board v2.3, the onboard LED will blink green 10 times once the baseboard successfully connects to your WiFi network, and will blink blue 6 times once the latest Adosia binary has been successfully downloaded.  The board should now be ready to connect to the Adosia IoT Platform just as if you purchased it from the [Adosia IoT Store](https://adosia.io)

***if the Arduino IDE doesn't recognize your board, you may need to [download the CH340 driver](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)***



# iot_prefix value to set based on Adosia IO Board

 - SPACE (or no IO board) => update iot_prefix on line 10 to **space_v2-2_fwv**