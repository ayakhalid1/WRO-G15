void loop() {
  long distance = readUltrasonicCM();
  Serial.print("Distance: ");
  if(distance < 0) Serial.println("No reading");
  else { Serial.print(distance); Serial.println(" cm"); }

  if(distance > 0 && distance < OBSTACLE_DISTANCE){
    stopMotors();
    delay(200);

    // Read colors
    int redFreq = readColor(LOW, LOW);
    int greenFreq = readColor(LOW, HIGH);

    Serial.print("RED: "); Serial.print(redFreq);
    Serial.print("  GREEN: "); Serial.println(greenFreq);

    // Simple color comparison to decide direction
    if(greenFreq > redFreq) turnRightAvoid();  // green stronger → right
    else if(redFreq > greenFreq) turnLeftAvoid(); // red stronger → left
    else turnRightAvoid();                       // if equal, default right
  } else {
    forward();
    steeringServo.write(SERVO_CENTER);
  }

  delay(100);
}
