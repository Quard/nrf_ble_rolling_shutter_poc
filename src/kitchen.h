#ifndef __KITCHEN_H__
#define __KITCHEN_H__

#include <zephyr.h>
#include <zephyr/types.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <errno.h>
#include <string.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/addr.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "rf.h"


#define KITCHEN_SERVICE_UUID 0x4e, 0x2f, 0xb7, 0xad, 0x24, 0x08, 0x5d, 0x20, 0xb5, 0x51, 0xe3, 0xb3, 0xb2, 0x37, 0x3e, 0x3b
#define KITCHEN_SERVICE_RX_CHARACTERISTIC_UUID 0x3b, 0x00, 0x03, 0x30, 0xda, 0x0c, 0x57, 0xb9, 0x8d, 0x26, 0x03, 0xb0, 0xc1, 0x85, 0xea, 0xc7

int kitchen_service_init(void);

#endif