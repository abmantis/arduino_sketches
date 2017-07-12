// #define MY_DEBUG
#define MY_RADIO_NRF24

#include <SPI.h>
#include <MySensors.h>
#include <readVcc.h>

#define MIN_V 2000             // empty voltage (0%)
#define MAX_V 3000             // full voltage (100%)

#define LIGHT_APIN 0   // The analog input you attached your light sensor.

#define LIGHT_CHILD_ID 1

#define MY_SMART_SLEEP_WAIT_DURATION 100

// Initialize motion message
MyMessage msg_light(LIGHT_CHILD_ID, V_LIGHT_LEVEL);
bool sentLight = false;

void before()
{
}

void setup()  
{    
}

void presentation()  {
  sendSketchInfo("Light Sensor", "1.1");
  present(LIGHT_CHILD_ID, S_LIGHT_LEVEL);
}

void loop()     
{    
  sentLight = false; 
  sendLightState();  

  if(sentLight)
  {    
    pingGateway();
    sendBattery();
  }  

  doSleep();
}

void doSleep()
{
  
  // TODO: remove this once it is fixed in mysensors (sleep does the check in the new version)
  if(!isTransportOK()) {
    wait(5000); // transport is not operational, allow the transport layer to fix this
  }
  
  sleep(2000);
}

void sendLightState()
{
  static int oldLightLevel = -10;
  
  int lightLevel = (1023-analogRead(LIGHT_APIN))/10.23;
  int oldToNew = oldLightLevel - lightLevel;
    
  if (abs(oldToNew) > 5) {
      send(msg_light.set(lightLevel));
      oldLightLevel = lightLevel;
      sentLight = true;
  }
}

void sendBattery() // Send battery percentage to GW
{
  static int oldBatteryPcnt = -10;
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage      
  int oldToNew = oldBatteryPcnt - batteryPcnt;

  if (abs(oldToNew) > 3 ) { // If battery percentage has changed
    sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt; 
  }
}

void pingGateway()
{
  sendHeartbeat();
  wait(MY_SMART_SLEEP_WAIT_DURATION);
}

