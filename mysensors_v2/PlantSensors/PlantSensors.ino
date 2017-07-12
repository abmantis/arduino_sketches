// Enable debug prints to serial monitor
//#define DEBUG_LOGS


#ifdef DEBUG_LOGS
  #define MY_DEBUG
#endif

#define MY_RADIO_NRF24

#include <MySensors.h>
#include <readVcc.h>

#define MIN_V 2000             // empty voltage (0%)
#define MAX_V 3200             // full voltage (100%)

#define SENSOR_VCC_PIN0 3
#define SENSOR_VCC_PIN1 4
#define SENSOR_ADT_PIN0 A0
#define SENSOR_ADT_PIN1 A1

#define NUM_READS 10    // Number of sensor reads for filtering
#define CHILD_ID0 0
#define CHILD_ID1 1

MyMessage msg0(CHILD_ID0, V_LEVEL);
MyMessage msg1(CHILD_ID1, V_LEVEL);

unsigned long SLEEP_TIME = 1800000; // Sleep time between reads (in milliseconds)

long buffer[NUM_READS];
int index;

#ifdef DEBUG_LOGS
  #define LOG(msg) Serial.print(msg);
#else 
  #define LOG(msg)
#endif

void setup() {
  pinMode(SENSOR_VCC_PIN0, OUTPUT);
  pinMode(SENSOR_VCC_PIN1, OUTPUT);
}

void presentation()
{
  sendSketchInfo("Plant Sensors", "1.0");
  present(CHILD_ID0, S_MOISTURE);
  present(CHILD_ID1, S_MOISTURE);
}

void loop() {
  int humidity0 = sensorCycle(SENSOR_ADT_PIN0, SENSOR_VCC_PIN0);
  LOG(humidity0);
  LOG("  ");
  
  int humidity1 = sensorCycle(SENSOR_ADT_PIN1, SENSOR_VCC_PIN1);
  LOG(humidity1);
  LOG("\n");

  send(msg0.set(humidity0));
  send(msg1.set(humidity1));
  
  sendBattery();
  
  // delay until next measurement (msec)
  sleep(SLEEP_TIME);
}

int sensorCycle(int pin, int vccPin) {
  digitalWrite(vccPin, HIGH);
  delay(10);  
  float humidity = readSensor(pin);
  delay(10);
  digitalWrite(vccPin, LOW);
  
  return humidity;
}

int readSensor(int pin) {
  int sensorValue = analogRead(pin);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  //return sensorValue * (MAX_V / 1023.0);   
  return map(sensorValue, 0, 1023, 100, 0);
}

void sendBattery()
{
  static int oldBatteryPcnt = -10;
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage      
  int oldToNew = oldBatteryPcnt - batteryPcnt;

  if (abs(oldToNew) > 3 ) { // If battery percentage has changed
    sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt; 
  }
}
