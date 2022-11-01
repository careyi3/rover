#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

long timer, timerOld = 0;
double dt = 0.0;
double yaw = 0.0;

void setup(void) {
  Serial.begin(115200);
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
}

void loop() {
  timerOld = timer;
  timer = micros();
  dt = (timer - timerOld)/1000000.0; 

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if (g.gyro.z > 0.02 || g.gyro.z < -0.02) {
    yaw = yaw + g.gyro.z * dt;
  }
  Serial.println(yaw * 180/PI);
}