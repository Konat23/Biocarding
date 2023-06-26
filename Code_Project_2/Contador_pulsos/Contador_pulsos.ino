
#include <LiquidCrystal.h>

LiquidCrystal lcd(7,6,5,4,3,2);
int status = 0; // 0 En espera, 1 Contando, 2 Resultado
int t_Rest = 60;
int n_pulsos = 0;
bool bipnow = false;
int diag = 3; // 0 None, 1 Normal, 2 Taquicardia, 3 Bradicardia

const unsigned long duracionContador = 60;  // Duración del contador en segundos (1 minuto)
const unsigned int duracionbip = 100; // Duracion en milisegundos que estara sonando el bip
unsigned long tiempoInicio;
float inicioTimerbip;

const int btPin = 13;
const int pwmPin = 9;  // Pin de entrada para la señal PWM
const int bipPin = 8; // pin para conectar el sonido

const int greenPin = 10; // Noramal
const int yellowPin = 11; // Bradicardia
const int redPin = 12;  // Taquicardia

int currentState;
int previousState;

void showInfo() {
  
  
  // Para cuando esta esperando
  if (status == 0){ 
        
    lcd.setCursor(0,0);
    lcd.print("Biocarding.");
    lcd.print("      "); //Limpia pantalla
    lcd.setCursor(0,1); 
    lcd.print("Presione Inicio");  
    lcd.print("    "); //Limpia pantalla 
  
  }
  // Para cuando esta contando
  if (status == 1){ 
        
    lcd.setCursor(0,0);
    lcd.print("Tiempo rest.: ");
    lcd.print(t_Rest);
    lcd.print("    "); //Limpia pantalla
    lcd.setCursor(0,1);
    lcd.print("# Pulsos: ");
    lcd.print(n_pulsos);
    lcd.print("    "); //Limpia pantalla

  }
  
  // Para cuando termino de contar
  if (status == 2){ 
        
    lcd.setCursor(0,0);
    lcd.print("Frec. Card: ");
    lcd.print(n_pulsos);
    lcd.print("    "); //Limpia pantalla
    lcd.setCursor(0,1);
    lcd.print("Diag:");
    
    switch (diag) {
    case 0: // None
          lcd.print("None");          
           break;
    case 1: // Normal
          lcd.print("Normal");          
          break;
    case 2: // Taquicardia
          lcd.print("Taquicardia");          
          break;
    case 3: // Bradicardia
          lcd.print("Bradicardia");          
          break;
           //Puede haber los “case” que se deseen, y al final una sección default
    default:
          lcd.print("None");          
       //Instrucciones que se ejecutan si no se ha ejecutado ningún case anterior

  }
  lcd.print("    "); //Limpia pantalla

  }
  //delay(200);
  //lcd.clear();
  
}

void countPulses(){

  // Leer la señal PWM
  currentState = digitalRead(pwmPin);
  Serial.println(currentState);

  // Verifica si se ha producido un flanco positivo
  if (currentState == HIGH && previousState == LOW) { // Flanco detectado   
     n_pulsos++;
     bipnow = true;
     inicioTimerbip = millis();
  }

  previousState = currentState;    
}

void countPulses_test(){
  static float previous_time = millis();
  // Leer la señal PWM
  
  Serial.println("Actual");
  Serial.println(millis());
  Serial.println("Last");
  Serial.println(previous_time);
  // Verifica si se ha producido un flanco positivo
  if ((millis()-previous_time)>260) { // Para test, activar cada segundo
     n_pulsos++;
     bipnow = true;
     inicioTimerbip = millis();
     previous_time = millis();
  }
  
  previousState = currentState;    
}


void bipsound(){
  //   Esta funcion sera la encargada  de hacer que suene correctamente el bip
  //  bipnow determina si debe estar sonando el biper.
  if (bipnow){
      // Esto solo se ejecuta la primera vez que se llame la funcion.
    
      // Prender por 0.1 segs y luego apagar 
            
      if ((millis() - inicioTimerbip) < duracionbip){
        digitalWrite(bipPin, HIGH);
      }
      else{
        // Apagar el bipper
        digitalWrite(bipPin, LOW);
        bipnow = false;
      }

  }
  else{
    digitalWrite(bipPin, LOW);
    
  }
    
}

void diagnostic(){
  if(n_pulsos<60){ // Bradicardia
    digitalWrite(yellowPin, HIGH);
    diag = 3;
  }
  else if(n_pulsos>100){ // Taquicardia
    digitalWrite(redPin, HIGH);
    diag = 2;

  }
  else{ // Normal
    digitalWrite(greenPin, HIGH);
    diag = 1;
  }
  
  
}


void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  pinMode(btPin,INPUT);

  pinMode(pwmPin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(pwmPin), handleInterrupt, RISING);

  pinMode(bipPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  Serial.begin(9600);
  tiempoInicio = millis()/1000;  // Guardar el tiempo de inicio

}

void loop() {
  

  // determinar status (para lo que hay que leer la entrada del usurio y ver los temporizadores)
  //int status = get_status();
  
  switch (status) {
    case 0: // Espera
      // Le indica al usuario que esta listo
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, LOW);  
      digitalWrite(bipPin, LOW); 
      if (digitalRead(btPin)==HIGH){ // Si se presiona el boton, entonces cambia de estado a contar
        //Serial.println("Boton pulsado");
        delay(1000); // Evitar efecto rebote      
        status = -1;
      }    
      break;
    case -1: //Reinicio
      // Esto lo prepara para empezar a contar
      
       t_Rest = duracionContador;
       n_pulsos = 0;
       diag = 0; // 0 None, 1 Normal, 2 Taquicardia, 3 Bradicardia
       tiempoInicio = millis()/1000;  // Guardar el tiempo de inicio
       digitalWrite(yellowPin, LOW);
       digitalWrite(redPin, LOW);
       digitalWrite(greenPin, LOW);  
       digitalWrite(bipPin, LOW);

       status = 1; // 1 Contando. Lo manda a contar 
       
      break;
    
    case 1: // Contando
      // Hace el conteo de pulsos      
      countPulses();
      //countPulses_test();
      bipsound();
      t_Rest = duracionContador - (millis()/1000 - tiempoInicio);   
      if(t_Rest <= 0){ // Se acabo el tiempo
        status = 2;
      }  
      if (digitalRead(btPin)==HIGH){ // Si se presiona el boton mientras esta contando, entonces pasa a espera
        digitalWrite(bipPin, LOW);
        delay(1000); // Evitar efecto rebote      
        status = 0;
      }    
      
      break;
    case 2: // Mostrar resultados
      digitalWrite(bipPin, LOW);
      // Aqui no se cambian las variables, solo se espera a que el usuario presione para hacer otro 
       if (digitalRead(btPin)==HIGH){ // Si se presiona el boton, entonces cambia de estado a contar
          delay(1000); // Evitar efecto rebote  
          status = 0;
        }

        diagnostic();
        
    break;
    default:
      //Serial.println("Unexpected status");
    break;    
  }
  
  //  

  showInfo();
}







