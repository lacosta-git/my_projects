import sys
import glob
import serial


class PortRS(object):
    def __init__(self):
        self.portName = None
        self.availablePorts = None
        self.portHandle = None
        
    def fild_avalilable_ports(self):
        """ Lists serial port names
    
            :raises EnvironmentError:
                On unsupported or unknown platforms
            :returns:
                A list of the serial ports available on the system
        """
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(16)]
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
        self.availablePorts = result
        return result
    
    
    def openPort(self, portName=None, baudRate=9600):
        '''
        Function opens serial port.
        If it is possible to open port than handle is returned
        otherwise error message is returned.
        '''
        try:
            return serial.Serial(port=portName,
                                 baudrate=baudRate,
                                 bytesize=serial.EIGHTBITS,
                                 parity=serial.PARITY_NONE,
                                 stopbits=serial.STOPBITS_ONE,
                                 timeout=None,
                                 xonxoff=False,
                                 rtscts=False,
                                 write_timeout=None,
                                 dsrdtr=False,
                                 inter_byte_timeout=None)
        except:
            return 'Could not open port {0}'.format(portName)
    
    def sendCmd(self, cmd='', port=None):
        b_cmd = cmd.encode('utf-8')
        port.write(b_cmd)
    
    def getChar(self):
        return 
    
if __name__ == '__main__':
    myPort = PortRS()
    myPort.fild_avalilable_ports()
    print (myPort.availablePorts)
    pr = myPort.openPort('COM1')
    if pr.is_open is True:
        myPort.sendCmd(cmd='ABC', port=pr)
        print(pr.inWaiting())
    pr.close()
    