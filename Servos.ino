//////////////////////////////////////////////////

void left(boolean doorOpen, unsigned long delayAfter) 
{
  if (doorOpen) servo1.write(open1);
  else servo1.write(closed1);
  nextAction = millis() + delayAfter;
  Serial.println(nextAction);
}

////////////////////////////////////////////////

void right(boolean doorOpen, unsigned long delayAfter) 
{
  if (doorOpen) servo2.write(open2);
  else servo2.write(closed2);
  nextAction = millis() + delayAfter;
    Serial.println(nextAction);
}

////////////////////////////////////////////////////////////////////////////

void both(boolean doorOpen, unsigned long delayAfter) {  //Open both doors function
  if (doorOpen) {
    servo1.write(open1);
    servo2.write(open2);
  }
  else {
    servo1.write(closed1);
    servo2.write(closed2);
  }
  nextAction = millis() + delayAfter;
  Serial.println(nextAction);
}



