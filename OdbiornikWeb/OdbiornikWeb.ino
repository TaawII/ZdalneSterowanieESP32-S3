#include <Arduino.h>
#include "ESP32_NOW.h"
#include "WiFi.h"
#include "util.h"
#include "hoverserial.h"
#include "WebServerManager.h"
#include "Types.h"

// =====================================================
// MAC nadajnika
// =====================================================

// uint8_t senderMac[] = {
//   0x80, 0xB5, 0x4E, 0xC6, 0xF1, 0x58
// };
uint8_t senderMac[] = {
  0x90, 0x70, 0x69, 0x31, 0x43, 0x70
};

// =====================================================
// Ustawienia Wi-Fi
// =====================================================

const char* AP_SSID = "ESP32-S3";
const char* AP_PASSWORD = "12345678";

// =====================================================
// Definicje
// =====================================================

// Piny dla ESP32-S3
#define LED_PIN 15  // dioda LED
#define PIN_RX 16   // RX ESP32-S3 do TX hoverboarda
#define PIN_TX 17   // TX ESP32-S3 do RX hoverboarda
#define PWM_LEFT 6  // PWM lewy
#define PWM_RIGHT 7 // PWM prawy
#define BAUDRATE 19200
#define SEND_INTERVAL 50 // ms
#define BUTTON1_OUT 1
#define BUTTON2_OUT 2
#define BUTTON3_OUT 4
#define BUTTON4_OUT 5

// --- UART ---
#define HOVER_SERIAL Serial1

// Kanał wifi (ustawić taki sam jak na odbiorniku)
#define ESPNOW_WIFI_CHANNEL 6

// =====================================================
// Zmiennne globalne
// =====================================================

const int pwmFreq = 15000;    // częstotliwość PWM (Hz)
const int pwmResolution = 8; // rozdzielczość (0–255)

// Odpowiedzi sterownika silników
SerialHover2Server hoverFeedback;
bool hoverFeedbackOk = false;

// --- Sterowanie ---
long speedL = 0;
long speedR = 0;
unsigned long lastSend = 0;
unsigned long lastReceive = 0;
unsigned long lastSendPhoneUpdate = 0;
unsigned long lastHoverboardDataUpdate = 0;
long speedMax = 300; // maksymalna prędkość silników (300-1000)
long ster = 0;
bool newJoystickData = false;
bool phoneMode = false;
bool smoothMode = true;
bool buttonState1 = false;
bool buttonState2 = false;
bool buttonState3 = false;
bool buttonState4 = false;
bool smoothModePressed = false;
bool buttonPressed1 = false;
bool buttonPressed2 = false;
bool buttonPressed3 = false;
bool buttonPressed4 = false;
uint8_t smoothModeCounter = 0;
uint8_t buttonCounter1 = 0;
uint8_t buttonCounter2 = 0;
uint8_t buttonCounter3 = 0;
uint8_t buttonCounter4 = 0;
int16_t phoneX = 0;
int16_t phoneY = 0;

long currentSpeedL = 0;
long currentSpeedR = 0;
long zero = 0;

unsigned long now = 0;
bool limitLeft = false;
bool limitRight = false;

// =====================================================
// Struktura danych (identyczna jak w nadajniku)
// =====================================================

JoystickData incomingJoystick;

// =====================================================
// Struktury pomocnicze dla serwera WEB
// =====================================================

HoverboardData hoverboardData;

WebData webData(
    incomingJoystick.x,
    incomingJoystick.y,
    phoneX,
    phoneY,
    speedMax,
    phoneMode,
    smoothMode,
    buttonState1,
    buttonState2,
    buttonState3,
    buttonState4,
    hoverboardData
);

// =====================================================
// Serwer web
// =====================================================

WebServerManager webServer(webData);

// ==============================
// Funkcje pomocnicze
// ==============================

long updateSpeed(long current, long target)
{
    const long DEADZONE = 100;
    const long ACCELERATION_STEP = 5;
    const long BRAKE_STEP = 20;
    const long saveTargetSign = (target >= 0) ? 1 : -1;
    long newSpeed = 0;

    // Deadzone dla docelowej prędkości
    if (abs(target) <= DEADZONE)
        target = 0;
        
    if(current == target){
        return current;
    }

    // Jeśli current i target mają ten sam znak lub któryś z nich to 0
    if (target == 0)
    {
        if(current > 0)
        {
            newSpeed = current - BRAKE_STEP;
            if (newSpeed < 0)
                newSpeed = 0;
        }else
        {
            newSpeed = current + BRAKE_STEP;
            if (newSpeed > 0)
                newSpeed = 0;
        }
        return newSpeed;
    }

    if((current >= 0 && target >= 0) || (current <= 0 && target <= 0))
    {
        if (abs(current) < abs(target))
        {
            // przyspieszanie
            if (abs(current) < DEADZONE && abs(target) >= DEADZONE)
                newSpeed = DEADZONE * saveTargetSign;
            else
                newSpeed = current + ACCELERATION_STEP * saveTargetSign;
        }
        else if (abs(current) > abs(target))
        {
            // hamowanie
            newSpeed = current - BRAKE_STEP * saveTargetSign;
            if (abs(newSpeed) < abs(target))
                newSpeed = target;
        }
    }
    else
    {
        // zmiana kierunku: najpierw wyhamuj do 0
        if(current > 0)
        {
            newSpeed = current - BRAKE_STEP;
            if (newSpeed < 0)
                newSpeed = 0;
        }else
        {
            newSpeed = current + BRAKE_STEP;
            if (newSpeed > 0)
                newSpeed = 0;
        }
    }

    return newSpeed;
}

