#include <windows.h>
#include <iostream>
#include <string>
#include <Wlanapi.h>
#pragma comment(lib, "wlanapi.lib")

bool displayESSID(HANDLE handle, GUID *guid)
{
	unsigned i;
	WLAN_AVAILABLE_NETWORK_LIST *networkList;
	
	if(WlanGetAvailableNetworkList(handle, guid, 1, NULL, &networkList) != ERROR_SUCCESS)
		return false;
	
	for(i = 0; i < networkList->dwNumberOfItems; ++i) {
		WLAN_AVAILABLE_NETWORK *nw = &networkList->Network[i];
		DOT11_SSID *ssid = &nw->dot11Ssid;
		std::cout << "  " << std::string((char *)ssid->ucSSID, (size_t)ssid->uSSIDLength);
		std::cout << " " << nw->wlanSignalQuality << std::endl;
	}
	
	WlanFreeMemory(networkList);
	return true;
}

int main(void)
{
	unsigned i;
	DWORD err, ver;
	HANDLE hClient;
	WLAN_INTERFACE_INFO_LIST *ifList;

	if((err = WlanOpenHandle(WLAN_API_MAKE_VERSION(1, 0), NULL, &ver, &hClient)) != ERROR_SUCCESS) {
		std::cerr << "error: WlanOpenHandle()" << std::endl;
		return 1;
	}

	// get interface list
	WlanEnumInterfaces(hClient, NULL, &ifList);
	std::cout << ifList->dwNumberOfItems << " interfaces found" << std::endl;
	for(i = 0; i < ifList->dwNumberOfItems; ++i) {
		WLAN_INTERFACE_INFO wii = ifList->InterfaceInfo[i];
		std::wcout << wii.strInterfaceDescription << std::endl;
		displayESSID(hClient, &wii.InterfaceGuid);
	}

	WlanFreeMemory(ifList);
	WlanCloseHandle(hClient, NULL);
	return 0;
}