#ifndef __RF_H__
#define __RF_H__

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/gpio.h>
#include <nrfx_timer.h>
#include <string.h>

struct rf_cmd_t {
    uint8_t seq[32];
    uint8_t seq_len;
    uint8_t cur_byte;
    uint8_t cur_bit;
};

int  rf_init(void);
void rf_start_send(void);
void rf_set_cmd(uint8_t cmd_code);

#endif
