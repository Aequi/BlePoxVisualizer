#ifndef __HID_DEVICE_H__
#define __HID_DEVICE_H__

#include <iostream>
#include <Windows.h>
#include "Hidsdi++.h"
#include <SetupAPI.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <mutex>

#include <QThread>
#include <QObject>
#include <QWidget>
#include <stdint.h>
#include <QElapsedTimer>
#include <QTimer>
#include <QVector>


class HidDevice : public QObject
{
    Q_OBJECT

	HANDLE hidHandle;

	static std::vector<std::string> getHidDevicePathList();
    std::string intToHex(uint16_t num);

public:
    bool writeHidReport(uint8_t data[], uint8_t len);
    bool open(uint16_t vid = 0xAAAA, uint16_t pid = 0xEEEE);
    bool setTxPower(int8_t txPowerDbm);
    bool setBatteryTestMode(bool isBatteryTestOn);
    const QVector<int8_t> getTxPowers(void);
    void close(void);
};

#endif
