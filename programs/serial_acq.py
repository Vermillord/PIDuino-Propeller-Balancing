# -*- coding: utf-8 -*-
"""
Created on Mon May 25 22:05:35 2020

@author: Vermillord
"""

import serial
import csv
from time import sleep

data_file = open('D:\\ITB\\FT\\TA\\PIDuino Balancing\\PIDuino_data_test.csv','w')
ser = serial.Serial('COM3', 250000)

print("connected to: " + ser.portstr)
sleep(2)

data =[]                       # empty list to store the data

with data_file:
    
    writer = csv.writer(data_file,delimiter=',')
    header = (['Angle','PWM_L','PWM_R'])
    writer.writerow(header)
    
    for i in range(10000):
        b = ser.readline()            # read a byte string
        string_n = b.decode()         # decode byte string into Unicode  
        string = string_n.rstrip()    # remove \n and \r
        data = string.split(",")      # splits data into a list
        print(data)
        writer.writerow(data)         # write data in csv
        data.append(string)           # add to the end of data list
        sleep(0.001)                  # wait (sleep) 0.1 seconds
    ser.close()
