# nRF BLE Rolling Shutter control POC

POC project for controlling rolling shutter at home.

For now list of available command should be placed in `rf.c:rf_send_cmd:seq` array. Index send to BLE attribute will be a command from this list.