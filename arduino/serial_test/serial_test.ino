char commands[4] = "FBLR";
char message[4];
char command;
int value;

void noOp() {}

void forwardCallback(char command, int value) {
}

void (*callbacks[4])(char command, int value) = {
  forwardCallback,  // F
  noOp,             // B
  noOp,             // L
  noOp,             // R
};

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.flush();
  if (Serial.available() > 0) {
    Serial.readBytesUntil("\n", message, 4);
    while(Serial.available() > 0 && !isValidCommand(Serial.peek())) {
      Serial.read();
    }
    handleMessage();
  }
  Serial.print("D");
  Serial.println(random(255));
  Serial.flush();
  delay(250);
}

void handleMessage() {
  command = message[0];
  value = atoi(&message[1]);
  bool valid = true;
  if (!isValidCommand(command)) {
    valid = false;
    Serial.println("E:C");
    Serial.flush();
  }
  if (!isValidValue(value)) {
    valid = false;
    Serial.println("E:V");
    Serial.flush();
  }
  if (valid) {
    int idx = commandIndex(command);
    Serial.println("A");
    Serial.flush();
    (*callbacks[idx])(command, value);
  }
}

bool isValidCommand(char command) {
  return commandIndex(command) != -1;
}

bool isValidValue(int value) {
  return value >= 0 && value <= 255;
}

int commandIndex(char command) {
  int idx = -1;
  for (int i = 0; i < 4; i++) {
    if (command == commands[i]) {
      idx = i;
    }
  }
  return idx;
}