void updateControl(int16_t y, int16_t x) {

  // Mapowanie i ograniczenie zakresu
  speedL = map(y, -1000, 1000, -speedMax, speedMax);
  speedR = map(y, -1000, 1000, -speedMax, speedMax);

  // Martwa strefa
  if (abs(y) < 100)
  {
    speedL = 0;
    speedR = 0;
  }

  // Ograniczenie
  speedL = constrain(speedL, -speedMax, speedMax);
  speedR = constrain(speedR, -speedMax, speedMax);

  // Debug — wyświetlanie L/R po mapowaniu
  // Serial.printf("Controller recv → L=%d  R=%d\n", speedL, speedR);

  ster = map(x, -1000, 1000, -255, 255);

  if (abs(x) < 100)
  {
    ster = 0;
  }

  ster = constrain(ster, -255, 255);
}

void UpdateWheelSpeed(){
  // Stopniowe narastanie prędkości, jeśli smoothMode == true
  if (smoothMode)
  {
    // Aktualizacja obu silników
    currentSpeedL = updateSpeed(currentSpeedL, speedL);
    currentSpeedR = updateSpeed(currentSpeedR, speedR);
  }
  else
  {
    currentSpeedL = speedL;
    currentSpeedR = speedR;
  }

  lastSend = now;
  // Serial.printf("speedL=%d  speedR=%d\n", currentSpeedL, currentSpeedR);
  HoverSendLR(HOVER_SERIAL, currentSpeedL, currentSpeedR);
}

// ==============================
// Aktualizacja przycisków
// ==============================

void ChceckButttonState(bool incButton, uint8_t &buttonCount, bool &buttonPress, bool &buttonState, int outputPin)
{
  if (incButton)
  {
      if (buttonCount < 2)
          buttonCount++;
  }
  else
  {
      buttonCount = 0;
  }

  if (buttonCount >= 2 && !buttonPress)
  {
      buttonPress = true;

      buttonState = !buttonState;

      digitalWrite(
          outputPin,
          buttonState ? HIGH : LOW
      );
  }

  if (!incButton)
  {
      buttonPress = false;
  }
}


// =====================================================
// Klasa peera
// =====================================================

class ESP_NOW_Peer_Class : public ESP_NOW_Peer {

public:

  // Konstruktor
  ESP_NOW_Peer_Class(
    const uint8_t *mac_addr,
    uint8_t channel,
    wifi_interface_t iface,
    const uint8_t *lmk
  ) : ESP_NOW_Peer(mac_addr, channel, iface, lmk) {}

  // Destruktor
  ~ESP_NOW_Peer_Class() {
    remove();
  }

  // Dodanie peera
  bool add_peer() {
    if (!add()) {
      Serial.println("Blad: nie udalo sie dodac nadajnika");
      return false;
    }
    Serial.println("Nadajnik zostal dodany jako peer");
    return true;
  }

  void onReceive(const uint8_t *data, size_t len, bool broadcast) override {
    // Sprawdzenie, czy dane maja poprawny rozmiar
    if (len != sizeof(JoystickData)) {
      Serial.printf("Blad: nieprawidlowy rozmiar danych: %u bajtow\n", (unsigned int)len);
      Serial.printf("Oczekiwano: %u bajtow\n", (unsigned int)sizeof(JoystickData));
      return;
    }
    // Aktualizacja timera
    lastReceive = millis();

    // Kopiowanie danych do struktury
    memcpy(&incomingJoystick, data, sizeof(JoystickData));

    if(!webData.phoneMode)
    {
      newJoystickData = true;
    }
  }
};

// =====================================================
// Utworzenie peera
// =====================================================

