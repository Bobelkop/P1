# -*- coding: utf-8 -*-
import subprocess, time

def main():
    print("Starter ffplay fra yolo.py...")
    proc = subprocess.Popen(
        ["ffplay", "-f", "v4l2", "-autoexit", "/dev/video0"]
    )
    time.sleep(10)
    proc.terminate()  # eller proc.kill()

if __name__ == "__main__":
    main()

