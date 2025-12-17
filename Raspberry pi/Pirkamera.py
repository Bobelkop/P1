# -*- coding: utf-8 -*-
from gpiozero import MotionSensor
import subprocess
import time

pir = MotionSensor(12)

led = LED(21)                # LED paa GPIO 21
print("Scanner efter bevaegelse")

logfile = "events.log"

while True:
    pir.wait_for_motion()
    print("Bevaegelse opfanget. Starter ekstern Python-fil...")
    led.on() # <-- Taend LED naar PIR registrerer bevaegelse


    # Start en anden Python-fil (fx yolo.py)
    process = subprocess.Popen(
        ["python3", "yolo.py"]   # <-- her angiver du din fil
    )

    # Skriv logbesked med timestamp
    with open(logfile, "a", encoding="utf-8") as f:
        f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - Motion detected, script started\n")

    start_time = time.time()

    # Vent enten 10 sekunder eller indtil PIR ikke registrerer mere
    while time.time() - start_time < 10:
        time.sleep(0.5)

    # Stop den eksterne proces
    process.kill()
    led.off() # <-- Sluk LED naar bevaegelsen er slut

    print("Bevaegelse stoppet. Log event gemt.")

# Skriv logbesked naar scriptet lukkes
    with open(logfile, "a", encoding="utf-8") as f:
        f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - Motion ended, script closed\n")

