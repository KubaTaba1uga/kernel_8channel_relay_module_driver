#include <linux/dev_printk.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

#include "common.h"
#include "init_driver.h"

#include <linux/delay.h> // for msleep()

void drive_gpios_high_for_10s(struct gpio_desc *gpio1,
                              struct gpio_desc *gpio2) {
  if (!gpio1 || !gpio2)
    return;

  gpiod_set_value(gpio1, 1); // Set pin high
  gpiod_set_value(gpio2, 1); // Set pin high
  msleep(10000);             // Sleep 2 seconds (2,000 ms)
  gpiod_set_value(gpio1, 0); // Set pin low
  gpiod_set_value(gpio2, 0); // Set pin low
}

int irrigation_controller_init(struct platform_device *pdev) {
  struct irrigation_controller_data *data;

  // Memory allocated with this function is automatically freed on driver
  //   detach.
  data = devm_kzalloc(&pdev->dev, sizeof(struct irrigation_controller_data),
                      GFP_KERNEL);
  if (!data)
    return -ENOMEM;

  // Resources allocated by this function are automatically freed on driver
  //   detach.
  data->pump = devm_gpiod_get(&pdev->dev, "pump", GPIOD_OUT_HIGH);
  if (IS_ERR(data->pump)) {
    return dev_err_probe(&pdev->dev, PTR_ERR(data->pump),
                         "Failed to get pump GPIO\n");
  }

  for (int i = 0; i < sizeof(data->valves) / sizeof(void *); i++) {
    char buffer[64];
    sprintf(buffer, "valve%d", i + 1);
    data->valves[i] = devm_gpiod_get(&pdev->dev, buffer, GPIOD_OUT_HIGH);
    if (IS_ERR(data->valves[i])) {
      return dev_err_probe(&pdev->dev, PTR_ERR(data->valves[i]),
                           "Failed to get %s GPIO\n", buffer);
    }
  }

  data->pdev = pdev;

  // This functions sets device data so it can be acessed from anwyehre in this
  //   device ctx.
  platform_set_drvdata(pdev, data);

  drive_gpios_high_for_10s(data->pump, data->valves[0]);
  drive_gpios_high_for_10s(data->valves[1], data->valves[2]);

  return 0;
}
