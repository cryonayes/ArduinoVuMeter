#include <LEDController.h>
#include <OneButton.h>

#define MIC_PIN A4
#define SENSITIVITY A0
#define LED_COUNT 28
#define LED_PIN 2
#define BUTTON_PIN 5

typedef LEDController<LED_COUNT, LED_PIN> MyController;
MyController* myController;

OneButton animationButton(BUTTON_PIN, true, true);

void onButtonShortPress() {
    myController->setColor(random8(255), random8(255), random8(255));
}

void onButtonLongPress() {
    static int animationIndex = 0;
    animationIndex++;
    if (animationIndex % 6 == 0) { animationIndex = 0; }

    if(animationIndex == 1) {
        myController->changeAnimation(AnimationTypes::ANIMATION_RAINBOW);
    }else if(animationIndex == 2) {
        myController->changeAnimation(AnimationTypes::ANIMATION_RAINBOW_CYCLE);
    }else if(animationIndex == 3) {
        myController->changeAnimation(AnimationTypes::ANIMATION_RAINBOW_MIDDLE);
    }else if(animationIndex == 4) {
        myController->changeAnimation(AnimationTypes::ANIMATION_RAINBOW_MIDDLE_CYCLE);
    }else if(animationIndex == 5) {
        myController->changeAnimation(AnimationTypes::ANIMATION_SOLID_TRAIL);
    }
}

void onButtonDoubleClick() {
    static int doubleClickIndex = 0;
    if (doubleClickIndex == 0) {
        myController->setColor(0, 255, 0);
    }else if (doubleClickIndex == 1) {
        myController->setColor(0, 0, 255);
    }else if (doubleClickIndex == 2) {
        myController->setColor(255, 0, 0);
    }
    doubleClickIndex++;

    if (doubleClickIndex % 3 == 0) {
        doubleClickIndex = 0;
    }
}

void setup() {
    Serial.begin(9600);
    LEDControllerSettings settings = {
        .ledPIN = 2,
        .brightness = 100,
        .delay = 0,
        .animationSettings = {
            .rainbowStep = 10,
            .peakTrailSize = 2,
            .countTrailStep = 20,
            .delayTrail = 0
        },
        .powerSettings = {
            .volt = 5,
            .miliamps = 50
        }
    };
    myController = &MyController::getInstance(settings);
    myController->changeAnimation(AnimationTypes::ANIMATION_RAINBOW_MIDDLE);

    animationButton.attachLongPressStop(onButtonLongPress);
    animationButton.attachClick(onButtonShortPress);
    animationButton.attachDoubleClick(onButtonDoubleClick);
}

int read_mic() {
    int value = analogRead(MIC_PIN);
    int sensitivity = 0; //analogRead(SENSITIVITY);
    sensitivity = map(sensitivity, 0, 1025, 0, 512);

    value = (value <= sensitivity) ? 0 : (value - sensitivity);
    return value;
}

void loop() {
    animationButton.tick();
    int micVal = read_mic();
    Serial.println(micVal, DEC);
    int ledVal = map(micVal, 0, 1025, 0, 29);

    myController->update(ledVal);

    if(myController->getAnimation() == AnimationTypes::ANIMATION_RAINBOW_CYCLE || myController->getAnimation() == AnimationTypes::ANIMATION_RAINBOW_MIDDLE_CYCLE) {
        EVERY_N_MILLISECONDS(20) {
            myController->setRainbowColor(myController->getRainbowColor().hue + 5 % 255);
        }
    }
}