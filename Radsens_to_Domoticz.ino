#include <Arduino.h>
#include <Wire.h>
#include "radSens1v2.h"
#include <SimpleTimer.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID "wifi_network"
#define WLAN_PASS "wifi_password"

#define MQTT_SERVER "192.168.1.42"
#define MQTT_SERVERPORT 1883
#define MQTT_USERNAME "mqtt_user"
#define MQTT_PASSWORD "mqtt_password"
#define MQTT_INTERVAL 30000 //MQTT publish interval in milliseconds

//Domoticz IDX (custom sensor)
#define DYNAMIC_IDX 40
#define STATIC_IDX 41
 
ClimateGuard_RadSens1v2 radSens(RS_DEFAULT_I2C_ADDRESS); /*Constructor of the class ClimateGuard_RadSens1v2,
                                                           sets the address parameter of I2C sensor.

                                                           Default address: 0x66.*/
WiFiClient client;
SimpleTimer mqttTimer;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);
Adafruit_MQTT_Publish domoticz = Adafruit_MQTT_Publish(&mqtt, "domoticz/in");

char mqtt_payload[100];
String mqtt_str; 


void setup()
{
  Serial.begin(115200);
  delay(1000);
  init_wifi_mqtt(); // run the function to initialize wifi and MQTT connectivity
  
  radSens.radSens_init(); /*Initialization function and sensor connection. 
                            Returns false if the sensor is not connected to the I2C bus.*/
                            
//  uint8_t sensorChipId = radSens.getChipId(); /*Returns chip id, default value: 0x7D.*/
//
//  Serial.print("Chip id: 0x");
//  Serial.println(sensorChipId, HEX);
//
//  uint8_t firmWareVer = radSens.getFirmwareVersion(); /*Returns firmware version.*/
//
//  Serial.print("Firmware version: ");
//  Serial.println(firmWareVer);
//
//  Serial.println("-------------------------------------");
//  Serial.println("Set Sensitivity example:\n");
//
//  uint16_t sensitivity = radSens.getSensitivity(); /*Rerutns the value coefficient used for calculating
//                                                    the radiation intensity or 0 if sensor isn't connected.*/
//
//  Serial.print("\t getSensitivity(): ");
//  Serial.println(sensitivity);
//  Serial.println("\t setSensitivity(55)... ");
//
//  radSens.setSensitivity(55); /*Sets the value coefficient used for calculating
//                                the radiation intensity*/
//
//  sensitivity = radSens.getSensitivity();
//  Serial.print("\t getSensitivity(): ");
//  Serial.println(sensitivity);
//  Serial.println("\t setSensitivity(105)... ");
//
//  radSens.setSensitivity(105);
//
//  Serial.print("\t getSensitivity(): ");
//  Serial.println(radSens.getSensitivity());
//  Serial.println("-------------------------------------");
//  Serial.println("HW generator example:\n");
//
//  bool hvGeneratorState = radSens.getHVGeneratorState(); /*Returns state of high-voltage voltage Converter.
//                                                           If return true -> on
//                                                           If return false -> off or sensor isn't conneted*/
//
//  Serial.print("\n\t HV generator state: ");
//  Serial.println(hvGeneratorState);
//  Serial.println("\t setHVGeneratorState(false)... ");
//
//  radSens.setHVGeneratorState(false); /*Set state of high-voltage voltage Converter.
//                                        if setHVGeneratorState(true) -> turn on HV generator
//                                        if setHVGeneratorState(false) -> turn off HV generator*/
//
//  hvGeneratorState = radSens.getHVGeneratorState();
//  Serial.print("\t HV generator state: ");
//  Serial.println(hvGeneratorState);
//  Serial.println("\t setHVGeneratorState(true)... ");
//
//  radSens.setHVGeneratorState(true);
//
//  hvGeneratorState = radSens.getHVGeneratorState();
//  Serial.print("\t HV generator state: ");
//  Serial.println(hvGeneratorState);
//  Serial.println("-------------------------------------");
//  Serial.println("LED indication control example:\n");
//
//  bool ledState = radSens.getLedState(); /*Returns state of high-voltage voltage Converter.
//                                                           If return true -> on
//                                                           If return false -> off or sensor isn't conneted*/
//
//  Serial.print("\n\t LED indication state: ");
//  Serial.println(ledState);
//  Serial.println("\t turn off LED indication... ");
//
//  radSens.setLedState(false); /*Set state of high-voltage voltage Converter.
//                                        if setHVGeneratorState(true) -> turn on HV generator
//                                        if setHVGeneratorState(false) -> turn off HV generator*/
//  ledState = radSens.getLedState();
//  Serial.print("\t LED indication state: ");
//  Serial.println(ledState);
//  Serial.println("\t turn on led indication... ");
//
//  radSens.setLedState(true);
//
//  ledState = radSens.getLedState();
//  Serial.print("\t LED indication state: ");
//  Serial.print(ledState);
//  Serial.println("\n-------------------------------------");

  delay(5000);
}

