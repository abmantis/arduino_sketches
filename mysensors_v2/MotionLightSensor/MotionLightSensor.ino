// Enable debug prints
// #define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

#include <SPI.h>
#include <MySensors.h>
#include <readVcc.h>

#define MIN_V 2000             // empty voltage (0%)
#define MAX_V 3000             // full voltage (100%)

#define MOTION_DPIN 2  // The digital input you attached your motion sensor.  (Only 2 and 3 generates interrupt!)
#define LIGHT_APIN 0   // The analog input you attached your light sensor.
#define TEMP_DPIN 4    // The analog input you attached your OneWire Temp sensor.

#define MOTION_CHILD_ID 1
#define LIGHT_CHILD_ID 2
#define TEMP_CHILD_ID 3

#define MY_SMART_SLEEP_WAIT_DURATION 100
// Initialize motion message
MyMessage msg_motion(MOTION_CHILD_ID, V_TRIPPED);
MyMessage msg_light(LIGHT_CHILD_ID, V_LIGHT_LEVEL);

uint8_t gMotionState = 2;

void before()
{
}

void setup()  
{    
  pinMode(MOTION_DPIN, INPUT);      // sets the motion sensor digital pin as input
}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Motion Light Sensor", "1.46");

  // Register all sensors to gw (they will be created as child devices)
  present(MOTION_CHILD_ID, S_MOTION);
  present(LIGHT_CHILD_ID, S_LIGHT_LEVEL);

}

void loop()     
{     
  uint8_t prevMotion = gMotionState;
  sendMotionLightState();  

  if(prevMotion != gMotionState)
  {
    // this should be one of the first messages, since the controller usually
    // sends messages to nodes right after receiving a message from the node
    // and we listen for messeges here
    pingGateway();
  }

  sendBattery();
  
  if (gMotionState == LOW)
  {
    // Sleep 3 secs since transmitting may trigger the motion sensor
    sleep(3100);
    // DO NOT SEND ANYTHING AFTER THIS, EXCEPT MOTION SENSOR CHANGES.
    // Since the motion sensor sometimes fires when the radio transmits, 
    // all transmissions should be done before the "stabilization" sleep
    // above.
  }

  sendMotionLightState(); 

  doSleep();
}

void doSleep()
{
  unsigned long sleepTime = 0;
  static int quickCheckCount = 0;
  if (gMotionState == HIGH)
  {
    // right after motion is detected wake periodically to check  for light changes 
    // quickly, since a light (from another room) could have just been turned off
    
    if (quickCheckCount < 5)
    {
      ++quickCheckCount;
      sleepTime = 1000;
    } 
    else
    {
      sleepTime = 10000;
    }
  }
  else
  {
    quickCheckCount = 0;
  }
 
  // Sleep until interrupt comes in on motion sensor.
  sleep(digitalPinToInterrupt(MOTION_DPIN), CHANGE, sleepTime);
}

void sendMotionLightState()
{
  sendMotionState();
  if (gMotionState == HIGH)
  {
    sendLightState();
  }
}

void sendMotionState()
{ 
  // Read digital motion value
  uint8_t tripped = digitalRead(MOTION_DPIN); 

  if (tripped != gMotionState)
  {
    send(msg_motion.set(tripped==HIGH ? 1 : 0));
    gMotionState = tripped;
  }
}

void sendLightState()
{
  static int oldLightLevel = -10;
  
  int lightLevel = (1023-analogRead(LIGHT_APIN))/10.23;
  int oldToNew = oldLightLevel - lightLevel;
    
  if (abs(oldToNew) > 3) {
      send(msg_light.set(lightLevel));
      oldLightLevel = lightLevel;
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

