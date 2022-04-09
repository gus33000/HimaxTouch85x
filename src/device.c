/*++
    Copyright (c) Microsoft Corporation. All Rights Reserved.
    Copyright (c) Bingxing Wang. All Rights Reserved.
    Copyright (c) LumiaWoA authors. All Rights Reserved.

    Module Name:

        device.c

    Abstract:

        Code for handling WDF device-specific requests

    Environment:

        Kernel mode

    Revision History:

--*/

#include <internal.h>
#include <controller.h>
#include <device.h>
#include <spb.h>
//#include <HimaxTouch85xDriverETW.h>
#include <idle.h>
#include <hid.h>
#include <gpio.h>
#include <device.h>
#include <hx85x/hxinternal.h>
#include <report.h>
#include <touch_power/touch_power.h>
#include <device.tmh>

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, OnD0Exit)
#endif

BOOLEAN
OnInterruptIsr(
    IN WDFINTERRUPT Interrupt,
    IN ULONG MessageID
)
/*++

  Routine Description:

    This routine responds to interrupts generated by the
    controller. If one is recognized, it queues a DPC for
    processing.

    This is a PASSIVE_LEVEL ISR. ACPI should specify
    level-triggered interrupts when using Himax 3202.

  Arguments:

    Interrupt - a handle to a framework interrupt object
    MessageID - message number identifying the device's
        hardware interrupt message (if using MSI)

  Return Value:

    TRUE if interrupt recognized.

--*/
{
    PDEVICE_EXTENSION devContext;
    NTSTATUS status;

    UNREFERENCED_PARAMETER(MessageID);

    Trace(
        TRACE_LEVEL_ERROR,
        TRACE_REPORTING,
        "OnInterruptIsr - Entry");

    status = STATUS_SUCCESS;
    devContext = GetDeviceContext(WdfInterruptGetDevice(Interrupt));

    //
    // For performance tracing, write an ETW event marker
    //
    //EventWriteTouchIsr(&TouchMiniDriverControlGuid);

    //
    // If we're in diagnostic mode, let the diagnostic application handle
    // interrupt servicing
    //
    if (devContext->DiagnosticMode != FALSE)
    {
        goto exit;
    }

    //
    // Service touch interrupts.
    //
    status = Hx85xServiceInterrupts(
        devContext->TouchContext,
        &devContext->I2CContext,
        &devContext->ReportContext);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_REPORTING,
            "Error servicing interrupts - 0x%08lX",
            status);
        goto exit;
    }

exit:
    return TRUE;
}

NTSTATUS
OnD0Entry(
    IN WDFDEVICE Device,
    IN WDF_POWER_DEVICE_STATE PreviousState
)
/*++

Routine Description:

    This routine will power on the hardware

Arguments:

    Device - WDF device to power on
    PreviousState - Prior power state

Return Value:

    NTSTATUS indicating success or failure

*/
{
    NTSTATUS status;
    PDEVICE_EXTENSION devContext;

    devContext = GetDeviceContext(Device);

    UNREFERENCED_PARAMETER(PreviousState);

    status = TchWakeDevice(devContext->TouchContext, &devContext->I2CContext);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_POWER,
            "Error setting device to D0 - 0x%08lX",
            status);
    }

    //
    // N.B. This HX85X chip's IRQ is level-triggered, but cannot be enabled in
    //      ACPI until passive-level interrupt handling is added to the driver.
    //      Service chip in case we missed an edge during D3 or boot-up.
    //
    devContext->ServiceInterruptsAfterD0Entry = TRUE;

    //
    // Complete any pending Idle IRPs
    //
    TchCompleteIdleIrp(devContext);

    return status;
}

NTSTATUS
OnD0Exit(
    IN WDFDEVICE Device,
    IN WDF_POWER_DEVICE_STATE TargetState
)
/*++

Routine Description:

    This routine will power down the hardware

Arguments:

    Device - WDF device to power off

    PreviousState - Prior power state

Return Value:

    NTSTATUS indicating success or failure

*/
{
    NTSTATUS status;
    PDEVICE_EXTENSION devContext;

    PAGED_CODE();

    devContext = GetDeviceContext(Device);

    UNREFERENCED_PARAMETER(TargetState);

    status = TchStandbyDevice(devContext->TouchContext, &devContext->I2CContext, &devContext->ReportContext);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_POWER,
            "Error exiting D0 - 0x%08lX", 
            status);
    }

    return status;
}

NTSTATUS GetGPIO(WDFIOTARGET gpio, unsigned char* value)
{
    NTSTATUS status = STATUS_SUCCESS;
    WDF_MEMORY_DESCRIPTOR outputDescriptor;

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDescriptor, value, 1);

    status = WdfIoTargetSendIoctlSynchronously(gpio, NULL, IOCTL_GPIO_READ_PINS, NULL, &outputDescriptor, NULL, NULL);

    return status;
}

