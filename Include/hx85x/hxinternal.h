/*++
	Copyright (c) Microsoft Corporation. All Rights Reserved.
	Sample code. Dealpoint ID #843729.

	Module Name:

		hxinternal.h

	Abstract:

		Contains common types and defintions used internally
		by the multi touch screen driver.

	Environment:

		Kernel mode

	Revision History:

--*/

#pragma once

#include <wdm.h>
#include <wdf.h>
#include <controller.h>
#include <resolutions.h>
#include <Cross Platform Shim/bitops.h>
#include <Cross Platform Shim/hweight.h>
#include <report.h>

// Ignore warning C4152: nonstandard extension, function/data pointer conversion in expression
#pragma warning (disable : 4152)

// Ignore warning C4201: nonstandard extension used : nameless struct/union
#pragma warning (disable : 4201)

// Ignore warning C4201: nonstandard extension used : bit field types other than in
#pragma warning (disable : 4214)

// Ignore warning C4324: 'xxx' : structure was padded due to __declspec(align())
#pragma warning (disable : 4324)

static BYTE HX85X_IC_POWER_ON_COMMAND[1]     = { 0x81 };
static BYTE HX85X_MCU_POWER_ON_COMMAND[2]    = { 0x35, 0x02 };
static BYTE HX85X_SENSE_ON_COMMAND[1]        = { 0x83 };
static BYTE HX85X_SENSE_OFF_COMMAND[1]       = { 0x82 };
static BYTE HX85X_GET_ID_COMMAND[1]          = { 0x31 };
static BYTE HX85X_GET_EVENT_COMMAND[1]       = { 0x85 };
static BYTE HX85X_GET_SLEEP_COMMAND[1]       = { 0x63 };

static BYTE HX8520_FLASH_POWER_ON_COMMAND[2] = { 0x36, 0x01 };
static BYTE HX8520_SPEED_MODE_COMMAND[2]     = { 0x9D, 0x80 };

static BYTE HX8526_FLASH_POWER_ON_COMMAND[3] = { 0x36, 0x0F, 0x53 };
static BYTE HX8526_FETCH_FLASH_COMMAND[3]    = { 0xDD, 0x04, 0x02 };

typedef struct _HIMAX_TOUCH_DATA
{
	BYTE PositionX_High;
	BYTE PositionX_Low;
	BYTE PositionY_High;
	BYTE PositionY_Low;
} HIMAX_TOUCH_DATA, * PHIMAX_TOUCH_DATA;

#define HX8526_MAX_TOUCH_DATA 5
#define HX8520_MAX_TOUCH_DATA 2

typedef struct _HX8526_EVENT_DATA
{
	HIMAX_TOUCH_DATA TouchData[HX8526_MAX_TOUCH_DATA];

	BYTE Reserved0[8];

	BYTE Reserved1 : 4;
	BYTE NumberOfTouchPoints : 4;

	BYTE ActivePointsMask;
	BYTE Reserved2[2];
} HX8526_EVENT_DATA, *PHX8526_EVENT_DATA;

typedef struct _HX8520_EVENT_DATA
{
	HIMAX_TOUCH_DATA TouchData[HX8520_MAX_TOUCH_DATA];

	BYTE Reserved0[4];

	BYTE Reserved1 : 4;
	BYTE NumberOfTouchPoints : 4;

	BYTE ActivePointsMask;
	BYTE Reserved2[2];
} HX8520_EVENT_DATA, *PHX8520_EVENT_DATA;

#define TOUCH_POOL_TAG_HX (ULONG)'xhoT'

//
// Logical structure for getting registry config settings
//
typedef struct _RM4_F01_CTRL_REGISTERS_LOGICAL
{
	UINT32 SleepMode;
	UINT32 NoSleep;
	UINT32 ReportRate;
	UINT32 Configured;
	UINT32 InterruptEnable;
	UINT32 DozeInterval;
	UINT32 DozeThreshold;
	UINT32 DozeHoldoff;
} HX85X_F01_CTRL_REGISTERS_LOGICAL;

#define HX85X_MILLISECONDS_TO_TENTH_MILLISECONDS(n) n/10
#define HX85X_SECONDS_TO_HALF_SECONDS(n) 2*n

//
// Function $11 - 2-D Touch Sensor
//

