import asyncio
from bleak import BleakClient
from HCSR04_python_lib import HCSR04
from time import sleep
import RPi.GPIO as GPIO

address = "EC:62:60:8E:EB:B6"
led_char_uuid = "19B10001-E8F2-537E-4F6C-D104768A1214"
GPIO.setmode(GPIO.BCM)
connectionLED = 26
GPIO.setup(connectionLED, GPIO.OUT)




hcsr_sensor = HCSR04(trigger_pin=18, echo_pin=24)

async def primeloop(address, char_uuid):
    try:
        client = BleakClient(address)
        try:
        
            if not client.is_connected:
                await client.connect()
                print("Connected to the device!")
                GPIO.output(connectionLED, GPIO.HIGH)

        
                while True:
                    currentDistance = await getDistance()

                    if currentDistance == -1:
                        print("Error getting distance")
                        await client.write_gatt_char(char_uuid, bytearray([0]))
                    
                    elif currentDistance < 50:
                        await client.write_gatt_char(char_uuid, bytearray([1]))
                        print("Send close")                    

                    elif currentDistance < 100:
                        await client.write_gatt_char(char_uuid, bytearray([2]))
                        print("Send Near")   

                    elif currentDistance < 150:
                        await client.write_gatt_char(char_uuid, bytearray([3]))
                        print("Send Far")
                        
                    else:
                        await client.write_gatt_char(char_uuid, bytearray([0]))
                        print("Send Off")
                      
        except Exception as e:
            print(f"An error occurred: {e}")
            GPIO.output(connectionLED, GPIO.LOW)

        finally:
            if client.is_connected:
                await client.disconnect()
                print("Disconnected from the device")
                GPIO.output(connectionLED, GPIO.LOW)
    except KeyboardInterrupt:
        print('Measurement stopped.')


async def getDistance():
    try:
                     
        distance = hcsr_sensor.get_distance(sample_size=5, decimal_places=2)
        print(f'Distance = {str(distance)} [cm]')
        await asyncio.sleep(0.5) 
        return distance

    except TimeoutError as ex:
        print(f'ERROR getting distance: {ex}')
        return -1

    except OSError as ex:
        print(f'ERROR getting distance: {ex}')
        return -1

asyncio.run(primeloop(address, led_char_uuid))
