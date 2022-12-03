/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHTesp.h>
#include <heltec.h>
#define BAND    915E6  //Escolha a frequência

const char *ssid = "YourSSIDHere";
const char *password = "YourPSKHere";

WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

           hr, min % 60, sec % 60
          );
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}

DHTesp dht;
String packet ;
float currentTemp;
float currentHumidity;
/* Protótipo da função */
void getTemp();
void sendPacket();
/*
  Nome da função: getTemp
  objetivo: ler a temperatura e atibiu a variável currentTemp.
*/
void getTemp()
{
  float temperature = dht.getTemperature();
 
  if (temperature != currentTemp) { //Verifica se houve mudança na temperatura
    currentTemp = temperature;
    digitalWrite(LED, HIGH);   // Liga o LED
    delay(500);                // Espera 500 milissegundos
    digitalWrite(LED, LOW);    // Desliiga o LED
    delay(500);                // Espera 500 milissegundos
  }
  delay(1000);
}
/*
  Nome da função: sendPacket
  objetivo: envia a temperatura via LoRa armazenada na variável currentTemp.
*/
void sendPacket()
{
  LoRa.beginPacket();
  LoRa.print("Temperatura: ");
  LoRa.print(currentTemp);
  LoRa.endPacket();
}
/******************* função principal (setup) *********************/
void setup()
{
  pinMode(LED,OUTPUT); //inicializa o LED
  
  Heltec.begin(true /*Habilita o Display*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Habilita debug Serial*/, true /*Habilita o PABOOST*/, BAND /*Frequência BAND*/);
 
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  Heltec.display->drawString(33, 5, "Iniciado");
  Heltec.display->drawString(10, 30, "com Sucesso!");
  Heltec.display->display();
  delay(1000);
  
  dht.setup(17, DHTesp::DHT11); //inicializa o DHT no pino 17
   
  currentTemp = dht.getTemperature();
}
/******************* função em loop (loop) *********************/
void loop()
{
  getTemp(); //Ler temperatura
  
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  
  Heltec.display->drawString(30, 5, "Enviando");
  Heltec.display->drawString(33, 30, (String)currentTemp);
  Heltec.display->drawString(78, 30, "°C");
  Heltec.display->display();
  sendPacket(); //Envia temperatura
}
#include "heltec.h"
#define BAND    915E6  //Escolha a frequência
String packSize = "--";
String packet ;
/* Protótipo da função */
void LoRaDataPrint();
void cbk(int packetSize);
/*
  Nome da função: LoRaDataPrint
  objetivo: imprime a temperatura e tamanho do pacote recebido no display.
*/
void LoRaDataPrint(){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0 , 1 , "Recebendo "+ packSize + " bytes");
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(15, 16, "Temperatura");
  Heltec.display->drawString(33, 38, packet);
  Heltec.display->drawString(78, 38, "°C");
  Heltec.display->display();
}
/*
  Nome da função: cbk
  recebe como parâmetos um inteiros (packetSize)
  objetivo: recebe a temperatura via LoRa e armazena na variável packet.
*/
void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) {
    packet += (char) LoRa.read(); //Atribui um caractere por vez a váriavel packet 
  }
  LoRaDataPrint();
}
/******************* função principal (setup) *********************/
void setup()
{
  pinMode(LED,OUTPUT); //inicializa o LED
  
  Serial.begin(9600);
  Heltec.begin(true /*Habilita o Display*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Habilita debug Serial*/, true /*Habilita o PABOOST*/, BAND /*Frequência BAND*/);
 
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();
  Heltec.display->drawString(10, 5, "Iniciado com Sucesso!");
  Heltec.display->drawString(10, 30, "Aguardando os dados...");
  Heltec.display->display();
  Serial.println("Iniciado com Sucesso!");
  Serial.println("Aguardando os dados...");
  delay(1000);
  
  LoRa.receive(); // Habilita o rádio LoRa para receber dados
}
/******************* função em loop (loop) *********************/
void loop()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) { //Verifica se há dados chegando via LoRa
    cbk(packetSize);
    digitalWrite(LED, HIGH);   // Liga o LED
    delay(500);                // Espera 500 milissegundos
    digitalWrite(LED, LOW);    // Desliiga o LED
    delay(500);                // Espera 500 milissegundos
    Serial.print("Recebendo a temperatura: ");
    Serial.print(packet); //Imprime no monitor serial a temperatura
    Serial.println("°C");
  }
  delay(10);
}
//PREPARANDO AS CONFIGURAÇÕES LORA
#include <heltec.h>//INCLUSÃO DE BIBLIOTECA DA HELTEC

//PREPARANDO AS CONFIGURAÇÕES DO SENSOR DE TEMP
#include <dht.h>//INCLUSÃO DE BIBLIOTECA

