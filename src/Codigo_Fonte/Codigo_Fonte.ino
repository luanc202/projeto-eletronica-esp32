#include <DHTesp.h>
#include "heltec.h"
#include <WiFi.h>
#include <WebServer.h>
#define BAND 915E6 // Escolha a frequência

DHTesp dht;

String packet;

float currentTemp;

float currentHumidity;

const int RelePin = 23;         // pino ao qual o Módulo Relé está conectado
String statusRele = "OFF"; // variavel para ler dados recebidos pela serial
String off = "OFF";

void getTemp()
{
  float temperature = dht.getTemperature();

  if (temperature != currentTemp)
  { // Verifica se houve mudança na temperatura
    currentTemp = temperature;
    digitalWrite(LED, HIGH); // Liga o LED
    delay(500);              // Espera 500 milissegundos
    digitalWrite(LED, LOW);  // Desliiga o LED
    delay(500);              // Espera 500 milissegundos
  }
  delay(1000);
}
void sendPacket()
{
  LoRa.beginPacket();
  LoRa.print("Temperatura: ");
  LoRa.print(currentTemp);
  LoRa.endPacket();
}

/*
  Nome da função: getTemp
  objetivo: ler a temperatura e atibiu a variável currentTemp.
*/

/*Put your SSID & Password*/
const char *ssid = "ESP32-AP-Temp"; // Enter SSID here
const char *password = "getTEMP32"; // Enter Password here (min. 8 characters)

/* Put IP Address details */
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
/*
  Nome da função: sendPacket
  objetivo: envia a temperatura via LoRa armazenada na variável currentTemp.
*/

/******************* função principal (setup) *********************/
void setup()
{
  Serial.begin(BAND);
  pinMode(RelePin, OUTPUT);   // seta o pino como saída
  digitalWrite(RelePin, LOW); // seta o pino com nivel logico baixo
  pinMode(LED, OUTPUT);       // inicializa o LED

  Heltec.begin(true /*Habilita o Display*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Habilita debug Serial*/, true /*Habilita o PABOOST*/, BAND /*Frequência BAND*/);

  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  Heltec.display->drawString(33, 5, "Iniciado");
  Heltec.display->drawString(10, 30, "com Sucesso!");
  Heltec.display->display();
  delay(1000);

  dht.setup(17, DHTesp::DHT11); // inicializa o DHT no pino 17

  currentTemp = dht.getTemperature();

  // Setup do Servidor Web

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");

  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);

  server.on("/", handle_OnConnect);
  server.on("/rele", handle_OnRele);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}
/******************* função em loop (loop) *********************/
void loop()
{
  getTemp(); // Ler temperatura
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);

  Heltec.display->drawString(30, 5, "temperatura");
  Heltec.display->drawString(33, 30, (String)currentTemp);
  Heltec.display->drawString(78, 30, "°C");
  Heltec.display->display();
  sendPacket(); // Envia temperatura

  Serial.println(currentTemp);

  // Codigo do Servidor
  server.handleClient();
}

void ativarRele()
{
  if (statusRele == "OFF")
  {
    Serial.println("Caiu no if do ON");
    statusRele = String("ON");
    int readStatus = digitalRead(RelePin);
    Serial.println(String(readStatus));
    digitalWrite(RelePin, HIGH);
  }

  else
  {
    Serial.println("Caiu no if do OFF");
    statusRele = String("OFF");
    int readStatus = digitalRead(RelePin);
    Serial.println(String(readStatus));
    digitalWrite(RelePin, LOW);
  }
}

void handle_OnRele() {
  ativarRele();
  server.send(200, "text/plain", (String)statusRele);
}

void handle_OnConnect()
{
  server.send(200, "text/plain", (String)currentTemp);
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}