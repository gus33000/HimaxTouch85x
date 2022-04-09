/*++
      Copyright (c) Microsoft Corporation. All Rights Reserved.
      Sample code. Dealpoint ID #843729.

      Module Name:

            rmiinternal.c

      Abstract:

            Contains Synaptics initialization code

      Environment:

            Kernel mode

      Revision History:

--*/

#include <Cross Platform Shim\compat.h>
#include <spb.h>
#include <report.h>
#include <hx85x\hxinternal.h>
#include <hxinternal.tmh>

NTSTATUS
Hx85xBuildFunctionsTable(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);

      return STATUS_SUCCESS;
}

NTSTATUS
Hx85xChangePage(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN int DesiredPage
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);
      UNREFERENCED_PARAMETER(DesiredPage);

      return STATUS_SUCCESS;
}

NTSTATUS Hx8526ConfigureController(
      IN SPB_CONTEXT* SpbContext
)
{
      NTSTATUS status = STATUS_SUCCESS;
      LARGE_INTEGER delay;

      //
      // Power On IC
      //
      status = SpbWriteDataSynchronously(
            SpbContext, 
            HX85X_IC_POWER_ON_COMMAND, 
            sizeof(HX85X_IC_POWER_ON_COMMAND), 
            NULL, 
            0);

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not power on IC - 0x%08lX",
                status);
            goto exit;
      }

      delay.QuadPart = -10 * 120;
      KeDelayExecutionThread(KernelMode, TRUE, &delay);

      //
      // Power On MCU
      //
      status = SpbWriteDataSynchronously(
            SpbContext, 
            HX85X_MCU_POWER_ON_COMMAND, 
            sizeof(HX85X_MCU_POWER_ON_COMMAND), 
            NULL, 
            0);

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not power on MCU - 0x%08lX",
                status);
            goto exit;
      }

      delay.QuadPart = -10 * 10;
      KeDelayExecutionThread(KernelMode, TRUE, &delay);

      //
      // Power On Flash
      //
      status = SpbWriteDataSynchronously(
            SpbContext, 
            HX8526_FLASH_POWER_ON_COMMAND, 
            sizeof(HX8526_FLASH_POWER_ON_COMMAND), 
            NULL, 
            0);

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not power on Flash - 0x%08lX",
                status);
            goto exit;
      }

      delay.QuadPart = -10 * 10;
      KeDelayExecutionThread(KernelMode, TRUE, &delay);

      //
      // Fetch Flash
      //
      status = SpbWriteDataSynchronously(
            SpbContext, 
            HX8526_FETCH_FLASH_COMMAND, 
            sizeof(HX8526_FETCH_FLASH_COMMAND), 
            NULL, 
            0);

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not fetch Flash - 0x%08lX",
                status);
            goto exit;
      }

      delay.QuadPart = -10 * 10;
      KeDelayExecutionThread(KernelMode, TRUE, &delay);

exit:
      return status;
}

NTSTATUS Hx8520ConfigureController(
      IN SPB_CONTEXT* SpbContext
)
{
      NTSTATUS status = STATUS_SUCCESS;
      LARGE_INTEGER delay;

      //
      // Power On IC
      //
      status = SpbWriteDataSynchronously(
            SpbContext, 
            HX85X_IC_POWER_ON_COMMAND, 
            sizeof(HX85X_IC_POWER_ON_COMMAND), 
            NULL, 
            0);

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not power on IC - 0x%08lX",
                status);
            goto exit;
      }

      delay.QuadPart = -10 * 120;
      KeDelayExecutionThread(KernelMode, TRUE, &delay);

      //
      // Speed Mode
      //
      status = SpbWriteDataSynchronously(
            SpbContext, 
            HX8520_SPEED_MODE_COMMAND, 
            sizeof(HX8520_SPEED_MODE_COMMAND), 
            NULL, 
            0);

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not fetch Flash - 0x%08lX",
                status);
            goto exit;
      }

      delay.QuadPart = -10 * 10;
      KeDelayExecutionThread(KernelMode, TRUE, &delay);

      //
      // Power On MCU
      //
      status = SpbWriteDataSynchronously(
            SpbContext, 
            HX85X_MCU_POWER_ON_COMMAND, 
            sizeof(HX85X_MCU_POWER_ON_COMMAND), 
            NULL, 
            0);

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not power on MCU - 0x%08lX",
                status);
            goto exit;
      }

      delay.QuadPart = -10 * 10;
      KeDelayExecutionThread(KernelMode, TRUE, &delay);

      //
      // Power On Flash
      //
      status = SpbWriteDataSynchronously(
            SpbContext, 
            HX8520_FLASH_POWER_ON_COMMAND, 
            sizeof(HX8520_FLASH_POWER_ON_COMMAND), 
            NULL, 
            0);

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not power on Flash - 0x%08lX",
                status);
            goto exit;
      }

      delay.QuadPart = -10 * 10;
      KeDelayExecutionThread(KernelMode, TRUE, &delay);

