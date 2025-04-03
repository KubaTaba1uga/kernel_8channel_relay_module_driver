# TO-DO

Change name to irrigation controller

## Connections

```
+ - 2
- - 6

pump - 22

valve 1 - 24

valve 2 - 26 

valve 3 - 28
```

## Overlay

Add the following snippet to `arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dts` after the `leds: leds` section:

```dts
	irrigation_controller: irrigation_controller {
	    compatible = "raspberrypi,irrigation_controller_device";
	    pump-gpios = <&rp1_gpio 25 GPIO_ACTIVE_HIGH>;
	    valve1-gpios = <&rp1_gpio 8 GPIO_ACTIVE_HIGH>;
	    valve2-gpios = <&rp1_gpio 7 GPIO_ACTIVE_HIGH>;
	    valve3-gpios = <&rp1_gpio 1 GPIO_ACTIVE_HIGH>;
	    status = "okay";
	};
```

After modifying the DTS file, recompile and update your boot partition. On boot, verify that the custom GPIO device is recognized by checking:

```bash
find /proc/device-tree/ | grep irrigation_controller
```