//PREPARANDO AS CONFIGURAÇÕES DO MODULO
#include <RTClib.h>               // BIBLIOTECA GERAL DOS MODULOS RTC 
#include <DS3231.h>               // BIBLIOTECA DO MODULO RTC DS3231

 
const int pinoDHT11 = A2; //PINO ANALÓGICO UTILIZADO PELO DHT11
 
dht DHT; //VARIÁVEL DO TIPO DHT
 
void setup(){
  Serial.begin(9600); //INICIALIZA A SERIAL
  delay(2000); //INTERVALO DE 2 SEGUNDO ANTES DE INICIAR
}
 
void loop(){
  DHT.read11(pinoDHT11); //LÊ AS INFORMAÇÕES DO SENSOR
  Serial.print("Umidade: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(DHT.humidity); //IMPRIME NA SERIAL O VALOR DE UMIDADE MEDIDO
  Serial.print("%"); //ESCREVE O TEXTO EM SEGUIDA
  Serial.print(" / Temperatura: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(DHT.temperature, 0); //IMPRIME NA SERIAL O VALOR DE UMIDADE MEDIDO E REMOVE A PARTE DECIMAL
  Serial.println("*C"); //IMPRIME O TEXTO NA SERIAL
  delay(2000); //INTERVALO DE 2 SEGUNDOS * NÃO DIMINUIR ESSE VALOR
}


int Rele = 13;                    //PINO QUE SERÁ USADO O RELÉ
int led_vermelho = 12;            //PINO QUE SERÁ USADO NO LED DE DESLIGADO
int led_verde = 8;                //PINO QUE SERÁ USADO NO LED DE LIGADO

DS3231  rtc(SDA, SCL);            //APLICANDO A BIBLOTECA AO MODULO RTC DS3231
Time t;                           //DANDO UM NOME AO MODULO QUE SERÁ UTLIZADO

const int HorarioLigado = 23;      //HORA EM QUE ESTARÁ LIGADO O RELÉ
const int MinutoLigado = 18;       //MINUTO EM QUE ESTARÁ LIGADO O RELÉ
const int HorarioDesligado = 23;   //HORA EM QUE ESTARÁ DESLIGADO O RELÉ
const int MinutoDesligado = 20;    //MINUTO EM QUE ESTARÁ DESLIGADO O RELÉ

//_______________________________________________________________________________________________________________________________

//CONFIGURANDO PARAMETROS DO MODULO
void setup() 
  {
  Serial.begin(9600);              // CONFIGURAÇÃO PARA MOSTRAR DADOS NO MONITOR SERIAL
  rtc.begin();                     // AJUSTE DA SAÍDA DE DADOS
  pinMode(Rele, OUTPUT);           //O PINO DO RELÉ SERÁ DE SAIDA
  digitalWrite(Rele, LOW);         //O PINO COMEÇARA LIGADO
  }
  
//_______________________________________________________________________________________________________________________________

//CONFIGURANDO SAIDA DE DADOS DO MODULO
void loop() 
{
  t = rtc.getTime();               //CHAMANDO OS DADOS DO MUDULO
  Serial.print(t.hour);            //COMANDO PARA CHAMAR AS HORAS
  Serial.print(" Hora(s), ");      //SAIDA DE DADOS DAS HORAS NO MONITOR SERIAL
  Serial.print(t.min);             //COMANDO PARA CHAMAR OS MINUTOS
  Serial.print(" Minuto(s)");      //SAIDA DE DADOS DOS MINUTOS NO MONITOR SERIAL
  Serial.println(" ");             //AJUSTE NA SAIDA DE DADOS
  delay (1000);                    //ATUALIZAÇÃO DE DADOS A CADA 1 SEGUNDO

//_______________________________________________________________________________________________________________________________
 
  //CONDIÇÃO PARA LIGAR A LAMPADA
  if(t.hour == HorarioLigado && t.min == MinutoLigado)                   //COMPARAÇÃO DE DADOS PARA LIGAR RELE/LED
    {
    digitalWrite(Rele,HIGH);                  //LIGAR O RELE
    digitalWrite(led_verde,HIGH);             //LIGAR LED VERDE
    digitalWrite(led_vermelho,LOW);           //DESLIGAR LED VERMELHO
    Serial.println("Luz Ligada");             //SAÍDA DE DADOS NOMONITOR SERIAL
    }
    
    //CONDIÇÃO PARA DESLIGADA A LAMPADA
    else if(t.hour == HorarioDesligado && t.min == MinutoDesligado)      //COMPARAÇÃO DE DADOS PARA DESLIGAR RELE/LED
    {
      digitalWrite(Rele,LOW);                //DESLIGA O RELE
      digitalWrite(led_vermelho,HIGH);       //LIGAR LED VERMELHO 
      digitalWrite(led_verde,LOW);           //DESLIGAR LED VERDE
      Serial.println("Luz Desligada");       //SAÍDA DE DADOS NOMONITOR SERIAL
    }
}
