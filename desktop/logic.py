import sys, serial, glob

ROTATE_BYTE = 1
HALT_BYTE = 2

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def rotate_command(steps: int, delay: int) -> int:
    if 0 <= steps <= 255 and 0 <= delay <= 255:
        return (ROTATE_BYTE << 2) + (steps << 1) + delay
    else:
        raise ValueError("Error! All parameters must be between 0 and 255.")

def halt_command() -> int:
    return (HALT_BYTE << 3)