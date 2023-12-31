/*
 * Copyright (c) 2024 Jakub Duchniewicz
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define STRIP_NODE  DT_ALIAS(led_strip)
#define STRIP_NUM_PIXELS DT_PROP(DT_ALIAS(led_strip), chain_length)

#define UPDATE_DELAY K_MSEC(50) /* in ms */

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
    RGB(0x0f, 0x00, 0x00), /* red */
    RGB(0x00, 0x0f, 0x00), /* green */
    RGB(0x00, 0x00, 0x0f), /* blue */
};

struct led_rgb pixels[STRIP_NUM_PIXELS];

static const struct device * const strip = DEVICE_DT_GET(STRIP_NODE);

int main(void)
{
    size_t cursor = 0, color = 0;
    int rc;

    if (device_is_ready(strip)) {
        LOG_INF("Found LED strip device %s", strip->name);
    } else {
        LOG_INF("LED strip device %s is not ready", strip->name);
        return 0;
    }

    LOG_INF("Displaying pattern on strip");

    // cycle the colors on the strip
    while (1) {
        memset(&pixels, 0x00, sizeof(pixels));
        memcpy(&pixels[cursor], &colors[color], sizeof(struct led_rgb));
        rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);

        if (rc) {
            LOG_ERR("could not update strip: %d", rc);
        }

        cursor++;
        if (cursor >= STRIP_NUM_PIXELS) {
            cursor = 0;
            color++;
            if (color == ARRAY_SIZE(colors)) {
                color = 0;
            }
        }

        k_sleep(UPDATE_DELAY);
    }

	return 0;
}

