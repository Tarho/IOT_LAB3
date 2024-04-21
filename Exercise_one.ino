#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "Vux";
const char *password = "opal18032001";

ESP8266WebServer server(80);

const int numLights = 5;
int ledPins[numLights] = {D1, D2, D3, D4, D5};

int score = 5;
int correctAnswer = -1;

unsigned long roundStartTime = 0;
bool roundStarted = false;

void setup() {
    Serial.begin(9600);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    for (int i = 0; i < numLights; i++) {
        pinMode(ledPins[i], OUTPUT);
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/select", HTTP_GET, handleSelection); 
    server.begin();
    Serial.println("HTTP server started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    server.handleClient();
}

void handleRoot() {
    String html = "<!DOCTYPE html><html><head><title>LED Game</title></head><body>";
    html += "<h1>LED Game</h1>";
    html += "<p>Score: " + String(score) + "</p>";
    html += "<p>Time left: <span id='countdown'>5</span> seconds</p>"; // Display countdown timer
    html += "<p>Count the number of lights and select the corresponding button:</p>";
    html += "<form id='gameForm' action='/select' method='get'>";
    html += "<button name='selection' value='0'>0</button>";
    html += "<button name='selection' value='1'>1</button>";
    html += "<button name='selection' value='2'>2</button>";
    html += "</form>";
    html += "<script>";
    html += "var timeLeft = 5;"; 
    html += "function startCountdown() {";
    html += "  var countdownInterval = setInterval(function() {";
    html += "    document.getElementById('countdown').innerHTML = timeLeft;";
    html += "    timeLeft -= 1;";
    html += "    if (timeLeft < 0) {";
    html += "      clearInterval(countdownInterval);";
    html += "      document.getElementById('gameForm').submit();"; // Submit the form when time is up
    html += "    }";
    html += "  }, 1000);";
    html += "}";
    html += "startCountdown();"; 
    html += "</script>";
    html += "</body></html>";

    server.send(200, "text/html", html);

    displayRandomLights();
}

void displayRandomLights() {
    for (int i = 0; i < numLights; i++) {
        digitalWrite(ledPins[i], LOW);
    }

    int numLightsOn = random(1, numLights + 1);
    correctAnswer = numLightsOn % 3;

    for (int i = 0; i < numLightsOn; i++) {
        int randomIndex = random(0, numLights);
        digitalWrite(ledPins[randomIndex], HIGH);
        delay(500); 
    }
    delay(2000);

    for (int i = 0; i < numLights; i++) {
        digitalWrite(ledPins[i], LOW);
    }
}

void handleSelection() {
    if (millis() - roundStartTime == 0000) {
        score--;
        server.sendHeader("Location", String("/"), true);
    }

    String selection = server.arg("selection");
    int selectedNumber = selection.toInt();

    if (selectedNumber == correctAnswer) {
        score++;
    } else {
        score--;
    }

    if (score <= 0) {
        server.send(200, "text/plain", "Game Over! You lose.");
    } else {
        server.sendHeader("Location", String("/"), true);
        server.send(302, "text/plain", ""); // 302 Redirect
    }
}