exit:
      return status;
}

NTSTATUS
Hx85xConfigureFunctions(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext
)
{
      NTSTATUS status = STATUS_SUCCESS;
      BYTE DeviceID[3] = {0};
      BYTE SleepStatus = 0;

      //
      // Read Chip ID
      //
      status = SpbReadDataSynchronously(
          SpbContext,
          HX85X_GET_ID_COMMAND,
          sizeof(HX85X_GET_ID_COMMAND),
          DeviceID,
          sizeof(DeviceID));

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not get Himax Device ID - 0x%08lX",
                status);
            goto exit;
      }

      Trace(
          TRACE_LEVEL_INFORMATION,
          TRACE_INIT,
          "Himax Device ID - 0x%02X%02X%02X",
          DeviceID[0],
          DeviceID[1],
          DeviceID[2]);

      ControllerContext->ChipModel = (DeviceID[0] << 8) | DeviceID[1];

      Trace(
          TRACE_LEVEL_INFORMATION,
          TRACE_INIT,
          "Chip Model - %04lX",
          ControllerContext->ChipModel);

      if (ControllerContext->ChipModel != 0x8526 || ControllerContext->ChipModel != 0x8520 || ControllerContext->ChipModel != 0x8528)
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Unsupported Device. Exiting");

            return STATUS_NOT_SUPPORTED;
      }

      //
      // Read Sleep Status
      //
      status = SpbReadDataSynchronously(
            SpbContext, 
            HX85X_GET_SLEEP_COMMAND, 
            sizeof(HX85X_GET_SLEEP_COMMAND), 
            &SleepStatus, 
            sizeof(SleepStatus));

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not get Himax Sleep Status - 0x%08lX",
                status);
            goto exit;
      }

      if (SleepStatus != 0)
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Device is already initialized!");

            return STATUS_SUCCESS;
      }

      Trace(
          TRACE_LEVEL_INFORMATION,
          TRACE_INIT,
          "Initializing Digitizer IC... Please wait");

      if (ControllerContext->ChipModel == 0x8526)
      {
            status = Hx8526ConfigureController(SpbContext);
      }
      else
      {
            status = Hx8520ConfigureController(SpbContext);
      }

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not configure specific controller - 0x%08lX",
                status);
            goto exit;
      }

exit:
      return status;
}

