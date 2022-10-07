bool dir = true;

void setup() {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  drive(0, dir);
  delay(250);
  drive(50, dir);
  delay(250);
  drive(100, dir);
  delay(250);
  drive(200, dir);
  delay(250);
  drive(255, dir);
  delay(250);
  drive(255, dir);
  delay(250);
  drive(200, dir);
  delay(250);
  drive(150, dir);
  delay(250);
  drive(100, dir);
  delay(250);
  drive(50, dir);
  delay(250);
  dir = !dir;
}

void drive(int speed, bool direction) {
  if (direction) {
    analogWrite(9, speed);
    analogWrite(10, 0);
    analogWrite(11, speed);
    analogWrite(13, 0);
  } else {
    analogWrite(9, 0);
    analogWrite(10, speed);
    analogWrite(11, 0);
    analogWrite(13, speed);
  }
}