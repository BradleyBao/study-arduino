import serial
import time
from pynput import keyboard

# =============== Configuration Parameters ===============
SERIAL_PORT = 'COM6'  # Replace with your actual serial port (Bluetooth/Wired)
BAUD_RATE = 9600
# =======================================

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
    time.sleep(2)
    print(f"Successfully connected to {SERIAL_PORT}!")
except Exception as e:
    print(f"Failed to connect to serial port {SERIAL_PORT}: {e}")
    exit()

# Store all pressed keys in a set to handle multiple key presses
pressed_keys = set()
last_sent_cmd = None

def get_cmd_from_keys(keys):
    """Send movement commands based on the current pressed keys."""
    has_w = 'w' in keys
    has_s = 's' in keys
    has_a = 'a' in keys
    has_d = 'd' in keys

    # combo key movement
    if has_w and has_d: return 'e' # 斜右前 (WD)
    if has_w and has_a: return 'q' # 斜左前 (WA)
    if has_s and has_d: return 'c' # 斜右后 (SD)
    if has_s and has_a: return 'z' # 斜左后 (SA)

    # single key movement
    if has_w: return 'w'
    if has_s: return 's'
    if has_a: return 'a'
    if has_d: return 'd'

    return ' ' # No movement (stop)

def update_movement():
    global last_sent_cmd
    cmd = get_cmd_from_keys(pressed_keys)
    # Only send a new command if it has changed, to avoid frequent congestion
    if cmd != last_sent_cmd:
        ser.write(cmd.encode())
        last_sent_cmd = cmd

def on_press(key):
    try:
        k = key.char.lower()
        if k in ['w', 'a', 's', 'd']:
            if k not in pressed_keys:
                pressed_keys.add(k)
                update_movement()
    except AttributeError:
        pass

def on_release(key):
    try:
        k = key.char.lower()
        if k in ['w', 'a', 's', 'd']:
            if k in pressed_keys:
                pressed_keys.remove(k)
                update_movement()
    except AttributeError:
        if key == keyboard.Key.esc:
            print("\n退出控制程序...")
            ser.write(b' ')
            return False

with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()

ser.close()