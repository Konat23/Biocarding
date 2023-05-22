#define in1 A0

float lectura, signal1;
float voltaje1;
String datos,numero, indice;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(in1,INPUT);
}

void loop() {
  signal1 = analogRead(A0);

  voltaje1 = ((signal1/1023)*5.5);
  
  Serial.print(voltaje1);
  Serial.print(',');
  Serial.println(0.0);
  delay(100);

}