NTSTATUS
Hx8526GetObjectStatusFromController(
      IN VOID* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN DETECTED_OBJECTS* Data
)
/*++

Routine Description:

      This routine reads raw touch messages from hardware. If there is
      no touch data available (if a non-touch interrupt fired), the
      function will not return success and no touch data was transferred.

Arguments:

      ControllerContext - Touch controller context
      SpbContext - A pointer to the current i2c context
      Data - A pointer to any returned F11 touch data

Return Value:

      NTSTATUS, where only success indicates data was returned

--*/
{
      NTSTATUS status;
      HX85X_CONTROLLER_CONTEXT* controller;

      int i, x, y;
      PHX8526_EVENT_DATA controllerData = NULL;
      controller = (HX85X_CONTROLLER_CONTEXT* )ControllerContext;

      controllerData = ExAllocatePoolWithTag(
          NonPagedPoolNx,
          sizeof(HX8526_EVENT_DATA),
          TOUCH_POOL_TAG_HX);

      if (controllerData == NULL)
      {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto exit;
      }

      //
      // Packets we need is determined by context
      //
      status = SpbReadDataSynchronously(
            SpbContext, 
            HX85X_GET_EVENT_COMMAND, 
            sizeof(HX85X_GET_EVENT_COMMAND), 
            controllerData, 
            sizeof(HX8526_EVENT_DATA));

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INTERRUPT,
                "Error reading finger status data - 0x%08lX",
                status);

            goto free_buffer;
      }

      if (controllerData->NumberOfTouchPoints == 0x0F)
      {
            controllerData->NumberOfTouchPoints = 0;
      }

      if (controllerData->NumberOfTouchPoints > HX8526_MAX_TOUCH_DATA)
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INTERRUPT,
                "Invalid Touch Point count. - %d",
                controllerData->NumberOfTouchPoints);

            goto free_buffer;
      }

      if (controllerData->ActivePointsMask == 0xFF)
      {
            controllerData->ActivePointsMask = 0;
      }

      BYTE X_MSB = 0;
      BYTE X_LSB = 0;
      BYTE Y_MSB = 0;
      BYTE Y_LSB = 0;

      for (i = 0; i < controllerData->NumberOfTouchPoints; i++)
      {
            X_MSB = controllerData->TouchData[i].PositionX_High;
            X_LSB = controllerData->TouchData[i].PositionX_Low;
            Y_MSB = controllerData->TouchData[i].PositionY_High;
            Y_LSB = controllerData->TouchData[i].PositionY_Low;

            if ((controllerData->ActivePointsMask & (1 << i)) != 0)
            {
                  Data->States[i] = OBJECT_STATE_FINGER_PRESENT_WITH_ACCURATE_POS;
            }
            else
            {
                  Data->States[i] = OBJECT_STATE_NOT_PRESENT;
            }

            x = (X_MSB << 8) | X_LSB;
            y = (Y_MSB << 8) | Y_LSB;

            Data->Positions[i].X = x;
            Data->Positions[i].Y = y;
      }

free_buffer:
      ExFreePoolWithTag(
          controllerData,
          TOUCH_POOL_TAG_HX);

exit:
      return status;
}

NTSTATUS
Hx8520GetObjectStatusFromController(
      IN VOID* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN DETECTED_OBJECTS* Data
)
/*++

Routine Description:

      This routine reads raw touch messages from hardware. If there is
      no touch data available (if a non-touch interrupt fired), the
      function will not return success and no touch data was transferred.

Arguments:

      ControllerContext - Touch controller context
      SpbContext - A pointer to the current i2c context
      Data - A pointer to any returned F11 touch data

Return Value:

      NTSTATUS, where only success indicates data was returned

--*/
{
      NTSTATUS status;
      HX85X_CONTROLLER_CONTEXT* controller;

      int i, x, y;
      PHX8520_EVENT_DATA controllerData = NULL;
      controller = (HX85X_CONTROLLER_CONTEXT* )ControllerContext;

      controllerData = ExAllocatePoolWithTag(
          NonPagedPoolNx,
          sizeof(HX8520_EVENT_DATA),
          TOUCH_POOL_TAG_HX);

      if (controllerData == NULL)
      {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto exit;
      }

      //
      // Packets we need is determined by context
      //
      status = SpbReadDataSynchronously(
            SpbContext, 
            HX85X_GET_EVENT_COMMAND, 
            sizeof(HX85X_GET_EVENT_COMMAND), 
            controllerData, 
            sizeof(HX8520_EVENT_DATA));

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INTERRUPT,
                "Error reading finger status data - 0x%08lX",
                status);

            goto free_buffer;
      }

      if (controllerData->NumberOfTouchPoints == 0x0F)
      {
            controllerData->NumberOfTouchPoints = 0;
      }

      if (controllerData->NumberOfTouchPoints > HX8520_MAX_TOUCH_DATA)
      {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INTERRUPT,
                "Invalid Touch Point count. - %d",
                controllerData->NumberOfTouchPoints);

            goto free_buffer;
      }

      if (controllerData->ActivePointsMask == 0xFF)
      {
            controllerData->ActivePointsMask = 0;
      }

      BYTE X_MSB = 0;
      BYTE X_LSB = 0;
      BYTE Y_MSB = 0;
      BYTE Y_LSB = 0;

      for (i = 0; i < controllerData->NumberOfTouchPoints; i++)
      {
            X_MSB = controllerData->TouchData[i].PositionX_High;
            X_LSB = controllerData->TouchData[i].PositionX_Low;
            Y_MSB = controllerData->TouchData[i].PositionY_High;
            Y_LSB = controllerData->TouchData[i].PositionY_Low;

            if ((controllerData->ActivePointsMask & (1 << i)) != 0)
            {
                  Data->States[i] = OBJECT_STATE_FINGER_PRESENT_WITH_ACCURATE_POS;
            }
            else
            {
                  Data->States[i] = OBJECT_STATE_NOT_PRESENT;
            }

            x = (X_MSB << 8) | X_LSB;
            y = (Y_MSB << 8) | Y_LSB;

            Data->Positions[i].X = x;
            Data->Positions[i].Y = y;
      }

