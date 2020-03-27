#include <AFMotor.h>
#include <pt.h>

#define TRIGGER 52
#define ECHO 53

#define PIN_SENSOR_LEFT 32
#define PIN_SENSOR_RIGHT 33
#define PIN_SENSOR_UP 50
#define PIN_SENSOR_DOWN 51

#define MOVE_UP 1
#define MOVE_DOWN 2
#define MOVE_RIGHT 3
#define MOVE_LEFT 4
#define MOVE_UP_DIAGONAL_RIGHT 5
#define MOVE_DOWN_DIAGONAL_LEFT 6
#define MOVE_UP_DIAGONAL_LEFT 7
#define MOVE_DOWN_DIAGONAL_RIGHT 8
#define TURN_RIGHT 9
#define TURN_LEFT 10

#define SPEED 200
#define MAX_SPEED 10
#define MIN_SPEED 1

#define NUM_PARAMS 4

double t1 = 0;
double t2 = 0;
double t3 = 0;
double t4 = 0;
double t5 = 0;
double tInicio=0, tFin =0;
double tPromedio = 0;
int contadorTiempos = 0;
int banderaTiempos = 1;
double velocidad = 50;
long duracion, distancia;  // para Calcular distacia

const float sonido = 34300.0; // Velocidad del sonido en cm/s
const float umbral1 = 10.0;

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

int estado = 0;
int movimiento = 0;
int actualSpeed = MAX_SPEED;

String lecturaEntrada;
String movimientos;

struct pt hilo1;
struct pt hilo2;
struct pt hilo3;
struct pt hilo4;

void setup() {     
  
  /*Inicializacion de entradas y salidas*/
  Serial2.begin(9600);  
  Serial.begin(9600);
  
  /*Inicializacion de detector de obstaculos IR*/
  pinMode(PIN_SENSOR_RIGHT, INPUT);  //definir pin como entrada
  pinMode(PIN_SENSOR_LEFT, INPUT);  //definir pin como entrada
  pinMode(PIN_SENSOR_UP, INPUT);  //definir pin como entrada
  pinMode(PIN_SENSOR_DOWN, INPUT);  //definir pin como entrada
  
  /*Inicializacion de sensor ultrasonico*/
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);
  
  /*Inicializacion de Motor Power Shield*/
  motor1.setSpeed(200);
  motor2.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);

  /*Iniciando Hilos*/
  PT_INIT(&hilo1);
  PT_INIT(&hilo2);
  PT_INIT(&hilo3);
  PT_INIT(&hilo4);
}

void loop() {
  readBluetooth();
  readIR();
  delay(100);
}

/*
Movimiento en diferentes direcciones:

-MOVIMIENTO HACIA ADELANTE:
  motor1 FORWARD
  motor2 FORWARD
  motor3 FORWARD
  motor4 FORWARD
-MOVIMIENTO HACIA ATRAS:
  motor1 BACKWARD
  motor2 BACKWARD
  motor3 BACKWARD
  motor4 BACKWARD
-MOVIMIENTO HACIA LA DERECHA:
  motor1 FORWARD
  motor2 BACKWARD
  motor3 BACKWARD
  motor4 FORWARD
-MOVIMIENTO HACIA LA IZQUIERDA:
  motor1 BACKWARD
  motor2 FORWARD
  motor3 FORWARD
  motor4 BACKWARD
-MOVIMIENTO EN DIAGONAL DERECHA SUPERIOR:
  motor1 FORWARD
  motor2 RELEASE
  motor3 RELEASE
  motor4 FORWARD
-MOVIMIENTO EN DIAGONAL IZQUIERDA INFERIOR:
  motor1 BACKWARD
  motor2 RELEASE
  motor3 RELEASE
  motor4 BACKWARD
-MOVIMIENTO EN DIAGONAL IZQUIERDA SUPERIOR:
  motor1 RELEASE
  motor2 FORWARD
  motor3 FORWARD
  motor4 RELEASE
-MOVIMIENTO EN DIAGONAL DERECHA INFERIOR:
  motor1 RELEASE
  motor2 BACKWARD
  motor3 BACKWARD
  motor4 RELEASE
-GIRO HACIA LA IZQUIERDA:
  motor1 BACKWARD
  motor2 FORWARD
  motor3 BACKWARD
  motor4 FORWARD
-GIRO HACIA LA DERECHA:
  motor1 FORWARD
  motor2 BACKWARD
  motor3 FORWARD
  motor4 BACKWARD
*/

