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

data =[]                       # List for storing data

with data_file:
    
    writer = csv.writer(data_file,delimiter=',')
    header = (['Angle','PWM_L','PWM_R'])
    writer.writerow(header)
    
    for i in range(10000):
        b = ser.readline()            # Read string
        string_n = b.decode()         # Decode string  
        string = string_n.rstrip()    
        data = string.split(",")      # Splits data into a list
        print(data)
        writer.writerow(data)         # Write data in CSV File
        data.append(string)           # Add data to list
        sleep(0.001)                  # Wait for 0.001 seconds
    ser.close()
