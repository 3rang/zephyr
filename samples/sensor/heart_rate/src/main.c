/*
 * Copyright (c) 2017, NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

int main(void)
{
	struct sensor_value green;
	const struct device *const dev = DEVICE_DT_GET(DT_ALIAS(heart_rate_sensor));

	if (dev == NULL) {
		printf("Could not get heart_rate_sensor\n");
		return 0;
	}
	if (!device_is_ready(dev)) {
		printf("Device %s is not ready\n", dev->name);
		return 0;
	}

	while (1) {
		sensor_sample_fetch(dev);
		sensor_channel_get(dev, SENSOR_CHAN_GREEN, &green);

		/* Print green LED data*/
		printf("GREEN=%d\n", green.val1);

		k_sleep(K_MSEC(20));
	}
	return 0;
}