NTSTATUS SetGPIO(WDFIOTARGET gpio, unsigned char* value)
{
    NTSTATUS status = STATUS_SUCCESS;
    WDF_MEMORY_DESCRIPTOR inputDescriptor, outputDescriptor;

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inputDescriptor, value, 1);
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDescriptor, value, 1);

    status = WdfIoTargetSendIoctlSynchronously(gpio, NULL, IOCTL_GPIO_WRITE_PINS, &inputDescriptor, &outputDescriptor, NULL, NULL);

    return status;
}

NTSTATUS OpenIOTarget(PDEVICE_EXTENSION ctx, LARGE_INTEGER res, ACCESS_MASK use, WDFIOTARGET* target)
{
    NTSTATUS status = STATUS_SUCCESS;
    WDF_OBJECT_ATTRIBUTES ObjectAttributes;
    WDF_IO_TARGET_OPEN_PARAMS OpenParams;
    UNICODE_STRING ReadString;
    WCHAR ReadStringBuffer[260];

    Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "OpenIOTarget Entry");

    RtlInitEmptyUnicodeString(&ReadString,
        ReadStringBuffer,
        sizeof(ReadStringBuffer));

    status = RESOURCE_HUB_CREATE_PATH_FROM_ID(&ReadString,
        res.LowPart,
        res.HighPart);
    if (!NT_SUCCESS(status)) {
        Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "RESOURCE_HUB_CREATE_PATH_FROM_ID failed 0x%x", status);
        goto Exit;
    }

    WDF_OBJECT_ATTRIBUTES_INIT(&ObjectAttributes);
    ObjectAttributes.ParentObject = ctx->FxDevice;

    status = WdfIoTargetCreate(ctx->FxDevice, &ObjectAttributes, target);
    if (!NT_SUCCESS(status)) {
        Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "WdfIoTargetCreate failed 0x%x", status);
        goto Exit;
    }

    WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(&OpenParams, &ReadString, use);
    status = WdfIoTargetOpen(*target, &OpenParams);
    if (!NT_SUCCESS(status)) {
        Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "WdfIoTargetOpen failed 0x%x", status);
        goto Exit;
    }

Exit:
    Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "OpenIOTarget Exit");
    return status;
}

NTSTATUS
OnPrepareHardware(
    IN WDFDEVICE FxDevice,
    IN WDFCMRESLIST FxResourcesRaw,
    IN WDFCMRESLIST FxResourcesTranslated
)
/*++

  Routine Description:

    This routine is called by the PnP manager and supplies thie device instance
    with it's SPB resources (CmResourceTypeConnection) needed to find the I2C
    driver.

  Arguments:

    FxDevice - a handle to the framework device object
    FxResourcesRaw - list of translated hardware resources that
        the PnP manager has assigned to the device
    FxResourcesTranslated - list of raw hardware resources that
        the PnP manager has assigned to the device

  Return Value:

    NTSTATUS indicating sucess or failure

--*/
{
    NTSTATUS status;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR res;
    PDEVICE_EXTENSION devContext;
    ULONG resourceCount;
    ULONG i;
    LARGE_INTEGER delay;
    unsigned char value;

    UNREFERENCED_PARAMETER(FxResourcesRaw);

    //EventRegisterMicrosoft_WindowsPhone_TouchMiniDriver();

    status = STATUS_INSUFFICIENT_RESOURCES;
    devContext = GetDeviceContext(FxDevice);

    //
    // Get the resouce hub connection ID for our I2C driver
    //
    resourceCount = WdfCmResourceListGetCount(FxResourcesTranslated);

    for (i = 0; i < resourceCount; i++)
    {
        res = WdfCmResourceListGetDescriptor(FxResourcesTranslated, i);

        if (res->Type == CmResourceTypeConnection &&
            res->u.Connection.Class == CM_RESOURCE_CONNECTION_CLASS_SERIAL &&
            res->u.Connection.Type == CM_RESOURCE_CONNECTION_TYPE_SERIAL_I2C)
        {
            devContext->I2CContext.I2cResHubId.LowPart =
                res->u.Connection.IdLowPart;
            devContext->I2CContext.I2cResHubId.HighPart =
                res->u.Connection.IdHighPart;

            status = STATUS_SUCCESS;
        }

        if (res->Type == CmResourceTypeConnection &&
            res->u.Connection.Class == CM_RESOURCE_CONNECTION_CLASS_GPIO &&
            res->u.Connection.Type == CM_RESOURCE_CONNECTION_TYPE_GPIO_IO)
        {
            devContext->ResetGpioId.LowPart =
                res->u.Connection.IdLowPart;
            devContext->ResetGpioId.HighPart =
                res->u.Connection.IdHighPart;

            devContext->HasResetGpio = TRUE;
        }
    }

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error finding CmResourceTypeConnection resource - 0x%08lX",
            status);

        goto exit;
    }

    if (devContext->HasResetGpio)
    {
        status = OpenIOTarget(devContext, devContext->ResetGpioId, GENERIC_READ | GENERIC_WRITE, &devContext->ResetGpio);
        if (!NT_SUCCESS(status)) {
            Trace(TRACE_LEVEL_ERROR, TRACE_DRIVER, "OpenIOTarget failed for Reset GPIO 0x%x", status);
            goto continue;
        }

        Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Starting bring up sequence for the controller");

        Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Setting reset gpio pin to low");

        value = 0;
        SetGPIO(devContext->ResetGpio, &value);

        Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Waiting...");

        delay.QuadPart = -10 * TOUCH_POWER_RAIL_STABLE_TIME;
        KeDelayExecutionThread(KernelMode, TRUE, &delay);

        Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Setting reset gpio pin to high");

        value = 1;
        SetGPIO(devContext->ResetGpio, &value);

        Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Waiting...");

        delay.QuadPart = -10 * TOUCH_DELAY_TO_COMMUNICATE;
        KeDelayExecutionThread(KernelMode, TRUE, &delay);

        Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Done");
    }

