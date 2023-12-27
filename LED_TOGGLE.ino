#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>


const char* ssid = "ESP32_AP";
const char* password = "your_password";
const int BL = 2; // Define the pin for the LED

bool ledState = LOW; // Initially OFF
WebServer server(80);

void setup() {
    pinMode(BL, OUTPUT);
    digitalWrite(BL, ledState);
    Serial.begin(115200);
    WiFi.softAP(ssid, password);



    

    

    server.on("/led", HTTP_GET, []() {
        String checked = (ledState == HIGH) ? "checked" : "";
        String htmlContent = "<!DOCTYPE html><html><head><title>ESP32 LED Control</title><style>.switch{position:relative;display:inline-block;width:60px;height:34px;}.switch input{display:none;}.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;transition:.4s ease;border-radius:34px;}.slider:before{position:absolute;content:'';height:26px;width:26px;left:4px;bottom:4px;background-color:white;transition:.4s ease;border-radius:50%;}.slider.round{border-radius:34px;}.slider.round:before{border-radius:50%;}.switch input:checked+.slider{background-color:#2196F3;}.switch input:checked+.slider:before{transform:translateX(26px);}</style><script>function toggleLED(){var xhr=new XMLHttpRequest();xhr.open('GET','/led/toggle',true);xhr.send();}</script></head><body><h1>ESP32 LED Control</h1><label class=\"switch\"><input type=\"checkbox\" id=\"toggleLED\" " + checked + " onclick=\"toggleLED()\"><span class=\"slider round\"></span></label><br><p id=\"ledStatus\">LED Status: " + (ledState ? "ON" : "OFF") + "</p><script>function updateLEDStatus(){var xhr=new XMLHttpRequest();xhr.onreadystatechange=function(){if(xhr.readyState==4&&xhr.status==200){document.getElementById('toggleLED').checked=(xhr.responseText==='ON');document.getElementById('ledStatus').innerHTML='LED Status: '+xhr.responseText;}};xhr.open('GET','/led/status',true);xhr.send();}setInterval(updateLEDStatus,1000);</script></body></html>";
        server.send(200, "text/html", htmlContent);
    });

    server.on("/led/toggle", HTTP_GET, []() {
        ledState = !ledState;
        digitalWrite(BL, ledState);
        server.send(200, "text/plain", ledState ? "ON" : "OFF");
    });

    server.on("/led/status", HTTP_GET, []() {
        server.send(200, "text/plain", ledState ? "ON" : "OFF");
    });
server.on("/wifi", HTTP_GET, []() {
        String htmlContent = "<!DOCTYPE html><html><head><title>ESP32 Wi-Fi Networks</title></head><body><h1>ESP32 Wi-Fi Networks</h1><form action=\"/connect\"><label for=\"network\">Select a network:</label><select name=\"network\">";
        
        int numNetworks = WiFi.scanNetworks();
        for (int i = 0; i < numNetworks; i++) {
            htmlContent += "<option value=\"" + WiFi.SSID(i) + "\">" + WiFi.SSID(i) + "</option>";
        }
        
        htmlContent += "</select><br><label for=\"password\">Password:</label><input type=\"password\" name=\"password\"><br><input type=\"submit\" value=\"Connect\"></form></body></html>";
        server.send(200, "text/html", htmlContent);
    });
    server.on("/connect", HTTP_GET, []() {
        String networkName = server.arg("network");
        String password = server.arg("password");

        // Connect to the specified network
        WiFi.begin(networkName.c_str(), password.c_str());

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }

        // Connected to WiFi
        Serial.println("Connected to WiFi!");
        Serial.print("Local IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Access Point IP address: ");
        Serial.println(WiFi.softAPIP());

        // Fetch public IP address using an external service
        HTTPClient http;
        http.begin("http://api.ipify.org"); // Use any service providing public IP
        int httpResponseCode = http.GET();
        String publicIP = "";

        if (httpResponseCode > 0) {
            if (httpResponseCode == HTTP_CODE_OK) {
                publicIP = http.getString();
                Serial.print("Public IP address: ");
                Serial.println(publicIP);
            }
        }

        http.end();

        String response = "Connected to WiFi!\n";
        response += "Local IP address: " + WiFi.localIP().toString() + "\n";
        response += "Access Point IP address: " + WiFi.softAPIP().toString() + "\n";
        response += "Public IP address: " + publicIP;

        server.send(200, "text/plain", response); // Send response back to client
    });


    server.begin();
    Serial.print("Access Point IP address: ");
 Serial.println(WiFi.softAPIP());
}

void loop() {
    server.handleClient();
    // Other code can be added here if needed
}