// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <stdio.h>
#include <Winsock2.h>
#include "motionVenusSDK.h"

#pragma comment(lib, "ws2_32.lib")


extern WSADATA wsaData;
extern char  errorMsg[100];
extern CRITICAL_SECTION   m_cs;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			InitializeCriticalSection(&m_cs);

			if (WSAStartup(MAKEWORD(2, 1), &wsaData)) //调用Windows Sockets DLL
			{
				memcpy(errorMsg, "socket init failed.", sizeof(errorMsg));
				WSACleanup();
				return FALSE;
			} 
			break;
		}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
		{
			DeleteCriticalSection(&m_cs);
			break;
		}
			
	}

	


	return TRUE;
}