continue:
    //
    // Initialize Spb so the driver can issue reads/writes
    //
    status = SpbTargetInitialize(FxDevice, &devContext->I2CContext);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error in Spb initialization - 0x%08lX", 
            status);

        goto exit;
    }

    //
    // Initialize Touch Power so the driver can issue power state changes
    //
    status = PowerInitialize(FxDevice);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error in touch power initialization - 0x%08lX",
            status);

        goto exit;
    }

    //
    // Get screen properties and populate context
    //
    TchGetScreenProperties(&devContext->ReportContext.Props);

    //
    // Prepare the hardware for touch scanning
    //
    status = TchAllocateContext(&devContext->TouchContext, FxDevice);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error allocating touch context - 0x%08lX", 
            status);

        goto exit;
    }

    //
    // Fetch controller settings from registry
    //
    status = TchRegistryGetControllerSettings(
        devContext->TouchContext,
        devContext->FxDevice);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error retrieving controller settings from registry - 0x%08lX",
            status);

        goto exit;
    }

    //
    // Configure the timer for continuous simulation on synaptics hardware that doesn't support it
    //
    status = ReportConfigureContinuousSimulationTimer(devContext->FxDevice);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error configuring continuous timer - 0x%08lX",
            status);

        goto exit;
    }

    //
    // Start the controller
    //
    status = TchStartDevice(devContext->TouchContext, &devContext->I2CContext);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error starting touch device - 0x%08lX",
            status);

        goto exit;
    }

    status = PoRegisterPowerSettingCallback(
        NULL,
        &GUID_ACDC_POWER_SOURCE,
        TchPowerSettingCallback,
        devContext,
        &devContext->PoFxPowerSettingCallbackHandle1
    );

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error registering power setting callback (1) - 0x%08lX",
            status);

        goto exit;
    }

    status = PoRegisterPowerSettingCallback(
        NULL,
        &GUID_CONSOLE_DISPLAY_STATE,
        TchPowerSettingCallback,
        devContext,
        &devContext->PoFxPowerSettingCallbackHandle2
    );

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error registering power setting callback (2) - 0x%08lX",
            status);

        goto exit;
    }

exit:

    return status;
}

NTSTATUS
OnReleaseHardware(
    IN WDFDEVICE FxDevice,
    IN WDFCMRESLIST FxResourcesTranslated
)
/*++

  Routine Description:

    This routine cleans up any resources provided.

  Arguments:

    FxDevice - a handle to the framework device object
    FxResourcesRaw - list of translated hardware resources that
        the PnP manager has assigned to the device
    FxResourcesTranslated - list of raw hardware resources that
        the PnP manager has assigned to the device

  Return Value:

    NTSTATUS indicating sucesss or failure

--*/
{
    NTSTATUS status;
    PDEVICE_EXTENSION devContext;

    UNREFERENCED_PARAMETER(FxResourcesTranslated);

    devContext = GetDeviceContext(FxDevice);

    status = PoUnregisterPowerSettingCallback(
        devContext->PoFxPowerSettingCallbackHandle1
    );

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error unregistering power setting callback - 0x%08lX",
            status);
    }

    status = PoUnregisterPowerSettingCallback(
        devContext->PoFxPowerSettingCallbackHandle2
    );

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error unregistering power setting callback - 0x%08lX",
            status);
    }

    status = TchStopDevice(devContext->TouchContext, &devContext->I2CContext);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_PNP,
            "Error stopping device - 0x%08lX",
            status);   
    }

    status = TchFreeContext(devContext->TouchContext);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_PNP,
            "Error freeing touch context - 0x%08lX",
            status);   
    }

    //EventUnregisterMicrosoft_WindowsPhone_TouchMiniDriver();

    //
    // DeInitialize Touch Power
    //
    status = PowerDeInitialize(FxDevice);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error in touch power deinitialization - 0x%08lX",
            status);
    }

    SpbTargetDeinitialize(FxDevice, &GetDeviceContext(FxDevice)->I2CContext);

    return status;
}

