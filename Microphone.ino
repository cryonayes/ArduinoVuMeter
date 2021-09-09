#include <FastLED.h>
#include <OneButton.h>

#define NUM_LEDS 28
#define DATA_PIN 2
#define COLOR_PIN A2
#define MIC_PIN A0

#define BUTTON_PIN 5
#define SHORT_PRESS 500
#define LONG_PRESS 1000

CRGBArray<NUM_LEDS> leds;
OneButton animationButton(BUTTON_PIN, true, true);

CRGB color(100, 100, 100);
uint8_t brightness = 10;
uint8_t selectedAnimation = 0;

enum animationTypes {
  ANIMATION_RAINBOW = 0,
  ANIMATION_RAINBOW_MIDDLE = 1,
  ANIMATION_SOLID_TRAIL = 2,
  ANIMATION_SOLID_TRAIL_DOUBLE = 3
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

    fill_rainbow(leds, mappedValue, color, 12);
}

void rainbow_middle() {
    int value = read_mic();
    int mappedValue = map(value, 0, 450, 0, NUM_LEDS);

    if(mappedValue <= 0) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        return;
    }

    fill_rainbow(&leds[NUM_LEDS / 2 - 1], mappedValue / 2 + 1, color, 12);

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

    fill_solid(leds, mappedValue, CRGB::Blue);
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
    pinMode(COLOR_PIN, INPUT);
    // Serial.begin(9600);

    animationButton.attachLongPressStop(onButtonLongPress);
    animationButton.attachClick(onButtonShortPress);

    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
    FastLED.setBrightness(10);
    FastLED.clear();
    FastLED.show();
}

void onButtonShortPress() {
    selectedAnimation++;
    if (selectedAnimation % 4 == 0) { selectedAnimation = 0; }
    FastLED.clear();
}

void onButtonLongPress() {
    color = CRGB(random(255), random(255), random(255));
}

void loop() {
    animationButton.tick();

    switch(selectedAnimation) {
        case animationTypes::ANIMATION_RAINBOW:
            rainbow();
            break;
        case animationTypes::ANIMATION_RAINBOW_MIDDLE:
            rainbow_middle();
            break;
        case animationTypes::ANIMATION_SOLID_TRAIL:
            solid_trail();
            break;
        case animationTypes::ANIMATION_SOLID_TRAIL_DOUBLE:
            solid_trail2();
            break;
        default:
            break;
    }
    FastLED.show();
}