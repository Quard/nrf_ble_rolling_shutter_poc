#include "rf.h"


#define RF_GPIO_PIN 26


static struct rf_cmd_t rf_cmd;
static const struct device *gpio0_dev;
static const nrfx_timer_t rf_timer = NRFX_TIMER_INSTANCE(1);

void rf_start_send(void) {
    nrfx_timer_disable(&rf_timer);
	nrf_timer_shorts_disable(rf_timer.p_reg, ~0);
	nrf_timer_int_disable(rf_timer.p_reg, ~0);

	nrfx_timer_extended_compare(
		&rf_timer,
		NRF_TIMER_CC_CHANNEL0,
		nrfx_timer_us_to_ticks(&rf_timer, 360),
		NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
		true
	);
	nrfx_timer_clear(&rf_timer);
	nrfx_timer_enable(&rf_timer);
}

static void rf_timer_handler(nrf_timer_event_t event_type, void *context) {
	struct rf_cmd_t *cmd = (struct rf_cmd_t *) context;

	if (event_type == NRF_TIMER_EVENT_COMPARE0) {
        if (cmd->resend_pause != 0) {
            cmd->resend_pause--;
            return;
        }

		if (cmd->cur_bit == 8) {
			cmd->cur_bit = 0;
			cmd->cur_byte++;
			if (cmd->cur_byte <= cmd->seq_len) {
			}
		}
		if (cmd->cur_byte > cmd->seq_len) {
			printk("\nSent\n");
			gpio_pin_set(gpio0_dev, RF_GPIO_PIN, 0);

            if (cmd->resend != 0) {
                cmd->resend--;
                cmd->resend_pause = 4;
                cmd->cur_bit = 0;
	            cmd->cur_byte = 0;
            } else {
                nrfx_timer_disable(&rf_timer);
			    return;
            }
		}

		uint8_t bt = (cmd->seq[cmd->cur_byte] >> (7 - cmd->cur_bit)) & 1;
		gpio_pin_set(gpio0_dev, RF_GPIO_PIN, bt);

		cmd->cur_bit++;

	}
}

int rf_timer_init(struct rf_cmd_t *cmd) {
    nrfx_err_t err;
	nrfx_timer_config_t timer_cfg = {
		.frequency = NRF_TIMER_FREQ_16MHz,
		.mode = NRF_TIMER_MODE_TIMER,
		.bit_width = NRF_TIMER_BIT_WIDTH_32,
		.p_context = (void *) cmd,
	};

	err = nrfx_timer_init(&rf_timer, &timer_cfg, rf_timer_handler);
	if (err != NRFX_SUCCESS) {
		printk("nrfx_timer_init failed with: %d\n", err);
	}

    return err;
}

int rf_init(void) {
    int err = 0;

    gpio0_dev = device_get_binding("GPIO_0");
	if (gpio0_dev == NULL) {
		printk("Error: could not find device GPIO_0\n");
		return -1;
	}
	err = gpio_pin_configure(gpio0_dev, RF_GPIO_PIN, GPIO_OUTPUT);
	if (err != 0) {
		printk("Error %d: failed to configure GPIO_0 pin %d\n", err, RF_GPIO_PIN);
		return err;
	}

    rf_timer_init(&rf_cmd);
    IRQ_CONNECT(TIMER1_IRQn, NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY, nrfx_timer_1_irq_handler, NULL, 0);

    return 0;
}

void rf_set_cmd(uint8_t cmd_code) {
    rf_cmd.seq_len = 17;
	rf_cmd.cur_bit = 0;
	rf_cmd.cur_byte = 0;
    rf_cmd.resend = 10;

    uint8_t seq[][17] = {
        // ... fill with your codes
    };

    if (cmd_code < 12) {
        memcpy(rf_cmd.seq, seq[cmd_code], rf_cmd.seq_len);
    }
}