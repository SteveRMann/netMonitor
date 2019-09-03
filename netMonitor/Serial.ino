void beginSerial() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println(F(SketchName));
  Serial.println(F(sketchVersion));
  Serial.println();
}
