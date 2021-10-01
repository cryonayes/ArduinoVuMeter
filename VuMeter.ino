#include <LEDController.h>
#include <OneButton.h>
#include <EEPROM.h>

#define MIC_PIN A0
#define LED_COUNT 27
#define LED_PIN 2
#define BUTTON_PIN 12
#define STATE_ADDRESS 0

int lvl = 10;

typedef LEDController<LED_COUNT, LED_PIN> MyController;
MyController* myController;

OneButton animationButton(BUTTON_PIN, true, true);

void onButtonShortPress() {
    myController->setColor(random8(255), random8(255), random8(255));
}

void onButtonLongPress() {
    static uint8_t animationIndex = 1;
    if (animationIndex % 13 == 0) { animationIndex = 1; }

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
    }else if(animationIndex == 6) {
        myController->changeAnimation(AnimationTypes::ANIMATION_SOLID_TRAIL_DOUBLE);
    }else if(animationIndex == 7) {
        myController->changeAnimation(AnimationTypes::ANIMATION_SOLID_TRAIL_MIDDLE);
    }else if(animationIndex == 8) {
        myController->changeAnimation(AnimationTypes::ANIMATION_SOLID_TRAIL_MIDDLE_DOUBLE);
    }else if(animationIndex == 9) {
        myController->changeAnimation(AnimationTypes::ANIMATION_RAINBOW_TRAIL_MIDDLE);
    }else if(animationIndex == 10) {
        myController->changeAnimation(AnimationTypes::ANIMATION_RAINBOW_TRAIL_MIDDLE_CYCLE);
    }else if(animationIndex == 11) {
        myController->changeAnimation(AnimationTypes::ANIMATION_RAINBOW_TRAIL_DOUBLE_MIDDLE);
    }else if(animationIndex == 12) {
        myController->changeAnimation(AnimationTypes::ANIMATION_RAINBOW_TRAIL_DOUBLE_MIDDLE_CYCLE);
    }
    updateEEPROM(STATE_ADDRESS, animationIndex);
    animationIndex++;
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
    Serial.begin(115200);
    LEDControllerSettings settings = {
        .ledPIN = 2,
        .brightness = 25,
        .delay = 0,
        .animationSettings = {
            .rainbowStep = 10,
            .countTrailStep = 10,
            .delayTrail = 0
        },
        .powerSettings = {
            .volt = 5,
            .miliamps = 50
        }
    };
    int state = EEPROM.read(STATE_ADDRESS);
    myController = &MyController::getInstance(settings);
    myController->changeAnimation(static_cast<AnimationTypes>(state));

    animationButton.attachLongPressStop(onButtonLongPress);
    animationButton.attachClick(onButtonShortPress);
    animationButton.attachDoubleClick(onButtonDoubleClick);
}

int read_mic() {
    int value = (analogRead(MIC_PIN) - 512);
   
    lvl = (lvl * 7 + value) >> 3;
    double ledLevel = lvl / 512.0;
    //ledLevel = (log(pow(18 * ledLevel, 1/0.9) - 1.1)) * 512;
    ledLevel = (log(pow(18 * ledLevel, 1/0.9) - 1)) * 512;
    
    if(ledLevel > 512) {
        ledLevel = 512;
    }
    if (ledLevel < 0){
        ledLevel = 0;
    }
    
    return ledLevel;
}

int updateEEPROM(uint8_t address, uint8_t value) {
    int storedValue = EEPROM.read(address);
    if (storedValue != value) {
        EEPROM.write(address, value);
    }
}

void loop() {
    animationButton.tick();
    int micVal = read_mic();
    int ledVal = map(micVal, 0, 513, 0, LED_COUNT);
    myController->update(ledVal);

    if(myController->getAnimation() == AnimationTypes::ANIMATION_RAINBOW_CYCLE ||
    myController->getAnimation() == AnimationTypes::ANIMATION_RAINBOW_MIDDLE_CYCLE || 
    myController->getAnimation() == AnimationTypes::ANIMATION_RAINBOW_TRAIL_DOUBLE_MIDDLE_CYCLE || 
    myController->getAnimation() == AnimationTypes::ANIMATION_RAINBOW_TRAIL_MIDDLE_CYCLE) {
        EVERY_N_MILLISECONDS(20) {
            myController->setRainbowColor(myController->getRainbowColor().hue + 5 % 255);
        }
    }
}