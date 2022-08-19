#pragma once

#include <Adafruit_PWMServoDriver.h>

class Leds
{
public:
    Leds()
    {
        _initializePca();
    }
    void on(int port) {
        _pca[0].setPWM(port, 0, 1024);
    }
    void off(int port) {
        _pca[0].setPWM(port, 0, 0);
    }
    void offAll() {
        for (int i = 0; i < 16; i++) {
            off(i);
        }
    }
private:
    Adafruit_PWMServoDriver _pca[1] = {0x40};
    void _initializePca(void)
    {
        for (auto pca : _pca)
        {
            pca.begin();
            pca.setOscillatorFrequency(27000000);
            pca.setPWMFreq(1600); // This is the maximum PWM frequency
        }
    };
};