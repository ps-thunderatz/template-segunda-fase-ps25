/**
 * @file main.cpp
 *
 * @brief Main function
 */

#include "controller.hpp"
#include "target.hpp"
#include "led.hpp"
#include "mcu.hpp"
#include "locomotion.hpp"
#include "rc.hpp"

/*****************************************
 * Main Function
 *****************************************/

int main(void) {
    // TODO: Adicionar a lógica da main.
    for (;;) { }
    return 0;
}

extern "C" void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
    Rc::handle_global_callback(htim);
}
