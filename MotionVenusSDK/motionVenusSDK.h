// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� FODATARECV_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// FODATARECV_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef FODATARECV_EXPORTS
#define FODATARECV_API __declspec(dllexport)
#else
#define FODATARECV_API __declspec(dllimport)
#endif

#include <stdint.h>

#define SOCKET_REF void*



typedef enum _StreamType
{
	ST_Binary,
	ST_String
}StreamType;

/*  ������������˳�� Position-��Euler/Quat��-Accel-Mag-Gyro  */
typedef struct _SendDataType
{
	_SendDataType() :bEuler(false), bQuat(false), bPosition(false),
	bAccel(false), bMag(false), bGyro(false){}
	bool bEuler;	//MotionVenus send euler angle data.
	bool bQuat;		//MotionVenus send quaternion data.
	bool bPosition; //MotionVenus send position data.
	bool bAccel;	//MotionVenus send Accel data.
	bool bMag;		//MotionVenus send magnetic data.
	bool bGyro;		//MotionVenus send gyroscope data.
}SendDataType;

typedef struct _data_header
{
	uint16_t   protocolVersion;   //protocol version
	uint8_t    AvatarName[50];	  //actor name
	uint32_t   suitNumber;		  //suit serial number
	uint8_t    suitType;          //suit type: 0:wireless 1:wired
	uint32_t   frameNumber;		  //frame number, start at 1
	uint8_t    boneCount;         //bone total number
}CalDataHeader, *LPCalDataHeader;

enum SuitType
{
	Wireless,
	wired,
};

/*
    Register receiving and parsed Calculation Data callback
	sender: socket obj
	header: data header
	data: ST_Binary: sort by position(float[3]),Euler(float[3]),Quat(float[4]),Accel(float[3]);
		  ST_String: (char*)data;
	countFloat: 
		Euler_only			: 3;
		Quat_only			: 4;
		Position_Euler		: 6;
		Position_Quat		: 7;
		Euler_Accel			: 6;
		Quat_Accel			: 7;
		Position_Euler_Accel: 9;
		Position_Quat_Accel	: 10;
*/
typedef void(__stdcall *CalculationDataRecv)(void* customedObj, SOCKET_REF sender, CalDataHeader* header, void* data, int countFloat);

#ifdef __cplusplus
extern "C" {
#endif

//Register function
/*
	customedObj : function identification number or user transmission data and so on, not null.
	StreamType :  ST_Binary / ST_String
	DataType: Configure the selection according to the Settings in the MotionVenus.
	sType: SuitType : Wireless/wired.
	handle : function pointer.
*/
	FODATARECV_API void FoRegisterCalDataRecvFunction(void* customedObj, StreamType streamType, SendDataType dataType, SuitType sType, CalculationDataRecv handle);

//connect to server, and return socket obj
FODATARECV_API SOCKET_REF FoConnectTo(char* serverIP, int nPort);

//Close a socket
FODATARECV_API void BRCloseSocket(SOCKET_REF sockRef);

// If any error, you can call 'FoGetLastErrorMessage' to get error information
FODATARECV_API char* FoGetLastErrorMessage();


#ifdef __cplusplus
};
#endif