void loop()
{
//  Serial.print("Rad intensy dyanmic: ");

//  Serial.println(radSens.getRadIntensyDyanmic()); /*Returns radiation intensity (dynamic period T < 123 sec).*/

//  Serial.print("Rad intensy static: ");
//
//  Serial.println(radSens.getRadIntensyStatic()); /*Returns radiation intensity (static period T = 500 sec).*/
//
//  Serial.print("Number of pulses: ");
//
//  Serial.println(radSens.getNumberOfPulses()); /*Returns the accumulated number of pulses registered by the 
//                                                 module since the last I2C data reading.*/
  MQTT_domoticz(radSens.getRadIntensyDyanmic(), radSens.getRadIntensyStatic());
  delay(2000);
}

void MQTT_domoticz(float cpm_dynamic, float cpm_static)
{
  MQTT_connect(); //run the function to (re)connect wifi and MQTT if a disconnection has occured
  delay(100);
  //if for some reason MQTT is still disconnected, do not attempt MQTT communication
  if (mqtt.connect() == 0) {
    //check if MQTT timer is ready, and also make sure the sensor isn't reading a 0 value which could happen if there is a bug 
    if (mqttTimer.isReady() & cpm_dynamic != 0) { 


          Serial.println("\nPublishing to MQTT");
          // build payload, convert to char array, publish and log
          mqtt_str = "{\"idx\":" + String(DYNAMIC_IDX) + ",\"svalue\":\"" + String(cpm_dynamic) + "\"}"; 
          mqtt_str.toCharArray(mqtt_payload, mqtt_str.length() + 1);
          domoticz.publish(mqtt_payload);
          Serial.println(mqtt_payload);
          
          mqtt_str = "{\"idx\":" + String(STATIC_IDX) + ",\"svalue\":\"" + String(cpm_static) + "\"}";
          mqtt_str.toCharArray(mqtt_payload, mqtt_str.length() + 1);
          domoticz.publish(mqtt_payload);
          Serial.println(mqtt_payload);

          //reset MQTT timer
          mqttTimer.reset();
      }
  }
}

void init_wifi_mqtt(){
  // initialize wifi connection, wait for wifi to connect, and start MQTT timer
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  mqttTimer.setInterval(MQTT_INTERVAL);  
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  //attempt to reconnect wifi/MQTT until connectivity is restored
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       if (WiFi.status() != WL_CONNECTED) {
          Serial.println();
          Serial.println("reconnecting.");
          WiFi.disconnect();
          Serial.print(".");
          delay(1500);
          WiFi.reconnect();
          Serial.print(".");
          delay(1500);
          Serial.print(".");
       }
       delay(5000);  // wait 5 seconds

  }
  Serial.println("MQTT Connected!");
}
