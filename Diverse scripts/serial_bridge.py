import serial
import requests
import time
from datetime import datetime

print("Script started")

#Fyri at koyra skal man skriva python3 serial_bridge.py

#Fyri at kanna hvat ttyACM* tað er so ls /dev/ttyACM*

# Serial setup
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2)

while True:
    if ser.in_waiting:
        line = ser.readline().decode(errors="ignore").strip()
        
        if "," not in line:
            print("Ignorer non data linje", line)
            continue
        
        uid, pin = line.split(",", 1)
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        print(f"Received UID = {uid}, PIN = {pin}")

        try:
            response = requests.post(
                "http://192.168.0.103/log_acces.php",
                data={"uid": uid, "pin": pin}
            )
            result = response.text.strip()
            
            if result not in ("ok", "fail"):
                result = "fail"
            
            print(f"[{timestamp}] UID: {uid}, PIN: {pin} -> {result}")

            ser.write((result + "\n").encode())
            time.sleep(0.1)
            print("Sent to Arduino:", result)

        except requests.RequestException as e:
            print("Request error:", e)
            ser.write(b"INVALID\n")
            time.sleep(0.1)