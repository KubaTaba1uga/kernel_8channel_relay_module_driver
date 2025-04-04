# irrigation_controller_driver

A Linux kernel module that implements a GPIO-based irrigation controller driver for Raspberry Pi. Supports controlling up to 7 valves and 1 water pump via sysfs interface.

## Building

1. **Install required headers and tools**:
   ```bash
   sudo apt-get install build-essential linux-headers-$(uname -r)
   ```
2. **Build the module**:
   ```bash
   make
   ```

## Usage

### 1. **Wiring Example**

![rpi GPIOs](https://pinout-ai.s3.eu-west-2.amazonaws.com/raspberry-pi-5-gpio-pinout-diagram.webp)

```
+ (VCC)  -> Pin 2  
- (GND)  -> Pin 6

Pump     -> Pin 22  
Valve 1  -> Pin 24  
Valve 2  -> Pin 26  
Valve 3  -> Pin 28  
Valve 4  -> Pin 19  
Valve 5  -> Pin 21  
Valve 6  -> Pin 23  
Valve 7  -> Pin 27  
```

### 2. **Device Tree Overlay**

Insert the following into `arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dts`:

```dts
	irrigation_controller: irrigation_controller {
	    compatible = "raspberrypi,irrigation_controller_device";
	    pump-gpios = <&rp1_gpio 25 GPIO_ACTIVE_HIGH>;
	    valve1-gpios = <&rp1_gpio 8 GPIO_ACTIVE_HIGH>;
	    valve2-gpios = <&rp1_gpio 7 GPIO_ACTIVE_HIGH>;
	    valve3-gpios = <&rp1_gpio 1 GPIO_ACTIVE_HIGH>;
	    valve4-gpios = <&rp1_gpio 10 GPIO_ACTIVE_HIGH>;
	    valve5-gpios = <&rp1_gpio 9 GPIO_ACTIVE_HIGH>;
	    valve6-gpios = <&rp1_gpio 11 GPIO_ACTIVE_HIGH>;
	    valve7-gpios = <&rp1_gpio 0 GPIO_ACTIVE_HIGH>;		
	    status = "okay";
	};
```

Rebuild the DTB, update the boot partition, and reboot. After booting, confirm device registration with:

```bash
find /proc/device-tree/ | grep irrigation_controller
```

### 3. **Load / Unload Module**
```bash
sudo insmod irrigation_controller_driver.ko
...
sudo rmmod irrigation_controller_driver
```

### 4. **Control via Sysfs**

Write attributes exposed under `/sys/.../` (exact path depends on platform):

- Set valve index (0–6):
  ```bash
  echo 0 > /sys/.../valve_index
  ```
- Set watering time in seconds:
  ```bash
  echo 10 > /sys/.../time
  ```
- Start watering:
  ```bash
  echo 1 > /sys/.../trigger
  ```
- Stop watering manually:
  ```bash
  echo 0 > /sys/.../trigger
  ```

## Debugging

To enable debug messages (if compiled with `MYDEBUG=y`):

```bash
mount -t debugfs none /sys/kernel/debug
echo 'module irrigation_controller_driver +p' > /sys/kernel/debug/dynamic_debug/control
```

Ensure kernel was built with:
```bash
CONFIG_DEBUG_FS=y
CONFIG_DYNAMIC_DEBUG=y
```

## License

GPL/MIT

