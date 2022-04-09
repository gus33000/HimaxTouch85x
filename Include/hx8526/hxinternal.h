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

#define TOUCH_POOL_TAG_HX              (ULONG)'xhoT'

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
} HX8526_F01_CTRL_REGISTERS_LOGICAL;

#define HX8526_MILLISECONDS_TO_TENTH_MILLISECONDS(n) n/10
#define HX8526_SECONDS_TO_HALF_SECONDS(n) 2*n

//
// Function $11 - 2-D Touch Sensor
//

//
// Logical structure for getting registry config settings
//
typedef struct _HX8526_F11_CTRL_REGISTERS_LOGICAL
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
} HX8526_F11_CTRL_REGISTERS_LOGICAL;

//
// Driver structures
//

typedef struct _HX8526_CONFIGURATION
{
	HX8526_F01_CTRL_REGISTERS_LOGICAL DeviceSettings;
	HX8526_F11_CTRL_REGISTERS_LOGICAL TouchSettings;
	UINT32 PepRemovesVoltageInD3;
} HX8526_CONFIGURATION;

typedef struct _HX8526_CONTROLLER_CONTEXT
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
	HX8526_CONFIGURATION Config;

	UCHAR Data1Offset;

	BYTE MaxFingers;

    int HidQueueCount;
} HX8526_CONTROLLER_CONTEXT;

NTSTATUS
Hx8526BuildFunctionsTable(
	IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
	IN SPB_CONTEXT* SpbContext
);

NTSTATUS
Hx8526ChangePage(
	IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
	IN SPB_CONTEXT* SpbContext,
	IN int DesiredPage
);

NTSTATUS
Hx8526ConfigureFunctions(
	IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
	IN SPB_CONTEXT* SpbContext
);

NTSTATUS
Hx8526ServiceInterrupts(
	IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
	IN SPB_CONTEXT* SpbContext,
	IN PREPORT_CONTEXT ReportContext
);

#define HX8526_F01_DEVICE_CONTROL_SLEEP_MODE_OPERATING  0
#define HX8526_F01_DEVICE_CONTROL_SLEEP_MODE_SLEEPING   1

NTSTATUS
Hx8526SetReportingFlagsF12(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext,
    IN UCHAR NewMode,
    OUT UCHAR* OldMode
);

NTSTATUS
Hx8526ChangeChargerConnectedState(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext,
    IN UCHAR ChargerConnectedState
);

NTSTATUS
Hx8526ChangeSleepState(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext,
    IN UCHAR SleepState
);

NTSTATUS
Hx8526GetFirmwareVersion(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext
);

NTSTATUS
Hx8526CheckInterrupts(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext,
    IN ULONG* InterruptStatus
);

NTSTATUS
Hx8526ConfigureInterruptEnable(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext
);