ESP_NOW_Peer_Class receiver(
  senderMac,
  ESPNOW_WIFI_CHANNEL,
  WIFI_IF_STA,
  nullptr
);

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);
  Serial.println("ESP-NOW - odbiornik");

  // --- Konfiguracja PWM ---
  ledcAttach(PWM_LEFT, pwmFreq, pwmResolution);
  ledcAttach(PWM_RIGHT, pwmFreq, pwmResolution);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(BUTTON1_OUT, OUTPUT);
  digitalWrite(BUTTON1_OUT, LOW);
  pinMode(BUTTON2_OUT, OUTPUT);
  digitalWrite(BUTTON2_OUT, LOW);
  pinMode(BUTTON3_OUT, OUTPUT);
  digitalWrite(BUTTON3_OUT, LOW);
  pinMode(BUTTON4_OUT, OUTPUT);
  digitalWrite(BUTTON4_OUT, LOW);

  // Inicjalizacja modułu wifi
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(
      AP_SSID,
      AP_PASSWORD,
      ESPNOW_WIFI_CHANNEL
  );

  WiFi.setChannel(
      ESPNOW_WIFI_CHANNEL,
      WIFI_SECOND_CHAN_NONE
  );

  while (!WiFi.STA.started()) {
    delay(100);
  }

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("MAC odbiornika: ");
  Serial.println(WiFi.macAddress());
  Serial.printf("Channel: %u\n", ESPNOW_WIFI_CHANNEL);
  Serial.print("Dozwolony nadajnik: ");
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      Serial.print(":");
    }
    if (senderMac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(senderMac[i],HEX);
  }
  Serial.println();

  // Uruchomienie ESP-NOW
  if (!ESP_NOW.begin()) {

    Serial.println("Blad: nie udalo sie uruchomic ESP-NOW");
    Serial.println("Restart za 5 sekund...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("ESP-NOW uruchomiony");

  // Dodanie nadajnika jako peera
  if (!receiver.add_peer()) {
    Serial.println("Blad: nie udalo sie dodac odbiornika");
    Serial.println("Restart za 5 sekund...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Inicjalizacja komunikacji UART z hoverboard");
  HoverSetupEsp32(HOVER_SERIAL, BAUDRATE, PIN_RX, PIN_TX);

  webServer.begin();

  Serial.println("Odbiornik gotowy");
  Serial.printf("ESP-NOW version: %d, max data length: %d\n", ESP_NOW.getVersion(), ESP_NOW.getMaxDataLen());
}


// =====================================================
// LOOP
// =====================================================

void loop() {
  now = millis();

  if(newJoystickData){
      // Przetwarzania otrzymanych danych
      updateControl(incomingJoystick.y,incomingJoystick.x);
      // Sprawdzanie przycisków
      ChceckButttonState(incomingJoystick.button, smoothModeCounter, smoothModePressed, smoothMode, LED_PIN);
      ChceckButttonState(incomingJoystick.button1, buttonCounter1, buttonPressed1, buttonState1, BUTTON1_OUT);
      ChceckButttonState(incomingJoystick.button2, buttonCounter2, buttonPressed2, buttonState2, BUTTON2_OUT);
      ChceckButttonState(incomingJoystick.button3, buttonCounter3, buttonPressed3, buttonState3, BUTTON3_OUT);
      ChceckButttonState(incomingJoystick.button4, buttonCounter4, buttonPressed4, buttonState4, BUTTON4_OUT);
      newJoystickData = false;
  }

  if (webData.phoneMode)
  {
    if (now - webServer.getLastPhoneReceive() > 500)
    {
        // Zatrzymanie przy braku sygnału z telefonu
        speedL = 0;
        speedR = 0;
        ster = 0;
    }
    else
    {
      if (now - lastSendPhoneUpdate > 25)
      {
        updateControl(
          webData.phoneY,
          webData.phoneX
        );
        lastSendPhoneUpdate = now;
      }
    }
  }
  else
  {
      if (now - lastReceive > 500)
      {
        // Zatrzymanie przy braku sygnału z joysticka
        speedL = 0;
        speedR = 0;
        ster = 0;
      }
  }

  if (now - lastSend > SEND_INTERVAL)
  {
    UpdateWheelSpeed();
    lastSend = now;
  }

  if (now - lastHoverboardDataUpdate > 500)
  {
    hoverboardData.hoverFeedbackOk = Receive(HOVER_SERIAL, hoverFeedback);
    if(hoverboardData.hoverFeedbackOk)
    {
      hoverboardData.iSpeedL = hoverFeedback.iSpeedL;
      hoverboardData.iSpeedR = hoverFeedback.iSpeedR;
      hoverboardData.iVolt   = hoverFeedback.iVolt;
      hoverboardData.iAmpL   = hoverFeedback.iAmpL;
      hoverboardData.iAmpR   = hoverFeedback.iAmpR;
    }
    lastHoverboardDataUpdate = now;
  }

  // limitLeft = digitalRead(END_LEFT) == LOW;
  // limitRight = digitalRead(END_RIGHT) == LOW;

  // Serial.printf("ster=%d  limitL=%d limitR=%d\n", ster, limitLeft, limitRight);
  if (ster != 0)
  {
    if (ster > 0)
    {
      if (!limitLeft)
      {
        ledcWrite(PWM_LEFT, ster); // ~78% mocy
        ledcWrite(PWM_RIGHT, 0);
      }
      else
      {
        ledcWrite(PWM_LEFT, 0);
        ledcWrite(PWM_RIGHT, 0);
      }
    }
    else if (!limitRight)
    {
      ledcWrite(PWM_LEFT, 0); // ~78% mocy
      ledcWrite(PWM_RIGHT, -ster);
    }
    else
    {
      ledcWrite(PWM_LEFT, 0);
      ledcWrite(PWM_RIGHT, 0);
    }
  }
  else
  {
    ledcWrite(PWM_LEFT, 0);
    ledcWrite(PWM_RIGHT, 0);
  }

  webServer.loop();
}