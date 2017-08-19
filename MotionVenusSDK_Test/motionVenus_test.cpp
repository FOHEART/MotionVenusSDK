// FoDataRecv_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <sstream>

#include "../MotionVenusSDK/motionVenusSDK.h"
using namespace std;

#ifdef _DEBUG
#pragma comment(lib, "lib\\Debug\\MotionVenusSDKd.lib")
#else
#pragma comment(lib, "lib\\Release\\MotionVenusSDK.lib")
#endif // DEBUG

#define HEADER_COMMENT "/*******MotionVenusSDK test program*********/\r\n"\
                       "This application is used for reveive data stream\r\n"\
	                   "from MotionVenus, total 23 skeletons is included,\r\n"\
	                   "we can get skeleton rotation(in euler or quaternion)\r\n"\
                       "and postion from here.\r\n"\
					   "For Axis and Rotation Defination, Please Checkout Chapter:4.2&4.3 in:\r\n"\
                       "https://github.com/FOHEART/FOHEART_Unity3D_Plugin"\
	                   "\r\n/*******************************************/\r\n"
typedef enum{
	HUMAN_DEFAULT_SKELETON_23 = 23,
	HUMAN_DEFAULT_SKELETON_53 = 53
}Skeleton_Template;

typedef enum{
	HUMAN_DEFAULT_JOINT_22 = 22,
	HUMAN_DEFAULT_JOINT_52 = 52
}Joint_Template;

const std::string kinemHumanSkeleton23[HUMAN_DEFAULT_SKELETON_23] = {
	"Pelvis",
	"L5",
	"L3",
	"T12",
	"T8",
	"Neck",
	"Head",
	"RightShoulder",
	"RightUpperArm",
	"RightForeArm",
	"RightHand",
	"LeftShoulder",
	"LeftUpperArm",
	"LeftForeArm",
	"LeftHand",
	"RightUpperLeg",
	"RightLowerLeg",
	"RightFoot",
	"RightToe",
	"LeftUpperLeg",
	"LeftLowerLeg",
	"LeftFoot",
	"LeftToe",
};

typedef enum{
	K23_Pelvis,
	K23_L5,
	K23_L3,
	K23_T12,
	K23_T8,
	K23_Neck,
	K23_Head,
	K23_RightShoulder,
	K23_RightUpperArm,
	K23_RightForeArm,
	K23_RightHand,
	K23_LeftShoulder,
	K23_LeftUpperArm,
	K23_LeftForeArm,
	K23_LeftHand,
	K23_RightUpperLeg,
	K23_RightLowerLeg,
	K23_RightFoot,
	K23_RightToe,
	K23_LeftUpperLeg,
	K23_LeftLowerLeg,
	K23_LeftFoot,
	K23_LeftToe,
	K23_Total
}K23_Skeleton;

void __stdcall CalDataRecv(void* customedObj, SOCKET_REF sender, CalDataHeader* header, float* data, int countFloat)
{
#define POS_RULER_PAIR_LEN (6)
#define POS_X_INDEX (0)
#define POS_Y_INDEX (1)
#define POS_Z_INDEX (2)
#define EULER_X_INDEX (3)
#define EULER_Y_INDEX (4)
#define EULER_Z_INDEX (5)
	char* buf = (char*) header->AvatarName;
	std::string posAndEuler;
	std::stringstream buffer;

	for (uint8_t i = 0; i < K23_Total; i++)
	{

		buffer<< kinemHumanSkeleton23[i]<<":pos[x,y,z](";
		buffer << data[i * POS_RULER_PAIR_LEN + POS_X_INDEX] << ", ";
		buffer << data[i * POS_RULER_PAIR_LEN + POS_Y_INDEX] << ", ";
		buffer << data[i * POS_RULER_PAIR_LEN + POS_Z_INDEX] << ") angle[x,y,z](";

		buffer << data[i * POS_RULER_PAIR_LEN + EULER_X_INDEX] << ", ";
		buffer << data[i * POS_RULER_PAIR_LEN + EULER_Y_INDEX] << ", ";
		buffer << data[i * POS_RULER_PAIR_LEN + EULER_Z_INDEX] << ") \r\n";
	}
	printf("sender:%d==,%s : %d -- \r\n%s\r\n", (int) sender, buf, countFloat, buffer.str().c_str());
	//system("cls");
}

int _tmain(int argc, _TCHAR* argv [])
{
	printf("%s", HEADER_COMMENT);
	printf("Try to Bind IP and port: 127.0.0.1:5001 using UDP.\r\n");
	SOCKET_REF nRet = FoConnectTo("127.0.0.1", 5001);
	if (nRet == nullptr)
	{
		printf("error SOCKET_REF, any key to exit.\n");
		getchar();
		return 0;
	}
	else
	{
		printf("Bind success. Start Reading Data...\r\n");
	}
	FoRegisterCalDataRecvFunction(NULL, Position_Euler, Wireless, CalDataRecv);
	while (true)
	{
		Sleep(1);
	}


	return 0;
}

