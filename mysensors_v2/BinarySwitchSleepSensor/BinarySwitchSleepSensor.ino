
// Enable debug prints to serial monitor
//#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24

#include <SPI.h>
#include <MySensors.h>
#include <readVcc.h>

#define SKETCH_NAME "Binary Sleep Switch Dual"
#define SKETCH_MAJOR_VER "1"
#define SKETCH_MINOR_VER "3"

#define PRIMARY_CHILD_ID 3
#define SECONDARY_CHILD_ID 4

#define PRIMARY_BUTTON_PIN 2   // Arduino Digital I/O pin for button/reed switch
#define SECONDARY_BUTTON_PIN 3 // Arduino Digital I/O pin for button/reed switch

#define MY_SMART_SLEEP_WAIT_DURATION 200

#define MIN_V 2000             // empty voltage (0%)
#define MAX_V 3000             // full voltage (100%)

// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg(PRIMARY_CHILD_ID, V_STATUS);
MyMessage msg2(SECONDARY_CHILD_ID, V_STATUS);

int oldBatteryPcnt = -1;
uint8_t sentValue = 2;
uint8_t sentValue2 = 2;

void setup()  
{  
  // Setup the buttons
  pinMode(PRIMARY_BUTTON_PIN, INPUT);
  pinMode(SECONDARY_BUTTON_PIN, INPUT);

  // Activate internal pull-ups
  //digitalWrite(PRIMARY_BUTTON_PIN, HIGH);
  //digitalWrite(SECONDARY_BUTTON_PIN, HIGH);
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  present(PRIMARY_CHILD_ID, S_LIGHT);  
  present(SECONDARY_CHILD_ID, S_LIGHT);  
}

// Loop will iterate on changes on the BUTTON_PINs
void loop() 
{

  sendSwitches();
  pingGateway();
  sendSwitches(); // check if switches have changed while pinging the GW
  
  sendBattery(); 

  // TODO: remove this once it is fixed in mysensors (sleep does the check in the new version)
  if(!isTransportOK()) {
    wait(5000); // transport is not operational, allow the transport layer to fix this
  }

  // Sleep until something happens with the sensor
  sleep(PRIMARY_BUTTON_PIN-2, CHANGE, SECONDARY_BUTTON_PIN-2, CHANGE, 0);
} 

void sendSwitches()
{
    // Short delay to allow buttons to properly settle
  sleep(10);
  
  uint8_t value = digitalRead(PRIMARY_BUTTON_PIN);  
  if (value != sentValue) {
     // Value has changed from last transmission, send the updated value
     send(msg.set(value==HIGH ? 1 : 0));
     sentValue = value;
  }

  value = digitalRead(SECONDARY_BUTTON_PIN);  
  if (value != sentValue2) {
     // Value has changed from last transmission, send the updated value
     send(msg2.set(value==HIGH ? 1 : 0));
     sentValue2 = value;
  }
}

void sendBattery() // Send battery percentage to GW
{
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

