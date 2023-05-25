
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS

const char* ssid = "*********";
const char* password = "************";

int ledState = LOW;

unsigned long previousMillis = 0;
const long interval = 1000;

const byte up_output = 5;
const byte down_output = 4;


// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>ESP JackStand Remote</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { 
        font-family: Times New Roman; 
        text-align: center; 
        margin:0px auto; 
        padding-top: 50px;
        color: #FFF;
        background-color: #000;
        }

      .button {
        padding: 36px 36px;
        font-size: 64px;
        text-align: center;
        outline: none;
        color: #000;
        background-color: #ff0522;
        border: none;
        border-radius: 10px;
        cursor: row-resize;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }  
      .button:hover {background-color: #ff0522 }
      .button:active {
        background-color: #1fe036;
        transform: translateY(5px);
      }
    </style>
  </head>
  <body>

    <button class="button" onmousedown="toggleCheckbox('ON1');" ontouchstart="toggleCheckbox('ON1');" onmouseup="toggleCheckbox('OFF1');" ontouchend="toggleCheckbox('OFF1');">UP</button>
    <div>
    <p>
    <h1>JackStand Remote</h1>
    <p>
    </div>
    <button class="button" onmousedown="toggleCheckbox('ON2');" ontouchstart="toggleCheckbox('ON2');" onmouseup="toggleCheckbox('OFF2');" ontouchend="toggleCheckbox('OFF2');">DOWN</button>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
   }
  </script>
  </body>
</html>)rawliteral";


AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  if (!MDNS.begin("JackStand")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) { delay(1000); }
  }
  Serial.println("mDNS responder started");
  MDNS.addService("http", "tcp", 80);
  pinMode(down_output, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(led_live, OUTPUT);
  pinMode(up_output, OUTPUT);
  digitalWrite(up_output, HIGH); //start in off 
  digitalWrite(down_output, HIGH);
    
  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Receive an HTTP GET request up
  server.on("/ON1", HTTP_GET, [] (AsyncWebServerRequest *request) {
    delay(10);
    digitalWrite(up_output, LOW);
    digitalWrite(down_output,HIGH); //FORCE OFF 
    request->send(200, "text/plain", "OK");
  });

  // Receive an HTTP GET request stop
  server.on("/OFF1", HTTP_GET, [] (AsyncWebServerRequest *request) {
    delay(10);
    digitalWrite(up_output, HIGH);
    digitalWrite(down_output,HIGH);
    request->send(200, "text/plain", "OK");
  });
  
  // Receive an HTTP GET request down
  server.on("/ON2", HTTP_GET, [] (AsyncWebServerRequest *request) {
    delay(10);
    digitalWrite(down_output, LOW);
    digitalWrite(up_output, HIGH);
    request->send(200, "text/plain", "OK");
  });

  // Receive an HTTP GET request stop 
  server.on("/OFF2", HTTP_GET, [] (AsyncWebServerRequest *request) {
    delay(10);
    digitalWrite(down_output, HIGH);
    digitalWrite(up_output, HIGH);
    request->send(200, "text/plain", "OK");
  });
  server.begin();

}

void loop() {
 
  MDNS.update();
   unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;  // Note that this switches the LED *off*
    } else {
      ledState = LOW;  // Note that this switches the LED *on*
    } //heartbeat 
    digitalWrite(LED_BUILTIN, ledState);
  }
}
