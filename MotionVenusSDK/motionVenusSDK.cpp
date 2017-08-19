// FoDataRecv.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <Winsock2.h>
#include "motionVenusSDK.h"

#define Max_Socket_Count	10
#define U3DUE_FOREPART_LENGTH	128	//UDP报文头长度
#define Bone_Count_Wireless		23	//无线套装发送骨骼数量
#define Bone_Count_Wired		23	//有线套装发送骨骼数量

const uint16_t QuatScale = (1 << 13);
const uint16_t EulerScale = (1 << 7);
const uint32_t LocScale = (1 << 16);

//socket list
SOCKET socketObj[Max_Socket_Count] = { INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET }; //socket list
//user obj data
void*  _customedObj = nullptr;
//Calculation type
CalculationType _calType = Position_Euler;
//Suit type
SuitType _suitType = Wireless;
//error msg
char  errorMsg[100] = { ' ' };
//socket init
WSADATA wsaData;

//lock
CRITICAL_SECTION    m_cs;

//function pointer
void(__stdcall *_RegisterCalDataFunctin)(void* customedObj, SOCKET_REF sender, CalDataHeader* header, float* data, int countFloat) = nullptr;



//thread function
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	//socket
	int nPos = (int)lpParam;
	struct sockaddr_in from;
	int fromlen = sizeof(from);
	
	//recv buffer
	char buffer[1440] = "\0";

	//every frame float data
	int maxFloatDataLen = (4 + 3)*(_suitType == Wireless ? Bone_Count_Wireless : Bone_Count_Wired);
	float* fData = new float[maxFloatDataLen];
	memset(fData, 0, maxFloatDataLen);

	//current suit bone count
	int curBoneCount = (_suitType == Wireless ? Bone_Count_Wireless : Bone_Count_Wired);
	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		if (socketObj[nPos] == INVALID_SOCKET)
			break;
		int nRet = recvfrom(socketObj[nPos], buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &fromlen);
		if( nRet != SOCKET_ERROR)
		{
			if (_RegisterCalDataFunctin == nullptr)
			{
				memcpy(errorMsg, "Unregister function pointer: CalculationDataRecv.", sizeof(errorMsg));
				continue;
			}
			//check calculate data type
			int totalBuffSize = 0;
			switch (_calType)
			{
			case Position_only:
			{
								  totalBuffSize = U3DUE_FOREPART_LENGTH + sizeof(int32_t)* 3 * curBoneCount;
								  if (totalBuffSize != nRet)
								  {
									  memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.", sizeof(errorMsg));
									  continue;
								  }
								  //head
								  CalDataHeader header;
								  int nReadLen = 0;
								  //version
								  memcpy(&header.protocolVersion, &buffer[0], sizeof(header.protocolVersion));
								  nReadLen += sizeof(header.protocolVersion);
								  //name len
								  uint8_t nNameLen;
								  memcpy(&nNameLen, &buffer[nReadLen], sizeof(nNameLen));
								  nReadLen += sizeof(nNameLen);
								  //name
								  memcpy(&header.AvatarName, &buffer[nReadLen], nNameLen);
								  nReadLen += nNameLen;
								  header.AvatarName[nNameLen] = '\0';
								  //suit no
								  memcpy(&header.suitNumber, &buffer[nReadLen], sizeof(header.suitNumber));
								  nReadLen += sizeof(header.suitNumber);
								  //suit type
								  memcpy(&header.suitType, &buffer[nReadLen], sizeof(header.suitType));
								  nReadLen += sizeof(header.suitType);
								  //frame number
								  memcpy(&header.frameNumber, &buffer[nReadLen], sizeof(header.frameNumber));
								  nReadLen += sizeof(header.frameNumber);
								  //bone counts
								  memcpy(&header.boneCount, &buffer[nReadLen], sizeof(header.boneCount));
								  nReadLen += sizeof(header.boneCount);
								  //int curBoneCount = header.boneCount;
								  
								  //init data
								  memset(fData, 0, maxFloatDataLen);
									
								  int32_t tmpData[3];
								  for (int i = 0; i < curBoneCount; i++)
								  {
									  memcpy(tmpData, &buffer[U3DUE_FOREPART_LENGTH + sizeof(tmpData)*i], sizeof(tmpData));
									  fData[i * 3 + 0] = ((float)tmpData[0] / LocScale);
									  fData[i * 3 + 1] = ((float)tmpData[1] / LocScale);
									  fData[i * 3 + 2] = ((float)tmpData[2] / LocScale);
								  }
								  EnterCriticalSection(&m_cs);
								  _RegisterCalDataFunctin(_customedObj, (void*)socketObj[nPos], &header, fData, curBoneCount*3);
								  LeaveCriticalSection(&m_cs);
								  break;

			}
				
			case Euler_only:
			{
							   totalBuffSize = U3DUE_FOREPART_LENGTH + sizeof(int16_t)* 3 * curBoneCount;

							   if (totalBuffSize != nRet)
							   {
								   memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.", sizeof(errorMsg));
								   continue;
							   }
							   //head
							   CalDataHeader header;
							   int nReadLen = 0;
							   //version
							   memcpy(&header.protocolVersion, &buffer[0], sizeof(header.protocolVersion));
							   nReadLen += sizeof(header.protocolVersion);
							   //name len
							   uint8_t nNameLen;
							   memcpy(&nNameLen, &buffer[nReadLen], sizeof(nNameLen));
							   nReadLen += sizeof(nNameLen);
							   //name
							   memcpy(&header.AvatarName, &buffer[nReadLen], nNameLen);
							   nReadLen += nNameLen;
							   header.AvatarName[nNameLen] = '\0';
							   //suit no
							   memcpy(&header.suitNumber, &buffer[nReadLen], sizeof(header.suitNumber));
							   nReadLen += sizeof(header.suitNumber);
							   //suit type
							   memcpy(&header.suitType, &buffer[nReadLen], sizeof(header.suitType));
							   nReadLen += sizeof(header.suitType);
							   //frame number
							   memcpy(&header.frameNumber, &buffer[nReadLen], sizeof(header.frameNumber));
							   nReadLen += sizeof(header.frameNumber);
							   //bone counts
							   memcpy(&header.boneCount, &buffer[nReadLen], sizeof(header.boneCount));
							   nReadLen += sizeof(header.boneCount);
							   //int curBoneCount = header.boneCount;

							   //init data
							   memset(fData, 0, maxFloatDataLen);

							   int16_t tmpData[3];
							   for (int i = 0; i < curBoneCount; i++)
							   {
								   memcpy(tmpData, &buffer[U3DUE_FOREPART_LENGTH + sizeof(tmpData)*i], sizeof(tmpData));
								   fData[i * 3 + 0] = ((float)tmpData[0] / EulerScale);
								   fData[i * 3 + 1] = ((float)tmpData[1] / EulerScale);
								   fData[i * 3 + 2] = ((float)tmpData[2] / EulerScale);
							   }

							   _RegisterCalDataFunctin(_customedObj, (void*)socketObj[nPos], &header, fData, curBoneCount * 3);
							   break;

			}
			case Quat_only:
			{
							  totalBuffSize = U3DUE_FOREPART_LENGTH + sizeof(int16_t)* 4 * curBoneCount;

							  if (totalBuffSize != nRet)
							  {
								  memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.", sizeof(errorMsg));
								  continue;
							  }
							  //head
							  CalDataHeader header;
							  int nReadLen = 0;
							  //version
							  memcpy(&header.protocolVersion, &buffer[0], sizeof(header.protocolVersion));
							  nReadLen += sizeof(header.protocolVersion);
							  //name len
							  uint8_t nNameLen;
							  memcpy(&nNameLen, &buffer[nReadLen], sizeof(nNameLen));
							  nReadLen += sizeof(nNameLen);
							  //name
							  memcpy(&header.AvatarName, &buffer[nReadLen], nNameLen);
							  nReadLen += nNameLen;
							  header.AvatarName[nNameLen] = '\0';
							  //suit no
							  memcpy(&header.suitNumber, &buffer[nReadLen], sizeof(header.suitNumber));
							  nReadLen += sizeof(header.suitNumber);
							  //suit type
							  memcpy(&header.suitType, &buffer[nReadLen], sizeof(header.suitType));
							  nReadLen += sizeof(header.suitType);
							  //frame number
							  memcpy(&header.frameNumber, &buffer[nReadLen], sizeof(header.frameNumber));
							  nReadLen += sizeof(header.frameNumber);
							  //bone counts
							  memcpy(&header.boneCount, &buffer[nReadLen], sizeof(header.boneCount));
							  nReadLen += sizeof(header.boneCount);
							  //int curBoneCount = header.boneCount;

							  //init data
							  memset(fData, 0, maxFloatDataLen);

							  int16_t tmpData[4];
							  for (int i = 0; i < curBoneCount; i++)
							  {
								  memcpy(tmpData, &buffer[U3DUE_FOREPART_LENGTH + sizeof(tmpData)*i], sizeof(tmpData));
								  fData[i * 3 + 0] = ((float)tmpData[0] / QuatScale);
								  fData[i * 3 + 1] = ((float)tmpData[1] / QuatScale);
								  fData[i * 3 + 2] = ((float)tmpData[2] / QuatScale);
								  fData[i * 3 + 3] = ((float)tmpData[3] / QuatScale);
							  }

							  _RegisterCalDataFunctin(_customedObj, (void*)socketObj[nPos], &header, fData, curBoneCount * 4);
							  break;
							  
			}
			case Position_Euler:
			{
								   totalBuffSize = U3DUE_FOREPART_LENGTH + (sizeof(int32_t)* 3 + sizeof(int16_t)* 3)* curBoneCount;

								   if (totalBuffSize != nRet)
								   {
									   memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.", sizeof(errorMsg));
									   continue;
								   }
								   //head
								   CalDataHeader header;
								   int nReadLen = 0;
								   //version
								   memcpy(&header.protocolVersion, &buffer[0], sizeof(header.protocolVersion));
								   nReadLen += sizeof(header.protocolVersion);
								   //name len
								   uint8_t nNameLen;
								   memcpy(&nNameLen, &buffer[nReadLen], sizeof(nNameLen));
								   nReadLen += sizeof(nNameLen);
								   //name
								   memcpy(&header.AvatarName, &buffer[nReadLen], nNameLen);
								   nReadLen += nNameLen;
								   header.AvatarName[nNameLen] = '\0';
								   //suit no
								   memcpy(&header.suitNumber, &buffer[nReadLen], sizeof(header.suitNumber));
								   nReadLen += sizeof(header.suitNumber);
								   //suit type
								   memcpy(&header.suitType, &buffer[nReadLen], sizeof(header.suitType));
								   nReadLen += sizeof(header.suitType);
								   //frame number
								   memcpy(&header.frameNumber, &buffer[nReadLen], sizeof(header.frameNumber));
								   nReadLen += sizeof(header.frameNumber);
								   //bone counts
								   memcpy(&header.boneCount, &buffer[nReadLen], sizeof(header.boneCount));
								   nReadLen += sizeof(header.boneCount);
								   //int curBoneCount = header.boneCount;

								   //init data
								   memset(fData, 0, maxFloatDataLen);

								   
								   int32_t dataPosition[3];
								   int16_t dataEuler[3];
								   for (int i = 0; i < curBoneCount; i++)
								   {
									   memcpy(dataPosition, &buffer[U3DUE_FOREPART_LENGTH + (sizeof(dataPosition)+sizeof(dataEuler))*i], sizeof(dataPosition));
									   memcpy(dataEuler, &buffer[U3DUE_FOREPART_LENGTH + (sizeof(dataPosition)+sizeof(dataEuler))*i + sizeof(dataPosition)], sizeof(dataEuler));

									   fData[i * 6 + 0] = ((float)dataPosition[0] / LocScale);
									   fData[i * 6 + 1] = ((float)dataPosition[1] / LocScale);
									   fData[i * 6 + 2] = ((float)dataPosition[2] / LocScale);
									   fData[i * 6 + 3] = ((float)dataEuler[0] / EulerScale);
									   fData[i * 6 + 4] = ((float)dataEuler[1] / EulerScale);
									   fData[i * 6 + 5] = ((float)dataEuler[2] / EulerScale);
								   }

								   _RegisterCalDataFunctin(_customedObj, (void*)socketObj[nPos], &header, fData, curBoneCount * 6);
								   break;
			}
			case Position_Quat:
			{
								  totalBuffSize = U3DUE_FOREPART_LENGTH + (sizeof(int32_t)* 3 + sizeof(int16_t)* 4)* curBoneCount;

								  if (totalBuffSize != nRet)
								  {
									  memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.", sizeof(errorMsg));
									  continue;
								  }
								  //head
								  CalDataHeader header;
								  int nReadLen = 0;
								  //version
								  memcpy(&header.protocolVersion, &buffer[0], sizeof(header.protocolVersion));
								  nReadLen += sizeof(header.protocolVersion);
								  //name len
								  uint8_t nNameLen;
								  memcpy(&nNameLen, &buffer[nReadLen], sizeof(nNameLen));
								  nReadLen += sizeof(nNameLen);
								  //name
								  memcpy(&header.AvatarName, &buffer[nReadLen], nNameLen);
								  nReadLen += nNameLen;
								  header.AvatarName[nNameLen] = '\0';
								  //suit no
								  memcpy(&header.suitNumber, &buffer[nReadLen], sizeof(header.suitNumber));
								  nReadLen += sizeof(header.suitNumber);
								  //suit type
								  memcpy(&header.suitType, &buffer[nReadLen], sizeof(header.suitType));
								  nReadLen += sizeof(header.suitType);
								  //frame number
								  memcpy(&header.frameNumber, &buffer[nReadLen], sizeof(header.frameNumber));
								  nReadLen += sizeof(header.frameNumber);
								  //bone counts
								  memcpy(&header.boneCount, &buffer[nReadLen], sizeof(header.boneCount));
								  nReadLen += sizeof(header.boneCount);
								  //int curBoneCount = header.boneCount;

								  //init data
								  memset(fData, 0, maxFloatDataLen);

								  int32_t dataPosition[3];
								  int16_t dataQuat[4];
								  for (int i = 0; i < curBoneCount; i++)
								  {
									  memcpy(dataPosition, &buffer[U3DUE_FOREPART_LENGTH + (sizeof(dataPosition)+sizeof(dataQuat))*i], sizeof(dataPosition));
									  memcpy(dataQuat, &buffer[U3DUE_FOREPART_LENGTH + (sizeof(dataPosition)+sizeof(dataQuat))*i + sizeof(dataPosition)], sizeof(dataQuat));
									  fData[i * 7 + 0] = ((float)dataPosition[0] / LocScale);
									  fData[i * 7 + 1] = ((float)dataPosition[1] / LocScale);
									  fData[i * 7 + 2] = ((float)dataPosition[2] / LocScale);
									  fData[i * 7 + 3] = ((float)dataQuat[0] / QuatScale);
									  fData[i * 7 + 4] = ((float)dataQuat[1] / QuatScale);
									  fData[i * 7 + 5] = ((float)dataQuat[2] / QuatScale);
									  fData[i * 7 + 6] = ((float)dataQuat[3] / QuatScale);
								  }

								  _RegisterCalDataFunctin(_customedObj, (void*)socketObj[nPos], &header, fData, curBoneCount * 7);
								  break;
			
			}
			case Euler_Quat:
			{
							   totalBuffSize = U3DUE_FOREPART_LENGTH + (sizeof(int16_t)* 3 + sizeof(int16_t)* 4)* curBoneCount;

							   if (totalBuffSize != nRet)
							   {
								   memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.", sizeof(errorMsg));
								   continue;
							   }
							   //head
							   CalDataHeader header;
							   int nReadLen = 0;
							   //version
							   memcpy(&header.protocolVersion, &buffer[0], sizeof(header.protocolVersion));
							   nReadLen += sizeof(header.protocolVersion);
							   //name len
							   uint8_t nNameLen;
							   memcpy(&nNameLen, &buffer[nReadLen], sizeof(nNameLen));
							   nReadLen += sizeof(nNameLen);
							   //name
							   memcpy(&header.AvatarName, &buffer[nReadLen], nNameLen);
							   nReadLen += nNameLen;
							   header.AvatarName[nNameLen] = '\0';
							   //suit no
							   memcpy(&header.suitNumber, &buffer[nReadLen], sizeof(header.suitNumber));
							   nReadLen += sizeof(header.suitNumber);
							   //suit type
							   memcpy(&header.suitType, &buffer[nReadLen], sizeof(header.suitType));
							   nReadLen += sizeof(header.suitType);
							   //frame number
							   memcpy(&header.frameNumber, &buffer[nReadLen], sizeof(header.frameNumber));
							   nReadLen += sizeof(header.frameNumber);
							   //bone counts
							   memcpy(&header.boneCount, &buffer[nReadLen], sizeof(header.boneCount));
							   nReadLen += sizeof(header.boneCount);
							   //int curBoneCount = header.boneCount;

							   //init data
							   memset(fData, 0, maxFloatDataLen);

							   int16_t dataEuler[3];
							   int16_t dataQuat[4];
							   for (int i = 0; i < curBoneCount; i++)
							   {
								   memcpy(dataEuler, &buffer[U3DUE_FOREPART_LENGTH + (sizeof(dataEuler)+sizeof(dataQuat))*i], sizeof(dataEuler));
								   memcpy(dataQuat, &buffer[U3DUE_FOREPART_LENGTH + (sizeof(dataEuler)+sizeof(dataQuat))*i + sizeof(dataEuler)], sizeof(dataQuat));
								   fData[i * 7 + 0] = ((float)dataEuler[0] / EulerScale);
								   fData[i * 7 + 1] = ((float)dataEuler[1] / EulerScale);
								   fData[i * 7 + 2] = ((float)dataEuler[2] / EulerScale);
								   fData[i * 7 + 3] = ((float)dataQuat[0] / QuatScale);
								   fData[i * 7 + 4] = ((float)dataQuat[1] / QuatScale);
								   fData[i * 7 + 5] = ((float)dataQuat[2] / QuatScale);
								   fData[i * 7 + 6] = ((float)dataQuat[3] / QuatScale);
							   }

							   _RegisterCalDataFunctin(_customedObj, (void*)socketObj[nPos], &header, fData, curBoneCount * 7);
							   break;
							   break;
			}
			default:
			{
					   memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.", sizeof(errorMsg));
					   continue;
			}
			}
		}
		else
		{
			//printf_s(errorMsg, "recvfrom error no: %d.", nRet);
		}
	}
	return 0;
}

