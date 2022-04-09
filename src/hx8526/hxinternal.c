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
#include <hx8526\hxinternal.h>
#include <hxinternal.tmh>

NTSTATUS
Hx8526BuildFunctionsTable(
      IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);

      return STATUS_SUCCESS;
}

NTSTATUS
Hx8526ChangePage(
      IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN int DesiredPage
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);
      UNREFERENCED_PARAMETER(DesiredPage);

      return STATUS_SUCCESS;
}

NTSTATUS
Hx8526ConfigureFunctions(
      IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);

      return STATUS_SUCCESS;
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
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);
      UNREFERENCED_PARAMETER(Data);

      return STATUS_INSUFFICIENT_RESOURCES;
}

NTSTATUS
TchServiceObjectInterrupts(
      IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
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
      status = Hx8526GetObjectStatusFromController(
            ControllerContext,
            SpbContext,
            &data
      );

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
Hx8526ServiceInterrupts(
      IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
      IN SPB_CONTEXT* SpbContext,
      IN PREPORT_CONTEXT ReportContext
)
{
      NTSTATUS status = STATUS_SUCCESS;

      TchServiceObjectInterrupts(ControllerContext, SpbContext, ReportContext);

      return status;
}

NTSTATUS
Hx8526SetReportingFlagsF12(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
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
Hx8526ChangeChargerConnectedState(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
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
Hx8526ChangeSleepState(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext,
    IN UCHAR SleepState
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);
      UNREFERENCED_PARAMETER(SleepState);

      return STATUS_SUCCESS;
}

NTSTATUS
Hx8526GetFirmwareVersion(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);

      return STATUS_SUCCESS;
}

NTSTATUS
Hx8526CheckInterrupts(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
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
Hx8526ConfigureInterruptEnable(
    IN HX8526_CONTROLLER_CONTEXT* ControllerContext,
    IN SPB_CONTEXT* SpbContext
)
{
      UNREFERENCED_PARAMETER(SpbContext);
      UNREFERENCED_PARAMETER(ControllerContext);

      return STATUS_SUCCESS;
}