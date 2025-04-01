#include <Arduino.h>
#include <WiFi.h>

const char* const WIFI_SSID = "koala";
const char* const WIFI_PWRD = "esp32esp32";
const IPAddress IP_LOCAL(192, 168, 1, 101);
const IPAddress IP_GATEWAY(192, 168, 12, 1);
const IPAddress SUBNET_MASK(255, 255, 255, 0);

const char* const SERVER_IP = "192.168.1.100";
const std::uint16_t SERVER_PORT = 1000;

WiFiClient client;
float rbuf[5] = {};

void connect_client(){
    Serial.println("connecting...");
    if(!client.connect(SERVER_IP, SERVER_PORT)){
        do{
            delay(1000);
            Serial.println("connecting...");
        }while(!client.connect(SERVER_IP, SERVER_PORT));
    }
    Serial.println("connected!");
}

void setup() {
    Serial.begin(115200);
    Serial.println("hello world");

    WiFi.config(IP_LOCAL, IP_GATEWAY, SUBNET_MASK);
    WiFi.begin(WIFI_SSID, WIFI_PWRD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("subnet: ");
    Serial.println(WiFi.subnetMask());

    connect_client();

}

void loop() {
    if(!client.connected()) connect_client();
    if(client.available() >= sizeof(rbuf)){
        do{
            client.read((uint8_t*)rbuf, sizeof(rbuf));
        }while(client.available() >= sizeof(rbuf));
        Serial.printf("%5.5f, %5.5f, %5.5f, %5.5f, %5.5f\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4]);
    }
}