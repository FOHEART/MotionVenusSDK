// FoDataRecv.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include<stdlib.h>
#include <Winsock2.h>
#include "motionVenusSDK.h"

#define Max_Socket_Count	10
#define U3DUE_FOREPART_LENGTH	128	//UDP报文头长度
#define Bone_Count_Wireless		23	//无线套装发送骨骼数量
#define Bone_Count_Wired		23	//有线套装发送骨骼数量
#define float2string_ValidPlaceCount (6) //float转字符串占用字符数，与MotionVenus同步
#define float2string_BlankPlace (float2string_ValidPlaceCount+1) //float转字符串占用5个字符和1个空格字符

const uint16_t QuatScale = (1 << 13);
const uint16_t EulerScale = (1 << 7);
const uint16_t AccelScale = (1 << 10);
const uint32_t LocScale = (1 << 16);

//socket list
SOCKET socketObj[Max_Socket_Count] = { INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET }; //socket list
//user obj data
void*  _customedObj = nullptr;
//StreamType type
StreamType _streamType = ST_Binary;
//DataType type
SendDataType _dataType;
//Suit type
SuitType _suitType = Wireless;
//error msg
char  errorMsg[100] = { ' ' };
//socket init
WSADATA wsaData;

//lock
CRITICAL_SECTION    m_cs;

//function pointer
void(__stdcall *_RegisterCalDataFunctin)(void* customedObj, SOCKET_REF sender, CalDataHeader* header, void* data, int countFloat) = nullptr;


