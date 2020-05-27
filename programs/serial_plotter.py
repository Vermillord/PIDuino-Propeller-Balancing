# -*- coding: utf-8 -*-
"""
Created on Tue May 26 17:03:03 2020

@author: Vermillord
"""

import serial
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from threading import Thread

class serialPlot:
    def __init__ (self, serialPort='COM4', serialBaud=250000, plotLength=100):
        self.port = serialPort
        self.baud = serialBaud
        self.plotMaxLength = plotLength
        self.data = []
        self.angleData = []
        self.N_max = 100
        self.pwmLData = []
        self.pwmRData = []
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.plotTimer = 0
        self.prevTimer = 0
        
        print('Trying to connect to ' + str(serialPort) + ' at ' + str(serialBaud) + ' baud rate.')
        try:
            self.serialConnection = serial.Serial(serialPort, serialBaud)
            print('Connected to ' + str(serialPort) + ' at ' + str(serialBaud) + ' baud rate.')
        except:
            print('Failed connecting to ' + str(serialPort) + ' at ' + str(serialBaud) + ' baud rate.')
    
    def readSerialStart(self):
        if self.thread == None:
            self.thread = Thread(target=self.dataRetrieve)
            self.thread.start()
            while self.isReceiving != True:
                time.sleep(0.1)
    
    def getSerialData(self, frame, lines, lineValueText, lineLabel, timeText, pltNumber):
        if pltNumber == 0:
            currentTimer = time.perf_counter()
            self.plotTimer = int((currentTimer - self.prevTimer) * 1000)
            self.prevTimer = currentTimer
        timeText.set_text('Timer: ' + str(self.plotTimer) + ' ms')
        print('Data:' + str(self.data))
        print('Angle Data' + str(self.angleData))
        if pltNumber == 0 and len(self.angleData) == self.N_max:
            lines.set_data(range(self.plotMaxLength), self.angleData)
            lineValueText.set_text('[' + lineLabel + '] = ' +  self.data[0])
        if pltNumber == 1 and len(self.pwmLData) == self.N_max:
            lines.set_data(range(self.plotMaxLength), self.pwmLData)
            lineValueText.set_text('[' + lineLabel + '] = ' +  self.data[1])
        if pltNumber == 2 and len(self.pwmRData) == self.N_max:
            lines.set_data(range(self.plotMaxLength), self.pwmRData)
            lineValueText.set_text('[' + lineLabel + '] = ' +  self.data[2])
        
    def dataRetrieve(self):
        time.sleep(0.1)
        while (self.isRun):
            dat = self.serialConnection.readline()
            string_n = dat.decode()
            string = string_n.rstrip()
            string_s = string.split(",")
            self.data = string_s
            self.angleData.append(float(self.data[0]))
            if len(self.angleData) > self.N_max:
                self.angleData = self.angleData[-self.N_max:]
            self.pwmLData.append(float(self.data[1]))
            if len(self.angleData) > self.N_max:
                self.pwmLData = self.pwmLData[-self.N_max:]
            self.pwmRData.append(float(self.data[2]))
            if len(self.angleData) > self.N_max:
                self.pwmRData = self.pwmRData[-self.N_max:]
            self.isReceiving = True
        
    def close(self):
        self.isRun = False
        self.thread.join()
        self.serialConnection.close()
        print('Disconnected')

def makeFig(xLim, yLim, title, dataName):
    fig = plt.figure()
    ax = plt.axes(xlim=(xLim[0], xLim[1]), ylim=(yLim[0], yLim[1]))
    ax.set_title(title)
    ax.set_xlabel("Time")
    ax.set_ylabel(dataName)
    return fig, ax

def main():
    portName = 'COM3'
    baudRate = 250000
    maxPlotLength = 100
    numPlots = 3
    
    s = serialPlot(portName, baudRate, maxPlotLength)
    s.readSerialStart()
    
    updateInterval = 50
    lineLabelText = ['Angle','Left Motor','Right Motor']
    xLim = [(0,maxPlotLength),(0,maxPlotLength),(0,maxPlotLength)]
    yLim = [(-(45),45),(900,2100),(900,2100)]
    title = ['Angle Value','Left Motor PWM Value','Right Motor PWM Value']
    dataName = ['Degrees', 'PWM', 'PWM']
    style = ['r-','g-','b-']
    anim = []
    
    for i in range(numPlots):
        fig, ax = makeFig(xLim[i], yLim[i], title[i], dataName[i])
        lines, = ax.plot([], [], style[i], label=lineLabelText[i])
        timeText = ax.text(0.50, 0.95, '', transform=ax.transAxes)
        lineValueText = ax.text(0.50, 0.95, '', transform=ax.transAxes)
        anim.append(animation.FuncAnimation(fig, s.getSerialData, fargs=(lines, lineValueText, lineLabelText[i], timeText, i), interval=updateInterval))
        plt.legend(loc="upper left")
    plt.show()
    
    s.close()
    
if __name__ == '__main__':
    main()