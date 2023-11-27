void beginSerial() {
#ifndef ESP01
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F(VERSION));
  Serial.println();
#endif
}
