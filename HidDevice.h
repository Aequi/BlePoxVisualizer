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

#include <QThread>
#include <QObject>
#include <QWidget>
#include <stdint.h>
#include <QElapsedTimer>
#include <QTimer>
#include <QVector>


class HidDevice : public QThread
{
    Q_OBJECT

	HANDLE hidHandle;

	static std::vector<std::string> getHidDevicePathList();
    std::string intToHex(uint16_t num);
    ~HidDevice();

public:
    bool writeHidReport(uint8_t data[], uint8_t len);
    bool readHidReport(uint8_t data[], uint8_t *len);
    bool open(uint16_t vid = 0xAAAA, uint16_t pid = 0xEEEE);
    void close(void);
    void hidDeviceProcess(void);

 signals:
    void hidDataReady(quint8 *data, quint8 length);

public slots:
    void run();

};

#endif
