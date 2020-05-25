#include <Wire.h>
#include <Servo.h>

#define RAD_TO_DEG 57.295779513082320876798154814105

Servo propR;
Servo propL;

/* Constants to store data from MPU6050 */
int16_t Acc_rawX, Acc_rawY, Acc_rawZ, Gyr_rawX, Gyr_rawY, Gyr_rawZ;

float Acc_angle[2];
float Gyr_angle[2];
float Total_angle[2];

float elapsedTime;
unsigned long time, timePrev;

float PID, pwmL, pwmR, err, errPrev;
float pid_p = 0;
float pid_i = 0;
float pid_d = 0;

/*------------------PID Constants------------------*/
double kp = 3.65;
double ki = 0.004;
double kd = 1.1;
/*-------------------------------------------------*/

/* Initial throttle and desired angle value */
double throttle = 1200;
float des_angle = 0;

void setup() {
  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(250000);
  
  // Attach the motor to the corresponding pins
  propR.attach(3);
  propL.attach(5);
  
  // Start time count in milliseconds
  time = millis();
  
  // Propeller PWM start-up with a value of 1000us
  propL.writeMicroseconds(1000);
  propR.writeMicroseconds(1000);

  // Delay for starting up
  delay(5000);
}

void loop() {
/*-------------------MPU-6050-------------------*/

  timePrev = time; // Store last counted time here
  time = millis(); // Read the actual time
  elapsedTime = (float(time) - float(timePrev)) / 1000; // To gain dt

  // Read the value of the accelerometers
  Wire.beginTransmission(0x68);
  Wire.write(0x3B); // Accelerometer data first address
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6, true); // Request 6 registers

  /* Assign the datas that were read by shifting the high values
   * by 8 bits (<< Operator) and add the low values by using or
   * (| operator). */
   
  Acc_rawX = Wire.read()<<8|Wire.read();
  Acc_rawY = Wire.read()<<8|Wire.read();
  Acc_rawZ = Wire.read()<<8|Wire.read();

  /* To obtain the value of acceleration in gravitational units, the raw
   * data must be divided by 16384.0 (according to the MPU6050 datasheet)
   * 
   * AccX = Acc_rawX/16384.0
   * 
   * Do this for each other angle. Apply the Euler Formula to calculate
   * the angle and change it to degrees using the constant RAD_TO_DEG
   * 
   * Acc_angleX = atan(AccY/sqrt(AccX^2 + AccZ^2))*RAD_TO_DEG
   * 
   * Do this for both X and Y angle.*/
  
  // X Angle
  Acc_angle[0] = atan((Acc_rawY/16384.0)/sqrt(pow((Acc_rawX/16384.0),2) + pow((Acc_rawZ/16384.0),2)))*RAD_TO_DEG;
  // Y Angle
  Acc_angle[1] = atan(-1*(Acc_rawX/16384.0)/sqrt(pow((Acc_rawY/16384.0),2) + pow((Acc_rawZ/16384.0),2)))*RAD_TO_DEG;
  
  // Read the value of the gyroscope
  Wire.beginTransmission(0x68);
  Wire.write(0x43); // Gyroscope data first address
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 4, true); // Request 4 registers

  /* Assign the datas that were read by shifting the high values
   * by 8 bits (<< Operator) and add the low values by using or
   * (| operator). */
   
  Gyr_rawX = Wire.read()<<8|Wire.read();
  Gyr_rawY = Wire.read()<<8|Wire.read();

  /* To obtain the value of gyro data in degrees/seconds units, the raw
   * data must be divided by 131 (according to the MPU6050 datasheet)
   * 
   * GyroX = Gyr_rawX/131.0
   * 
   * Do this for both X and Y angle.*/

  // X Angle
  Gyr_angle[0] = Gyr_rawX/131.0;
  // Y Angle
  Gyr_angle[1] = Gyr_rawY/131.0;

  /* Apply the Kalman Filter by combining the accelerometer data and
   * the gyroscope data. The function used for the filter is
   * 
   * AngleX = 0.98 *(AngleX+GyroX*dt) + 0.02*AccX
   * 
   * Do this for both X and Y angle.
   * Note: Try to change the Kalman Filter to a more stable filter.*/

  // X Angle
  Total_angle[0] = 0.98*(Total_angle[0] + Gyr_angle[0]*elapsedTime) + 0.02*Acc_angle[0];
  // Y Angle
  Total_angle[1] = 0.98*(Total_angle[1] + Gyr_angle[1]*elapsedTime) + 0.02*Acc_angle[1];

/*----------------------PID----------------------*/

  /* Calculate the error generated by angle read and the desired angle.
   * The angle that will be controlled is the Y angle, hence the X axis
   * needs to be facing the motors.*/
   
  err = Total_angle[1] - des_angle;

  // Proportional part
  pid_p = kp*err;

  // Integral part
  if(-3 < err < 3){
    pid_i = pid_i + (ki*err);
  }
  
  /* Note: The range of error can be changed. Currently its -2 to 2. It can be
   * changed to find the best value for tuning by using the integral part of
   * the PID control.*/

  // Derivative part
  pid_d = kd*((err - errPrev)/elapsedTime);

  // PID final value
  PID = pid_p + pid_i + pid_d;

  /* To make sure the PID value will change the PWM motor only in the range of
   * 1000us and 2000us, the maximum value that we can substract from 2000us is 1000us
   * and the maximum value we can add to 1000us is 1000us. Hence there needs to be a
   * function to make sure the value does not exceeds the motor PWM range.*/

  if(PID < -1000){
    PID = -1000;
  }
  if(PID > 1000){
    PID = 1000;
  }

  /* Calculate the PWM that will be sent to the motor by summing the PID value.*/

  pwmL = throttle + PID;
  pwmR = throttle - PID;

  /* Note: The function maybe can be changed to a PID function for right and left,
   * rather than the current which creates only one PID value and calculates the
   * PWM value based on the system as a whole (one time calculation only, positive
   for left and negative for right). */

  /* To make sure the PWM value sent to the motors will stay on the range of 1000us
   * and 2000us, there needs to be adjustment again by creating a maximum and
   * minimum value based on that range. */

  // Right motor
  if(pwmR < 1000){
    pwmR = 1000;
  }
  if(pwmR > 2000){
    pwmR = 2000;
  }
  if(pwmL < 1000){
    pwmL = 1000;
  }
  if(pwmL > 2000){
    pwmL = 2000;
  }

  /* Write the PWM value to each corresponding motors. */

  propR.writeMicroseconds(pwmR);
  propL.writeMicroseconds(pwmL);

  errPrev = err; // Store the previous error for the next loop

/*---------------------Graph---------------------*/

  /* Will add graph by using either arduino's Serial Plotter or
   * by using Processing. */
}