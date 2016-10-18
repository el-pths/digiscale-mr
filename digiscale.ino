
const int LED_PIN = 16;
const int SCK_PIN = 15;
const int DT_PIN = 14;
const int TARE_PIN = 5;
const double CONVERSION_FACTOR = 217;
uint32_t adjustment = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(TARE_PIN, INPUT_PULLUP);
}

void loop() {
  if(digitalRead(TARE_PIN) == LOW){
    digitalWrite(LED_PIN, HIGH);
    setAdjustment();
    digitalWrite(LED_PIN, LOW);
  }
  Serial.println((long)(getScaledValue() - adjustment)/CONVERSION_FACTOR);
  delay(1000);
}

void setAdjustment(){
  int n = 10;
  uint32_t s = 0;
  for(int i = 0; i<n; i++){
    s += getScaledValue();
    delay(10);
  }
  adjustment = s/n;
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
