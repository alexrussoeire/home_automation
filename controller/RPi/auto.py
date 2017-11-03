"""
Demonstrates how to schedule multiple jobs and remove them one by one based on a counter.
"""

import schedule
import time
import os.path
import csv

def csv_test():
    f = open('test.csv')
    csv_f = csv.reader(f)

    for row in csv_f:
        print row

class FileTimeStamp():
    def __init__(self):
        self.timestamp = time.ctime(os.path.getmtime("greenhouse_schedule.csv"))
    
    def updateInitialTimeStamp(self, newTimeStamp):
        self.timestamp = newTimeStamp
        print(self.timestamp)
        
    def initialTimeStamp(self):
        return self.timestamp
    
def checkForSystemFileChange():
    initialTimeStamp = fileTimeStamp.initialTimeStamp()
    currentTimeStamp = time.ctime(os.path.getmtime("greenhouse_schedule.csv"))
    
    if initialTimeStamp != currentTimeStamp:        
        print("File has changed.")
        fileTimeStamp.updateInitialTimeStamp(currentTimeStamp)
        # Kill all the schedule tasks.
        
        # Read the new schedule from the test.csv file.
        
        # Create the new schedule tasks.
    else:
        print("File not changed.")

fileTimeStamp = FileTimeStamp()
schedule.every(1).seconds.do(checkForSystemFileChange)

while True:
    schedule.run_pending()
    time.sleep(1)