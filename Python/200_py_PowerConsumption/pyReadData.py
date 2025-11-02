import threading
import pySerial as S
import time

class KeyBoard(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.k_input = ''
        self.runKeyborReading = True
        self.cmd = ''
        self.readRange = False
        self.readRangeIndex = 0
        self.str_readRangeStartAddr = ''
        self.str_readRangeEndAddr = ''
        self.charCounter = 0
    
    def run(self):   
        print('Press button for:')
        print('gd - Get Date')
        print('rn - Read Measurement Number')
        print('rm-hhhh - Read Measurement. hhhh - hex number of measurement (8 byte data batch)')
        print('rr-hhhh-hhhh - Read Range of Measurements')
        print('b1 - set baud rate to 9600')
        print('b2 - set baud rate to 19200')
        print('q - QUIT')
        while self.runKeyborReading == True:
            try:
                self.k_input = input('>>> ')
            except:
                print('Key Error')

            if self.k_input == 'gd':
                print('Executing: Get date')
                self.cmd = ':' + self.k_input + ';'
            
            if self.k_input == 'rn':
                print('Executing: Read Measurement Number')
                self.cmd = ':' + self.k_input + ';'
                
            if self.k_input[0:3] == 'rm-':
                print('Executing: Read Measurement')
                self.cmd = ':' + self.k_input + ';'
            
            if self.k_input[0:3] == 'rr-':
                print('Executing: Read Range of Measurements')
                self.str_readRangeStartAddr = self.k_input[3:7]
                self.str_readRangeEndAddr = self.k_input[8:12]
                self.readRangeIndex = 0
                self.cmd = ':rm-' + self.str_readRangeStartAddr + ';'
                self.readRange = True
            
            if self.k_input == 'b1':
                print('Executing: Set Baud Rate to 9600')
                self.cmd = ':b1;'
            
            if self.k_input == 'b2':
                print('Executing: Set Baud Rate to 19200')
                self.cmd = ':b2;'
            
            if self.k_input == 'q':
                print('QUITING')
                self.runKeyborReading = False

if __name__ == '__main__':
    
    # Communication with keyboard
    kb = KeyBoard()
    kb.start()
    
    # RS init
    o_p = S.PortRS()
    p = o_p.openPort(portName='COM1', baudRate=9600)
    p.reset_input_buffer()
    p.reset_output_buffer()
        
    # Main loop
    continueRunning = True
    rx_data = []
    while(continueRunning is True):
        if kb.cmd != '':
            p.write(kb.cmd.encode('utf-8'))
            print('send CMD: {0}'.format(kb.cmd), end=' => ')
            
            if kb.cmd == ':b2;':
                p.close()
                p = o_p.openPort(portName='COM1', baudRate=19200)
                p.reset_input_buffer()
                p.reset_output_buffer()
            
            if kb.cmd == ':b1;':
                p.close()
                p = o_p.openPort(portName='COM1', baudRate=9600)
                p.reset_input_buffer()
                p.reset_output_buffer()

            kb.cmd = ''
            
        if p.in_waiting > 0:
            rawRxChar = p.read(size=1)
            try:
                rx_char = rawRxChar.decode('utf-8')
            except:
                rx_char = str(rawRxChar.hex())
            
            rx_data.append(rx_char)
            print(rx_char, end='')
            
            if rx_char == b'\x0d'.decode('utf-8'):
                kb.charCounter += 1
            
            if kb.charCounter >= 1000:
                f = open('rawMeasuremnt{0}.txt'
                         .format(time.strftime('%Y%m%d_%H%M%S', time.localtime())), 'w')
                f.write(''.join(rx_data))
                f.close()
                rx_data = []
                kb.charCounter = 0
            
            if kb.readRange is True:
                if rx_char == b'\x0d'.decode('utf-8'):
                    if kb.cmd == '':
                        kb.readRangeIndex += 1
                        hex_readRangeStartAddr = int(kb.str_readRangeStartAddr, 16)
                        readRangeCurrAddr = hex_readRangeStartAddr + kb.readRangeIndex
                        str_readRangeCurrAddr = '{0:04X}'.format(readRangeCurrAddr)
                        
                        kb.cmd = ':rm-' + str_readRangeCurrAddr + ';'
                        if str_readRangeCurrAddr == kb.str_readRangeEndAddr:
                            kb.readRange = False
                
        if kb.k_input == 'q':  # escape key
            continueRunning = False
        
        
    time.sleep(0.3)
    f = open('rawMeasuremnt.txt', 'w')
    f.write(''.join(rx_data))
    f.close()
    print(''.join(rx_data))
    
    # close port
    p.close()
    
    # wait for end of keyboard process
    kb.join()
