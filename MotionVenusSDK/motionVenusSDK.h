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

typedef enum _CalculationType
{
	Position_only,		//MotionVenus only send bone position data.
	Euler_only,			//MotionVenus only send euler data.
	Quat_only,			//MotionVenus only send quaternion data.
	Position_Euler,		//MotionVenus send position and euler data.
	Position_Quat,		//MotionVenus send position and quaternion data.
	Euler_Quat,			//MotionVenus send euler and quaternion data.
}CalculationType;
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
	data: sort by position(float[3]),Euler(float[3]),Quat(float[4])
	countFloat: 
		Position_only: 3;
		Euler_only	: 3;
		Quat_only	: 4;
		Position_Euler:6;
		Position_Quat:7;
*/
typedef void(__stdcall *CalculationDataRecv)(void* customedObj, SOCKET_REF sender, CalDataHeader* header, float* data, int countFloat);

#ifdef __cplusplus
extern "C" {
#endif

//Register function
/*
	customedObj : function identification number or user transmission data and so on, not null.
	calType: Calculation type:
								Position_only;
								Euler_only;
								Quat_only;
								Position_Euler;
								Position_Quat.
	sType: SuitType : Wireless/wired.
	handle : function pointer.
*/
FODATARECV_API void FoRegisterCalDataRecvFunction(void* customedObj, CalculationType calType, SuitType sType, CalculationDataRecv handle);

//connect to server, and return socket obj
FODATARECV_API SOCKET_REF FoConnectTo(char* serverIP, int nPort);

//Close a socket
FODATARECV_API void BRCloseSocket(SOCKET_REF sockRef);

// If any error, you can call 'FoGetLastErrorMessage' to get error information
FODATARECV_API char* FoGetLastErrorMessage();


#ifdef __cplusplus
};
#endif