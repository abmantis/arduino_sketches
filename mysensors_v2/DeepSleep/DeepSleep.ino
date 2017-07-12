// Enable debug prints
// #define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

#include <MySensors.h>


void setup()  
{  
}

void presentation()  {
  sendSketchInfo("Deep sleep", "1.0");
}

void loop()     
{     
  sleep(600000);
}
