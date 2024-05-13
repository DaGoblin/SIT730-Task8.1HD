
#include <ArduinoBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int ledPin = 3; // pin to use for the LED
const int connectionLED = 4;
int postion = 0;
int postionDelayFar = 500;
int postionDelayNear = 250;
int postionDelayClose = 100;
unsigned long lastInterval = 0;
const int buzzerPin = 2; 
const int buzzerTone = 1000; //1Khz for buzzer

void setup() {
  Serial.begin(9600);
  // while (!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("MYLED");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();
  
  Serial.println("BLE LED Peripheral");
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    digitalWrite(connectionLED, HIGH);

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {   // any value other than 0
          postion = (int)(switchCharacteristic.value());
          Serial.print(F("LED Postion: "));
          Serial.println(postion);
        } else {       
          postion = 0;                        // a 0 value
          Serial.println(F("LED off"));
        }
      }
      LedBlinkBuzzer();
    }
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin); 
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(connectionLED, LOW);
  }
  // Serial.println("Do we reach here?");
  
}

void LedBlinkBuzzer()
{
  int interval = 0;
  
  if(postion == 0 ){
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin); 
  }
  else {
    if(postion == 1)
    {
      interval = postionDelayClose;
    }
    else if (postion == 2)
    {
      interval = postionDelayNear;
    }
    else if (postion == 3)
    {
      interval = postionDelayFar;
    }
    
    if (millis() - lastInterval > interval)
    {
      digitalWrite(ledPin, !digitalRead(ledPin));

      // Toggle buzzer based on LED state
      if (digitalRead(ledPin) == HIGH) {
        tone(buzzerPin, buzzerTone); 
      } else {
        noTone(buzzerPin); 
      }

      lastInterval = millis();
    }
  }
}

