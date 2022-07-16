
#define PULSADOR 2    // pulsador en pin 2
#define BUZZER_PASIVO 8   // buzzer pasivo en pin 8
#define NOTE_B0  31 // notas y frecuencias
#define ROJO 38
#define AZUL 39
#define VERDE 40
#define analogPin A0 //the thermistor attach to
#define beta 4090 //the beta of the thermistor
#define resistance 10 //the value of the pull-down resistor
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
const int rs = 12, en = 11, d4 = 31, d5 = 32, d6 = 33, d7 = 34;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// duracion de la nota en milisegundos

//read thermistor value
long a;
//the calculating formula of temperature
float tempC;
float tempF;
char estado;

enum State
{
  PosicionA,
  PosicionB,
  PosicionC,
  PosicionD
};

enum Input
{
  Unknown,
  Reset,
  Forward,
  Backward
};

// Variables globales
State currentState;
Input currentInput;

// Acciones de los estados y condiciones de transiciones
void stateA()
{
  if (currentInput == Input::Forward)
    changeState(State::PosicionB);
}

void stateB()
{
  if (currentInput == Input::Backward)
    changeState(State::PosicionA);
  if (currentInput == Input::Forward)
         changeState(State::PosicionC); 
  if (currentInput == Input::Reset)
     changeState(State::PosicionA);
}

void stateC()
{
  if (currentInput == Input::Backward)
    changeState(State::PosicionB);
  if (currentInput == Input::Forward)  
      changeState(State::PosicionD);
  if (currentInput == Input::Reset)
      changeState(State::PosicionA);
}

void stateD()
{ 
  if (currentInput == Input::Reset)
      changeState(State::PosicionB);
}

// Salidas asociadas a las transiciones
void outputA()
{
        digitalWrite(AZUL, HIGH);
        digitalWrite(ROJO, LOW);
        digitalWrite(VERDE, LOW);
        estado = 'A';
        
}

void outputB()
{
        digitalWrite(AZUL, LOW );
        digitalWrite(ROJO, LOW);
        digitalWrite(VERDE, HIGH);
        estado = 'B';
}

void outputC(){
        digitalWrite(AZUL, LOW );
        digitalWrite(ROJO, HIGH);
        digitalWrite(VERDE, LOW );
        estado = 'C';
}

void outputD()
{
       tone(BUZZER_PASIVO, NOTE_B0, 3000);
        digitalWrite(AZUL, HIGH );
        digitalWrite(ROJO, HIGH);
        digitalWrite(VERDE, LOW );
        estado = 'D';
}

void setup()
{

  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  currentState = PosicionA;
  outputA();
  pinMode(PULSADOR, INPUT_PULLUP);  // pin 2 como entrada con resistencia de pull-up
  pinMode(BUZZER_PASIVO, OUTPUT);
  pinMode(ROJO, OUTPUT);
  pinMode(AZUL, OUTPUT);
  pinMode(VERDE, OUTPUT);
  digitalWrite(AZUL, HIGH);
  digitalWrite(AZUL, LOW);
}

void loop() 
{

  readInput();
  updateStateMachine();
}

// Actualiza el estado de la maquina
void updateStateMachine()
{
  switch (currentState)
  {
    case PosicionA: stateA(); break;
    case PosicionB: stateB(); break;
    case PosicionC: stateC(); break;
    case PosicionD: stateD(); break;
  }
}


/*
  * Procedimiento que le dice a la maquia que hacer <br>
  * <b>post: </b> procedimiento que se encarga de analizar el caracter
  * que dice que accion debe hacer la maquina para ir al estado en el que debe 
  * estar
  * @see validarEstado()
*/

void readInput()
{


  currentInput = Input::Unknown;

    char incomingChar = validarEstado() ;

    switch (incomingChar)
    {
    case 'R': currentInput = Input::Reset;  break;
    case 'A': currentInput = Input::Backward; break;
    case 'D': currentInput = Input::Forward; break;
    default: break;
    }
  
}

/*
  * Funcion que identifica que se debe hacer segun el estado en el que se encuentre y en cual 
  * se debe encontrar la maquina <br>
  * <b>post: </b> procedimiento que se encarga de analizar en que estado se encuntra la maquina 
  * y en cual deberia estar segun la tempreatura actual, segun esto se manda una orden 
  * para que la maquina cambie al estado que debe estar
  * @return un caracter que dice que debe hace la maquina
  * @see retonarEstado()
   */

char validarEstado(){

    switch (currentState)
  {
    case PosicionA: 
             switch (retonarEstado())
                {
                  case 'A': return 'N'; break;
                  case 'B': return 'D'; break;
                  case 'C': return 'D'; break;
                  case 'D': return 'D'; break;
                }
         break;
    case PosicionB: 
              switch (retonarEstado())
                {
                  case 'A': return 'A'; break;
                  case 'B': return 'N'; break;
                  case 'C': return 'D'; break;
                  case 'D': return 'D'; break;
                }
         break;
    case PosicionC:  
                switch (retonarEstado())
                {
                  case 'A': return 'R'; break;
                  case 'B': return 'A'; break;
                  case 'C': return 'N'; break;
                  case 'D': return 'D'; break;
                }
         break;
    case PosicionD:
                switch (retonarEstado())
                {
                  case 'A': return 'R'; break;
                  case 'B': return 'R'; break;
                  case 'C': return 'R'; break;
                  case 'D': return 'N'; break;
                }
                 break;
  }
  
}



/*
  * Funcion que detecta la temperatura que se encuentra el sensor<br>
  * <b>post: </b> procedimiento que se encarga de analizar el valor de temperatura 
  * y le hace la conversion a grados centigrados para mostrarla en el lcd y 
  * despues compararla con la temperatura ya establecida para cada estado para poseteriormente
  * retornar en que estado se debe encontrar.
  * @return un caracter que dice en que estado debe estar la maquina
   */

char retonarEstado(){

  a =1023 - analogRead(analogPin);
  tempC = beta /(log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273.0;
  tempF = tempC + 273.15;

    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    // Print a centigrade temperature to the LCD.
    lcd.print(tempC);
    // Print the unit of the centigrade temperature to the LCD.
    lcd.print(" C");
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    lcd.print("Estado:  ");
    // Print a Fahrenheit temperature to the LCD.
    lcd.print(estado);
    // Print the unit of the Fahrenheit temperature to the LCD.
    
    delay(200); //wait for 100 milliseconds

  if(tempC < 18)
  return 'A';
  if(tempC > 18 && tempC <= 25)
  return 'B';
  if(tempC > 25 && tempC < 30)
  return 'C';
  if(tempC >= 30)
  return 'D';
}


/*
  * Funcion que cambia el estado y dispara las transiciones <br>
  * <b>post: </b> procedimiento que se encarga de analizar la varible @newState 
  * e identifica en que estado se debe encontrar la maquina y le una salida 
  * llamando una funcion segun el estado
  * @parameto: @newState
  * @see: outputA(), outputB(), outputA(), outputD()
*/
void changeState(int newState)
{
  currentState = newState;

  switch (currentState)
  {
    case State::PosicionA: outputA();   break;
    case State::PosicionB: outputB();   break;
    case State::PosicionC: outputC();   break;
    case State::PosicionD: outputD();   break;
    default: break;
  }
  delay(1000);
}
