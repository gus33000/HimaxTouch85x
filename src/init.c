/*++
    Copyright (c) Microsoft Corporation. All Rights Reserved.
    Copyright (c) Bingxing Wang. All Rights Reserved.
    Copyright (c) LumiaWoA authors. All Rights Reserved.

	Module Name:

		init.c

	Abstract:

		Contains Himax initialization code

	Environment:

		Kernel mode

	Revision History:

--*/

#include <Cross Platform Shim\compat.h>
#include <spb.h>
#include <hx8526\hxinternal.h>
#include <init.tmh>

NTSTATUS
TchStartDevice(
	IN VOID* ControllerContext,
	IN SPB_CONTEXT* SpbContext
)
/*++

  Routine Description:

	This routine is called in response to the KMDF prepare hardware call
	to initialize the touch controller for use.

  Arguments:

	ControllerContext - A pointer to the current touch controller
	context

	SpbContext - A pointer to the current i2c context

  Return Value:

	NTSTATUS indicating success or failure

--*/
{
	HX8526_CONTROLLER_CONTEXT* controller;
	ULONG interruptStatus;
	NTSTATUS status;

	controller = (HX8526_CONTROLLER_CONTEXT*)ControllerContext;
	interruptStatus = 0;
	status = STATUS_SUCCESS;

	//
	// Populate context with HX8526 function descriptors
	//
	status = Hx8526BuildFunctionsTable(
		ControllerContext,
		SpbContext);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_INIT,
			"Could not build table of HX8526 functions - 0x%08lX",
			status);
		goto exit;
	}

	//
	// Initialize HX8526 function control registers
	//
	status = Hx8526ConfigureFunctions(
		ControllerContext,
		SpbContext);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_INTERRUPT,
			"Could not configure chip - 0x%08lX",
			status);

		goto exit;
	}

	status = Hx8526ConfigureInterruptEnable(
		ControllerContext,
		SpbContext);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_INIT,
			"Could not configure interrupt enablement - 0x%08lX",
			status);
		goto exit;
	}

	//
	// Read and store the firmware version
	//
	status = Hx8526GetFirmwareVersion(
		ControllerContext,
		SpbContext);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_INIT,
			"Could not get HX8526 firmware version - 0x%08lX",
			status);
		goto exit;
	}

	//
	// Clear any pending interrupts
	//
	status = Hx8526CheckInterrupts(
		ControllerContext,
		SpbContext,
		&interruptStatus
	);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_INIT,
			"Could not get interrupt status - 0x%08lX%",
			status);
	}

exit:
	return status;
}

NTSTATUS
TchStopDevice(
	IN VOID* ControllerContext,
	IN SPB_CONTEXT* SpbContext
)
/*++

Routine Description:

	This routine cleans up the device that is stopped.

Argument:

	ControllerContext - Touch controller context

	SpbContext - A pointer to the current i2c context

Return Value:

	NTSTATUS indicating sucess or failure
--*/
{
	HX8526_CONTROLLER_CONTEXT* controller;

	UNREFERENCED_PARAMETER(SpbContext);

	controller = (HX8526_CONTROLLER_CONTEXT*)ControllerContext;

	return STATUS_SUCCESS;
}

NTSTATUS
TchAllocateContext(
	OUT VOID** ControllerContext,
	IN WDFDEVICE FxDevice
)
/*++

Routine Description:

	This routine allocates a controller context.

Argument:

	ControllerContext - Touch controller context
	FxDevice - Framework device object

Return Value:

	NTSTATUS indicating sucess or failure
--*/
{
	HX8526_CONTROLLER_CONTEXT* context;
	NTSTATUS status;
	
	context = ExAllocatePoolWithTag(
		NonPagedPoolNx,
		sizeof(HX8526_CONTROLLER_CONTEXT),
		TOUCH_POOL_TAG);

	if (NULL == context)
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_INIT,
			"Could not allocate controller context!");

		status = STATUS_UNSUCCESSFUL;
		goto exit;
	}

	RtlZeroMemory(context, sizeof(HX8526_CONTROLLER_CONTEXT));
	context->FxDevice = FxDevice;

	//
	// Get Touch settings and populate context
	//
	TchGetTouchSettings(&context->TouchSettings);

	//
	// Allocate a WDFWAITLOCK for guarding access to the
	// controller HW and driver controller context
	//
	status = WdfWaitLockCreate(
		WDF_NO_OBJECT_ATTRIBUTES,
		&context->ControllerLock);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_INIT,
			"Could not create lock - 0x%08lX",
			status);

		TchFreeContext(context);
		goto exit;

	}

	*ControllerContext = context;

exit:

	return status;
}

NTSTATUS
TchFreeContext(
	IN VOID* ControllerContext
)
/*++

Routine Description:

	This routine frees a controller context.

Argument:

	ControllerContext - Touch controller context

Return Value:

	NTSTATUS indicating sucess or failure
--*/
{
	HX8526_CONTROLLER_CONTEXT* controller;

	controller = (HX8526_CONTROLLER_CONTEXT*)ControllerContext;

	if (controller != NULL)
	{

		if (controller->ControllerLock != NULL)
		{
			WdfObjectDelete(controller->ControllerLock);
		}

		ExFreePoolWithTag(controller, TOUCH_POOL_TAG);
	}

	return STATUS_SUCCESS;
}