free_buffer:
      ExFreePoolWithTag(
          controllerData,
          TOUCH_POOL_TAG_HX);

exit:
      return status;
}

NTSTATUS
TchServiceObjectInterrupts(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN PREPORT_CONTEXT ReportContext
)
{
      NTSTATUS status = STATUS_SUCCESS;
      DETECTED_OBJECTS data;

      RtlZeroMemory(&data, sizeof(data));

      //
      // See if new touch data is available
      //
      if (ControllerContext->ChipModel == 0x8526)
      {
            status = Hx8526GetObjectStatusFromController(
                  ControllerContext,
                  SpbContext,
                  &data);
      }
      else
      {
            status = Hx8520GetObjectStatusFromController(
                  ControllerContext,
                  SpbContext,
                  &data);
      }


      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_VERBOSE,
                TRACE_SAMPLES,
                "No object data to report - 0x%08lX",
                status);

            goto exit;
      }

      status = ReportObjects(
          ReportContext,
          data);

      if (!NT_SUCCESS(status))
      {
            Trace(
                TRACE_LEVEL_VERBOSE,
                TRACE_SAMPLES,
                "Error while reporting objects - 0x%08lX",
                status);

            goto exit;
      }

exit:
      return status;
}

NTSTATUS
Hx85xServiceInterrupts(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN PREPORT_CONTEXT ReportContext
)
{
      NTSTATUS status = STATUS_SUCCESS;

      TchServiceObjectInterrupts(ControllerContext, SpbContext, ReportContext);

      return status;
}

NTSTATUS
Hx85xSetReportingFlags(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN UCHAR NewMode,
    OUT UCHAR* OldMode
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);
      UNREFERENCED_PARAMETER(NewMode);
      UNREFERENCED_PARAMETER(OldMode);

      return STATUS_SUCCESS;
}

NTSTATUS
Hx85xChangeChargerConnectedState(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN UCHAR ChargerConnectedState
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);
      UNREFERENCED_PARAMETER(ChargerConnectedState);

      return STATUS_SUCCESS;
}

NTSTATUS
Hx85xChangeSleepState(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN UCHAR SleepState
)
{
      NTSTATUS status = STATUS_SUCCESS;
      LARGE_INTEGER delay;

      UNREFERENCED_PARAMETER(ControllerContext);

      if (SleepState == HX85X_F01_DEVICE_CONTROL_SLEEP_MODE_SLEEPING)
      {
            //
            // Sense ON
            //
            status = SpbWriteDataSynchronously(
                  SpbContext, 
                  HX85X_SENSE_OFF_COMMAND, 
                  sizeof(HX85X_SENSE_OFF_COMMAND), 
                  NULL, 
                  0);

            if (!NT_SUCCESS(status))
            {
                  Trace(
                      TRACE_LEVEL_ERROR,
                      TRACE_INIT,
                      "Could not turn off Sense - 0x%08lX",
                      status);
                  goto exit;
            }
      }
      else
      {
            //
            // Sense ON
            //
            status = SpbWriteDataSynchronously(
                  SpbContext, 
                  HX85X_SENSE_ON_COMMAND, 
                  sizeof(HX85X_SENSE_ON_COMMAND), 
                  NULL, 
                  0);

            if (!NT_SUCCESS(status))
            {
                  Trace(
                      TRACE_LEVEL_ERROR,
                      TRACE_INIT,
                      "Could not turn on Sense - 0x%08lX",
                      status);
                  goto exit;
            }

            delay.QuadPart = -10 * 100;
            KeDelayExecutionThread(KernelMode, TRUE, &delay);
      }

exit:
      return status;
}

NTSTATUS
Hx85xGetFirmwareVersion(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);

      return STATUS_SUCCESS;
}

NTSTATUS
Hx85xCheckInterrupts(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN ULONG* InterruptStatus
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);
      UNREFERENCED_PARAMETER(InterruptStatus);

      return STATUS_SUCCESS;
}

NTSTATUS
Hx85xConfigureInterruptEnable(
      IN HX85X_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);

      return STATUS_SUCCESS;
}