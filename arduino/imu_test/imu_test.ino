#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

long timer, timerOld = 0;
double dt = 0.0;
double yaw = 0.0;
double heading = 0.0;
double error = 0.0;

void setup(void) {
  Serial.begin(115200);
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);
}

void loop() {
  timerOld = timer;
  timer = micros();
  dt = (timer - timerOld)/1000000.0; 

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
  double yaw_x = cos(yaw*(PI/180));
  double yaw_y = sin(yaw*(PI/180));
  double heading_x = cos(heading*(PI/180));
  double heading_y = sin(heading*(PI/180));
  error = atan((yaw_y/yaw_x - heading_y/heading_x)/(1 + (yaw_y/yaw_x)*(heading_y/heading_x))) * (180/PI);

  Serial.print(g.gyro.z);
  Serial.print(",");
  Serial.print(heading);
  Serial.print(",");
  Serial.print(yaw);
  Serial.print(",");
  Serial.println(error);
}