'''
Created on 30.11.2017

@author: autoMaster
'''
import time

my_time = time.time()
from calculator import *

print(my_sub(34, 54))
print(my_sum(34, 54))
print(my_div(34, 54))
print(my_mul(34, 54))
print(time.time()-my_time)