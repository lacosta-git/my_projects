'''
Created on 24.11.2017

@author: autoMaster
'''
import os
from glob import glob

def createListOfFiles(dirPaths=[os.curdir]):
    listOfFiles = []
    for dirPath in dirPaths:
        pathName = os.path.join('', dirPath, '**', '*.jpg')
        listOfFiles.extend(glob(pathname=pathName, recursive = True))
    
    return listOfFiles
    
def getAttrOfFile(fPath=None):
    fName = 'ERROR.ERR'
    fSize = 0
    try: 
        fSize = os.path.getsize(fPath)
        fName = os.path.split(fPath)[1]
        return fName, fSize
    except:
        return fName, fSize

def buildFilesDatabase(listOfFiles = []):
    fDB = []
    for fFile in listOfFiles:
        fn, fs =  getAttrOfFile(fFile)
        fString = '{0}::{1};;{2}'.format(fn, fs, fFile)
        fDB.append(fString)
    return fDB

def listDuplicates(listOfEntries=[]):
    lastEntry = ''
    lastEntryNameSize = ''
    duplicates = []
    for entry in listOfEntries:
        entryNameSize = entry.split(';;')[0] 
        if entryNameSize == lastEntryNameSize:
            duplicates.append(lastEntry)
            duplicates.append(entry)
        lastEntry = entry
        lastEntryNameSize = entryNameSize
    return duplicates

if __name__ == '__main__':
    
    print('START')
    print('Reading list of files')
    myFiles = createListOfFiles(['C:\\030_Zdjecia'])
    
    print('Number of found files: ', len(myFiles))
    print('Building database')
    db = buildFilesDatabase(myFiles)
    
    print('Sorting database')
    db.sort()
    
    print('Building list of duplicates')
    dups = listDuplicates(db)
    
    print('Number of duplicated files : {}'.format(len(dups)))
    for dup in dups:
        print(dup)
    
    print('END')