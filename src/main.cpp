#include <Arduino.h>

#include <WiFi.h>

// const char* ssid = "mark_allan";
// const char* password = "pinging_here";

const char* ssid_custom = "KINGS";
// const char *password_custom = "12345678";

WiFiServer server(80); //webserver port

String header; //for storing HTTP request
//variable to  store the current output state
String output26State = "off";
String output27State = "off";
String stopped = "STOP";

const int output26 = 26;
const int output27 = 27;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup()
{
  Serial.begin(230400);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);

  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // WiFi.mode(WIFI_AP);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid_custom);
  Serial.println("\nConnecting to WiFi Network..");

  // while(WiFi.status() != WL_CONNECTED){
  //   Serial.print(".");
  //   delay(100);
  // }

  Serial.println("\nConnected to WiFI network");
  Serial.print("GATEWAY ESP#@ IP: ");
  Serial.println(WiFi.broadcastIP());
  server.begin();
}

void loop()
{
  // while (WiFi.status() == WL_CONNECTED)
  // {
  digitalWrite(LED_BUILTIN, HIGH);
  // }

  WiFiClient client = server.available();

  if (client)
  {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    Serial.println(client.connected());
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    {
      currentTime = millis();
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /26/on") >= 0)
            {
              Serial.println("GPIO 26 on ");
              output26State = "on";
              digitalWrite(output26, HIGH);
            }
            else if (header.indexOf("GET /26/off") >= 0)
            {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            }
            else if (header.indexOf("GET /27/on") >= 0)
            {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            }
            else if (header.indexOf("GET /27/off") >= 0)
            {
              Serial.println("GPIO 27off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name = \"viewport\" content = \"width = device-width , initial-scale = 1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>html {font-family: Helvetica; display: inline-block; margin: 0px; text-align: center;}");
             client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            client.println("<body><h1>ESP32 Web Server</h1>");

            client.println("<p>GPIO 26 - State" + output26State + "</p>");


            if (output26State == "on")
            {
              client.println("<p><a href=\"/26/off\"><button class =\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/26/on\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("<p>GPIO 27 - State" + output27State + "</p>");

            if (output27State == "on")
            {
              client.println("<p><a href=\"/27/off\"><button class =\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/27/on\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body><html>");
            client.println();
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    header = "";

    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}