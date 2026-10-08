# 🚗 Zdalne sterowanie pojazdem – ESP32-S3

Projekt systemu zdalnego sterowania pojazdem elektrycznym z wykorzystaniem **ESP32-S3**.

System umożliwia sterowanie pojazdem za pomocą **joysticka i przycisków**, a także z poziomu **strony internetowej uruchomionej bezpośrednio na ESP32-S3**.

Do sterowania silnikami wykorzystano sterownik pochodzący z hoverboarda z firmware'em **[Hoverboard-Firmware-Hack-Gen2.x](https://github.com/RoboDurden/Hoverboard-Firmware-Hack-Gen2.x)**.

---

## ⚙️ Jak to działa?

System składa się z dwóch głównych urządzeń:

### 🎮 Nadajnik – joystick

ESP32-S3 znajdujący się przy operatorze:

- odczytuje pozycję joysticka,
- obsługuje **4 przyciski**,
- przetwarza dane sterujące,
- wysyła je bezprzewodowo do pojazdu za pomocą **ESP-NOW**.

### 🚙 Odbiornik – pojazd

ESP32-S3 znajdujący się w pojeździe:

- odbiera dane z nadajnika poprzez **ESP-NOW**,
- przetwarza polecenia sterujące,
- komunikuje się ze sterownikiem hoverboarda,
- udostępnia dodatkowo **stronę internetową do sterowania pojazdem**.

---

## 🌐 Sterowanie i monitoring przez stronę WWW

Odbiornik ESP32-S3 posiada wbudowany interfejs WWW.

Po połączeniu z ESP32 można otworzyć stronę internetową, która umożliwia:

- **sterowanie pojazdem** bez użycia fizycznego nadajnika,
- **odczyt dodatkowych informacji o stanie pojazdu i układu sterowania**,
- monitorowanie parametrów podczas pracy,
- wykorzystanie interfejsu podczas testów i diagnostyki.

Dzięki temu pojazdem można sterować na dwa sposoby:

**1. Nadajnik**
```text
Joystick + 4 przyciski
          ↓
       ESP-NOW
          ↓
       Pojazd
```

**2. Strona WWW**
```text
Telefon / komputer
        ↓
   Strona WWW
        ↓
    ESP32-S3
        ↓
     Pojazd
```

Interfejs WWW może być szczególnie przydatny podczas testów, diagnostyki oraz uruchamiania pojazdu.

---

## 📁 Struktura projektu

```text
ZdalneSterowanieESP32-S3/
│
├── Nadajnik/
│   └── kod nadajnika ESP32-S3
│
├── OdbiornikWeb/
│   └── kod odbiornika + Web UI
│
└── README.md
```

---

## 🔧 Wykorzystane technologie

- **ESP32-S3**
- **ESP-NOW** – komunikacja pomiędzy nadajnikiem i pojazdem
- **UART** – komunikacja z kontrolerem hoverboarda
- **Web UI** – sterowanie przez przeglądarkę
- **Hoverboard-Firmware-Hack-Gen2.x** – sterowanie silnikami

---

## ⚙️ Konfiguracja i wgranie

Przed kompilacją należy skonfigurować adresy MAC urządzeń używanych w komunikacji **ESP-NOW**.

### 🎮 Nadajnik

W kodzie znajdującym się w katalogu `Nadajnik/` należy ustawić adres MAC **odbiornika**, czyli ESP32-S3 znajdującego się w pojeździe.

```text
Nadajnik → MAC odbiornika
```

### 🚙 Odbiornik

W kodzie znajdującym się w katalogu `OdbiornikWeb/` należy ustawić adres MAC **nadajnika**, czyli ESP32-S3 obsługującego joystick.

```text
Odbiornik → MAC nadajnika
```

Po ustawieniu odpowiednich adresów MAC należy:

1. skompilować projekt `Nadajnik/` i wgrać go na ESP32-S3 nadajnika,
2. skompilować projekt `OdbiornikWeb/` i wgrać go na ESP32-S3 odbiornika znajdującego się w pojeździe.

Po poprawnym skonfigurowaniu urządzenia mogą komunikować się ze sobą za pomocą **ESP-NOW**.

---

## 📚 Źródła

**Firmware sterownika hoverboarda:**  
[Hoverboard-Firmware-Hack-Gen2.x – RoboDurden](https://github.com/RoboDurden/Hoverboard-Firmware-Hack-Gen2.x)
