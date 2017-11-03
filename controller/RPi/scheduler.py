"""
Demonstrates how to schedule multiple jobs and remove them one by one based on a counter.
"""

import schedule
import time

class Counter():
    def __init__(self):
        self.count = 0
        
    def decr(self):
        if self.count > 0:
            self.count -= 1
            
    def incr(self):
        self.count += 1
        
    def val(self):
        return self.count

def job_1():
    print("job 1")
    cnt = counter.val()
    if cnt == 5:        
        print cnt
        schedule.clear('repeatTask')

def job_2():
    print("job 2")
    cnt = counter.val()
    if cnt == 10:        
        print cnt
        schedule.clear('allTasks')

def job_3():
    print("job 3")

def job_4():
    print("job 4")
    counter.incr()
    
    cnt = counter.val()
    if cnt == 15:
        print cnt
        schedule.every(1).seconds.do(job_1).tag('allTasks')

counter = Counter()
schedule.every(1).seconds.do(job_1).tag('allTasks')
schedule.every(1).seconds.do(job_2).tag('allTasks')
schedule.every(1).seconds.do(job_3).tag('repeatTask', 'allTasks')
schedule.every(1).seconds.do(job_4)

'''
schedule.every().hour.do(job)
schedule.every().day.at("10:30").do(job)
schedule.every().monday.do(job)
schedule.every().wednesday.at("13:15").do(job)
'''

while True:
    schedule.run_pending()
    time.sleep(1)