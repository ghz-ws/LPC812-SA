import serial
import time
import numpy
import csv
import matplotlib.pyplot as plt

sg=serial.Serial("COM9",115200)
sa=serial.Serial("COM15",115200)
Start_Freq=1  ##MHz unit
Stop_Freq=100   ##MHz unit
Step_Freq=1     ##MHz unit
Wait=0.001      ##sec
LO=[-5,-5]      ##dBm unit. 50ohm loaded.
BW='2'          ##1=Wide BW, 2=narrow BW
Freq=[100,100]  ##ch3&4, MHz unit
Pha=[0,0]       ##ch3&4, deg. unit
Ampl=[-10,-10]  ##ch3&4, dBm unit. 50ohm loaded.

step=int((Stop_Freq-Start_Freq)/Step_Freq)
data=numpy.zeros((step,3))
sa.write(BW.encode())
for i in range(step):
    freq=Start_Freq+i*Step_Freq
    set_freq=434-freq
    buf=f'{set_freq*1000000:09}'+'000'+f'{LO[0]:+03}'+f'{set_freq*1000000:09}'+'000'+f'{LO[1]:+03}'+f'{Freq[0]*1000000:09}'+f'{Pha[0]:03}'+f'{Ampl[0]:+03}'+f'{Freq[1]*1000000:09}'+f'{Pha[1]:03}'+f'{Ampl[1]:+03}'
    sg.write(buf.encode())
    sa.write(b'0')
    time.sleep(Wait)
    buf=sa.read(5)
    data1=int(buf)
    buf=sa.read(5)
    data2=int(buf)
    print('Freq=',int(freq*1000000),'Hz, Ch1=',data1,'mV, Ch2=',data2,'mV')
    data[i][0]=int(freq*1000)
    data[i][1]=int(data1)
    data[i][2]=int(data2)
    
with open('data.csv','w',newline="") as f:
    writer=csv.writer(f)
    writer.writerows(data)
    
plt.subplot(1,2,1)
plt.plot(data[:,0],data[:,1])
plt.subplot(1,2,2)
plt.plot(data[:,0],data[:,2])
plt.show()