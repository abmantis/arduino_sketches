// Enable debug prints
// #define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

#include <SPI.h>
#include <MySensor.h>
#include <readVcc.h>

#define DIGITAL_INPUT_SENSOR 3   // The digital input you attached your motion sensor.  (Only 2 and 3 generates interrupt!)
#define CHILD_ID 1   // Id of the sensor child

#define MY_SMART_SLEEP_WAIT_DURATION 100

#define MIN_V 2000             // empty voltage (0%)
#define MAX_V 3000             // full voltage (100%)

// Initialize motion message
MyMessage msg(CHILD_ID, V_TRIPPED);

void setup()  
{  
  pinMode(DIGITAL_INPUT_SENSOR, INPUT);      // sets the motion sensor digital pin as input
}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Motion Sensor", "1.11");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID, S_MOTION);
}

void loop()     
{     
  sendMotionState();
  pingGateway();
  sendMotionState();

  sendBattery();

  // Sleep until interrupt comes in on motion sensor. Send update every two minute.
  sleep(digitalPinToInterrupt(DIGITAL_INPUT_SENSOR), CHANGE, 0);
}

void sendMotionState()
{     
  static uint8_t oldTripped = 2;
  // Read digital motion value
  uint8_t tripped = digitalRead(DIGITAL_INPUT_SENSOR); 

  if (tripped != oldTripped)
  {
    send(msg.set(tripped==HIGH ? 1 : 0));
    oldTripped = tripped;
  }
}

void sendBattery() // Send battery percentage to GW
{
  static int oldBatteryPcnt = -1;
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage      
  if (batteryPcnt != oldBatteryPcnt) { // If battery percentage has changed
    sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt; 
  }
}

void pingGateway()
{
  sendHeartbeat();
  wait(MY_SMART_SLEEP_WAIT_DURATION);
}
