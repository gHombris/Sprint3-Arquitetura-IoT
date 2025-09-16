#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>          
#include <PubSubClient.h>

// --- Configurações de Rede --- // <<< SEÇÃO ADICIONADA
const char* ssid = "Wokwi-GUEST"; // Use "Wokwi-GUEST" para simulação no Wokwi, ou o nome da sua rede
const char* password = "";     // Use "" (vazio) para a rede do Wokwi, ou a senha da sua rede
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

// --- Configuração dos Pinos ---
const int PINO_BOTAO_CONTROLE = 12;
const int PINO_LED_STATUS = 27;

// --- Configuração do LCD I2C ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Configuração do Acelerômetro ---
Adafruit_MPU6050 mpu;

// --- Constantes de Lógica ---
const float LIMIAR_IMPACTO = 35.0;
const long COOLDOWN_MS = 500;

// --- Variáveis de Estado ---
bool treinoAtivo = false;
int contadorToques = 0;
unsigned long ultimoImpactoTempo = 0;
bool estadoBotaoControleAnterior = HIGH;
unsigned long ultimoDebounceControle = 0;
const long debounceDelay = 50;

// <<< FUNÇÃO ADICIONADA: Conecta ao WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// <<< FUNÇÃO ADICIONADA: Reconecta ao Broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "SkillHerClient-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setup_wifi(); // <<< ADICIONADO
  client.setServer(mqtt_server, 1883); // <<< ADICIONADO

  pinMode(PINO_BOTAO_CONTROLE, INPUT_PULLUP);
  pinMode(PINO_LED_STATUS, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Skill Her");

  if (!mpu.begin()) {
    Serial.println("Falha ao encontrar o MPU6050");
    lcd.setCursor(0, 1);
    lcd.print("Erro no Sensor!");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  lcd.setCursor(0, 1);
  lcd.print("Pronta?");
  digitalWrite(PINO_LED_STATUS, LOW);
}

void loop() {
  if (!client.connected()) { // <<< ADICIONADO
    reconnect();
  }
  client.loop(); // <<< ADICIONADO

  handleBotaoControle();

  if (treinoAtivo) {
    detectarToque();
  }
}

void detectarToque() {
  if (millis() - ultimoImpactoTempo < COOLDOWN_MS) {
    return;
  }
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float magnitude = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
  if (magnitude > LIMIAR_IMPACTO) {
    ultimoImpactoTempo = millis();
    contadorToques++;
    lcd.setCursor(8, 1);
    lcd.print(contadorToques);
    Serial.print("Impacto detectado! Magnitude: ");
    Serial.println(magnitude);
  }
}

// <<< FUNÇÃO ADICIONADA: Envia os dados via MQTT
void finalizarTreino() {
  char payload[10];
  sprintf(payload, "%d", contadorToques);
  
  Serial.print("Enviando resultado via MQTT: ");
  Serial.println(payload);

  client.publish("skillher/treinos/resultado", payload);
}

void handleBotaoControle() {
  bool estadoAtual = digitalRead(PINO_BOTAO_CONTROLE);
  if (estadoAtual != estadoBotaoControleAnterior) {
    ultimoDebounceControle = millis();
  }
  if ((millis() - ultimoDebounceControle) > debounceDelay) {
    if (estadoAtual == LOW) {
      treinoAtivo = !treinoAtivo;
      if (treinoAtivo) {
        contadorToques = 0;
        digitalWrite(PINO_LED_STATUS, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Treino Iniciado!");
        lcd.setCursor(0, 1);
        lcd.print("Toques: 0");
      } else {
        digitalWrite(PINO_LED_STATUS, LOW);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Treino Finalizado");
        lcd.setCursor(0, 1);
        lcd.print("Total: " + String(contadorToques));

        finalizarTreino(); // <<< ADICIONADO: Chamada da função para enviar os dados
      }
      while (digitalRead(PINO_BOTAO_CONTROLE) == LOW);
    }
  }
  estadoBotaoControleAnterior = estadoAtual;
}