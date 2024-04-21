#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

const char *ssid = ""; //Nome do wifi
const char *password = ""; // Senha do wifi
const char *serverUrl = ""; //Alterar aqui e adicionar o link que receberá os dados

#define DHTPIN 5 //porta para d1 wemos = 5        -------          Utilizar a porta 2 para esp01 com modo programação no botão
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi!");
  Serial.println(WiFi.localIP());
  dht.begin();
}

void loop() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  
  enviarDadosParaServidor(temperatura, umidade);
  delay(5000);
}

void enviarDadosParaServidor(float temperatura, float umidade) {
  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(15000);  
  Serial.println(client.getLastSSLError());

  HTTPClient http;
  http.setTimeout(15000);
  String url = String(serverUrl);
  String jsonData = "{\"temperatura\":" + String(temperatura) + ",\"umidade\":" + String(umidade) + "}";
  Serial.println("JSON a ser enviado: " + jsonData);

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonData);

  if (httpCode > 0) {
    Serial.printf("[HTTP] POST realizado com sucesso, código: %d\n", httpCode);
    String resposta = http.getString();  
    Serial.println("Resposta do servidor: " + resposta);
  } else {
    Serial.print("[HTTP] Falha na requisição POST, código de erro: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }
  http.end();
}
