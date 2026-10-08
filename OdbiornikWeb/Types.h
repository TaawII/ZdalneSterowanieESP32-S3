#pragma once

#include <stdint.h>

struct JoystickData {
    int16_t x;
    int16_t y;
    bool button;
    bool button1;
    bool button2;
    bool button3;
    bool button4;
};

struct HoverboardData {
    bool hoverFeedbackOk = false; // Czy dane zostały poprawnie pobrane
    int16_t iSpeedL = 0;   // 100* km/h
    int16_t iSpeedR = 0;   // 100* km/h
    uint16_t iVolt = 0;    // 100* V
    int16_t iAmpL = 0;   // 100* A
    int16_t iAmpR = 0;   // 100* A
};

struct WebData {
    int16_t &x;
    int16_t &y;
    int16_t &phoneX;
    int16_t &phoneY;
    long &speedMax;
    bool &phoneMode;
    bool &smoothMode;
    bool &button1;
    bool &button2;
    bool &button3;
    bool &button4;
    HoverboardData &hoverboardData;

    WebData(
        int16_t &xRef,
        int16_t &yRef,
        int16_t &phoneXRef,
        int16_t &phoneYRef,
        long &speedMaxRef,
        bool &phoneModeRef,
        bool &smoothModeRef,
        bool &button1Ref,
        bool &button2Ref,
        bool &button3Ref,
        bool &button4Ref,
        HoverboardData &hoverboardDataRef
    )
        : x(xRef),
          y(yRef),
          phoneX(phoneXRef),
          phoneY(phoneYRef),
          speedMax(speedMaxRef),
          phoneMode(phoneModeRef),
          smoothMode(smoothModeRef),
          button1(button1Ref),
          button2(button2Ref),
          button3(button3Ref),
          button4(button4Ref),
          hoverboardData(hoverboardDataRef)
    {
    }
};