//
// Logical structure for getting registry config settings
//
typedef struct _HX85X_F11_CTRL_REGISTERS_LOGICAL
{
	UINT32 ReportingMode;
	UINT32 AbsPosFilt;
	UINT32 RelPosFilt;
	UINT32 RelBallistics;
	UINT32 Dribble;
	UINT32 PalmDetectThreshold;
	UINT32 MotionSensitivity;
	UINT32 ManTrackEn;
	UINT32 ManTrackedFinger;
	UINT32 DeltaXPosThreshold;
	UINT32 DeltaYPosThreshold;
	UINT32 Velocity;
	UINT32 Acceleration;
	UINT32 SensorMaxXPos;
	UINT32 SensorMaxYPos;
	UINT32 ZTouchThreshold;
	UINT32 ZHysteresis;
	UINT32 SmallZThreshold;
	UINT32 SmallZScaleFactor;
	UINT32 LargeZScaleFactor;
	UINT32 AlgorithmSelection;
	UINT32 WxScaleFactor;
	UINT32 WxOffset;
	UINT32 WyScaleFactor;
	UINT32 WyOffset;
	UINT32 XPitch;
	UINT32 YPitch;
	UINT32 FingerWidthX;
	UINT32 FingerWidthY;
	UINT32 ReportMeasuredSize;
	UINT32 SegmentationSensitivity;
	UINT32 XClipLo;
	UINT32 XClipHi;
	UINT32 YClipLo;
	UINT32 YClipHi;
	UINT32 MinFingerSeparation;
	UINT32 MaxFingerMovement;
} HX85X_F11_CTRL_REGISTERS_LOGICAL;

//
// Driver structures
//

typedef struct _HX85X_CONFIGURATION
{
	HX85X_F01_CTRL_REGISTERS_LOGICAL DeviceSettings;
	HX85X_F11_CTRL_REGISTERS_LOGICAL TouchSettings;
	UINT32 PepRemovesVoltageInD3;
} HX85X_CONFIGURATION;

typedef struct _HX85X_CONTROLLER_CONTEXT
{
	WDFDEVICE FxDevice;
	WDFWAITLOCK ControllerLock;

	//
	// Power state
	//
	DEVICE_POWER_STATE DevicePowerState;

	//
	// Register configuration programmed to chip
	//
	TOUCH_SCREEN_SETTINGS TouchSettings;
	HX85X_CONFIGURATION Config;

	UCHAR Data1Offset;

	BYTE MaxFingers;

	int ChipModel;

	int HidQueueCount;
} HX85X_CONTROLLER_CONTEXT;

NTSTATUS
Hx85xBuildFunctionsTable(
	IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
	IN SPB_CONTEXT* SpbContext
);

NTSTATUS
Hx85xChangePage(
	IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
	IN SPB_CONTEXT* SpbContext,
	IN int DesiredPage
);

NTSTATUS
Hx85xConfigureFunctions(
	IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
	IN SPB_CONTEXT* SpbContext
);

NTSTATUS
Hx85xServiceInterrupts(
	IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
	IN SPB_CONTEXT* SpbContext,
	IN PREPORT_CONTEXT ReportContext
);

#define HX85X_F01_DEVICE_CONTROL_SLEEP_MODE_OPERATING  0
#define HX85X_F01_DEVICE_CONTROL_SLEEP_MODE_SLEEPING   1

#pragma pack(push)
#pragma pack(1)
typedef enum _HX85X_REPORTING_FLAGS
{
	HX85X_REPORTING_CONTINUOUS_MODE = 0,
	HX85X_REPORTING_REDUCED_MODE = 1,
	HX85X_REPORTING_WAKEUP_GESTURE_MODE = 2,
} HX85X_REPORTING_FLAGS;
#pragma pack(pop)

NTSTATUS
Hx85xSetReportingFlags(
    IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext,
    IN UCHAR NewMode,
    OUT UCHAR* OldMode
);

NTSTATUS
Hx85xChangeChargerConnectedState(
    IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext,
    IN UCHAR ChargerConnectedState
);

NTSTATUS
Hx85xChangeSleepState(
    IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext,
    IN UCHAR SleepState
);

NTSTATUS
Hx85xGetFirmwareVersion(
    IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext
);

NTSTATUS
Hx85xCheckInterrupts(
    IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext,
    IN ULONG* InterruptStatus
);

NTSTATUS
Hx85xConfigureInterruptEnable(
    IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext
);

