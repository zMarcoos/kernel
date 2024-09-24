#include <TimerOne.h>
#include "kernel.h"

int distanceThreshold = 100;  // Distância máxima para detectar objetos em cm
int dangerThreshold = 40;     // Distância muito próxima para pisca-pisca
float cm = 0;
int ledState = 0;

unsigned long lastBlinkTime = 0;
unsigned long blinkInterval = 200;  // Intervalo para o pisca-pisca (em milissegundos)
unsigned long lastDistanceReadTime = 0; // Tempo da última leitura de distância
unsigned long distanceReadInterval = 500; // Intervalo de leitura de distância

const int triggerPin = 3;
const int echoPin = 2;
const int buzzer = 5;
const int redPin = 9;
const int greenPin = 8;

// Função não bloqueante para ler a distância do sensor ultrassônico
float getDistance() {
  // Envia um pulso de 10 microsegundos para o Trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Recebe o tempo de viagem do pulso no Echo
  long duracao = pulseIn(echoPin, HIGH, 30000);  // Timeout de 30ms

  // Calcula a distância em cm
  return duracao > 0 ? duracao * 0.034 / 2.0 : 0;
}

// Função para mostrar a distância medida
char showDistance(void) {
  Serial.print(cm);
  Serial.println(" cm");
  return REPEAT;
}

// Função para gerenciar o comportamento dos LEDs com base na distância
char manageLeds(void) {
  if (cm == 0 || cm > distanceThreshold) {
    // Nenhum objeto à vista: LED verde
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
  } else if (cm <= distanceThreshold && cm > dangerThreshold) {
    // Objeto detectado, mas não muito perto: LED laranja (mistura de vermelho e verde)
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
  } else if (cm <= dangerThreshold) {
    // Objeto muito próximo: Inicia o processo de pisca-pisca
    startBlinking();
  }
  return REPEAT;
}

// Função para iniciar o pisca-pisca e o buzzer com duas cores
void startBlinking() {
  unsigned long currentTime = millis();
  if (currentTime - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentTime;
    ledState = !ledState;  // Alterna entre dois estados

    // Alterna entre vermelho e verde para o pisca-pisca
    digitalWrite(redPin, ledState);
    digitalWrite(greenPin, !ledState);
    tone(buzzer, 2500, 100);  // Emite um som curto no buzzer
  }
}

// Função para ler a distância a cada intervalo definido sem bloquear o código
char readDistance(void) {
  unsigned long currentTime = millis();
  if (currentTime - lastDistanceReadTime >= distanceReadInterval) {
    lastDistanceReadTime = currentTime;
    cm = getDistance();
    Serial.print("Distancia: ");
    Serial.println(cm);
  }
  return REPEAT;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  digitalWrite(buzzer, LOW);
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);

  kernelInit();

  // Adiciona processos ao kernel
  kernelAddProc(readDistance, 1, 1);  // Lê a distância a cada 1 tick
  kernelAddProc(manageLeds, 1, 1);  // Gerencia os LEDs a cada 1 tick
  kernelAddProc(showDistance, 1, 5);  // Mostra a distância no Serial Monitor a cada 5 ticks

  Timer1.initialize(1000000);  // Configura Timer1 para gerar interrupções a cada 1 segundo
  Timer1.attachInterrupt(kernelTick);  // Interrupção chama o kernelTick para contar ticks

  kernelLoop();  // Executa o kernel se o sistema estiver ativo
}

void loop() { }