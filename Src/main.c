#include <stdint.h>
#include "i2c.h"

int main(void)
{
    uint8_t rx2[2];

    I2C1_init();

    led_off();

    I2C1_target_receiver(rx2, 2);

    /*
     * STOP HERE.
     *
     * We will inspect rx2[] in the debugger.
     */
    while (1)
    {
    }
}