void moveMotor(String datos[NUM_PARAMS]){
  int motor = atoi(datos[0].c_str());
  int resultSpeed = atoi(datos[2].c_str());
  //strcpy(motor, datos[0].c_str());
  if(resultSpeed > MAX_SPEED){
    resultSpeed = MAX_SPEED;
  } else if (resultSpeed < MIN_SPEED){
    resultSpeed = MIN_SPEED;
  }
  resultSpeed = getRealSpeed(resultSpeed);
  double tiempo = atoi(datos[1].c_str()) / getLinealSpeed(atoi(datos[2].c_str()));
  int direccion = atoi(datos[3].c_str());
  if(direccion == 1){
    direccion = FORWARD;
  } else {
    direccion = BACKWARD;
  }

  /*
  switch(motor){
    case 1:
    motor1.setSpeed(resultSpeed);
    Serial.println("a");
    //motor1.run(direccion);
    break;
    case 2:
    motor2.setSpeed(resultSpeed);
    Serial.println("b");
    //motor2.run(direccion);
    break;
    case 3:
    motor3.setSpeed(resultSpeed);
    Serial.println("c");
    //motor3.run(direccion);
    break;
    case 4:
    motor4.setSpeed(resultSpeed);
    Serial.println("d");
    //motor4.run(direccion);
    break;
  }
  */
  moveMotorNum(resultSpeed, motor, direccion);
  
  Serial.println(resultSpeed);
}

void moveMotorNum(int realSpeed, int motorNum, int motorDirection){
  switch(motorNum){
    case 1:
      motor1.setSpeed(realSpeed);
      motor1.run(motorDirection);
      break;
    case 2:
      motor2.setSpeed(realSpeed);
      motor2.run(motorDirection);
      break;
    case 3:
      motor3.setSpeed(realSpeed);
      motor3.run(motorDirection);
      break;
    case 4:
      motor4.setSpeed(realSpeed);
      motor4.run(motorDirection);
      break;
  }
}

void moveMotor(int relativeSpeed, int motorNum, int motorDirection){
  if(relativeSpeed > MAX_SPEED){
    relativeSpeed = MAX_SPEED;
  } else if (relativeSpeed < MIN_SPEED){
    relativeSpeed = MIN_SPEED;
  }
  int resultSpeed = getRealSpeed(relativeSpeed);
  moveMotorNum(resultSpeed, motorNum, motorDirection);
}

/*Funcion que devuelve la velocidad real basada en una velocidad relativa
 *Resultado calculado a partir de una regla de 3, por ejemplo (5 * 200) / 10 = 100
 */
double getRealSpeed(int relativeSpeed){
  return (relativeSpeed * SPEED) / MAX_SPEED;
}

double getLinealSpeed(int relativeSpeed){
  return (relativeSpeed * velocidad) / MAX_SPEED;
}

//Funcion encargada de mover el objeto hacia adelante
void moveUp(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, FORWARD);
  moveMotor(speed2, 2, FORWARD);
  moveMotor(speed3, 3, FORWARD);
  moveMotor(speed4, 4, FORWARD);
}

//Funcion encargada de mover el objeto hacia atras
void moveDown(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, BACKWARD);
  moveMotor(speed2, 2, BACKWARD);
  moveMotor(speed3, 3, BACKWARD);
  moveMotor(speed4, 4, BACKWARD);
}

//Funcion encargada de mover el objeto hacia la derecha
void moveRight(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, FORWARD);
  moveMotor(speed2, 2, BACKWARD);
  moveMotor(speed3, 3, BACKWARD);
  moveMotor(speed4, 4, FORWARD);
}

//Funcion encargada de mover el objeto hacia la izquierda
void moveLeft(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, BACKWARD);
  moveMotor(speed2, 2, FORWARD);
  moveMotor(speed3, 3, FORWARD);
  moveMotor(speed4, 4, BACKWARD);
}

void moveUpDiagonalRight(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, FORWARD);
  notMoveMotor(2);
  notMoveMotor(3);
  moveMotor(speed4, 4, FORWARD);
}

void moveDownDiagonalLeft(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, BACKWARD);
  notMoveMotor(2);
  notMoveMotor(3);
  moveMotor(speed4, 4, BACKWARD);
}

