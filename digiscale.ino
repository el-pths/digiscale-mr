
#define LED_PIN 16
#define TARE_PIN 5
#define N_SENSORS 2

const int SCK_PIN[N_SENSORS] = {8, 6};
const int DT_PIN[N_SENSORS] = {9, 7};
const double CONVERSION_FACTOR[N_SENSORS] = {-2110, -1981};
long adjustment[N_SENSORS] = {0, 0};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TARE_PIN, INPUT_PULLUP);
  for(int i = 0; i<N_SENSORS; i++){
    pinMode(SCK_PIN[i], OUTPUT);
    pinMode(DT_PIN[i], INPUT);
  }
}

void loop() {
  if(digitalRead(TARE_PIN) == LOW){
    digitalWrite(LED_PIN, HIGH);
    for(int i = 0; i<N_SENSORS; i++){
      adjustment[i] = setAdjustment(SCK_PIN[i], DT_PIN[i]);
      adjustment[i] = adjustment[i]<0x888888?adjustment[i]:(adjustment[i] - 0xffffff);
    }
    digitalWrite(LED_PIN, LOW);
  }
  long value[N_SENSORS];
  getScaledValueSimultaneously((uint32_t *) value, SCK_PIN, DT_PIN, N_SENSORS);
  for(int i = 0; i<N_SENSORS; i++){
    value[i] = value[i]<0x888888?value[i]:(value[i] - 0xffffff);
    Serial.print((value[i] - adjustment[i])/CONVERSION_FACTOR[i]);
    Serial.print('\t');
  }
  Serial.println();
  delay(1000);
}

uint32_t setAdjustment(int sckPin, int dtPin){
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

void getScaledValueSimultaneously(uint32_t val[], const int sckPin[], const int dtPin[], int num){
  for(int i = 0; i<num; i++){
    digitalWrite(sckPin[i], LOW);
  }
  for(int i = 0; i<num; i++){
    while(digitalRead(dtPin[i]) == HIGH);
    val[i] = 0x00;
    for(int j = 0; j < 24; j++){
      digitalWrite(sckPin[i], HIGH);
      digitalWrite(sckPin[i], LOW);
      val[i] <<= 1;
      val[i] |= (0x01 && digitalRead(dtPin[i]));
    }
    digitalWrite(sckPin[i], HIGH);
    digitalWrite(sckPin[i], LOW);
  }
}

