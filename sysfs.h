#ifndef IRRIGATION_CONTROLLER_SYSFS
#define IRRIGATION_CONTROLLER_SYSFS

#include "common.h"

int irrigation_controller_init_sysfs(struct irrigation_controller_data *data);

void irrigation_controller_destroy_sysfs(struct irrigation_controller_data *data);

#endif
