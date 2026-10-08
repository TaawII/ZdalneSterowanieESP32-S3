#include <Arduino.h>
#include "ESP32_NOW.h"
#include "WiFi.h"

// =====================================================
// MAC odbiornika
// =====================================================

// uint8_t receiverMac[] = {
//   0x80, 0xB5, 0x4E, 0xC6, 0xEF, 0xC8
// };
uint8_t receiverMac[] = {
  0x90, 0x70, 0x69, 0x33, 0x76, 0x38
};

// =====================================================
// Definicje
// =====================================================

// Piny dla ESP32-S3
#define VRX_PIN 7    // ADC1_CH5 (GPIO6) – oś X
#define VRY_PIN 6    // ADC1_CH6 (GPIO7) – oś Y
#define SW_PIN  8    // GPIO8 – przycisk
#define SW_BUTTON1 1
#define SW_BUTTON2 2
#define SW_BUTTON3 5
#define SW_BUTTON4 4

// Kanał wifi (ustawić taki sam jak na odbiorniku)
#define ESPNOW_WIFI_CHANNEL 6

// =====================================================
// Zmiennne globalne
// =====================================================

const int16_t X_CENTER = 1960;
const int16_t Y_CENTER = 1900;
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 100;  // co 100 ms

// =====================================================
// Struktura danych (identyczna jak w odbiorniku)
// =====================================================

typedef struct {
  int16_t x;       // -1000..1000
  int16_t y;       // -1000..1000
  bool button;     // true = wciśnięty
  bool button1;
  bool button2;
  bool button3;
  bool button4;
} JoystickData;

JoystickData joystick;

// ==============================
// Funkcje pomocnicze
// ==============================

int16_t mapWithCenterX(int raw, int center) {
  if (raw >= center) {
    return map(raw, center, 4095, 0, 1000);
  } else {
    return map(raw, 0, center, -1000, 0);
  }
}

int16_t mapWithCenterY(int raw, int center) {
  if (raw >= center) {
    return map(raw, center, 4095, 0, 1000);
  } else {
    return map(raw, 0, center, -1000, 0);
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
      Serial.println("Blad: nie udalo sie dodac peera");
      return false;
    }
    return true;
  }

  // Wysłanie danych
  bool send_data(const uint8_t *data, size_t len) {
    if (!send(data, len)) {
      Serial.println("Blad: nie udalo sie wyslac danych");
      return false;
    }
    return true;
  }
};


// =====================================================
// Utworzenie peera
// =====================================================

ESP_NOW_Peer_Class receiver(
  receiverMac,
  ESPNOW_WIFI_CHANNEL,
  WIFI_IF_STA,
  nullptr
);

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);
  Serial.println("ESP-NOW - nadajnik");

  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(SW_BUTTON1, INPUT_PULLUP);
  pinMode(SW_BUTTON2, INPUT_PULLUP);
  pinMode(SW_BUTTON3, INPUT_PULLUP);
  pinMode(SW_BUTTON4, INPUT_PULLUP);

  // Inicjalizacja modułu wifi
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  Serial.print("MAC nadajnika: ");
  Serial.println(WiFi.macAddress());
  Serial.printf("Channel: %u\n", ESPNOW_WIFI_CHANNEL);

  // Uruchomienie ESP-NOW
  if (!ESP_NOW.begin()) {
    Serial.println("Blad: nie udalo sie uruchomic ESP-NOW");
    Serial.println("Restart za 5 sekund...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("ESP-NOW uruchomiony");

  // Dodanie odbiornika jako peera
  if (!receiver.add_peer()) {
    Serial.println("Blad: nie udalo sie dodac odbiornika");
    Serial.println("Restart za 5 sekund...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Odbiornik zostal dodany");
  Serial.printf("ESP-NOW version: %d, max data length: %d\n", ESP_NOW.getVersion(), ESP_NOW.getMaxDataLen());
}


// =====================================================
// LOOP
// =====================================================

void loop() {
  unsigned long now = millis();
  if (now - lastSendTime >= sendInterval) {
    lastSendTime = now;

    int16_t rawX = analogRead(VRX_PIN);
    int16_t rawY = analogRead(VRY_PIN);
    bool buttonPressed = digitalRead(SW_PIN) == LOW;
    bool buttonPressed1 = digitalRead(SW_BUTTON1) == LOW;
    bool buttonPressed2 = digitalRead(SW_BUTTON2) == LOW;
    bool buttonPressed3 = digitalRead(SW_BUTTON3) == LOW;
    bool buttonPressed4 = digitalRead(SW_BUTTON4) == LOW;

    joystick.x = mapWithCenterX(rawX, X_CENTER);
    joystick.y = mapWithCenterY(rawY, Y_CENTER);
    joystick.button = buttonPressed;
    joystick.button1 = buttonPressed1;
    joystick.button2 = buttonPressed2;
    joystick.button3 = buttonPressed3;
    joystick.button4 = buttonPressed4;

    if (abs(joystick.x) < 10) joystick.x = 0;
    if (abs(joystick.y) < 10) joystick.y = 0;

    joystick.x = constrain(joystick.x, -1000, 1000);
    joystick.y = constrain(joystick.y, -1000, 1000);

    receiver.send_data((uint8_t *)&joystick, sizeof(joystick));
    Serial.printf("X=%4d  Y=%4d  Btn=%d %d %d %d %d\n", joystick.x, joystick.y, joystick.button, joystick.button1, joystick.button2, joystick.button3, joystick.button4);
  }
}