void GetHeadInfo(CalDataHeader& header, char* buffer)
{
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
}

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
	int maxFloatDataLen = (3 + 4 + 3 + 3 + 3)*(_suitType == Wireless ? Bone_Count_Wireless : Bone_Count_Wired);
	int maxSzDataLen = 2048;
	float* fData = new float[maxFloatDataLen];
	char* szData = new char[maxSzDataLen];
	memset(fData, 0, maxFloatDataLen);
	memset(szData, 0, maxSzDataLen);

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

			if ((_dataType.bEuler && _dataType.bQuat) || (!_dataType.bEuler && !_dataType.bQuat))
			{
				memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.[Euler and Quat]", sizeof(errorMsg));
				continue;
			}
			//check calculate data type
			int totalBuffSize = 0;

			if (_streamType == ST_String)
			{
				if ((_dataType.bAccel || _dataType.bMag || _dataType.bGyro))
				{
					memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.[ST_String error]", sizeof(errorMsg));
					continue;
				}

				totalBuffSize = U3DUE_FOREPART_LENGTH;
				/*if (_dataType.bPosition)
				{
				totalBuffSize = totalBuffSize + (float2string_BlankPlace * 3)* curBoneCount;
				}

				if (_dataType.bEuler )
				{
				totalBuffSize = totalBuffSize + (float2string_BlankPlace * 3)* curBoneCount;
				}

				if (_dataType.bQuat)
				{
				totalBuffSize = totalBuffSize + (float2string_BlankPlace * 4)* curBoneCount;
				}

				if (totalBuffSize != nRet)
				{
				memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.[SendDataType error]", sizeof(errorMsg));
				continue;
				}*/

				//head
				CalDataHeader header;
				GetHeadInfo(header, buffer);

				//init data
				memset(szData, 0, maxSzDataLen);
				memcpy(szData, &buffer[U3DUE_FOREPART_LENGTH], nRet);
				szData[nRet-U3DUE_FOREPART_LENGTH] = '\0';
				_RegisterCalDataFunctin(_customedObj, (void*)socketObj[nPos], &header, szData, curBoneCount * 3);
			}
			else  //ST_Binary
			{
				totalBuffSize = U3DUE_FOREPART_LENGTH;

				int32_t dataPosition[3];
				int16_t dataEuler[3];
				int16_t dataQuat[4];
				int32_t dataAccel[3];
				int32_t dataMag[3];
				int32_t dataGyro[3];

				if (_dataType.bPosition)
				{
					totalBuffSize = totalBuffSize + sizeof(int32_t)* 3 * curBoneCount;
				}
				if (_dataType.bEuler)
				{
					totalBuffSize = totalBuffSize + sizeof(int16_t)* 3 * curBoneCount;
				}
				if (_dataType.bQuat)
				{
					totalBuffSize = totalBuffSize + sizeof(int16_t)* 4 * curBoneCount;
				}
				if (_dataType.bAccel)
				{
					totalBuffSize = totalBuffSize + sizeof(int16_t)* 3 * curBoneCount;
				}
				if (_dataType.bMag)
				{
					totalBuffSize = totalBuffSize + sizeof(int16_t)* 3 * curBoneCount;
				}
				if (_dataType.bGyro)
				{
					totalBuffSize = totalBuffSize + sizeof(int16_t)* 3 * curBoneCount;
				}

				if (nRet != totalBuffSize)
				{
					memcpy(errorMsg, "recvfrom data to calculate data error: Unrecognized data type.[SendDataType error]", sizeof(errorMsg));
					continue;
				}

				//head
				CalDataHeader header;
				GetHeadInfo(header, buffer);

				//init data
				memset(fData, 0, maxFloatDataLen*sizeof(float));

				int pos_buf = U3DUE_FOREPART_LENGTH;
				int pos_float = 0;
				for (int i = 0; i < curBoneCount; i++)
				{
					if (_dataType.bPosition)
					{
						memcpy(dataPosition, &buffer[pos_buf], sizeof(dataPosition));
						fData[pos_float++] = ((float)dataPosition[0] / LocScale);
						fData[pos_float++] = ((float)dataPosition[1] / LocScale);
						fData[pos_float++] = ((float)dataPosition[2] / LocScale);
						pos_buf += sizeof(dataPosition);
					}

					if (_dataType.bEuler)
					{
						memcpy(dataEuler, &buffer[pos_buf], sizeof(dataEuler));
						fData[pos_float++] = ((float)dataEuler[0] / EulerScale);
						fData[pos_float++] = ((float)dataEuler[1] / EulerScale);
						fData[pos_float++] = ((float)dataEuler[2] / EulerScale);
						pos_buf += sizeof(dataEuler);
					}

					if (_dataType.bQuat)
					{
						memcpy(dataQuat, &buffer[pos_buf], sizeof(dataQuat));
						fData[pos_float++] = ((float)dataQuat[0] / QuatScale);
						fData[pos_float++] = ((float)dataQuat[1] / QuatScale);
						fData[pos_float++] = ((float)dataQuat[2] / QuatScale);
						fData[pos_float++] = ((float)dataQuat[3] / QuatScale);
						pos_buf += sizeof(dataQuat);
					}

					if (_dataType.bAccel)
					{
						memcpy(dataAccel, &buffer[pos_buf], sizeof(dataAccel));
						fData[pos_float++] = ((float)dataAccel[0] / AccelScale);
						fData[pos_float++] = ((float)dataAccel[1] / AccelScale);
						fData[pos_float++] = ((float)dataAccel[2] / AccelScale);
						pos_buf += sizeof(dataAccel);
					}

					if (_dataType.bMag)
					{
						memcpy(dataMag, &buffer[pos_buf], sizeof(dataMag));
						fData[pos_float++] = ((float)dataMag[0] / AccelScale);
						fData[pos_float++] = ((float)dataMag[1] / AccelScale);
						fData[pos_float++] = ((float)dataMag[2] / AccelScale);
						pos_buf += sizeof(dataMag);
					}

					if (_dataType.bGyro)
					{
						memcpy(dataGyro, &buffer[pos_buf], sizeof(dataGyro));
						fData[pos_float++] = ((float)dataGyro[0] / AccelScale);
						fData[pos_float++] = ((float)dataGyro[1] / AccelScale);
						fData[pos_float++] = ((float)dataGyro[2] / AccelScale);
						pos_buf += sizeof(dataGyro);
					}
				}

				_RegisterCalDataFunctin(_customedObj, (void*)socketObj[nPos], &header, fData, pos_float);
			}
		}
		else
		{
			printf_s(errorMsg, "recvfrom error no: %d.", nRet);
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

FODATARECV_API void FoRegisterCalDataRecvFunction(void* customedObj, StreamType streamType, SendDataType dataType, SuitType sType, CalculationDataRecv handle)
{
	if ((dataType.bEuler && dataType.bQuat) || (!dataType.bEuler && !dataType.bQuat))
	{
		printf("Register data type error : Unrecognized data type.[SendDataType error(Quat or Euler)]");
		return;
	}

	_customedObj = customedObj;
	_streamType = streamType;
	_dataType = dataType;
	_suitType = sType;
	_RegisterCalDataFunctin = handle;
}
