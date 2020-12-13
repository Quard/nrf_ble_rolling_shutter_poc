#include <zephyr.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <errno.h>
#include <drivers/gpio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "rf.h"
#include "kitchen.h"


#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static K_SEM_DEFINE(ble_init_ok, 0, 1);
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN)
};
static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, KITCHEN_SERVICE_UUID)
};

static void error(void) {
    for (;;) {
        printk("Unexpected Error\n");
        k_sleep(K_MSEC(1000));
    }
}

static void bt_connected(struct bt_conn *conn, uint8_t err) {
    struct bt_conn_info info;
    char addr[BT_ADDR_LE_STR_LEN];

    if (err) {
        printk("BLE connection failed: err=%d\n", err);
        return;
    }

    if (bt_conn_get_info(conn, &info)) {
        printk("BLE could not parse connection info\n");
    } else {
        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

        printk("Connection established!\n\
        Connected to: %sn\n\
        Role: %u\n\
        Connection interval: %u\n\
        Slave latency: %u\n\
        Connection supervisory timeout: %u\n", addr, info.role, info.le.interval, info.le.latency, info.le.timeout);
    }
}

static void bt_disconnected(struct bt_conn *conn, uint8_t reason) {
    printk("BLE disconnected (reason: %d)\n", reason);
}

static bool bt_le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param) {
    return true;
}

static void bt_le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout) {
    struct bt_conn_info info;
    char addr[BT_ADDR_LE_STR_LEN];

    if (bt_conn_get_info(conn, &info)) {
        printk("BLE could not parse connection info\n");
    } else {
        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

        printk("BT connection parameters updated!\n\
        Connected to: %s\n\
        New connection interval: %u\n\
        New slave latency: %u\n\
        New connection supervisory timeout: %u\n", addr, info.le.interval, info.le.latency, info.le.timeout);
    }
}

static struct bt_conn_cb bt_conn_callbacks = {
    .connected = bt_connected,
    .disconnected = bt_disconnected,
    .le_param_req = bt_le_param_req,
    .le_param_updated = bt_le_param_updated
};

static void bt_ready(int err) {
    if (err) {
        printk("BLE init failed with error code: %d\n", err);
        return;
    }

    err = bt_le_adv_start(
        BT_LE_ADV_CONN,
        ad, ARRAY_SIZE(ad),
        sd, ARRAY_SIZE(sd)
    );
    if (err) {
        printk("BT Advertising failed to start (err %d)\n", err);
        return;
    }

    bt_conn_cb_register(&bt_conn_callbacks);

    k_sem_give(&ble_init_ok);
}

void main(void) {
    int err = 0;

    printk("Starting nRF BLE Rolling Shutter controll POC\n");

    err = bt_enable(bt_ready);
    if (err != 0) {
        printk("BLE initialization failed\n");
        error();
    }
    err = k_sem_take(&ble_init_ok, K_MSEC(500));
    if (!err) {
        printk("BLE initialized\n");
    } else {
        printk("BLE initialization did not complete in time\n");
        error();
    }

    rf_init();

    err = kitchen_service_init();
    if (err != 0) {
        printk("Unable to initilize Kitchen BT service");
    }
}