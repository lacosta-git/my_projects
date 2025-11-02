import time
import serial


# --------------------------------------------------- #
# ---------------  Serial --------------------------- #
# --------------------------------------------------- #
def init_serial():
    ser = serial.Serial(port='COM3', baudrate=115200, timeout=1)
    ser.flushInput()
    ser.flushOutput()
    return ser


def serial_close(serial_handle=None):
    try:
        serial_handle.close()
        print("OK: COM port closed")
    except:
        print("ERROR: No open COM port")


def reset_relays(com=None):
    com.write(b'w:r:1:0;')
    com.readline()
    time.sleep(0.1)
    com.write(b'w:r:2:0;')
    com.readline()
    time.sleep(0.1)
    com.write(b'w:r:3:0;')
    com.readline()
    time.sleep(0.1)
    com.write(b'w:r:4:0;')
    com.readline()
    time.sleep(0.1)


def update_relay(com=None, relay_number=None, status=None):
    if status is True:
        stat = 1
    else:
        stat = 0
    cmd = "w:r:{0}:{1};".format(relay_number, stat)
    com.write(cmd.encode('utf-8'))
    com.readline()
    time.sleep(0.05)


def substract_digit(r=None):
    sr = r.find(b';>') + 2
    sp = r.find(b'<', sr)
    digit = int(r[sr:sp])
    return digit


if __name__ == "__main__":
    sr = init_serial()
    update_relay(com=sr, relay_number=2, status=0)
    #serial_close(sr)

    t = b'>r:v:h:c;>0<>OK:\n'
    sr = t.find(b';>') + 2
    sp = t.find(b'<', sr)
    d = t[sr:sp]
    my_digit = int(d)
    print(my_digit)
