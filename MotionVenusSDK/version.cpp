#include "stdafx.h"
#include "version.h"

uint32_t getMVenusSDKVerInt()
{
	return (MOTIONVENUSSDK_MAIN_VERSION << 24) |
		(MOTIONVENUSSDK_SUB_VERSION << 16) |
		(MOTIONVENUSSDK_MODIFY_VERSION << 8);
}

std::string getMVenusSDKVerString()
{
	std::string ret;
	ret += to_string(MOTIONVENUSSDK_MAIN_VERSION) + "." +
		to_string(MOTIONVENUSSDK_SUB_VERSION) + "." +
		to_string(MOTIONVENUSSDK_MODIFY_VERSION);
	return ret;
}

std::string getMVenusSDKVerDesString()
{
	std::string ret(MOTIONVENUSSDK_HEADER);
	ret += " ";
	ret += getMVenusSDKVerString();
	return ret;
}

std::string getMVenusSDKReleaseDate()
{
	return "";
}