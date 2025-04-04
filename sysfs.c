#include <linux/delay.h>	// for msleep()
#include <linux/device.h>
#include <linux/kernel.h>	// for kstrtoint
#include <linux/ktime.h>
#include <linux/mutex.h>	// fixed path
#include <linux/sysfs.h>	// for DEVICE_ATTR macros

#include "asm-generic/errno-base.h"
#include "common.h"
#include "sysfs.h"

static void irrigate(struct irrigation_controller_data *data);
static void stop_irrigation(struct irrigation_controller_data *data);

// Write-only handlers (store functions)
static ssize_t trigger_irrigation(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count);

static ssize_t set_valve_index(struct device *dev,
			       struct device_attribute *attr, const char *buf, size_t count);

static ssize_t set_time(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count);

// Sysfs attributes: write-only (0220)
static DEVICE_ATTR(trigger, 0220, NULL, trigger_irrigation);
static DEVICE_ATTR(valve_index, 0220, NULL, set_valve_index);
static DEVICE_ATTR(time, 0220, NULL, set_time);

int irrigation_controller_init_sysfs(struct irrigation_controller_data *data)
{
	int err;

	data->current_valve_index = 0;
	data->time_to_water = 0;

	err = device_create_file(&data->pdev->dev, &dev_attr_trigger);
	if (err) {
		return err;
	}

	err = device_create_file(&data->pdev->dev, &dev_attr_valve_index);
	if (err) {
		return err;
	}

	err = device_create_file(&data->pdev->dev, &dev_attr_time);
	if (err) {
		return err;
	}

	mutex_init(&data->access_mutex);

	return 0;
};

void irrigation_controller_destroy_sysfs(struct irrigation_controller_data *data)
{
	device_remove_file(&data->pdev->dev, &dev_attr_trigger);
	device_remove_file(&data->pdev->dev, &dev_attr_valve_index);
	device_remove_file(&data->pdev->dev, &dev_attr_time);
};

static ssize_t trigger_irrigation(struct device *dev,
				  struct device_attribute *attr, const char *buf, size_t count)
{
	int ret, index;
	// Convert string from userspace to an integer
	ret = kstrtoint(buf, 0, &index);
	if (ret < 0)
		return ret;

	struct irrigation_controller_data *data = dev_get_drvdata(dev);

	switch (index) {
	case 0:
		stop_irrigation(data);
		break;
	case 1:
		irrigate(data);
		break;
	default:
		return -EINVAL;
	}

	return count;		// Must return number of bytes consumed
};

static ssize_t set_valve_index(struct device *dev,
			       struct device_attribute *attr, const char *buf, size_t count)
{
	int ret, index;

	struct irrigation_controller_data *data = dev_get_drvdata(dev);

	// Convert string from userspace to an integer
	ret = kstrtoint(buf, 0, &index);
	if (ret < 0)
		return ret;

	if (index < 0 || index > sizeof(data->valves) / sizeof(void *) - 1) {
		return -EINVAL;
	}

	mutex_lock(&data->access_mutex);
	data->current_valve_index = index;
	mutex_unlock(&data->access_mutex);

	return count;
};

static ssize_t set_time(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	int ret, time;

	struct irrigation_controller_data *data = dev_get_drvdata(dev);

	// Convert string from userspace to an integer
	ret = kstrtoint(buf, 0, &time);
	if (ret < 0)
		return ret;

	if (time < 0) {
		return -EINVAL;
	}

	mutex_lock(&data->access_mutex);
	data->time_to_water = time;
	mutex_unlock(&data->access_mutex);

	return count;		// Must return number of bytes consumed
};

static void irrigate(struct irrigation_controller_data *data)
{
	if (!data)
		return;

	mutex_lock(&data->access_mutex);
	gpiod_set_value(data->pump, 1);	// Set pin high
	gpiod_set_value(data->valves[data->current_valve_index], 1);	// Set pin high
	mutex_unlock(&data->access_mutex);

	msleep(data->time_to_water * 1000);	// Sleep N seconds (N,000 ms)

	stop_irrigation(data);
}

static void stop_irrigation(struct irrigation_controller_data *data)
{
	mutex_lock(&data->access_mutex);
	gpiod_set_value(data->pump, 0);	// Set pin low
	gpiod_set_value(data->valves[data->current_valve_index], 0);	// Set pin low
	mutex_unlock(&data->access_mutex);
}