void moveUpDiagonalLeft(int speed1, int speed2, int speed3, int speed4){
  notMoveMotor(1);
  moveMotor(speed2, 2, FORWARD);
  moveMotor(speed3, 3, FORWARD);
  notMoveMotor(4);
}

void moveDownDiagonalRight(int speed1, int speed2, int speed3, int speed4){
  notMoveMotor(1);
  moveMotor(speed2, 2, BACKWARD);
  moveMotor(speed3, 3, BACKWARD);
  notMoveMotor(4);
}

void turnLeft(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, BACKWARD);
  moveMotor(speed2, 2, FORWARD);
  moveMotor(speed3, 3, BACKWARD);
  moveMotor(speed4, 4, FORWARD);
}

void turnRight(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, FORWARD);
  moveMotor(speed2, 2, BACKWARD);
  moveMotor(speed3, 3, FORWARD);
  moveMotor(speed4, 4, BACKWARD);
}

void notMoveMotor(int motorNum){
  switch(motorNum){
  case 1:
    motor1.run(RELEASE);
    break;
  case 2:
    motor2.run(RELEASE);
    break;
  case 3:
    motor3.run(RELEASE);
    break;
  case 4:
    motor4.run(RELEASE);
    break;
  }
}

//Funcion encargada de detener el objeto
void notMove(){
  notMoveMotor(1);
  notMoveMotor(2);
  notMoveMotor(3);
  notMoveMotor(4);
}

/*Funcion encargada de leer la entrada de bluetooth. 
 *Dependiendo de la señal de entrada de bluetooth se moveran los motores
 */
void readBluetooth(){
  if(Serial2.available()){        // lee el bluetooth y almacena en estado
    lecturaEntrada = Serial2.readString();
    Serial.println(lecturaEntrada);
    int n = lecturaEntrada.length();
    char char_array[n];
    char moveMotorA[n];
    char moveMotorB[n];
    char moveMotorC[n];
    char moveMotorD[n];
    strcpy(char_array, lecturaEntrada.c_str());
    int count = sscanf(char_array, "<%[^>]><%[^>]><%[^>]><%[^>]>", moveMotorA, moveMotorB, moveMotorC, moveMotorD);
    if (count == 4){
      
      char move01[NUM_PARAMS][n];
      char move02[NUM_PARAMS][n];
      char move03[NUM_PARAMS][n];
      char move04[NUM_PARAMS][n];
            
      sscanf(moveMotorA, "(%[^)])(%[^)])(%[^)])(%[^)])", move01[0], move01[1], move01[2], move01[3]);
      sscanf(moveMotorB, "(%[^)])(%[^)])(%[^)])(%[^)])", move02[0], move02[1], move02[2], move02[3]);
      sscanf(moveMotorC, "(%[^)])(%[^)])(%[^)])(%[^)])", move03[0], move03[1], move03[2], move03[3]);
      sscanf(moveMotorD, "(%[^)])(%[^)])(%[^)])(%[^)])", move04[0], move04[1], move04[2], move04[3]);

      String str01[NUM_PARAMS] = {move01[0], move01[1], move01[2], move01[3]};
      String str02[NUM_PARAMS] = {move02[0], move02[1], move02[2], move02[3]};
      String str03[NUM_PARAMS] = {move03[0], move03[1], move03[2], move03[3]};
      String str04[NUM_PARAMS] = {move04[0], move04[1], move04[2], move04[3]};

      /*los cuatro deben moverse ibdependientemente*/
      
      /*
      moveMotor(str01);
      moveMotor(str02);
      moveMotor(str03);
      moveMotor(str04);
      */
      int resultSpeed1 = getRealSpeed(atoi(str01[2].c_str()));

      /*Si ster01 != null ejecutar*/
      double tiempo1 = (atoi(str01[1].c_str()) / getLinealSpeed(atoi(str01[2].c_str())))*1000;
      double tiempo2 = (atoi(str02[1].c_str()) / getLinealSpeed(atoi(str02[2].c_str())))*1000;
      double tiempo3 = (atoi(str03[1].c_str()) / getLinealSpeed(atoi(str03[2].c_str())))*1000;
      double tiempo4 = (atoi(str04[1].c_str()) / getLinealSpeed(atoi(str04[2].c_str())))*1000;
      Serial.println(tiempo1);
      Serial.println(tiempo2);
      Serial.println(tiempo3);
      Serial.println(tiempo4);  


      double tiempoMayor = tiempo1;
      if(tiempo2 > tiempoMayor){
        tiempoMayor = tiempo2;
      } else if (tiempo3 > tiempoMayor){
        tiempoMayor = tiempo3;
      } else if (tiempo4 > tiempoMayor){
        tiempoMayor = tiempo4;
      }

      double tiempoActual = millis();
      Serial.println("TiempoActual");
      Serial.println(tiempoActual);
      
      moveMotor(str01);
      moveMotor(str02);
      moveMotor(str03);
      moveMotor(str04);

      
      while(millis() < tiempoActual + tiempoMayor){
        if(millis() > tiempoActual + tiempo1){
          notMoveMotor(1);
          Serial.println("Parando 1");
          Serial.println(millis());
        }
        if(millis() > tiempoActual + tiempo2){
          notMoveMotor(2);
          Serial.println("Parando 2");
          Serial.println(millis());
        }
        if(millis() > tiempoActual + tiempo3){
          notMoveMotor(3);
          Serial.println("Parando 3");
          Serial.println(millis());
        }
        if(millis() > tiempoActual + tiempo4){
          notMoveMotor(4);
          Serial.println("Parando 4");
          Serial.println(millis());
        }
      }
      notMove();
      
      //hiloMotor1(&hilo1, tiempo1, str01);
      /*Si ster02 != null ejecutar*/
      //hiloMotor2(&hilo2, tiempo2, str02);
      /*Si ster03 != null ejecutar*/
      //hiloMotor3(&hilo3, tiempo3, str03);
      /*Si ster04 != null ejecutar*/
      //hiloMotor4(&hilo4, tiempo4, str04);

      //delay(tiempo*1000);
      //notMove();
      
    } else {
      searchEstado(lecturaEntrada);
    }
  }
}

