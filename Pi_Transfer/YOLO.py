import cv2
import serial
import time
from ultralytics import YOLO

# --- CONFIGURATION ---
USE_ARDUINO = False  # Set to True when you actually plug in the Arduino
ARDUINO_PORT = 'COM3' # You will change this later (e.g., 'COM3' on PC, '/dev/ttyACM0' on Pi)
CONFIDENCE_THRESHOLD = 0.7 # How sure the AI needs to be (0.5 = 50%)

# --- ARDUINO CONNECTION ---
arduino = None
if USE_ARDUINO:
    try:
        arduino = serial.Serial(ARDUINO_PORT, 9600, timeout=1)
        time.sleep(2) # Give connection time to settle
        print(f"✅ Connected to Arduino on {ARDUINO_PORT}")
    except Exception as e:
        print(f"⚠️ Arduino connection failed: {e}")
        print("Continuing without Arduino...")

# --- LOAD BRAIN ---
print("Loading AI Model...")
model = YOLO("yolo11n.pt") # This will auto-download the small, fast model

# --- START CAMERA ---
cap = cv2.VideoCapture(0) # 0 is usually the default laptop cam

if not cap.isOpened():
    print("❌ Error: Could not open webcam.")
    exit()

print("🎥 Camera started. Press 'Q' to quit.")

while True:
    success, frame = cap.read()
    if not success:
        break

    # 1. Run Detection
    results = model(frame, stream=True, verbose=False) # verbose=False keeps terminal clean

    person_detected = False

    # 2. Process Results
    for r in results:
        # Draw boxes on the frame
        frame = r.plot()
        
        # Check if a 'person' was found
        for box in r.boxes:
            class_id = int(box.cls[0])
            confidence = float(box.conf[0])
            
            # Class 0 is 'person' in standard YOLO
            if class_id == 0 and confidence > CONFIDENCE_THRESHOLD:
                person_detected = True

    # 3. Logic & Arduino Communication
    if person_detected:
        cv2.putText(frame, "Uautoriseret adgang", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
        
        if arduino:
            arduino.write(b'1') # Send signal to Arduino
            print("Sent signal to Arduino!")
    
    # 4. Show Video
    cv2.imshow("Uni Security System", frame)

    # 5. Quit controls
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
if arduino:
    arduino.close()

# run .\.venv\Scripts\python.exe P1/YOLO.py for testing