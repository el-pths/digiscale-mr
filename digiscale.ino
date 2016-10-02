
const int LED_PIN = 16;
const int SCK_PIN = 15;
const int DT_PIN = 14;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(DT_PIN, INPUT);
}

void loop() {
  Serial.println(((float)getScaledValue() - 59600)/217);
  delay(1000);
}

uint32_t getScaledValue(){
  digitalWrite(SCK_PIN, LOW);
  while(digitalRead(DT_PIN) == HIGH);
  uint32_t value = 0;
  for(int i = 0; i < 24; i++){
    digitalWrite(SCK_PIN, HIGH);
    digitalWrite(SCK_PIN, LOW);
    value <<= 1;
    value |= (0x01 && digitalRead(DT_PIN));
  }
  digitalWrite(SCK_PIN, HIGH);
  digitalWrite(SCK_PIN, LOW);
  return value;
}
