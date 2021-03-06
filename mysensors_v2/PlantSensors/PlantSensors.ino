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

#define SENSOR_VCC_PIN0 2
#define SENSOR_VCC_PIN1 3
#define SENSOR_VCC_PIN2 4
#define SENSOR_VCC_PIN3 5
#define SENSOR_ADT_PIN0 A3
#define SENSOR_ADT_PIN1 A2
#define SENSOR_ADT_PIN2 A1
#define SENSOR_ADT_PIN3 A0

#define CHILD_ID0 0
#define CHILD_ID1 1
#define CHILD_ID2 2
#define CHILD_ID3 3

MyMessage msg0(CHILD_ID0, V_LEVEL);
MyMessage msg1(CHILD_ID1, V_LEVEL);
MyMessage msg2(CHILD_ID2, V_LEVEL);
MyMessage msg3(CHILD_ID3, V_LEVEL);
MyMessage msg0Prefix(CHILD_ID0, V_UNIT_PREFIX);
MyMessage msg1Prefix(CHILD_ID1, V_UNIT_PREFIX);
MyMessage msg2Prefix(CHILD_ID2, V_UNIT_PREFIX);
MyMessage msg3Prefix(CHILD_ID3, V_UNIT_PREFIX);

unsigned long SLEEP_TIME = 1800000; // Sleep time between reads (in milliseconds)

#ifdef DEBUG_LOGS
  #define LOG(msg) Serial.print(msg);
#else 
  #define LOG(msg)
#endif

void setup() {
  pinMode(SENSOR_VCC_PIN0, OUTPUT);
  pinMode(SENSOR_VCC_PIN1, OUTPUT);
  pinMode(SENSOR_VCC_PIN2, OUTPUT);
  pinMode(SENSOR_VCC_PIN3, OUTPUT);

  send(msg0Prefix.set("%"));
  send(msg1Prefix.set("%"));
  send(msg2Prefix.set("%"));
  send(msg3Prefix.set("%"));
}

void presentation()
{
  sendSketchInfo("Plant Sensors", "1.22");
  present(CHILD_ID0, S_MOISTURE);
  present(CHILD_ID1, S_MOISTURE);
  present(CHILD_ID2, S_MOISTURE);
  present(CHILD_ID3, S_MOISTURE);
}

void loop() {
  int humidity0 = readSensor(SENSOR_ADT_PIN0, SENSOR_VCC_PIN0);
  LOG(humidity0);
  LOG("  ");
  
  int humidity1 = readSensor(SENSOR_ADT_PIN1, SENSOR_VCC_PIN1);
  LOG(humidity1);
  LOG("  ");

  int humidity2 = readSensor(SENSOR_ADT_PIN2, SENSOR_VCC_PIN2);
  LOG(humidity2);
  LOG("  ");

  int humidity3 = readSensor(SENSOR_ADT_PIN3, SENSOR_VCC_PIN3);
  LOG(humidity3);
  LOG("  ");
    
  LOG("\n");

  send(msg0.set(humidity0));
  send(msg1.set(humidity1));
  send(msg2.set(humidity2));
  send(msg3.set(humidity3));
  
  sendBattery();
  
  sleep(SLEEP_TIME);
}

int readSensor(int pin, int vccPin) {
  digitalWrite(vccPin, HIGH);
  delay(10);

  static int numReads = 10;
  long sensorSum = 0;
  for (int i = 0; i < numReads; ++i) {
    sensorSum += analogRead(pin);
  }
  
  long sensorAvg = (long)(sensorSum / numReads);
  float humidity = map(sensorAvg, 0, 1023, 100, 0);
  
  delay(10);
  digitalWrite(vccPin, LOW);
  
  return humidity;
}

void sendBattery()
{
  static int oldBatteryPcnt = -10;
//  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage
  int batteryPcnt = map(readVcc(), MIN_V, MAX_V, 0, 100); // Get VCC and convert to percentage
  int oldToNew = oldBatteryPcnt - batteryPcnt;

  if (abs(oldToNew) > 3 ) { // If battery percentage has changed
    sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt; 
  }
}