void searchEstado(String estado){
  char state[estado.length()];
  //Serial.println(estado);
  strcpy(state, estado.c_str());
  switch(state[0]){
    case 'F':
    moveUp(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_UP;
    break;
    case 'L':
    moveLeft(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_LEFT;
    break;
    case 'S':
    notMove();
    break;
    case 'R':
    moveRight(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_RIGHT;
    break;
    case 'G':
    moveDown(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_DOWN;
    break;
    case 'E':
    moveUpDiagonalRight(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_UP_DIAGONAL_RIGHT;
    break;
    case 'Z':
    moveDownDiagonalLeft(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_DOWN_DIAGONAL_LEFT;
    break;
    case 'Q':
    moveUpDiagonalLeft(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_UP_DIAGONAL_LEFT;
    break;
    case 'C':
    moveDownDiagonalRight(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_DOWN_DIAGONAL_RIGHT;
    break;
    case 'X':
    turnLeft(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = TURN_LEFT;
    break;
    case 'Y':
    turnRight(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = TURN_RIGHT;
    break;
    case '1':
    actualSpeed = 1;
    break;
    case '2':
    actualSpeed = 2;
    break;
    case '3':
    actualSpeed = 3;
    break;
    case '4':
    actualSpeed = 4;
    break;
    case '5':
    actualSpeed = 5;
    break;
    case '6':
    actualSpeed = 6;
    break;
    case '7':
    actualSpeed = 7;
    break;
    case '8':
    actualSpeed = 8;
    break;
    case '9':
    actualSpeed = 9;
    break;
    case '0':
    actualSpeed = 10;
    break;
    case 'M':
    Serial.println("Voy A entrar a recalibracion\n");
    reiniciarRecalibracion();
    recalibrar();
    break;
    case 'N':
     Serial.println("Me movere con velocidad de:");
    Serial.println(velocidad);
    moverConVelocidad(100, velocidad);
    break;
    }
}

/*Funcion encargada de detectar proximidad por los costados del objeto*/
int readIR(){
  int upSensor = 1;
  int downSensor = 1;
  int rightSensor = 1;
  int leftSensor = 1;
  upSensor = digitalRead(PIN_SENSOR_UP);  //lectura digital de pin
  downSensor = digitalRead(PIN_SENSOR_DOWN);
  rightSensor = digitalRead(PIN_SENSOR_RIGHT);
  leftSensor = digitalRead(PIN_SENSOR_LEFT);
  
  switch(movimiento){
  case MOVE_UP:
  if(upSensor == LOW){
    notMove();
  }
  break;
  case MOVE_LEFT:
  if(leftSensor == LOW){
    notMove();
  }
  break;
  case MOVE_RIGHT:
  if(rightSensor == LOW){
    notMove();
  }
  break;
  case MOVE_DOWN:
  if(downSensor == LOW){
    notMove();
  }
  break;
  case MOVE_UP_DIAGONAL_RIGHT:
  if(upSensor == LOW || rightSensor == LOW){
    notMove();
  }
  break;
  case MOVE_DOWN_DIAGONAL_LEFT:
  if(downSensor == LOW || leftSensor == LOW){
    notMove();
  }
  break;
  case MOVE_UP_DIAGONAL_LEFT:
  if(upSensor == LOW || leftSensor == LOW){
    notMove();
  }
  break;
  case MOVE_DOWN_DIAGONAL_RIGHT:
  if(downSensor == LOW || rightSensor == LOW){
    notMove();
  }
  break;
  }
}

int proximity(){
  // Preparamos el sensor de ultrasonidos
  iniciarTrigger();

  // Obtenemos la distancia
  float distancia = calcularDistancia();
  return alerta(distancia);
}

// Función que comprueba si hay que lanzar alguna alerta visual o sonora
int alerta(float distancia)
{
  // Lanzamos alerta si estamos dentro del rango de peligro
  if (distancia <= umbral1)
  {
    // Lanzamos alertas
    Serial.println("Peligroooooooo!");
    return HIGH;
  }
  return LOW;
}

// Método que calcula la distancia a la que se encuentra un objeto.
// Devuelve una variable tipo float que contiene la distancia
float calcularDistancia()
{
  // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
  unsigned long tiempo = pulseIn(ECHO, HIGH);

  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
  // por eso se multiplica por 0.000001
  float distancia = tiempo * 0.000001 * sonido / 2.0;
  delay(500);

  return distancia;
}

// Método que inicia la secuencia del Trigger para comenzar a medir
void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);

  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);

  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(TRIGGER, LOW);
}



void recalibrar(){
   
   tInicio = millis();
   Serial.println("TINicio: ");
    Serial.println(tInicio);
   moveUp(actualSpeed,actualSpeed,actualSpeed,actualSpeed);
  if(contadorTiempos < 5){
    do{
     digitalWrite(TRIGGER, LOW);   // genera el pulso de trigger por 10us
     delayMicroseconds(2);
     digitalWrite(TRIGGER, HIGH);
     delayMicroseconds(2);
     digitalWrite(TRIGGER, LOW);
     duracion = pulseIn(ECHO, HIGH);          // Lee el tiempo del Echo
     duracion = duracion/2;
     distancia = (duracion) / 29;            // calcula la distancia en centimetros
     delay(2); 
    }while(distancia >5);
     
     if (distancia <= 5){    // si la distancia es menor de 5cm
         tFin = millis();
         notMove();
         tFin = tFin - tInicio;
         
         if(contadorTiempos==0){
           t1 = tFin;
             Serial.println(t1);
         }else if(contadorTiempos==1){
           t2 = tFin;
             Serial.println(t2);

         }else if(contadorTiempos==2){
           t3 = tFin;
             Serial.println(t3);

         }else if(contadorTiempos==3){
           t4 = tFin;
             Serial.println(t4);

         }else if(contadorTiempos==4){
           t5 = tFin;
             Serial.println(t5);

         }
         contadorTiempos++;      
         delay(4000);
         tInicio=0;
         tFin=0;
         if(contadorTiempos<=5){
          recalibrar(); 
         }
         
     } 
  }else{
    Serial.println("Entre a calcular velocidad");
    contadorTiempos = 0;
    tPromedio = (t1+t2+t3+t4+t5)/5000;
    Serial.println("TPromedio");
    Serial.println(tPromedio);
    velocidad = 100 / tPromedio;
    Serial.println("Velocidad:");  
   Serial.println(velocidad);  
  }
  
}

void moverConVelocidad(double distancia, double velocidad){
    double tiempo = distancia/velocidad;
    Serial.println("Me movere un tiempo de ");
    Serial.println(tiempo);
    Serial.println("Valor actual speed");
    Serial.println(actualSpeed);
    moveUp(actualSpeed,actualSpeed,actualSpeed,actualSpeed);
    movimiento = MOVE_UP;
    delay(tiempo*1000);
    notMove(); 
}

void reiniciarRecalibracion(){
 contadorTiempos=0;
 t1=0;
 t2=0;
 t3=0;
 t4=0;
 t5=0; 
 tInicio=0; 
 tFin =0;
 tPromedio = 0;
 contadorTiempos = 0;
 banderaTiempos = 1;
 Serial.println("t1");
 Serial.println(t1);
 Serial.println("t2");
  Serial.println(t2);
 Serial.println("t3");
  Serial.println(t3);
 Serial.println("t4");
  Serial.println(t4);
 Serial.println("t5");
  Serial.println(t5);
 
 Serial.println("tinicio");
  Serial.println(tInicio);
  Serial.println("tfin");
   Serial.println(tFin);
  
  
 
}

/*Hilos para cada motor*/
void hiloMotor1(struct pt *pt, double tiempo, String ins_1[]) {

  //Inicia pt Trhead con el puntero que le envian
  PT_BEGIN(pt);
  // void setup() {
  static long t = 0;
  //pinMode(10, OUTPUT);
  // }
  moveMotor(ins_1);
  do {
  // void loop() {
    //moveMotor(ins_1);
    t = millis();
    PT_WAIT_WHILE(pt, (millis()-t)< (tiempo*1000));
    
    //moveMotor(ins_1);
    t = millis();
    PT_WAIT_UNTIL(pt, (millis()-t)>=(tiempo*1000));
  // }
  } while(true);

  //int motor_num = atoi(ins_1[0].c_str());
  /*Parando motor*/
  notMoveMotor(atoi(ins_1[0].c_str()));
  
  
  PT_END(pt);//termina el proto trehead
}

void hiloMotor2(struct pt *pt, double tiempo, String ins_1[]) {

  //Inicia pt Trhead con el puntero que le envian
  PT_BEGIN(pt);
  // void setup() {
  static long t = 0;
  //pinMode(10, OUTPUT);
  // }

  moveMotor(ins_1);
  do {
  // void loop() {
    //moveMotor(ins_1);
    t = millis();
    PT_WAIT_WHILE(pt, (millis()-t)< (tiempo*1000));
    
    //moveMotor(ins_1);
    t = millis();
    PT_WAIT_UNTIL(pt, (millis()-t)>=(tiempo*1000));
  // }
  } while(true);

    /*Parar motor*/
   notMoveMotor(atoi(ins_1[0].c_str()));
  
  PT_END(pt);//termina el proto trehead
}

void hiloMotor3(struct pt *pt, double tiempo, String ins_1[]) {

  //Inicia pt Trhead con el puntero que le envian
  PT_BEGIN(pt);
  // void setup() {
  static long t = 0;
  //pinMode(10, OUTPUT);
  // }

  moveMotor(ins_1);
  do {
  // void loop() {
    //moveMotor(ins_1);
    t = millis();
    PT_WAIT_WHILE(pt, (millis()-t)< (tiempo*1000));
    
    //moveMotor(ins_1);
    t = millis();
    PT_WAIT_UNTIL(pt, (millis()-t)>=(tiempo*1000));
  // }
  } while(true);
  
   /*Parar motor*/
   notMoveMotor(atoi(ins_1[0].c_str()));
   
  PT_END(pt);//termina el proto trehead
}
void hiloMotor4(struct pt *pt, double tiempo, String ins_1[]) {

  //Inicia pt Trhead con el puntero que le envian
  PT_BEGIN(pt);
  // void setup() {
  static long t = 0;
  //pinMode(10, OUTPUT);
  // }
  pinMode(10, OUTPUT);
  do {
  // void loop() {
    //moveMotor(ins_1);
    t = millis();
    PT_WAIT_WHILE(pt, (millis()-t)< (tiempo*1000));
    
    //moveMotor(ins_1);
    t = millis();
    PT_WAIT_UNTIL(pt, (millis()-t)>=(tiempo*1000));
  // }
  } while(true);

   /*Parar motor*/
   notMoveMotor(atoi(ins_1[0].c_str()));
  
  PT_END(pt);//termina el proto trehead
}
