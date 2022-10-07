#include <HCSR04.h>

char commands[5] = "FBLRS";
char message[4];
char command;
int value;

HCSR04 hc(8, 12);

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

driveLeft(int speed, bool direction)
{
  if (direction)
  {
    analogWrite(9, speed);
    analogWrite(10, 0);
  }
  else
  {
    analogWrite(9, 0);
    analogWrite(10, speed);
  }
}

driveRight(int speed, bool direction)
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
  Serial.begin(9600);
}

void loop()
{
  Serial.flush();
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
  delay(250);
}

void writeSensorData()
{
  Serial.print("D");
  Serial.println(hc.dist());
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
  for (int i = 0; i < 4; i++)
  {
    if (command == commands[i])
    {
      idx = i;
    }
  }
  return idx;
}