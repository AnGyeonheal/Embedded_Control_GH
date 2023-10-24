#include "stm32f411xe.h"
#include "math.h"

// #include "ecSTM32F411.h"
#include "ecPinNames.h"
#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecTIM.h"
#include "ecPWM.h"
#include "ecEXTI.h"


// Definition Button Pin & PWM Port, Pin
#define BUTTON_PIN 13
PinName_t PWM_PIN = PA_0;

void setup(void);

void TIM3_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

uint32_t count = 0;
float period = 1;
float duty = 0;
float n = 1;
static int stop = 0;

int main(void) {
    // Initialization --------------------------------------------------
    setup();

    // Infinite Loop ---------------------------------------------------
    while (1) {
        if(stop == 0){
            PWM_duty(PWM_PIN, (duty / period));
        }
        else if(stop == 1){
            PWM_duty(PWM_PIN, (0 / period));
        }
    }
}

void TIM3_IRQHandler(void) {
    if (is_UIF(TIM3)) {            // Check UIF(update interrupt flag)
        count++;
        if(count > 2000){
            if(n == 1){
                duty = 0.25;
                n=2;
            }
            else if (n == 2){
                duty = 0.75;
                n=1;
            }
            count = 0;
        }
        clear_UIF(TIM3);            // Clear UI flag by writing 0
    }
}

void EXTI15_10_IRQHandler(void) {
    if (is_pending_EXTI(BUTTON_PIN)){
        stop ^= 1;
        clear_pending_EXTI(BUTTON_PIN); // cleared by writing '1'
    }
}

// Initialiization
void setup(void) {
    // CLK SETUP
    RCC_PLL_init();
    // EXTI: BUTTON SETUP
    GPIO_init(GPIOC, BUTTON_PIN, INPUT);
    GPIO_pupd(GPIOC, BUTTON_PIN, EC_PU);
    EXTI_init(GPIOC, BUTTON_PIN, FALL, 0);
    // TIMER SETUP
    TIM_UI_init(TIM3, 1);
    // PWM SETUP
    PWM_init(PWM_PIN);
    PWM_period_ms(PWM_PIN, period);

}
