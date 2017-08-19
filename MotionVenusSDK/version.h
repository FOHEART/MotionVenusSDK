#pragma  once

#include <cstdint>
#include <string>
using namespace std;

#define MOTIONVENUSSDK_MAIN_VERSION   (1)
#define MOTIONVENUSSDK_SUB_VERSION    (3)
#define MOTIONVENUSSDK_MODIFY_VERSION (0)
#define MOTIONVENUSSDK_HEADER         ("FOHEART MotionVenusSDK ")

#define MOTIONVENUSSDK_RELEASE_YEAR  (2017)
#define MOTIONVENUSSDK_RELEASE_MONTH (8)
#define MOTIONVENUSSDK_RELEASE_DAY   (19)

uint32_t getMVenusSDKVerInt();
std::string getMVenusSDKVerString();
std::string getMVenusSDKVerDesString();
std::string getMVenusSDKReleaseDate();
