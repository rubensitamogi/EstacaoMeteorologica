#include <ICMPPing.h>
#include <Wire.h>
#include "DHT.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP085_U.h"
#include "SPI.h"
#include "Ethernet.h"
#include <SFE_BMP180.h>
#include<stdlib.h>
#include <SoftwareSerial.h>
#define IP "184.106.153.149" // thingspeak.com
#define DHT22_PIN 2

char buffer[10];

unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long delay_time = 30000;
unsigned long lowpulseoccupancy = 0;

String readString;
DHT dht(5,DHT11);
int numeroSensor =0;

char status; //função de estado do sensor
double tem,presion;
SFE_BMP180 sensorPresion;
int pinoSensor = A0;
int valorSensor = 0;

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192, 168, 2, 199);

// fill in your Domain Name Server address here:
IPAddress myDns(1, 1, 1, 1);

// initialize the library instance:
EthernetClient client;

char server[] = "api.thingspeak.com";
String cmd;
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

void setup() {
  
  Serial.begin(9600);
  sensorPresion.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    lersensores();
    httpRequest();
  }

}

void lersensores() {
  float h = dht.readHumidity();
    float t = dht.readTemperature(); 
    Serial.print("Umidade ");
    Serial.print(h);
    Serial.println(" %");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" *C");
    

    

    status = sensorPresion.startTemperature();
    delay(status);
    sensorPresion.getTemperature(tem);
    status = sensorPresion.startPressure(3);
    delay(status);
    sensorPresion.getPressure(presion, tem);
    Serial.print("Pressão Atimosférica: ");
    Serial.print(presion*0.1,2);
 
float sensorPressao;
     
     sensorPressao = (presion*0.1);
     float altitud = -(sensorPressao*35.6);
     Serial.println( + " KPa");
    valorSensor = analogRead(pinoSensor);
    //Serial.print("Leitura Chuva= ");
    //Serial.println (valorSensor);
String sensordChuva = "";
    if(valorSensor > 800){


      
      
 sensordChuva = "Sem Chuva";
      Serial.println("Sem Chuva");
      numeroSensor = 0;
    }else if (valorSensor <= 800 && valorSensor > 500){
      Serial.println("Chuva Fina");
      numeroSensor = 1;
     sensordChuva = "Chuva Fina";
    }else if(valorSensor <= 500){
      Serial.println("Chuva Forte");
      numeroSensor = 2;
       sensordChuva = "Chuva Forte";
      }
      delay(5000);
  
String numeroStr = dtostrf(numeroSensor, 4, 1, buffer);

    
    //conversão dos dados lidos para strings
    String temperaturaStr = dtostrf(t, 4, 1, buffer);
    temperaturaStr.replace(" ","");
    String umidadeStr = dtostrf(h, 4, 1, buffer);
    umidadeStr.replace(" ","");
    String pressaoStr = dtostrf(sensorPressao, 4, 1, buffer);
    pressaoStr.replace(" ","");
    String sensorDChuva = sensordChuva;
    numeroStr.replace(" ","");
      String altitudeStr = dtostrf(altitud, 4, 1, buffer);
    altitudeStr.replace(" ","");
    Serial.println("Altitude " + altitudeStr + " Metros");
    
    //enviar dados dos sensores para o thingspeak
    updateSensors(temperaturaStr, umidadeStr, pressaoStr, numeroStr, altitudeStr);
}

//Função para envio dos dados dos sensores para o thingspeak
void updateSensors(String temperaturaStr, String umidadeStr, String pressaoStr, String sensorDChuva, String altitudeStr) {
  cmd = "GET /update?api_key=0JPG2XTL9N0TSNY3&field1=0";
  cmd += temperaturaStr;
  cmd += "&field2=";
  cmd += umidadeStr;
  cmd += "&field3=";
  cmd += pressaoStr;
  cmd += "&field4=";
  cmd += sensorDChuva;
  cmd += "&field5=";
  cmd += altitudeStr;
  cmd += "HTTPS/1.1";
  int strsize = cmd.length();

}


// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println(cmd);
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
