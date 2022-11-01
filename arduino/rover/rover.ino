#include <HCSR04.h>
#include <MovingAverage.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const int FILTER_LENGTH = 10;

char commands[5] = "FBLRS";
char message[4];
char command;
int value;

long timer, timerOld = 0;
double dt = 0.0;
double yaw = 0.0;

HCSR04 hc(8, 12);
MovingAverage filter(FILTER_LENGTH);
Adafruit_MPU6050 mpu;

void noOp() {}

void forwardCallback(char command, int value)
{
  driveLeft(value, true);
  driveRight(value, true);
}

void backwardsCallback(char command, int value)
{
  driveLeft(value, false);
  driveRight(value, false);
}

void leftCallback(char command, int value)
{
  driveLeft(value, false);
  driveRight(value, true);
}

void rightCallback(char command, int value)
{
  driveLeft(value, true);
  driveRight(value, false);
}

void stopCallback(char command, int value)
{
  analogWrite(9, 0);
  analogWrite(10, 0);
  analogWrite(11, 0);
  analogWrite(13, 0);
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

void (*callbacks[5])(char command, int value) = {
    forwardCallback,   // F
    backwardsCallback, // B
    leftCallback,      // L
    rightCallback,     // R
    stopCallback       // S
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
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
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
  // delay(50);
}

void writeSensorData()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if (g.gyro.z > 0.02 || g.gyro.z < -0.02)
  {
    yaw = yaw + g.gyro.z * dt;
  }
  Serial.print("Y");
  Serial.println(yaw * 180 / PI);
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
  return value >= 0 && value <= 255;
}

int commandIndex(char command)
{
  int idx = -1;
  for (int i = 0; i < 5; i++)
  {
    if (command == commands[i])
    {
      idx = i;
    }
  }
  return idx;
}