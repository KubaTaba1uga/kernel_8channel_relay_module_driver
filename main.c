/*
 * 8 realy irrigation driver
 ****************************************************************
 * Brief Description:
 * A simple module which implements driver for 8 channel relay for simple
 *  irrigation system.
 */
#include <linux/of_device.h>
#include <linux/platform_device.h>

#include "common.h"
#include "init_driver.h"
#include "sysfs.h"

/***************************************************************
 *                        PUBLIC API
 **************************************************************/
static int irrigation_controller_probe(struct platform_device *pdev) {
  int err;

  dev_info(&pdev->dev, "Probing...\n");

  err = irrigation_controller_init(pdev);
  if (err) {
    LKM_PRINT_ERR(pdev, "Unable to init driver\n");
    return err;
  }

  struct irrigation_controller_data *data = dev_get_drvdata(&pdev->dev);

  err = irrigation_controller_init_sysfs(data);
  if (err) {
    LKM_PRINT_ERR(pdev, "Unable to init sysfs\n");
    return err;
  }

  dev_info(&pdev->dev, "IRRIGATION_CONTROLLER probed\n");

  return 0;
}

static void irrigation_controller_remove(struct platform_device *pdev) {
  struct irrigation_controller_data *data = dev_get_drvdata(&pdev->dev);

  irrigation_controller_destroy_sysfs(data);

  dev_info(&pdev->dev, "IRRIGATION_CONTROLLER removed\n");
}

static const struct of_device_id irrigation_controller_of_match[] = {
    {
        // This is a unique value which should match `compatibile` field in
        // overlay.
        .compatible = "raspberrypi,irrigation_controller_device",
    },
    {},
};
MODULE_DEVICE_TABLE(of, irrigation_controller_of_match);

static struct platform_driver irrigation_controller_driver = {
    .probe = irrigation_controller_probe,
    .remove_new = irrigation_controller_remove,
    .driver =
        {
            .name = "irrigation_controller_gpio",
            .of_match_table = irrigation_controller_of_match,
        },
};

module_platform_driver(irrigation_controller_driver);

MODULE_AUTHOR("Jakub Buczynski");
MODULE_DESCRIPTION("Irrigation controller driver");
MODULE_LICENSE("GPL");
