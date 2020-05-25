# PIDuino Propeller Balancing
![PIDuino](/images/787464resized.jpg)

Project in creating a platform for testing PID control. The control is to be used for balancing an arm by using two brushless motor with propellers. Currently uses Kalman Filter for filtering datas obtained from MPU6050. Files are to be uploaded which includes:

1. [Programs](https://github.com/Vermillord/PIDuino-Propeller-Balancing/tree/master/programs)
2. [3D parts](https://github.com/Vermillord/PIDuino-Propeller-Balancing/tree/master/3d%20models) (.STL files)
3. Circuit diagram

## Circuit
![Circuit](/images/circuit_bb.jpg)

The electrical system used is as shown in the above image. The components used are:
  1. Arduino UNO R3
  2. MPU6050
  3. ESC 30A
  4. Brushless Motor A2212 10000KV
  5. Power Supply 12V 30A

These components creates the whole system where the arduino acts as the main data processor. Data that are used for the arduino to control the motors are generated by the MPU6050. The motors used are connected to the ESCs differently to make sure that each motor either rotates CW or CCW.

## Notes
To be improved:
1. Graph for datas
2. Filter
3. PID function
4. Angle control

## References
1. Electronoobs. n.d. "PID control with arduino". Retrieved from: http://www.electronoobs.com/eng_robotica_tut6.php
2. Arduino Forum. 2011. "Guide to gyro and accelerometer with Arduino including Kalman filtering". Retrieved from: https://forum.arduino.cc/index.php?topic=58048.0
