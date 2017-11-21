#define BUTTON_WAIT 500

void diag(){
  int count = 0;
  if (bigRedButton.update()){
    if (bigRedButton.fallingEdge()){
      count = countButtonPresses();
    }
  }
  Serial.print("Count button presses: ");
  Serial.print(count);
  Serial.println();

  if (count != 0){
    playFile(fileNameArr[count - 1]);
  }

  diag();
}

int countButtonPresses(){
  int pressCount = 0;
  unsigned long startTime = millis();
  unsigned long currTime = millis();

  while (currTime - startTime < BUTTON_WAIT){
    if (bigRedButton.update()){
      if (bigRedButton.risingEdge()){
        pressCount++;
        startTime = millis();
      }
    }
    currTime = millis();
  }

  return pressCount;
}

