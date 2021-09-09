#include <FastLED.h>
#include <OneButton.h>

enum AnimationTypes {
    ANIMATION_RAINBOW = 0,
    ANIMATION_RAINBOW_MIDDLE,
    ANIMATION_SOLID_TRAIL
};

struct AnimationSettings {
    uint8_t rainbowStep;
    uint8_t peakTrailSize;
    uint8_t countTrailStep;
    uint8_t delayTrail;
};

struct MicroPhoneNewSettings {
    uint8_t ledPIN;
    uint8_t brightness;
    AnimationSettings animationSettings;
};

template <int numberOfLEDs>
class LEDController {
    private:
        MicroPhoneNewSettings config;

        CRGB color(255, 0, 0);
        CRGBArray<numberOfLEDs> leds;
        AnimationTypes selectedAnimation;
    public:
        LEDController(MicroPhoneNewSettings config): config(config) {
            FastLED.addLeds<WS2812B, ledPIN, GRB>(this->config.ledPIN, numberOfLEDs);
        };

        void setBrightness(uint8_t brightness) {
            this->config.brightness = brightness;
            FastLED.setBrightness(this->config.brightness);
        }

        uint8_t getBrightness(uint8_t brightness) {
            return this->config.brightness;
        }

        void setColor(uint8_t r, uint8_t g, uint8_t b) {
            this->color = CRGB(r, g, b);
        }

        CRGB getColor() {
            return this->color;
        }

        void changeAnimation(AnimationTypes selectAnimation) {
            this->selectedAnimation = selectAnimation;
        }

        void update(uint8_t fillCount) {
            switch (selectedAnimation) {
                case ANIMATION_RAINBOW:
                    this->rainbowAnimation(fillCount);
                    break;
                case ANIMATION_RAINBOW_MIDDLE:
                    this->rainbowAnimationMiddle(fillCount);
                    break;
                case ANIMATION_SOLID_TRAIL:
                    this->solidTrailAnimation(fillCount);
                    break;
            }
            FastLED.show();
        }

        ~LEDController() {}
    private:
        void rainbowAnimation(uint8_t fillCount) {
            if(fillCount == 0) {
                fill_solid(this->leds, numberOfLEDs, CRGB::Black);
                return;
            }
            fill_rainbow(this->leds, fillCount, rgb2hsv_approximate(this->color).hue, this->config.animationSettings.rainbowStep);
        }

        void rainbowAnimationMiddle(uint8_t fillCount) {
            if(fillCount == 0) {
                fill_solid(this->leds, numberOfLEDs, CRGB::Black);
                return;
            }
            
            fill_rainbow(this->&leds[numberOfLEDs / 2 - 1], fillCount / 2 + 1, rgb2hsv_approximate(this->color).hue, 
                this->config.animationSettings.rainbowStep);

            for(int i = (numberOfLEDs / 2 - 2); i >= 0; i--) { 
                if (i == 0) {
                    this->leds[i] = this->leds[numberOfLEDs - 1];
                    break;
                }
                this->leds[i] = this->leds[numberOfLEDs - i];
            }
        }

        void solidTrailAnimation(uint8_t fillCount) {
            static uint8_t currentStep = 0;
            static uint8_t currentPeak = 0;

            if(fillCount == 0) {
                fill_solid(this->leds, numberOfLEDs, CRGB::Black);
                return;
            }

            if (fillCount > currentPeak) {
                currentPeak = fillCount;
            }

            if(currentStep % this->config.animationSettings.countTrailStep == 0) {
                if(currentPeak > 0) { currentPeak--; }
                currentStep = 0;
            }

            fill_solid(this->leds, currentPeak, this->color);
            for(int i = 0; i < this->config.animationSettings.peakTrailSize; i++) {
                this->leds[currentPeak - i].setRGB(255, 255, 255);
            }

            currentStep++;
            FastLED.delay(sleepTime);
        }
};