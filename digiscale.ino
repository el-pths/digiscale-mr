template<size_t Size> double getArithmeticMean(uint32_t (&values)[Size]);

const int LED_PIN = 16;
const int SCK_PIN = 15;
const int DT_PIN = 14;
const int TARE_PIN = 5;
double adjustment = 0;

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
  uint32_t arr[10];
  for(int i = 0; i<10; i++){
    arr[i] = getScaledValue();
    delay(10);
  }
  Serial.println((getArithmeticMean(arr) - adjustment)/217);
  delay(1000);
}

void setAdjustment(){
  uint32_t arr[10];
  for(int i = 0; i<10; i++){
    arr[i] = getScaledValue();
    delay(10);
  }
  adjustment = getArithmeticMean(arr);
}

template<size_t Size> double getArithmeticMean(uint32_t (&values)[Size]){
  double s = 0;
  for(int i = 0; i<Size; i++){
    s += values[i];
  }
  return s/Size;
}

template<size_t Size> double getStandardDeviation(uint32_t (&values)[Size]){
  double s = 0;
  double mean = getArithmeticMean(values);
  for(int i = 0; i<Size; i++){
    s += sq(values[i] - mean);
  }
  return sqrt(s/Size);
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
