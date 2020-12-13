#include "kitchen.h"


#define MAX_TRANSMIT_SIZE 240

#define BT_UUID_KITCHEN_SERVICE    BT_UUID_DECLARE_128(KITCHEN_SERVICE_UUID)
#define BT_UUID_KITCHEN_SERVICE_RX BT_UUID_DECLARE_128(KITCHEN_SERVICE_RX_CHARACTERISTIC_UUID)


uint8_t data_rx[MAX_TRANSMIT_SIZE];

static ssize_t on_receive(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags);
void on_cccd_changed(const struct bt_gatt_attr *attr, uint16_t value);

BT_GATT_SERVICE_DEFINE(
    kitchen_service,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_KITCHEN_SERVICE),
    BT_GATT_CHARACTERISTIC(
        BT_UUID_KITCHEN_SERVICE_RX,
        BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
        NULL, on_receive, NULL
    ),
    BT_GATT_CCC(on_cccd_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

int kitchen_service_init(void) {
    int err = 0;

    memcpy(&data_rx, 0, MAX_TRANSMIT_SIZE);

    return err;
}

static ssize_t on_receive(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    const uint8_t *buffer = buf;

    printk("Receive data, handle %d, conn %p, data: 0x", attr->handle, conn);
    for (uint8_t i=0; i<len; i++) {
        printk("%02X", buffer[i]);
    }
    printk("\n");

    if (len == 1) {
        rf_set_cmd(buffer[0]);
        rf_start_send();
    }

    return len;
}

void on_cccd_changed(const struct bt_gatt_attr *attr, uint16_t value) {
    ARG_UNUSED(attr);

    switch (value) {
    case BT_GATT_CCC_NOTIFY:
        break;
    case BT_GATT_CCC_INDICATE:
        break;
    case 0:
        break;
    
    default:
        printk("Error, CCCD has been set to an invalid value");
        break;
    }
}