#include <HCSR04.h>
#include <MovingAverage.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <PID_v1.h>

const int FILTER_LENGTH = 10;
const int NUM_COMMANDS = 6;

char commands[NUM_COMMANDS] = "FBLRSH";
char message[4];
char command;
int value;

long timer, timerOld = 0;
double dt = 0.0;
double yaw = 0.0;
double heading = 0.0;
double error = 0.0;
double throttle = 0.0;
bool forward = false;

HCSR04 hc(8, 12);
MovingAverage filter(FILTER_LENGTH);
Adafruit_MPU6050 mpu;

double setPoint, input, output;
double kp, ki, kd;
PID pid(&input, &output, &setPoint, kp, ki, kd, DIRECT);

void noOp() {}

void forwardCallback(char command, int value)
{
  throttle = value;
  forward = true;
}

void backwardsCallback(char command, int value)
{
  forward = false;
  driveLeft(value, false);
  driveRight(value, false);
}

void leftCallback(char command, int value)
{
  forward = false;
  driveLeft(value, false);
  driveRight(value, true);
}

void rightCallback(char command, int value)
{
  forward = false;
  driveLeft(value, true);
  driveRight(value, false);
}

void stopCallback(char command, int value)
{
  forward = false;
  analogWrite(9, 0);
  analogWrite(10, 0);
  analogWrite(11, 0);
  analogWrite(13, 0);
}

void headingCallback(char command, int value)
{
  heading = value;
  yaw = value;
  setPoint = heading;
  pid = PID(&input, &output, &setPoint, kp, ki, kd, DIRECT);
}

void driveLeft(int speed, bool direction)
{
  if (direction)
  {
    analogWrite(10, speed);
    analogWrite(9, 0);
  }
  else
  {
    analogWrite(10, 0);
    analogWrite(9, speed);
  }
}

void driveRight(int speed, bool direction)
{
  if (direction)
  {
    analogWrite(11, speed);
    analogWrite(13, 0);
  }
  else
  {
    analogWrite(11, 0);
    analogWrite(13, speed);
  }
}

void driveForward()
{
  if (!forward)
  {
    return;
  }
  input = error;
  pid.Compute();
  driveLeft(throttle + output, true);
  driveRight(throttle - output, true);
}

void (*callbacks[NUM_COMMANDS])(char command, int value) = {
    forwardCallback,   // F
    backwardsCallback, // B
    leftCallback,      // L
    rightCallback,     // R
    stopCallback,      // S
    headingCallback    // H
};

void setup()
{
  for (int i = 0; i < FILTER_LENGTH; i++)
  {
    filter.addSample(hc.dist());
  }
  Serial.begin(921600);
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);

  kp = 1;
  ki = 0.1;
  kd = 0.0;
  setPoint = heading;
  pid = PID(&input, &output, &setPoint, kp, ki, kd, DIRECT);
  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(-5000000, 5000000);
}

void loop()
{
  timerOld = timer;
  timer = micros();
  dt = (timer - timerOld) / 1000000.0;

  if (Serial.available() > 0)
  {
    Serial.readBytesUntil("\n", message, 4);
    while (Serial.available() > 0 && !isValidCommand(Serial.peek()))
    {
      Serial.read();
    }
    handleMessage();
  }
  writeSensorData();
  if (forward)
  {
    driveForward();
  }
}

void writeSensorData()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if (g.gyro.z > 0.01 || g.gyro.z < -0.03) {
    yaw = yaw + (((g.gyro.z + 0.015) * dt) * (180/PI));
    if (yaw >= 360) {
      yaw = yaw - 360;
    }
    if (yaw < 0) {
      yaw = yaw + 360;
    }
  }
  double yaw_x = cos(yaw * (PI / 180));
  double yaw_y = sin(yaw * (PI / 180));
  double heading_x = cos(heading * (PI / 180));
  double heading_y = sin(heading * (PI / 180));
  error = atan((yaw_y / yaw_x - heading_y / heading_x) / (1 + (yaw_y / yaw_x) * (heading_y / heading_x))) * (180 / PI);

  Serial.print("Y");
  Serial.println(yaw);
  Serial.flush();

  Serial.print("D");
  Serial.println(filter.addSample(hc.dist()));
  Serial.flush();
}

void handleMessage()
{
  command = message[0];
  value = atoi(&message[1]);
  bool valid = true;
  if (!isValidCommand(command))
  {
    valid = false;
    Serial.println("E:C");
    Serial.flush();
  }
  if (!isValidValue(value))
  {
    valid = false;
    Serial.println("E:V");
    Serial.flush();
  }
  if (valid)
  {
    int idx = commandIndex(command);
    Serial.println("A");
    Serial.flush();
    (*callbacks[idx])(command, value);
  }
}

bool isValidCommand(char command)
{
  return commandIndex(command) != -1;
}

bool isValidValue(int value)
{
  return value >= 0 && value <= 360;
}

int commandIndex(char command)
{
  int idx = -1;
  for (int i = 0; i < NUM_COMMANDS; i++)
  {
    if (command == commands[i])
    {
      idx = i;
    }
  }
  return idx;
}