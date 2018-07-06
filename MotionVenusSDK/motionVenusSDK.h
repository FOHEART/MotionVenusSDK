// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 FODATARECV_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// FODATARECV_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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

/*  发送数据类型顺序 Position-（Euler/Quat）-Accel-Mag-Gyro  */
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