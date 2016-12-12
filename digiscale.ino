
const int LED_PIN = 16;
const int TARE_PIN = 5;
const int SCK_PIN1 = 8;
const int DT_PIN1 = 9;
const int SCK_PIN2 = 6;
const int DT_PIN2 = 7;

const double CONVERSION_FACTOR1 = -2110;
const double CONVERSION_FACTOR2 = -1981;
long adjustment1 = 0;
long adjustment2 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TARE_PIN, INPUT_PULLUP);
  pinMode(SCK_PIN1, OUTPUT);
  pinMode(DT_PIN1, INPUT);
  pinMode(SCK_PIN2, OUTPUT);
  pinMode(DT_PIN2, INPUT);
}

void loop() {
  if(digitalRead(TARE_PIN) == LOW){
    digitalWrite(LED_PIN, HIGH);
    adjustment1 = adjustment(SCK_PIN1, DT_PIN1);
    adjustment2 = adjustment(SCK_PIN2, DT_PIN2);
    adjustment1 = adjustment1<0x888888?adjustment1:(adjustment1 - 0xffffff);
    adjustment2 = adjustment2<0x888888?adjustment2:(adjustment2 - 0xffffff);
    digitalWrite(LED_PIN, LOW);
  }
  long value1 = getScaledValue(SCK_PIN1, DT_PIN1);
  long value2 = getScaledValue(SCK_PIN2, DT_PIN2);
  value1 = value1<0x888888?value1:(value1 - 0xffffff);
  value2 = value2<0x888888?value2:(value2 - 0xffffff);
  Serial.print("first:\t");
  Serial.print((value1 - adjustment1)/CONVERSION_FACTOR1);
  Serial.print("\tsecond:\t");
  Serial.println((value2 - adjustment2)/CONVERSION_FACTOR2);
  delay(1000);
}

uint32_t adjustment(int sckPin, int dtPin){
  int n = 10;
  uint32_t s = 0;
  for(int i = 0; i<n; i++){
    s += getScaledValue(sckPin, dtPin);
    delay(10);
  }
  return s/n;
}

uint32_t getScaledValue(int sckPin, int dtPin){
  digitalWrite(sckPin, LOW);
  while(digitalRead(dtPin) == HIGH);
  uint32_t value = 0x00;
  for(int i = 0; i < 24; i++){
    digitalWrite(sckPin, HIGH);
    digitalWrite(sckPin, LOW);
    value <<= 1;
    value |= (0x01 && digitalRead(dtPin));
  }
  digitalWrite(sckPin, HIGH);
  digitalWrite(sckPin, LOW);
  return value;
}

void getScaledValueSimultaneously(uint32_t &val1, int sckPin1, int dtPin1, uint32_t &val2, int sckPin2, int dtPin2){
  digitalWrite(sckPin1, LOW);
  digitalWrite(sckPin2, LOW);
  while(digitalRead(dtPin1) == HIGH);
  val1 = 0x00;
  for(int i = 0; i < 24; i++){
    digitalWrite(sckPin1, HIGH);
    digitalWrite(sckPin1, LOW);
    val1 <<= 1;
    val1 |= (0x01 && digitalRead(dtPin1));
  }
  while(digitalRead(dtPin2) == HIGH);
  val2 = 0x00;
  for(int i = 0; i < 24; i++){
    digitalWrite(sckPin2, HIGH);
    digitalWrite(sckPin2, LOW);
    val2 <<= 1;
    val2 |= (0x01 && digitalRead(dtPin2));
  }
  digitalWrite(sckPin1, HIGH);
  digitalWrite(sckPin1, LOW);
  digitalWrite(sckPin2, HIGH);
  digitalWrite(sckPin2, LOW);
}

void writeUint(uint32_t v){
  char c = 0xff&v;
  Serial.write(c);
  c = (0xff00&v)>>8;
  Serial.write(c);
  c = (0xff0000&v)>>16;
  Serial.write(c);
  c = (0xff000000&v)>>24;
  Serial.write(c);
}

