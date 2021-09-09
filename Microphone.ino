#include <FastLED.h>

#define NUM_LEDS 28
#define DATA_PIN 2
#define MIC_PIN A0

CRGBArray<NUM_LEDS> leds;

enum animationTypes {
  ANIMATION_NONE = 0,
  ANIMATION_RAINBOW,
  ANIMATION_SOLID
};

int read_mic() {
    int value = analogRead(MIC_PIN);
    value = abs(value - 512); // max 450
    value = (value <= 0) ? 0 : (value - 0);
    return value;
}

void rainbow() {
    int value = read_mic();
    int mappedValue = map(value, 0, 450, 0, NUM_LEDS);

    if(mappedValue <= 0) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        return;
    }

    fill_rainbow(leds, mappedValue, 100, 20);
}

void rainbow_middle() {
    int value = read_mic();
    int mappedValue = map(value, 0, 450, 0, NUM_LEDS);

    if(mappedValue <= 0) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        return;
    }

    fill_rainbow(&leds[NUM_LEDS / 2 - 1], mappedValue / 2, 120, 5);

    for(int i = (NUM_LEDS / 2 - 1); i >= 0; i--) { 
        if (i == 0) {
            leds[i] = leds[NUM_LEDS - 1];
            break;
        }

        leds[i] = leds[NUM_LEDS - i];
    }

}

void solid_trail() {
    static int calledTime = 0;
    static int lastHead = 0;

    int value = read_mic();
    int mappedValue = map(value, 0, 450, 0, NUM_LEDS);

    if (mappedValue > lastHead) {
        lastHead = mappedValue;
        leds[lastHead] = CRGB::White;
    }else {
        if (calledTime % 10 == 0) {
            calledTime = 0;
            leds[lastHead] = CRGB::Black;
            lastHead = lastHead - 1;
            leds[lastHead] = CRGB::White;
        }
    }

    fill_solid(leds, mappedValue, CRGB::Red);
    calledTime++;
}

void solid_trail2() {
    static int calledTime = 0;
    static int lastHead = 0;

    int value = read_mic();
    int mappedValue = map(value, 0, 450, 0, NUM_LEDS);

    if (mappedValue > lastHead) {
        lastHead = mappedValue;
        leds[lastHead] = CRGB::White;
        leds[lastHead - 1] = CRGB::White;
    }else {
        if (calledTime % 10 == 0) {
            calledTime = 0;
            leds[lastHead] = CRGB::Black;
            leds[lastHead - 1] = CRGB::Black;
            lastHead = lastHead - 1;
            leds[lastHead] = CRGB::White;
            leds[lastHead - 1] = CRGB::White;
        }
    }

    fill_solid(leds, mappedValue, CRGB::Red);
    calledTime++;
}

void setup() {
    pinMode(MIC_PIN, INPUT);
    Serial.begin(9600);

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(10);
    FastLED.clear();
    FastLED.show();
}

void loop() {
    solid_trail2();
    FastLED.show();
    delay(5);
}