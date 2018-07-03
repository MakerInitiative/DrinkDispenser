#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "LAPTOP-QFJLVROC 8500";
const char* password = "294Bb$03";

const int MotorLeftAPin = D1;
const int MotorLeftBPin = D3;

WiFiServer server(80);

void DispenseLiquid()
{
  Serial.println("Dispensing");
  for(int i = 0; i < 250; i ++)
  {
    digitalWrite(MotorLeftAPin, HIGH);
    digitalWrite(MotorLeftBPin, LOW);
    delay(8);
    digitalWrite(MotorLeftAPin, LOW);
    digitalWrite(MotorLeftBPin, LOW);
    delay(10);
  }
  for(int i = 0; i < 10; i ++)
  {
    digitalWrite(MotorLeftBPin, HIGH);
    digitalWrite(MotorLeftAPin, LOW);
    delay(20);
    digitalWrite(MotorLeftAPin, LOW);
    digitalWrite(MotorLeftBPin, LOW);
    delay(80);
  }
  digitalWrite(MotorLeftAPin, LOW);
  digitalWrite(MotorLeftBPin, LOW);
  Serial.println("Done");

}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(MotorLeftAPin, OUTPUT);
  pinMode(MotorLeftBPin, OUTPUT);

  
  digitalWrite(MotorLeftAPin, LOW);
  digitalWrite(MotorLeftBPin, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

// Start the server
  server.begin();
  Serial.println("Server started");
 


  
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void wifiServer()
{
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }


  // Read the first line of the request
  String request = client.readStringUntil('\r');
  // Read the next line?
  String requestValue = client.readStringUntil('\r');
  Serial.println(request);
  Serial.println(requestValue);
  client.flush();

  // Match the request


  if (request.indexOf("/DISPENSE=START") != -1) {
    DispenseLiquid();
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html><font size=\"+12\">");
  //client.prin tln("<head><meta http-equiv=\"refresh\" content=\"1\"></head>");

  client.println("Click <a href=\"/DISPENSE=START\">here</a> DISPENSE<br>");


client.println("<form action=\"/DISPENSE=START\">");
client.println("<input type=\"submit\" value=\"DISPENSE\" />");
client.println("</form>");


  client.println("</font></html>");

  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");

}

void OTAHandler()
{
  ArduinoOTA.handle();
  delay(10);                  // delay to gife the arduino time to move to the next operation.
}

void loop() {
  
  OTAHandler();
  wifiServer();

  
}