FODATARECV_API SOCKET_REF FoConnectTo(char* serverIP, int nPort)
{
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(nPort); ///监听端口
	local.sin_addr.s_addr = INADDR_ANY; ///本机

	int nPos = 0;
	for (; nPos < Max_Socket_Count; nPos++)
	{
		if (socketObj[nPos] == INVALID_SOCKET)
			break;
	}
	if (nPos >= Max_Socket_Count)
	{
		memcpy(errorMsg, "socket counts is beyond the max counts.", sizeof(errorMsg));
		return nullptr;
	}

	socketObj[nPos] = socket(AF_INET, SOCK_DGRAM, 0);
	int nRet = bind(socketObj[nPos], (struct sockaddr*)&local, sizeof(local));

	if (nRet == 0)
	{
	CreateThread(
		NULL,              // default security attributes
		0,                 // use default stack size  
		ThreadProc,        // thread function 
		(void*)nPos,             // argument to thread function 
		0,                 // use default creation flags 
		NULL);           // returns the thread identifier 
	}
	else
	{
		printf("Bind local port failed!\r\n");
		return nullptr;
	}


	return (void*)socketObj[nPos];
}

FODATARECV_API void BRCloseSocket(SOCKET_REF sockRef)
{
	for (int i = 0; i < Max_Socket_Count; i++)
	{
		if (socketObj[i] == (SOCKET)sockRef)
		{
			socketObj[i] = INVALID_SOCKET;
			closesocket((SOCKET)sockRef);
			break;
		}
	}
}

FODATARECV_API char* FoGetLastErrorMessage()
{
	return errorMsg; 
}

FODATARECV_API void FoRegisterCalDataRecvFunction(void* customedObj, CalculationType calType, SuitType sType, CalculationDataRecv handle)
{
	_customedObj = customedObj;
	_calType = calType;
	_suitType = sType;
	_RegisterCalDataFunctin = handle;
}
