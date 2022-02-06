# Radsens_to_Domoticz
Arduino sketch (for ESP32 devices) based on [climateguard/RadSens](https://github.com/climateguard/RadSens) to transmit [RadSens](https://www.hackster.io/news/radsens-is-a-universal-dosimeter-radiometer-based-on-a-geiger-muller-tube-3d6cbb1637cf) Geiger counter readings to [Domoticz](https://domoticz.com/) via MQTT

![RadSens_ESP32](https://github.com/node-ray/Radsens_to_Domoticz/blob/main/RadsSens_ESP32.jpg)

# Usage

1. Download and extract the zip file
2. Open Radsens_to_Domoticz-main.ino in [Arduino IDE](https://www.arduino.cc/en/software)
3. Add the URL https://dl.espressif.com/dl/package_esp32_index.json to the list of board managers under **File -> Preferences** to add support for ESP32
4. Install the Adafruit MQTT library and the SimpleTimer library from **Tools -> Library Manager**
5. Select your ESP32 board from **Tools -> Board** (for example ESP32 Dev Module)
6. Select the COM port from **Tools -> Port**. If no COM port is listed, install the USB to Serial drivers from https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
7. In the #define section, configure the wifi network and password
8. Then configure the MQTT server IP, username, and password (and optionally change the MQTT publish interval)
9. Change DYNAMIC_IDX and STATIC_IDX to the IDX values for "custom sensor" virtual sensors created within Domoticz.
10. Upload to the ESP32 device connected to the RadSens unit. If everything is configured correctly, the sensor readings should be displayed in Domoticz 
