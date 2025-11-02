import glob

class Measurement():
    def __init__(self):
        self.year = '00'
        self.month = '00'
        self.day = '00'
        self.hour = '00'
        self.minute = '00'
        self.mesL1 = '00'
        self.mesL2 = '00'
        self.mesL3 = '00'
        self.lineMes = [self.year, self.month, self.day,
                        self.hour, self.minute,
                        self.mesL1, self.mesL2, self.mesL3]
        
        self.dayMes = []
        for hour in range(0, 24):
            for minute in range(0, 60):
                str_hour = '{0:02}'.format(hour)
                str_minute = '{0:02}'.format(minute)
                str_time = str_hour + str_minute
                self.dayMes.append({'time': str_time, 
                                    'L1': 0, 'L2': 0, 'L3': 0,
                                    'count': 0})
                
    def parseFile(self, my_file):
        for line in my_file.split('\n'):
            try:
                lineData=line.split(':')
                self.year = lineData[0]
                self.month = lineData[1]
                self.day = lineData[2]
                self.hour = lineData[3]
                self.minute = lineData[4]
                self.mesL1 = lineData[5]
                self.mesL2 = lineData[6]
                self.mesL3 = lineData[7]
                for timeMes in self.dayMes:
                    lineTime = self.hour + self.minute
                    if lineTime == timeMes['time']:
                        timeMes['L1'] += int(self.mesL1, 16)
                        timeMes['L2'] += int(self.mesL2, 16)
                        timeMes['L3'] += int(self.mesL3, 16)
                        timeMes['count'] += 1
            except:
                if lineData != ['']:
                    print('ERROR: {0}'.format(lineData))

    def countAVR(self):
        for timeMes in self.dayMes:
            if timeMes['count'] > 0:
                timeMes['L1'] = round(timeMes['L1'] / timeMes['count'], 2)
                timeMes['L2'] = round(timeMes['L2'] / timeMes['count'], 2)
                timeMes['L3'] = round(timeMes['L3'] / timeMes['count'], 2)

    def calculateHourAVR(self):
        self.hoursAvr = []
        for hour in range(0, 24):
            str_hour = '{0:02}'.format(hour)
            str_time = str_hour + '30'
            self.hoursAvr.append({'hour': str_time, 
                                  'L1': 0,
                                  'L2': 0,
                                  'L3': 0,
                                  'avrH': 0, 
                                  'count': 0})
            
        for timeMes in self.dayMes:
            for avrH in self.hoursAvr:
                if avrH['hour'][:2] == timeMes['time'][:2]:
                    if timeMes['count'] > 0:
                        avrH['L1'] += timeMes['L1']
                        avrH['L2'] += timeMes['L2']
                        avrH['L3'] += timeMes['L3']
                        avrH['count'] += 1
        
        for avrH in self.hoursAvr:
            if avrH['count'] > 0:
                avrH['L1'] = round(avrH['L1'] / avrH['count'], 2)
                avrH['L2'] = round(avrH['L2'] / avrH['count'], 2)
                avrH['L3'] = round(avrH['L3'] / avrH['count'], 2)
                avrH['avrH'] = round((avrH['L1'] + avrH['L2'] + avrH['L3']) / 3, 2)

print('Initialization')
lm = Measurement()
listOfFiles = glob.glob('Archive\\raw*.txt')
listOfFiles = glob.glob('raw*.txt')
for fileToRead in listOfFiles:
    print('Parsing : {0}'.format(fileToRead))
    f = open(fileToRead, 'r')
    readFile = f.read()
    f.close()
    lm.parseFile(readFile)

print('Calculating AVR')
lm.countAVR()

print('Calculating avr for hours')
lm.calculateHourAVR()

print('Preparing AVR data to save')
fileData='HHMM:avrL1:avrL2:avrL3\n'
for mes in lm.dayMes:
    fileData += '{0}:{1}:{2}:{3}\n'.format(mes['time'], mes['L1'],
                                           mes['L2'], mes['L3']) 
#save data to file
fileToSave = 'avrDay.txt'
print('Saving data to file: {0}'.format(fileToSave))
f = open(fileToSave, 'w')
f.write(fileData.replace('.', ','))
f.close()


print('Preparing AVR for Hour to save')
fileData='HHMM:avrALL:avrL1:avrL2:avrL3\n'
for mes in lm.hoursAvr:
    fileData += '{0}:{1}:{2}:{3}:{4}\n'.format(mes['hour'], 
                                           mes['avrH'],
                                           mes['L1'],
                                           mes['L2'],
                                           mes['L3']) 
#save data to file
fileToSave = 'avrHours.txt'
print('Saving data to file: {0}'.format(fileToSave))
f = open(fileToSave, 'w')
f.write(fileData.replace('.', ','))
f.close()
print('Done')
