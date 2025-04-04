#ifndef IRRIGATION_CONTROLLER_COMMON
#define IRRIGATION_CONTROLLER_COMMON
#include <linux/gpio/consumer.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/types.h>

struct irrigation_controller_data {
	struct platform_device *pdev;
	struct mutex access_mutex;
	struct gpio_desc *valves[6];	// we support maximum 6 valves
	struct gpio_desc *pump;
	int current_valve_index;
	int time_to_water;
};

#define LKM_PRINT_ERR(pdev, fmt, ...)                                          \
  dev_err(&pdev->dev, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif
