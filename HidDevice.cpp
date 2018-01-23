#include "HidDevice.h"
#include <QDebug>



std::vector<std::string> HidDevice::getHidDevicePathList()
{
	std::vector<std::string> hidDevicePathList;

	GUID hidGUID;
	//Get the HID GUID value - used as mask to get list of devices
	HidD_GetHidGuid(&hidGUID);
	//Get a list of devices matching the criteria (hid interface, present)
	HDEVINFO hwDevInf = SetupDiGetClassDevs(&hidGUID, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

	SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	//Go through the list and get the interface data
	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hwDevInf, NULL, &hidGUID, i, &deviceInterfaceData); i++) {
		ULONG reqSize;
		//Get the details with null values to get the required size of the buffer
		SetupDiGetDeviceInterfaceDetail(hwDevInf, &deviceInterfaceData, NULL, 0, &reqSize, 0);

		//Allocate the buffer
		PSP_INTERFACE_DEVICE_DETAIL_DATA deviceDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)(new uint8_t[reqSize]);
		deviceDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

		//Fill the buffer with the device details
		if (SetupDiGetDeviceInterfaceDetail(hwDevInf, &deviceInterfaceData, deviceDetail, reqSize, &reqSize, NULL)) {
			std::string devPath = deviceDetail->DevicePath;
			hidDevicePathList.push_back(devPath);
		}

		delete deviceDetail;
	}

	SetupDiDestroyDeviceInfoList(hwDevInf);
	return hidDevicePathList;
}


std::string HidDevice::intToHex(uint16_t num)
{
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(sizeof(num) * 2) << std::hex << num;
	return stream.str();
}

HidDevice::HidDevice()
{

}

bool HidDevice::open(uint16_t vid, uint16_t pid)
{
    std::string str("\\\\?\\hid#vid_" + HidDevice::intToHex(vid) + "&pid_" + HidDevice::intToHex(pid));

	for (auto hidDevicePath : this->getHidDevicePathList()) {
        //std::cout << hidDevicePath << std::endl;
		if (hidDevicePath.find(str) == 0) {

			hidHandle = CreateFile(hidDevicePath.c_str(), GENERIC_READ | GENERIC_WRITE,
								   FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

			if (hidHandle == INVALID_HANDLE_VALUE) {
				continue;
			}

			PHIDP_PREPARSED_DATA preparsedData = NULL;

			if (HidD_GetPreparsedData(hidHandle, &preparsedData) == FALSE) {
				CloseHandle(hidHandle);
				hidHandle = INVALID_HANDLE_VALUE;
				continue;
			}

			HIDP_CAPS capabilities;

			if (HidP_GetCaps(preparsedData, &capabilities) != HIDP_STATUS_SUCCESS) {
				CloseHandle(hidHandle);
				hidHandle = INVALID_HANDLE_VALUE;
				continue;
			}

            if (capabilities.OutputReportByteLength == 20 && capabilities.InputReportByteLength == 20) {

				return true;
			}
		}
	}

	return false;
}

void HidDevice::close(void)
{
    CloseHandle(hidHandle);
    hidHandle = INVALID_HANDLE_VALUE;
}

bool HidDevice::writeHidReport(uint8_t data[], uint8_t len)
{
	if (hidHandle == INVALID_HANDLE_VALUE) {
		return false;
	}

    uint8_t report[65];
    memset(report, 0, sizeof(report));
    memcpy(&report[0], data, len);
    DWORD bytesWritten;
    return WriteFile(hidHandle, report, 20, &bytesWritten, NULL);
}

bool HidDevice::readHidReport(uint8_t data[], uint8_t *len)
{
    if (hidHandle == INVALID_HANDLE_VALUE || data == NULL || len == NULL) {
        return false;
    }

    uint8_t report[65];
    DWORD bytesRead = 0;
    LPDWORD pBytesRead = 0;

    bool res = ReadFile(hidHandle, report, 20, pBytesRead, NULL);
    *len = bytesRead;

    if (!res || bytesRead != 20) {
        return false;
    }

    memcpy(data, report, bytesRead);

    return true;
}

void HidDevice::run()
{
    for(;;) {
        if (open() == true) {
            break;
        }
        Sleep(5000);
    }
    uint8_t data[2] = {0x0E, 0x01};
    writeHidReport(data, sizeof(data));
    for(;;) {
        static uint8_t hidReport[65];
        uint8_t length = 0;
        if (readHidReport(hidReport, &length) == false) {
            continue;
        }
        if (length == 20) {
            emit hidDataReady(hidReport, length);
        }
    }
}

HidDevice::~HidDevice()
{
    uint8_t data[2] = {0x0E, 0x00};
    writeHidReport(data, sizeof(data));
    close();
}
