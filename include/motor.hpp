/**
 * @file motor.hpp
 *
 * @author Comp Thunderatz <comp@thunderatz.org>
 *
 * @brief Motor class definition.
 *
 * @date 04/2025
 */

#ifndef MOTOR_HPP
#define MOTOR_HPP

#include "tim.h"
#include "gpio.h"
#include <cstdint>

/**
 * @brief Motor class.
 */
class Motor {
public:
    /**
     * @brief Construct a new Motor object.
     *
     */
    Motor(
        void (*tim_init)(), TIM_HandleTypeDef* tim_handle, uint8_t tim_ch, GPIO_TypeDef* direction_port,
        uint16_t direction_pin, int8_t min_speed = -100, int8_t max_speed = 100
    );

    /**
     * @brief Set the speed of the motor.
     *
     * @param speed Motor speed.
     *
     */
    void set_speed(int8_t speed);

    /**
     * @brief Stop the motor.
     *
     */
    void stop();

private:
    TIM_HandleTypeDef* tim_handle;
    uint8_t            tim_ch;

    GPIO_TypeDef* direction_port;
    uint16_t      direction_pin;

    const int8_t min_speed;
    const int8_t max_speed;
};

#endif  // MOTOR_HPP
