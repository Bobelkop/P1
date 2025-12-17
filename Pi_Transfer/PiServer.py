import cv2
import time
import serial
from flask import Flask, Response
from ultralytics import YOLO

# --- CONFIGURATION ---
app = Flask(__name__)
USE_ARDUINO = True  # ✅ Enabled for Pi
# On Pi, Arduino is usually ttyACM0. If that fails, the code tries ttyUSB0 automatically.
ARDUINO_PORT = '/dev/ttyACM0' 
CONFIDENCE_THRESHOLD = 0.6

# --- ARDUINO CONNECTION ---
arduino = None
if USE_ARDUINO:
    try:
        try:
            arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
            print("✅ Arduino connected on ttyACM0")
        except:
            arduino = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
            print("✅ Arduino connected on ttyUSB0")
        time.sleep(2)
    except Exception as e:
        print(f"⚠️ Arduino NOT found: {e}")
        print("Running in 'Simulation Mode' (Camera only)")

# --- LOAD BRAIN ---
# Using the Nano model for speed
model = YOLO("yolo11n.pt") 

# --- START CAMERA ---
cap = cv2.VideoCapture(0)
# vital: Lower resolution to 640x480 so the Pi doesn't lag/overheat
cap.set(3, 640)
cap.set(4, 480)

def generate_frames():
    while True:
        success, frame = cap.read()
        if not success:
            break

        # 1. Run Detection
        results = model(frame, stream=True, verbose=False)
        person_detected = False

        for r in results:
            frame = r.plot() # Draw boxes
            for box in r.boxes:
                # Class 0 is Person
                if int(box.cls[0]) == 0 and float(box.conf[0]) > CONFIDENCE_THRESHOLD:
                    person_detected = True

        # 2. Logic & Arduino
        if person_detected:
            # Your custom Danish text
            cv2.putText(frame, "UAUTORISERET ADGANG", (20, 50), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
            
            if arduino:
                arduino.write(b'1') # Send signal to Arduino
                # No print() here to avoid spamming the terminal log

        # 3. Encode for Web
        ret, buffer = cv2.imencode('.jpg', frame)
        frame_bytes = buffer.tobytes()

        # 4. Stream
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

# --- WEBSITE ROUTES ---
@app.route('/')
def index():
    return """
    <html>
    <head>
        <title>Uni Security System</title>
        <style>
            body { background-color: #1a1a1a; color: white; text-align: center; font-family: Arial, sans-serif; }
            h1 { color: #ff4444; }
            img { border: 4px solid #444; border-radius: 8px; max-width: 100%; }
        </style>
    </head>
    <body>
        <h1>⚠️ Security Feed Live ⚠️</h1>
        <img src="/video_feed">
    </body>
    </html>
    """

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    # 0.0.0.0 is crucial - it lets other devices on WiFi see the site
    app.run(host='0.0.0.0', port=5000, debug=False)