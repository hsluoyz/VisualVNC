/*++

Copyright (c) 1992-2000  Microsoft Corporation
 
Module Name:
 
    passthru.c

Abstract:

    Ndis Intermediate Miniport driver sample. This is a passthru driver.

Author:

Environment:


Revision History:


--*/


#include "precomp.h"
//#include "../CommonIO.h"
#pragma hdrstop

#include <ntstrsafe.h>
#include <strsafe.h>

#pragma NDIS_INIT_FUNCTION(DriverEntry)

#define MEM_TAG "mytag"

#define PROT_ICMP                               0x01 
#define PROT_TCP                                0x06 
#define PROT_UDP                                0x11 

NDIS_HANDLE         ProtHandle = NULL;
NDIS_HANDLE         DriverHandle = NULL;

///////////////////////////////////////////////原来的MediumArray
/*
NDIS_MEDIUM         MediumArray[4] =
                    {
                        NdisMedium802_3,    // Ethernet
                        NdisMedium802_5,    // Token-ring
                        NdisMediumFddi,     // Fddi
                        NdisMediumWan       // NDISWAN
                    };
*/

///////////////////////////////////////////////我修改的MediumArray
NDIS_MEDIUM         MediumArray[21] =
                    {
						NdisMedium802_3,
						NdisMedium802_5,
						NdisMediumFddi,
						NdisMediumWan,
						NdisMediumLocalTalk,
						NdisMediumDix,              // defined for convenience, not a real medium
						NdisMediumArcnetRaw,
						NdisMediumArcnet878_2,
						NdisMediumAtm,
						NdisMediumWirelessWan,
						NdisMediumIrda,
						NdisMediumBpc,
						NdisMediumCoWan,
						NdisMedium1394,
						NdisMediumInfiniBand,
					#if ((NTDDI_VERSION >= NTDDI_VISTA) || NDIS_SUPPORT_NDIS6)
						NdisMediumTunnel,
						NdisMediumNative802_11,
						NdisMediumLoopback,
					#endif // (NTDDI_VERSION >= NTDDI_VISTA)
						
					#if (NTDDI_VERSION >= NTDDI_WIN7)
						NdisMediumWiMAX,
						NdisMediumIP,
					#endif
						
						NdisMediumMax               // Not a real medium, defined as an upper-bound
                    };


NDIS_SPIN_LOCK		GlobalLock;

PADAPT				pAdaptList = NULL;
UINT				g_nMode = MODE_UNKNOWN; //MODE_CENTER | MODE_MARGIN | MODE_UNKNOWN
UINT				g_nStatus = STATUS_STOP; //STATUS_START | STATUS_STOP
LIST_ENTRY			pListCoreRules;
LIST_ENTRY			pListMappingRules;
UINT				g_nCoreRuleReady = 0;
UINT				g_nMappingRuleReady = 0;

#define				STR_BUFFER_LEN		40
CHAR				g_strBuffer1[STR_BUFFER_LEN];
CHAR				g_strBuffer2[STR_BUFFER_LEN];
CHAR				g_strBuffer3[STR_BUFFER_LEN];
CHAR				g_strBuffer4[STR_BUFFER_LEN];
CHAR				g_strBuffer5[STR_BUFFER_LEN];
CHAR				g_strBuffer6[STR_BUFFER_LEN];
CHAR				g_strBuffer7[STR_BUFFER_LEN];
CHAR				g_strBuffer8[STR_BUFFER_LEN];
CHAR				g_strBuffer9[STR_BUFFER_LEN];
CHAR				g_strBuffer10[STR_BUFFER_LEN];

UCHAR g_pMAC_FF[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
UCHAR g_pMAC_00[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

LONG				MiniportCount = 0;

NDIS_HANDLE			NdisWrapperHandle;

LIST_ENTRY			list_head;//过滤规则链表
//
// To support ioctls from user-mode:
//

#define LINKNAME_STRING     L"\\DosDevices\\VNCPassthru" //设备连接符(用户级)
#define NTDEVICE_STRING     L"\\Device\\VNCPassthru" //设备名称(内核级)

NDIS_HANDLE     NdisDeviceHandle = NULL;
PDEVICE_OBJECT  ControlDeviceObject = NULL;

enum _DEVICE_STATE
{
    PS_DEVICE_STATE_READY = 0,    // ready for create/delete
    PS_DEVICE_STATE_CREATING,    // create operation in progress
    PS_DEVICE_STATE_DELETING    // delete operation in progress
} ControlDeviceState = PS_DEVICE_STATE_READY;

// 定义一个函数指针变量，保存Ndis库中的AddDevice实现
AddDeviceFunc systemAddDevice = NULL; 

// 下面4个函数指针变量，用来保存系统对这个IRP的处理
DispatchFunc systemCreate = NULL;
DispatchFunc systemWrite = NULL;
DispatchFunc systemRead = NULL;
DispatchFunc systemDeviceControl = NULL;

NTSTATUS myCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS myWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS myRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS myDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS myAddDevice(IN PDRIVER_OBJECT  DriverObject,
					 IN PDEVICE_OBJECT  PhysicalDeviceObject);
//驱动入口
NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT        DriverObject,
    IN PUNICODE_STRING       RegistryPath
    )
/*++

Routine Description:

    First entry point to be called, when this driver is loaded.
    Register with NDIS as an intermediate driver.

Arguments:

    DriverObject - pointer to the system's driver object structure
        for this driver
    
    RegistryPath - system's registry path for this driver
    
Return Value:

    STATUS_SUCCESS if all initialization is successful, STATUS_XXX
    error code if not.

--*/
{
    NDIS_STATUS                        Status;
    NDIS_PROTOCOL_CHARACTERISTICS      PChars;
    NDIS_MINIPORT_CHARACTERISTICS      MChars;
    NDIS_STRING                        Name;

	//__asm int 3;
	//breakpoint

    Status = NDIS_STATUS_SUCCESS;
	
	InitializeListHead(&list_head);
	InitializeListHead(&pListCoreRules);
	InitializeListHead(&pListMappingRules);

    NdisAllocateSpinLock(&GlobalLock);

    NdisMInitializeWrapper(&NdisWrapperHandle, DriverObject, RegistryPath, NULL);

    do
    {
        //
        // Register the miniport with NDIS. Note that it is the miniport
        // which was started as a driver and not the protocol. Also the miniport
        // must be registered prior to the protocol since the protocol's BindAdapter
        // handler can be initiated anytime and when it is, it must be ready to
        // start driver instances.
        //

        NdisZeroMemory(&MChars, sizeof(NDIS_MINIPORT_CHARACTERISTICS));

        MChars.MajorNdisVersion = PASSTHRU_MAJOR_NDIS_VERSION;
        MChars.MinorNdisVersion = PASSTHRU_MINOR_NDIS_VERSION;

        MChars.InitializeHandler = MPInitialize;
        MChars.QueryInformationHandler = MPQueryInformation;
        MChars.SetInformationHandler = MPSetInformation;
        MChars.ResetHandler = NULL;
        MChars.TransferDataHandler = MPTransferData;
        MChars.HaltHandler = MPHalt;
#ifdef NDIS51_MINIPORT
        MChars.CancelSendPacketsHandler = MPCancelSendPackets;
        MChars.PnPEventNotifyHandler = MPDevicePnPEvent;
        MChars.AdapterShutdownHandler = MPAdapterShutdown;
#endif // NDIS51_MINIPORT

        //
        // We will disable the check for hang timeout so we do not
        // need a check for hang handler!
        //
        MChars.CheckForHangHandler = NULL;
        MChars.ReturnPacketHandler = MPReturnPacket;

        //
        // Either the Send or the SendPackets handler should be specified.
        // If SendPackets handler is specified, SendHandler is ignored
        //
        MChars.SendHandler = NULL;    // MPSend;
        MChars.SendPacketsHandler = MPSendPackets;

        Status = NdisIMRegisterLayeredMiniport(NdisWrapperHandle,
                                                  &MChars,
                                                  sizeof(MChars),
                                                  &DriverHandle);
        if (Status != NDIS_STATUS_SUCCESS)
        {
            break;
        }

#ifndef WIN9X
        NdisMRegisterUnloadHandler(NdisWrapperHandle, PtUnload);
#endif

        //
        // Now register the protocol.
        //
        NdisZeroMemory(&PChars, sizeof(NDIS_PROTOCOL_CHARACTERISTICS));
        PChars.MajorNdisVersion = PASSTHRU_PROT_MAJOR_NDIS_VERSION;
        PChars.MinorNdisVersion = PASSTHRU_PROT_MINOR_NDIS_VERSION;

        //
        // Make sure the protocol-name matches the service-name
        // (from the INF) under which this protocol is installed.
        // This is needed to ensure that NDIS can correctly determine
        // the binding and call us to bind to miniports below.
        //
        NdisInitUnicodeString(&Name, L"Passthru");    // Protocol name
        PChars.Name = Name;
        PChars.OpenAdapterCompleteHandler = PtOpenAdapterComplete;
        PChars.CloseAdapterCompleteHandler = PtCloseAdapterComplete;
        PChars.SendCompleteHandler = PtSendComplete;
        PChars.TransferDataCompleteHandler = PtTransferDataComplete;
    
        PChars.ResetCompleteHandler = PtResetComplete;
        PChars.RequestCompleteHandler = PtRequestComplete;
        PChars.ReceiveHandler = PtReceive;
        PChars.ReceiveCompleteHandler = PtReceiveComplete;
        PChars.StatusHandler = PtStatus;
        PChars.StatusCompleteHandler = PtStatusComplete;
        PChars.BindAdapterHandler = PtBindAdapter;
        PChars.UnbindAdapterHandler = PtUnbindAdapter;
        PChars.UnloadHandler = PtUnloadProtocol;

        PChars.ReceivePacketHandler = PtReceivePacket;
        PChars.PnPEventHandler= PtPNPHandler;

        NdisRegisterProtocol(&Status,
                             &ProtHandle,
                             &PChars,
                             sizeof(NDIS_PROTOCOL_CHARACTERISTICS));

        if (Status != NDIS_STATUS_SUCCESS)
        {
            NdisIMDeregisterLayeredMiniport(DriverHandle);
            break;
        }

        NdisIMAssociateMiniport(DriverHandle, ProtHandle);
    }
    while (FALSE);

    if (Status != NDIS_STATUS_SUCCESS)
    {
        NdisTerminateWrapper(NdisWrapperHandle, NULL);
    }

	// 这里实现Hook
	// 
	systemAddDevice = DriverObject->DriverExtension->AddDevice; //备份系统Device
	DriverObject->DriverExtension->AddDevice = myAddDevice;

	// Hook分发函数
	//
	systemCreate = DriverObject->MajorFunction[IRP_MJ_CREATE];
	DriverObject->MajorFunction[IRP_MJ_CREATE] = myCreate;

	systemWrite = DriverObject->MajorFunction[IRP_MJ_WRITE];
	DriverObject->MajorFunction[IRP_MJ_WRITE] = myWrite;

	systemRead = DriverObject->MajorFunction[IRP_MJ_READ];
	DriverObject->MajorFunction[IRP_MJ_READ] = myRead;

	systemDeviceControl = DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL];
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = myDeviceControl;

    return(Status);
}

//PassThru设备注册
NDIS_STATUS
PtRegisterDevice(
    VOID
    )
/*++

Routine Description:

    Register an ioctl interface - a device object to be used for this
    purpose is created by NDIS when we call NdisMRegisterDevice.

    This routine is called whenever a new miniport instance is
    initialized. However, we only create one global device object,
    when the first miniport instance is initialized. This routine
    handles potential race conditions with PtDeregisterDevice via
    the ControlDeviceState and MiniportCount variables.

    NOTE: do not call this from DriverEntry; it will prevent the driver
    from being unloaded (e.g. on uninstall).// 不知道是为什么，要考究一下!! MV_COMM_3_17

Arguments:

    None

Return Value:

    NDIS_STATUS_SUCCESS if we successfully register a device object.

--*/
{
    NDIS_STATUS            Status = NDIS_STATUS_SUCCESS;
    UNICODE_STRING         DeviceName;
    UNICODE_STRING         DeviceLinkUnicodeString;
    PDRIVER_DISPATCH       DispatchTable[IRP_MJ_MAXIMUM_FUNCTION+1];

    KdPrint(("==> Passthru PtRegisterDevice方法\n"));

    NdisAcquireSpinLock(&GlobalLock);

    ++MiniportCount;
    
    if (1 == MiniportCount)
    {
        ASSERT(ControlDeviceState != PS_DEVICE_STATE_CREATING);

        //
        // Another thread could be running PtDeregisterDevice on
        // behalf of another miniport instance. If so, wait for
        // it to exit.
        //
        while (ControlDeviceState != PS_DEVICE_STATE_READY)
        {
            NdisReleaseSpinLock(&GlobalLock);
            NdisMSleep(1);
            NdisAcquireSpinLock(&GlobalLock);
        }

        ControlDeviceState = PS_DEVICE_STATE_CREATING;

        NdisReleaseSpinLock(&GlobalLock);

    
        NdisZeroMemory(DispatchTable, (IRP_MJ_MAXIMUM_FUNCTION+1) * sizeof(PDRIVER_DISPATCH));

        DispatchTable[IRP_MJ_CREATE] = PtDispatch;
        DispatchTable[IRP_MJ_CLEANUP] = PtDispatch;
        DispatchTable[IRP_MJ_CLOSE] = PtDispatch;
        DispatchTable[IRP_MJ_DEVICE_CONTROL] = PtDispatch;
        

        NdisInitUnicodeString(&DeviceName, NTDEVICE_STRING);
        NdisInitUnicodeString(&DeviceLinkUnicodeString, LINKNAME_STRING);

        //
        // Create a device object and register our dispatch handlers
        //
        
        Status = NdisMRegisterDevice(
                    NdisWrapperHandle, 
                    &DeviceName,
                    &DeviceLinkUnicodeString,
                    &DispatchTable[0],
                    &ControlDeviceObject,
                    &NdisDeviceHandle
                    );

		KdPrint(("<== Passthru PtRegisterDevice方法,DeviceName: \"%wZ\", DeviceLink: \"%wZ\", Status: 0x%p\n", &DeviceName,
			&DeviceLinkUnicodeString, Status));

        NdisAcquireSpinLock(&GlobalLock);

        ControlDeviceState = PS_DEVICE_STATE_READY;
    }
	else
	{
		KdPrint(("<== Passthru PtRegisterDevice方法,不再产生设备对象\n"));
	}

	

    NdisReleaseSpinLock(&GlobalLock);

    return (Status);
}

//Passthru数据包分发
NTSTATUS
PtDispatch(
    IN PDEVICE_OBJECT    DeviceObject,
    IN PIRP              Irp
    )
/*++
Routine Description:

    Process IRPs sent to this device.

Arguments:

    DeviceObject - pointer to a device object
    Irp      - pointer to an I/O Request Packet

Return Value:

    NTSTATUS - STATUS_SUCCESS always - change this when adding
    real code to handle ioctls.

--*/
{
    PIO_STACK_LOCATION  irpStack;
    NTSTATUS            status = STATUS_SUCCESS;

    ULONG               FunctionCode;
    PUCHAR              pIoBuffer = NULL;	
    ULONG               uInputBufferLength;	
    ULONG               uOutputBufferLength;
	INT					nCopySize;

	PCore_Rule			pCoreRule;
	PMapping_Rule		pMappingRule;
	PDebug_Rule			pDebugRule;
	PNT_Core_Rule		pNtCoreRule;
	PNT_Mapping_Rule	pNtMappingRule;
	PNT_Debug_Rule		pNtDebugRule;
	CHAR				strEType[20];
	CHAR				strIType[20];

	PMYRULE				temp_rule;
	PFILTER				filter_entry;
	PFILTER				pFilter_entry;
	PLIST_ENTRY			pList_entry;

	UINT *pMode;
	UINT *pStatus;

    UNREFERENCED_PARAMETER(DeviceObject);
    
    KdPrint(("==>Pt Dispatch(PtDispatch)\n"));
    irpStack = IoGetCurrentIrpStackLocation(Irp);
//
// add 2010.06.20 begin
//

	// 得到缓冲区位置
    pIoBuffer = Irp->AssociatedIrp.SystemBuffer;
	// 得到输入驱动的缓冲长度
    uInputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
	// 得到输出驱动的缓冲长度
    uOutputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
	// 得到功能码
    FunctionCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

	switch (FunctionCode)
    {
	case IOCTL_VNCIO_SIMPLE_READ:
		nCopySize = uOutputBufferLength < sizeof("hello from vncpassthru") ? uOutputBufferLength : sizeof("hello from vncpassthru");
		NdisMoveMemory(pIoBuffer, "hello from vncpassthru", nCopySize);
		Irp->IoStatus.Information = nCopySize;
		KdPrint(("I'm VNCPassthru NDIS Driver! I'm alive!\n"));
		break;

	case IOCTL_VNCIO_CHANGE_MODE:
		pMode = (UINT*) pIoBuffer;
		g_nMode = *pMode;

		*((UINT*) pIoBuffer) = 1;
		Irp->IoStatus.Information = 4;
		if (g_nMode == MODE_CENTER)
		{
			KdPrint(("Driver Mode is changed to [Center]!\n"));
		}
		else if (g_nMode == MODE_MARGIN)
		{
			KdPrint(("Driver Mode is changed to [Margin]!\n"));
		}
		else
		{
			KdPrint(("Driver Mode is changed to [Unknown]!\n"));
		}
		break;
		
	case IOCTL_VNCIO_START_STOP:
		pStatus = (UINT*) pIoBuffer;
		g_nStatus = *pStatus;
		
		*((UINT*) pIoBuffer) = 1;
		Irp->IoStatus.Information = 4;
		if (g_nStatus == STATUS_START)
		{
			KdPrint(("Driver Status is changed to [Start]!\n"));
		}
		else
		{
			KdPrint(("Driver Status is changed to [Stop]!\n"));
		}
		break;

	case IOCTL_VNCIO_WRITE_CORE_RULE:
		pCoreRule = (PCore_Rule) pIoBuffer;
		pNtCoreRule = (PNT_Core_Rule) ExAllocatePoolWithTag(PagedPool, sizeof(NT_Core_Rule), MEM_TAG);
		if (pNtCoreRule == NULL)
		{
			KdPrint(("内存不足，分配失败!\n"));
			*((UINT*) pIoBuffer) = 0;
			Irp->IoStatus.Information = 4;
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		/*
		KdPrint(("[Core Rule]  pNtCoreRule: 0x%p\n", pNtCoreRule));
		KdPrint(("[Core Rule]  8: 0x%p\n", 8));
		KdPrint(("[Core Rule]  sizeof(LIST_ENTRY): 0x%p\n", sizeof(LIST_ENTRY)));
		KdPrint(("[Core Rule]  pNtCoreRule + 1: 0x%p\n", pNtCoreRule + 1));
		KdPrint(("[Core Rule]  pNtCoreRule + 0x1: 0x%p\n", pNtCoreRule + 0x1));
		KdPrint(("[Core Rule]  pNtCoreRule + 8: 0x%p\n", pNtCoreRule + 8));
		KdPrint(("[Core Rule]  pNtCoreRule + sizeof(LIST_ENTRY): 0x%p\n", pNtCoreRule + sizeof(LIST_ENTRY)));
		*/
		RtlZeroMemory(pNtCoreRule, NT_CORE_RULE_LEN);
		RtlMoveMemory(pNtCoreRule->cMAC, pCoreRule, CORE_RULE_LEN);
		KdPrint(("[Core Rule]  CMAC: %s, CIP: %s\n",
			stringFromMAC1(pNtCoreRule->cMAC), stringFromNetworkIP1(pNtCoreRule->cIP)));
		KdPrint(("is written to address: %p!\n", pNtCoreRule));

		if (g_nMode == MODE_CENTER)
		{
			pNtCoreRule->cAdapt = getAdapterByMAC(pNtCoreRule->cMAC);
		}
		InsertHeadList(&pListCoreRules, &pNtCoreRule->listEntry);
		g_nCoreRuleReady = 1;

		*((UINT*) pIoBuffer) = 1;
		Irp->IoStatus.Information = 4;
		break;

	case IOCTL_VNCIO_WRITE_MAPPING_RULE:
		pMappingRule = (PMapping_Rule) pIoBuffer;
		pNtMappingRule = (PNT_Mapping_Rule) ExAllocatePoolWithTag(PagedPool, sizeof(NT_Mapping_Rule), MEM_TAG);
		if (pNtMappingRule == NULL)
		{
			KdPrint(("内存不足，分配失败!\n"));
			*((UINT*) pIoBuffer) = 0;
			Irp->IoStatus.Information = 4;
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		RtlZeroMemory(pNtMappingRule, NT_MAPPING_RULE_LEN);
		RtlMoveMemory(pNtMappingRule->eMAC, pMappingRule, MAPPING_RULE_LEN);
		KdPrint(("[Mapping Rule]  VIP: %s, PIP: %s, IMAC: %s, EMAC: %s\n",
			stringFromNetworkIP1(pNtMappingRule->vIP), stringFromNetworkIP2(pNtMappingRule->pIP),
			stringFromMAC1(pNtMappingRule->iMAC), stringFromMAC2(pNtMappingRule->eMAC)));
		KdPrint(("is written to address: %p!\n", pNtMappingRule));

		if (g_nMode == MODE_CENTER)
		{
			pNtMappingRule->iAdapt = getAdapterByMAC(pNtMappingRule->iMAC);
		}
		InsertHeadList(&pListMappingRules, &pNtMappingRule->listEntry);
		g_nMappingRuleReady = 1;

		*((UINT*) pIoBuffer) = 1;
		Irp->IoStatus.Information = 4;
		break;

	case IOCTL_VNCIO_WRITE_DEBUG_RULE:
		pDebugRule = (PDebug_Rule) pIoBuffer;
		pNtDebugRule = (PNT_Debug_Rule) ExAllocatePoolWithTag(PagedPool, sizeof(NT_Debug_Rule), MEM_TAG);
		if (pNtDebugRule == NULL)
		{
			KdPrint(("内存不足，分配失败!\n"));
			*((UINT*) pIoBuffer) = 0;
			Irp->IoStatus.Information = 4;
			return STATUS_INSUFFICIENT_RESOURCES;	
		}

		RtlZeroMemory(pNtDebugRule, NT_DEBUG_RULE_LEN);
		RtlMoveMemory(pNtDebugRule, pDebugRule, DEBUG_RULE_LEN);
		
		switch (pNtDebugRule->eType)
		{
		case TYPE_IP:
			RtlStringCchCopyA(strEType, 20, "IP");
			break;
		case TYPE_ARP_REQUEST:
			RtlStringCchCopyA(strEType, 20, "ARP Request");
			break;
		case TYPE_ARP_REPLY:
			RtlStringCchCopyA(strEType, 20, "ARP Reply");
			break;
		}

		switch (pNtDebugRule->iType)
		{
		case TYPE_IP:
			RtlStringCchCopyA(strIType, 20, "IP");
			break;
		case TYPE_ARP_REQUEST:
			RtlStringCchCopyA(strIType, 20, "ARP Request");
			break;
		case TYPE_ARP_REPLY:
			RtlStringCchCopyA(strIType, 20, "ARP Reply");
			break;
		case TYPE_NONE:
			RtlStringCchCopyA(strIType, 20, "None");
			break;
		}

		KdPrint(("[Debug Rule]  eType: %s, iType: %s, eDstMAC: %s, eSrcMAC: %s, eDstIP: %s, eSrcIP: %s, iDstMAC: %s, iSrcMAC: %s, iDstIP: %s, iSrcIP: %s\n",
			strEType, strIType,
			stringFromMAC1(pNtDebugRule->eDstMAC), stringFromMAC2(pNtDebugRule->eSrcMAC),
			stringFromNetworkIP1(pNtDebugRule->eDstIP), stringFromNetworkIP2(pNtDebugRule->eSrcIP),
			stringFromMAC3(pNtDebugRule->iDstMAC), stringFromMAC4(pNtDebugRule->iSrcMAC),
			stringFromNetworkIP3(pNtDebugRule->iDstIP), stringFromNetworkIP4(pNtDebugRule->iSrcIP));
		KdPrint(("[%2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X]\n", (PCHAR) (pNtDebugRule->content),
			(PCHAR) (pNtDebugRule->content + 1), (PCHAR) (pNtDebugRule->content + 2), (PCHAR) (pNtDebugRule->content + 3),
			(PCHAR) (pNtDebugRule->content + 4), (PCHAR) (pNtDebugRule->content + 5), (PCHAR) (pNtDebugRule->content + 6),
			(PCHAR) (pNtDebugRule->content + 7), (PCHAR) (pNtDebugRule->content + 8), (PCHAR) (pNtDebugRule->content + 9),
			(PCHAR) (pNtDebugRule->content + 10), (PCHAR) (pNtDebugRule->content + 11), (PCHAR) (pNtDebugRule->content + 12),
			(PCHAR) (pNtDebugRule->content + 13), (PCHAR) (pNtDebugRule->content + 14), (PCHAR) (pNtDebugRule->content + 15),
			(PCHAR) (pNtDebugRule->content + 16), (PCHAR) (pNtDebugRule->content + 17), (PCHAR) (pNtDebugRule->content + 18),
			(PCHAR) (pNtDebugRule->content + 19), (PCHAR) (pNtDebugRule->content + 20), (PCHAR) (pNtDebugRule->content + 21),
			(PCHAR) (pNtDebugRule->content + 22), (PCHAR) (pNtDebugRule->content + 23), (PCHAR) (pNtDebugRule->content + 24),
			(PCHAR) (pNtDebugRule->content + 25), (PCHAR) (pNtDebugRule->content + 26), (PCHAR) (pNtDebugRule->content + 27),
			(PCHAR) (pNtDebugRule->content + 28), (PCHAR) (pNtDebugRule->content + 29)));
		KdPrint(("is written to address: %p!\n", pNtDebugRule)));

		sendDebugPacket(pNtDebugRule);
		ExFreePool(pNtDebugRule);

		*((UINT*) pIoBuffer) = 1;
		Irp->IoStatus.Information = 4;
		break;

	case IOCTL_VNCIO_CLEAR_RULES:
		while (!IsListEmpty(&pListCoreRules))
		{
			pCoreRule= (PNT_Core_Rule) RemoveTailList(&pListCoreRules);
			ExFreePool(pCoreRule);
		}
		g_nCoreRuleReady = 0;

		while (!IsListEmpty(&pListMappingRules))
		{
			pMappingRule= (PNT_Mapping_Rule) RemoveTailList(&pListMappingRules);
			ExFreePool(pMappingRule);
		}
		g_nMappingRuleReady = 0;

		*((UINT*) pIoBuffer) = 1;
		Irp->IoStatus.Information = 4;
		KdPrint(("All Rules cleared!\n"));
		break;
	/*
	case IOCTL_VNCIO_SIMPLE_WRITE:
		
		temp_rule=(PMYRULE) pIoBuffer;

		filter_entry=(PFILTER)ExAllocatePoolWithTag(PagedPool, sizeof(FILTER), MEM_TAG);
	
		if (filter_entry == NULL)
		{
			KdPrint(("内存不足，分配失败!\n"));
			return STATUS_INSUFFICIENT_RESOURCES;	
		}
		
		switch (*(temp_rule->protocol)){
		case 'T':
			filter_entry->protocol=PROT_TCP;
			break;
		case 'U':
			filter_entry->protocol=PROT_UDP;
			break;
		case 'I':
			filter_entry->protocol=PROT_ICMP;
			break;
		}

		//filter_entry->myrule.protocol=temp_rule->protocol;
		//RtlCopyMemory(filter_entry->myrule.protocol,temp_rule->protocol,sizeof(*temp_rule->protocol));
		filter_entry->ip_start=temp_rule->ip_start;
		filter_entry->ip_end=temp_rule->ip_end;
		InsertHeadList(&list_head, &filter_entry->list_entry);
		
		//pList_entry=list_head.Flink;
		
		for(pList_entry=list_head.Flink; pList_entry!=&list_head.Flink; pList_entry=pList_entry->Flink){ 
			
			pFilter_entry=CONTAINING_RECORD(pList_entry, FILTER, list_entry); 
			
			switch (pFilter_entry->protocol){
			case PROT_TCP:
				KdPrint(("写入过滤规则数据:%s,%u,%u\n","TCP", pFilter_entry->ip_start,pFilter_entry->ip_end));
				break;
			case PROT_UDP:
				KdPrint(("写入过滤规则数据:%s,%u,%u\n", "UDP", pFilter_entry->ip_start,pFilter_entry->ip_end));
				break;
			case PROT_ICMP:
				KdPrint(("写入过滤规则数据:%s,%u,%u\n","ICMP", pFilter_entry->ip_start,pFilter_entry->ip_end));
				break;
			}
		}

//		while(pList_entry!=&list_head){
//			pFilter_entry=(PFILTER)pList_entry;
//			KdPrint(("收到写入的数据：%s,%u,%u\n", pFilter_entry->myrule.protocol, pFilter_entry->myrule.ip_start,
//		pFilter_entry->myrule.ip_end));
//			pList_entry=pList_entry->Flink;
//		}
		//KdPrint(("收到写入的数据：%s,%u,%u\n", filter_entry->myrule.protocol, filter_entry->myrule.ip_start,
//		filter_entry->myrule.ip_end));

		break;
		*/
	}
	

//
// add 2010.06.20 end
//

    switch (irpStack->MajorFunction)
    {
        case IRP_MJ_CREATE:
            break;
            
        case IRP_MJ_CLEANUP:
            break;
            
        case IRP_MJ_CLOSE:
            break;        
            
        case IRP_MJ_DEVICE_CONTROL:
            //
            // Add code here to handle ioctl commands sent to passthru.
            //
            break;        
        default:
            break;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    KdPrint(("<== Pt Dispatch(PtDispatch)\n"));

    return status;
} 

//Passthru设备注销
NDIS_STATUS
PtDeregisterDevice(
    VOID
    )
/*++

Routine Description:

    Deregister the ioctl interface. This is called whenever a miniport
    instance is halted. When the last miniport instance is halted, we
    request NDIS to delete the device object

Arguments:

    NdisDeviceHandle - Handle returned by NdisMRegisterDevice

Return Value:

    NDIS_STATUS_SUCCESS if everything worked ok

--*/
{
    NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

    DBGPRINT(("==>PassthruDeregisterDevice\n"));

    NdisAcquireSpinLock(&GlobalLock);

    ASSERT(MiniportCount > 0);

    --MiniportCount;
    
    if (0 == MiniportCount)
    {
        //
        // All miniport instances have been halted. Deregister
        // the control device.
        //

        ASSERT(ControlDeviceState == PS_DEVICE_STATE_READY);

        //
        // Block PtRegisterDevice() while we release the control
        // device lock and deregister the device.
        // 
        ControlDeviceState = PS_DEVICE_STATE_DELETING;

        NdisReleaseSpinLock(&GlobalLock);

        if (NdisDeviceHandle != NULL)
        {
            Status = NdisMDeregisterDevice(NdisDeviceHandle);
            NdisDeviceHandle = NULL;
        }

        NdisAcquireSpinLock(&GlobalLock);
        ControlDeviceState = PS_DEVICE_STATE_READY;
    }

    NdisReleaseSpinLock(&GlobalLock);

    DBGPRINT(("<== PassthruDeregisterDevice: %x\n", Status));
    return Status;
    
}

VOID
PtUnload(
    IN PDRIVER_OBJECT        DriverObject
    )
//
// PassThru driver unload function
//
{
	PLIST_ENTRY p;
	PFILTER pFilter;
	
	
//	while(!IsListEmpty(&list_head)){
//		pFilter=(PFILTER)RemoveTailList(&list_head);
//		ExFreePool(pFilter);
//	}
	
	UNREFERENCED_PARAMETER(DriverObject);
    
    DBGPRINT(("PtUnload: entered\n"));
    
    PtUnloadProtocol();
    
    NdisIMDeregisterLayeredMiniport(DriverHandle);
    
    NdisFreeSpinLock(&GlobalLock);
	
	for(p =list_head.Flink; p!=&list_head.Flink; p=p->Flink){ 
		pFilter=(PFILTER)RemoveTailList(&list_head);
		ExFreePool(pFilter);
	}

    DBGPRINT(("PtUnload: done!\n"));
}

NDIS_STATUS getMemoryCopyFromPacket(PNDIS_PACKET pPacket, PUCHAR *ppMemory, UINT *pnMemLen)
{
	NDIS_STATUS nStatus;
	PNDIS_BUFFER pBuffer;
	UINT nTotalPacketLength = 0;
	UINT nCopySize = 0;
	UINT nDataOffset = 0;
	UINT nPhysicalBufferCount;
	UINT nBufferCount;
	PUCHAR pCacheMemory = NULL;
	PUCHAR pMemory = NULL;
	
	nStatus = NdisAllocateMemoryWithTag(&pCacheMemory, 2048, TAG); //分配带标签的内存

	if (nStatus != NDIS_STATUS_SUCCESS)
	{
		return nStatus;
	}
	
	NdisZeroMemory(pCacheMemory, 2048); //分配2KB内存
	
	//找到第一个Ndis_Buffer。然后通过通过NdisGetNextBuffer来获得后续的NDIS_BUFFER。
	//如果只是找第一个节点，更快且方便的方法是调用NdisGetFirstBufferFromPacket。
	NdisQueryPacket(pPacket,  // NDIS_PACKET        
		&nPhysicalBufferCount,// 内存中的物理块数
		&nBufferCount,		 // 多少个NDIS_BUFFER包
		&pBuffer,         // 将返回第一个包
		&nTotalPacketLength	 // 总共的包数据长度
		);
	
	while (TRUE)
	{
		// 取得Ndis_Buffer中存储缓冲区的虚拟地址。
		// 这个函数的另一个版本是NdisQueryBuffer。
		// 后者在系统资源低或者甚至耗尽的时候，会产生Bug Check，导致蓝屏。
		NdisQueryBufferSafe(pBuffer,
			&pMemory,// 缓冲区地址
			&nCopySize, // 缓冲区大小
			NormalPagePriority
			);
		
		// 如果tembuffer为NULL，说明当前系统资源匮乏。
		if (pMemory != NULL)
		{
			NdisMoveMemory(pCacheMemory + nDataOffset , pMemory, nCopySize) ;			
			nDataOffset += nCopySize;
		}
		else
		{
			nStatus = NDIS_STATUS_FAILURE;
			return nStatus;
		}
		
		// 获得下一个NDIS_BUFFER。
		// 如果得到的是一个NULL指针，说明已经到了链式缓冲区的末尾，我们的循环应该结束了。
		NdisGetNextBuffer(pBuffer, &pBuffer);
		
		if (pBuffer == NULL)
			break;
	}
	
	// 取得数据包内容后，下面将对其内容进行过滤。
	(*ppMemory) = pCacheMemory;
	(*pnMemLen) = nDataOffset;
	return FILTER_STATUS_PASS;
}


void freeMemory(PCHAR pBuffer)
{
	NdisFreeMemory(pBuffer, 0, 0);
}


NDIS_STATUS sendMemory(PADAPT pAdapt, PUCHAR pMemory, ULONG nMemLen)
{
    NDIS_STATUS     nStatus = NDIS_STATUS_SUCCESS;
    PNDIS_PACKET    pPacket = NULL;
    PNDIS_BUFFER    pBuffer = NULL;
	
    PSEND_RSVD      sendRsvd = NULL;
    //NDIS_PHYSICAL_ADDRESS highestAcceptableAddress;
    //highestAcceptableAddress.QuadPart = -1;
    
    KdPrint(("passthru sendMemory,正在发包\n"));
	
    NdisAllocatePacket(&nStatus, &pPacket, pAdapt->SendPacketPoolHandle);
    if (nStatus != NDIS_STATUS_SUCCESS)
    {
        return nStatus;
    }
	
    NdisAllocateBuffer(&nStatus, &pBuffer, pAdapt->SendPacketPoolHandle, pMemory, nMemLen);
	
    if (nStatus != NDIS_STATUS_SUCCESS)
    {
        NdisFreePacket(pPacket);
        return nStatus;
    }
	
    NdisChainBufferAtFront(pPacket, pBuffer);
    sendRsvd = (PSEND_RSVD) (pPacket->ProtocolReserved);
    sendRsvd->OriginalPkt = 0xfcfcfcfc; //标记自己
	
	
    pPacket->Private.Head->Next = NULL;
    pPacket->Private.Tail = NULL;
	
    NdisSetPacketFlags(pPacket, NDIS_FLAGS_DONT_LOOPBACK);
  
    NdisSend(&nStatus, pAdapt->BindingHandle, pPacket);
    
	
    if (nStatus != STATUS_PENDING)
    {
        NdisUnchainBufferAtFront(pPacket, &pBuffer);
        //NdisQueryBufferSafe(pBuffer, &pMemory, &nMemLen, NormalPagePriority);
		//NdisFreeMemory(pMemory);
        NdisFreeBuffer(pBuffer);
        NdisFreePacket(pPacket);
    }
	
    return nStatus;
}

PNDIS_PACKET addPacketWithMemory_ReceivePool(PADAPT pAdapt, PNDIS_PACKET pPacket, PUCHAR pMemory, ULONG nMemLen)
{
    NDIS_STATUS     nStatus = NDIS_STATUS_SUCCESS;
    //PNDIS_PACKET    ndisPacket = NULL;
    PNDIS_BUFFER    pBuffer = NULL;
	
    //PSEND_RSVD      sendRsvd = NULL;
    //NDIS_PHYSICAL_ADDRESS highestAcceptableAddress;
    //highestAcceptableAddress.QuadPart = -1;
    
    KdPrint(("passthru addPacketWithMemory_ReceivePool，正在组包\n"));
	
    NdisAllocateBuffer(&nStatus, &pBuffer, pAdapt->RecvPacketPoolHandle, pMemory, nMemLen);
	
    if (nStatus != NDIS_STATUS_SUCCESS)
    {
        NdisFreePacket(pPacket);
        return NULL;
    }
	
    NdisChainBufferAtFront(pPacket, pBuffer);
    //sendRsvd = (PSEND_RSVD)(ndisPacket->ProtocolReserved);
    //sendRsvd->OriginalPkt = 0xfcfcfcfc; //标记自己
	
	
    pPacket->Private.Head->Next = NULL;
    pPacket->Private.Tail = NULL;
	
    NdisSetPacketFlags(pPacket, NDIS_FLAGS_DONT_LOOPBACK);
	
	return pPacket;
}

PNDIS_PACKET createPacketFromMemory_ReceivePool(PADAPT pAdapt, PUCHAR pMemory, ULONG nMemLen)
{
    NDIS_STATUS     nStatus = NDIS_STATUS_SUCCESS;
    PNDIS_PACKET    pPacket = NULL;
    PNDIS_BUFFER    pBuffer = NULL;
	
    //PSEND_RSVD      sendRsvd = NULL;
    //NDIS_PHYSICAL_ADDRESS highestAcceptableAddress;
    //highestAcceptableAddress.QuadPart = -1;
    
    KdPrint(("passthru createPacketFromMemory_ReceivePool，正在组包\n"));
	
    NdisAllocatePacket(&nStatus, &pPacket, pAdapt->RecvPacketPoolHandle);
    if (nStatus != NDIS_STATUS_SUCCESS)
    {
        return NULL;
    }
	
    NdisAllocateBuffer(&nStatus, &pBuffer, pAdapt->RecvPacketPoolHandle, pMemory, nMemLen);
	
    if (nStatus != NDIS_STATUS_SUCCESS)
    {
        NdisFreePacket(pPacket);
        return NULL;
    }
	
    NdisChainBufferAtFront(pPacket, pBuffer);
    //sendRsvd = (PSEND_RSVD)(ndisPacket->ProtocolReserved);
    //sendRsvd->OriginalPkt = 0xfcfcfcfc; //标记自己
	
	
    pPacket->Private.Head->Next = NULL;
    pPacket->Private.Tail = NULL;
	
    NdisSetPacketFlags(pPacket, NDIS_FLAGS_DONT_LOOPBACK);
	
	return pPacket;
}

PNDIS_PACKET createPacketFromMemory_SendPool(PADAPT pAdapt, PUCHAR pMemory, ULONG nMemLen)
{
    NDIS_STATUS     nStatus = NDIS_STATUS_SUCCESS;
    PNDIS_PACKET    pPacket = NULL;
    PNDIS_BUFFER    pBuffer = NULL;
	
    //PSEND_RSVD      sendRsvd = NULL;
    //NDIS_PHYSICAL_ADDRESS highestAcceptableAddress;
    //highestAcceptableAddress.QuadPart = -1;
    
    KdPrint(("passthru createPacketFromMemory_SendPool，正在组包\n"));
	
    NdisAllocatePacket(&nStatus, &pPacket, pAdapt->SendPacketPoolHandle);
    if (nStatus != NDIS_STATUS_SUCCESS)
    {
        return NULL;
    }
	
    NdisAllocateBuffer(&nStatus, &pBuffer, pAdapt->SendPacketPoolHandle, pMemory, nMemLen);
	
    if (nStatus != NDIS_STATUS_SUCCESS)
    {
        NdisFreePacket(pPacket);
        return NULL;
    }
	
    NdisChainBufferAtFront(pPacket, pBuffer);
    //sendRsvd = (PSEND_RSVD)(ndisPacket->ProtocolReserved);
    //sendRsvd->OriginalPkt = 0xfcfcfcfc; //标记自己
	
	
    pPacket->Private.Head->Next = NULL;
    pPacket->Private.Tail = NULL;
	
    NdisSetPacketFlags(pPacket, NDIS_FLAGS_DONT_LOOPBACK);
	
	return pPacket;
}

void freePacketBufferAndMemory(PNDIS_PACKET pPacket)
{
	NDIS_STATUS nStatus;
	PNDIS_BUFFER pBuffer;
	PNDIS_BUFFER pOldBuffer;
	UINT nTotalPacketLength = 0;
	UINT nPhysicalBufferCount;
	UINT nBufferCount;
	PUCHAR pMemory = NULL;
	UINT nCopySize = 0;

	if (!pPacket)
	{
		nStatus = NDIS_STATUS_FAILURE;
		return nStatus;
	}

	NdisQueryPacket(pPacket,  // NDIS_PACKET        
		&nPhysicalBufferCount,// 内存中的物理块数
		&nBufferCount,		 // 多少个NDIS_BUFFER包
		&pBuffer,         // 将返回第一个包
		&nTotalPacketLength	 // 总共的包数据长度
		);

	while (TRUE)
	{
		NdisQueryBufferSafe(pBuffer,
			&pMemory,// 缓冲区地址
			&nCopySize, // 缓冲区大小
			NormalPagePriority
			);
		
		// 如果tembuffer为NULL，说明当前系统资源匮乏。
		if (pMemory == NULL)
		{
			nStatus = NDIS_STATUS_FAILURE;
			return nStatus;
		}
		// 获得下一个NDIS_BUFFER。
		// 如果得到的是一个NULL指针，说明已经到了链式缓冲区的末尾，我们的循环应该结束了。
		pOldBuffer = pBuffer;
		NdisGetNextBuffer(pBuffer, &pBuffer);
		NdisUnchainBufferAtFront(pPacket, &pOldBuffer);
		NdisFreeBuffer(pOldBuffer);
		NdisFreeMemory(pMemory, 0, 0);
		
		if (pBuffer == NULL)
			break;
	}

	NdisFreePacket(pPacket);
}

void dprFreePacketBufferAndMemory(PNDIS_PACKET pPacket)
{
	NDIS_STATUS nStatus;
	PNDIS_BUFFER pBuffer;
	PNDIS_BUFFER pOldBuffer;
	UINT nTotalPacketLength = 0;
	UINT nPhysicalBufferCount;
	UINT nBufferCount;
	PUCHAR pMemory = NULL;
	UINT nCopySize = 0;
	
	if (!pPacket)
	{
		nStatus = NDIS_STATUS_FAILURE;
		return nStatus;
	}
	
	NdisQueryPacket(pPacket,  // NDIS_PACKET        
		&nPhysicalBufferCount,// 内存中的物理块数
		&nBufferCount,		 // 多少个NDIS_BUFFER包
		&pBuffer,         // 将返回第一个包
		&nTotalPacketLength	 // 总共的包数据长度
		);
	
	while (TRUE)
	{
		NdisQueryBufferSafe(pBuffer,
			&pMemory,// 缓冲区地址
			&nCopySize, // 缓冲区大小
			NormalPagePriority
			);
		
		// 如果tembuffer为NULL，说明当前系统资源匮乏。
		if (pMemory == NULL)
		{
			nStatus = NDIS_STATUS_FAILURE;
			return nStatus;
		}
		// 获得下一个NDIS_BUFFER。
		// 如果得到的是一个NULL指针，说明已经到了链式缓冲区的末尾，我们的循环应该结束了。
		pOldBuffer = pBuffer;
		NdisGetNextBuffer(pBuffer, &pBuffer);
		NdisUnchainBufferAtFront(pPacket, &pOldBuffer);
		NdisFreeBuffer(pOldBuffer);
		NdisFreeMemory(pMemory, 0, 0);
		
		if (pBuffer == NULL)
			break;
	}
	
	NdisDprFreePacket(pPacket);
}

void freeBufferAndMemory(PNDIS_PACKET pPacket)
{
	NDIS_STATUS nStatus;
	PNDIS_BUFFER pBuffer;
	PNDIS_BUFFER pOldBuffer;
	UINT nTotalPacketLength = 0;
	UINT nPhysicalBufferCount;
	UINT nBufferCount;
	PUCHAR pMemory = NULL;
	UINT nCopySize = 0;
	
	if (!pPacket)
	{
		nStatus = NDIS_STATUS_FAILURE;
		return nStatus;
	}
	
	NdisQueryPacket(pPacket,  // NDIS_PACKET        
		&nPhysicalBufferCount,// 内存中的物理块数
		&nBufferCount,		 // 多少个NDIS_BUFFER包
		&pBuffer,         // 将返回第一个包
		&nTotalPacketLength	 // 总共的包数据长度
		);
	
	while (TRUE)
	{
		NdisQueryBufferSafe(pBuffer,
			&pMemory,// 缓冲区地址
			&nCopySize, // 缓冲区大小
			NormalPagePriority
			);
		
		// 如果tembuffer为NULL，说明当前系统资源匮乏。
		if (pMemory == NULL)
		{
			nStatus = NDIS_STATUS_FAILURE;
			return nStatus;
		}
		// 获得下一个NDIS_BUFFER。
		// 如果得到的是一个NULL指针，说明已经到了链式缓冲区的末尾，我们的循环应该结束了。
		pOldBuffer = pBuffer;
		NdisGetNextBuffer(pBuffer, &pBuffer);
		NdisUnchainBufferAtFront(pPacket, &pOldBuffer);
		NdisFreeBuffer(pOldBuffer);
		NdisFreeMemory(pMemory, 0, 0);
		
		if (pBuffer == NULL)
			break;
	}
	
	//NdisFreePacket(Packet);
}

BOOLEAN checkMacAddrEqual(PUCHAR pMacAddr1, PUCHAR pMacAddr2)
{
	UINT nRet = RtlCompareMemory(pMacAddr1, pMacAddr2, ETHER_ADDR_LEN);
	if (nRet == ETHER_ADDR_LEN)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void copyMacAddr(PUCHAR pMacAddrDst, PUCHAR pMacAddrSrc)
{
	RtlMoveMemory(pMacAddrDst, pMacAddrSrc, ETHER_ADDR_LEN);
}

PADAPT getAdapterByMAC(PUCHAR pMAC)
{
	PADAPT pAdapt;
	for (pAdapt = pAdaptList; pAdapt != NULL; pAdapt = pAdapt->Next)
	{
		if (pAdapt->bMac && checkMacAddrEqual(pAdapt->macAddr, pMAC))
		{
			KdPrint(("Passthru getAdapterByMAC方法,对应pMAC: %s的pAdapt为0x%p\n", stringFromMAC1(pMAC), pAdapt));
			return pAdapt;
		}
	}
	KdPrint(("Passthru getAdapterByMAC方法,找不到对应pMAC: %s的pAdapt!\n", stringFromMAC1(pMAC), pAdapt));
	return NULL;
}

PNT_Core_Rule getCoreRuleByCMAC(PUCHAR pCMAC)
{
	PLIST_ENTRY pListEntry;
	PNT_Core_Rule pCoreRule;
	for (pListEntry = pListCoreRules.Flink; pListEntry != &pListCoreRules.Flink; pListEntry = pListEntry->Flink)
	{ 
		pCoreRule = CONTAINING_RECORD(pListEntry, NT_Core_Rule, listEntry); 
		
		if (checkMacAddrEqual(pCoreRule->cMAC, pCMAC))
		{
			return pCoreRule;
		}
	}
	return NULL;
}

PNT_Mapping_Rule getMappingRuleByIMAC(PUCHAR pIMAC)
{
	PLIST_ENTRY pListEntry;
	PNT_Mapping_Rule pMappingRule;
	for (pListEntry = pListMappingRules.Flink; pListEntry != &pListMappingRules.Flink; pListEntry = pListEntry->Flink)
	{ 
		pMappingRule = CONTAINING_RECORD(pListEntry, NT_Mapping_Rule, listEntry); 
		
		if (checkMacAddrEqual(pMappingRule->iMAC, pIMAC))
		{
			return pMappingRule;
		}
	}
	return NULL;
}

PNT_Mapping_Rule getMappingRuleByPIP(ULONG nPIP)
{
	PLIST_ENTRY pListEntry;
	PNT_Mapping_Rule pMappingRule;
	for (pListEntry = pListMappingRules.Flink; pListEntry != &pListMappingRules.Flink; pListEntry = pListEntry->Flink)
	{ 
		pMappingRule = CONTAINING_RECORD(pListEntry, NT_Mapping_Rule, listEntry); 
		
		if (pMappingRule->pIP == nPIP)
		{
			return pMappingRule;
		}
	}
	return NULL;
}

PNT_Core_Rule getCoreRule()
{
	PLIST_ENTRY pListEntry;
	PNT_Core_Rule pCoreRule;
	for (pListEntry = pListCoreRules.Flink; pListEntry != &pListCoreRules.Flink; pListEntry = pListEntry->Flink)
	{ 
		pCoreRule = CONTAINING_RECORD(pListEntry, NT_Core_Rule, listEntry); 
		return pCoreRule;
	}
	return NULL;
}

PNT_Mapping_Rule getMappingRule()
{
	PLIST_ENTRY pListEntry;
	PNT_Mapping_Rule pMappingRule;
	for (pListEntry = pListMappingRules.Flink; pListEntry != &pListMappingRules.Flink; pListEntry = pListEntry->Flink)
	{ 
		pMappingRule = CONTAINING_RECORD(pListEntry, NT_Mapping_Rule, listEntry); 
		return pMappingRule;
	}
	return NULL;
}

USHORT getShortFromNetBytes(PUCHAR pData)
{
	USHORT nResult;
	*((UCHAR *) (&nResult)) = pData[1];
	*((UCHAR *) (&nResult) + 1) = pData[0];
	return nResult;
}

USHORT checkSum(USHORT *pBuf, UINT nBufLen)
{
	ULONG sum;
	for (sum = 0; nBufLen > 0; nBufLen --)
		sum += *pBuf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}

void prepare34Bytes(PUCHAR pBuffer, UINT *pnBufLen)
{
	RtlMoveMemory(pBuffer + 34, pBuffer, *pnBufLen);
	RtlZeroMemory(pBuffer, 34);
	*pnBufLen = *pnBufLen + 34;
}

void shrink34Bytes(PUCHAR pBuffer, UINT *pnBufLen)
{
	RtlMoveMemory(pBuffer, pBuffer + 34, *pnBufLen);
	*pnBufLen = *pnBufLen - 34;
}

BOOLEAN checkIPProtocol(PUCHAR pBuffer)
{
	PEth_Header pEthHeader = (PEth_Header) pBuffer;
	if (pEthHeader->eth_Type == 0x0008) //Protocol type: [08 00](IP)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOLEAN checkVEPProtocol(PUCHAR pBuffer)
{
	PIP_Header pIPHeader = (PIP_Header) (pBuffer + ETH_HEADER_LEN);
	if (pIPHeader->ip_Protocol == 0x20) //IP protocol type: [20](VEP)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int checkARPProtocol(PUCHAR pBuffer)
{
	PEth_Header pEthHeader = (PEth_Header) pBuffer;
	if (pEthHeader->eth_Type == 0x0608) //Protocol type: [08 06](ARP)
	{
		PARP_Header pARPHeader = (PARP_Header) (pBuffer + ETH_HEADER_LEN);
		if (pARPHeader->arp_Opcode == 0x0100) //Opcode: [00 01](request)
		{
			return 1;
		}
		else if (pARPHeader->arp_Opcode == 0x0200) //Opcode: [00 02](reply)
		{
			return 2;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

BOOLEAN checkRARPProtocol(PUCHAR pBuffer)
{
	PIP_Header pIPHeader = (PIP_Header) (pBuffer + ETH_HEADER_LEN);
	if (pIPHeader->ip_Protocol == 0x3580) //Protocol type: [80 35](RARP)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOLEAN checkIPv6Protocol(PUCHAR pBuffer)
{
	PIP_Header pIPHeader = (PIP_Header) (pBuffer + ETH_HEADER_LEN);
	if (pIPHeader->ip_Protocol == 0x86DD) //Protocol type: [DD 86](IPv6)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void writeEthHeaderForIP(PUCHAR pBuffer, PUCHAR pDstAddr, PUCHAR pSrcAddr)
{
	//以太网帧头填写方式
	//	[6] 目的MAC**********************
	//	[6] 源MAC**********************
	//  [2] 类型: 0x0800(IP)
	copyMacAddr(pBuffer, pDstAddr);
	copyMacAddr(pBuffer + ETHER_ADDR_LEN, pSrcAddr);
	*(pBuffer + 2 * ETHER_ADDR_LEN) = 0x08;
	*(pBuffer + 2 * ETHER_ADDR_LEN + 1) = 0x00;
}

void writeEthHeaderForARP(PUCHAR pBuffer, PUCHAR pDstAddr, PUCHAR pSrcAddr)
{
	//以太网帧头填写方式
	//	[6] 目的MAC**********************
	//	[6] 源MAC**********************
	//  [2] 类型: 0x0806(ARP)
	copyMacAddr(pBuffer, pDstAddr);
	copyMacAddr(pBuffer + ETHER_ADDR_LEN, pSrcAddr);
	*(pBuffer + 2 * ETHER_ADDR_LEN) = 0x08;
	*(pBuffer + 2 * ETHER_ADDR_LEN + 1) = 0x06;
}

void writeIPHeader(PUCHAR pBuffer, ULONG nDstIP, ULONG nSrcIP, USHORT nTotalLen, BOOLEAN bVEPFlag)
{
	//IP报头填写方式
	//	[0.5] Version: 0x4
	//	[0.5] Header length: 0x5(5x4=20Bytes)
	//	[1] Differentiated services Field: 0x0
	//	[2] Total Length: 包括IP报头在内的IP报文总字节数，自己算******************************************************************************
	//	[2] Identification: 0x0, 组装包的时候用的ID，只要不让它分片，就随便填就好了
	//	[1] Flags: 0x0
	//	[1] Fragment offset: 0x0
	//	[1] Time to live: 0x80
	//	[1] Protocol: 0x20(1-ICMP 2-IGMP 6-TCP 17-UDP 89-OSPF，我们用0x20承载我们自己的Ehternet In IP协议，就叫做Virtual Ethernet Protocol，VEP吧)
	//	[2] Header checksum: IP报头自己的校验和，先假设校验和为0，然后调用checkSum算法计算即可**************************************************
	//	[4] Source IP(源IP)**********************
	//	[4] Destination IP(目的IP)**********************
	pBuffer[0] = 0x45;
	pBuffer[1] = 0x0;

	pBuffer[2] = *(((UCHAR*) &nTotalLen) + 1); // Packet:12 34 <-> Mem:34 12 00 00
	pBuffer[3] = *(((UCHAR*) &nTotalLen) + 0);

	pBuffer[4] = 0x0;
	pBuffer[5] = 0x0;
	pBuffer[6] = 0x0;
	pBuffer[7] = 0x0;
	pBuffer[8] = 0x80; //TTL
	if (bVEPFlag)
	{
		pBuffer[9] = 0x20; //VEP Protocol
	}
	else
	{
		pBuffer[9] = 0x00;
	}
	pBuffer[10] = 0x0; //checksum to 0
	pBuffer[11] = 0x0; //checksum to 0

	*((ULONG*) (pBuffer + 12)) = nSrcIP;
	*((ULONG*) (pBuffer + 16)) = nDstIP;

	*((USHORT*) (pBuffer + 10)) = checkSum(pBuffer, 20 / 2); ////checksum
}

void writeARPHeader(PUCHAR pBuffer, PUCHAR pDstAddr, PUCHAR pSrcAddr, ULONG nDstIP, ULONG nSrcIP, BOOLEAN bRequestOrReply)
{
	//ARP报头填写方式
	//	[2] Hardware type: 0x0001(Ethernet)
	//	[2] Protocol type: 0x0800(IP)
	//	[1] Hardware size: 0x06
	//	[1] Protocol size: 0x04
	//	[2] Opcode: 0x0001(request) 或者 0x0002(reply)
	//	[6] Source MAC: 0xXXXXXXXXXXXX(6个字节)
	//	[4] Source IP: 0xXXXXXXXX(4个字节)
	//	[6] Target MAC: 0xXXXXXXXXXXXX(6个字节，查询时空着为0，而外面Ehternet的Target MAC全F，回复时Source Target颠倒，并且全填好）
	//	[4] Target IP: 0xXXXXXXXX(4个字节)
	pBuffer[0] = 0x00;
	pBuffer[1] = 0x01;
	pBuffer[2] = 0x08;
	pBuffer[3] = 0x00;
	pBuffer[4] = 0x06;
	pBuffer[5] = 0x04;

	pBuffer[6] = 0x00;
	//pBuffer[7] = 0x02 - bRequestOrReply; //0x01为request 0x02为reply
	if (bRequestOrReply)
	{
		pBuffer[7] = 0x01;
	}
	else
	{
		pBuffer[7] = 0x02;
	}

	copyMacAddr(pBuffer + 8, pSrcAddr);
	*((ULONG*) (pBuffer + 14)) = nSrcIP;
	copyMacAddr(pBuffer + 18, pDstAddr);
	*((ULONG*) (pBuffer + 24)) = nDstIP;
	RtlZeroMemory(pBuffer + 28, 18);
}

NDIS_STATUS sendARPResponse(PADAPT pAdapt, PUCHAR pDstAddr, PUCHAR pSrcAddr, ULONG nDstIP, ULONG nSrcIP)
{
	NDIS_STATUS nStatus;
	PUCHAR pMemory = NULL;
	ULONG nMemLen;
	
	nStatus = NdisAllocateMemoryWithTag(&pMemory, 2048, TAG); //分配带标签的内存
	if (nStatus != NDIS_STATUS_SUCCESS)
	{
		return nStatus;
	}
	NdisZeroMemory(pMemory, 2048); //分配2KB内存

	writeEthHeaderForARP(pMemory, pDstAddr, pSrcAddr);
	writeARPHeader(pMemory + sizeof(Eth_Header), pDstAddr, pSrcAddr, nDstIP, nSrcIP, FALSE);
	nMemLen = sizeof(Eth_Header) + sizeof(ARP_Header);

	sendMemory(pAdapt, pMemory, nMemLen);
	//freeMemory(pMemory);
	return nStatus;
}

NDIS_STATUS sendDebugPacket(NT_Debug_Rule *pDebugRule)
{
	NDIS_STATUS nStatus;
	PUCHAR pMemory = NULL;
	ULONG nMemLen;
	PADAPT pAdapt = NULL;
	USHORT nIPTotalLen;

	pAdapt = getAdapterByMAC(pDebugRule->eSrcMAC);
	if (!pAdapt)
	{
		KdPrint(("Passthru sendDebugPacket方法,对应eSrcMAC的pAdapt无法找到,无法发送\n"));
		return NDIS_STATUS_FAILURE;
	}
	
	nStatus = NdisAllocateMemoryWithTag(&pMemory, 2048, TAG); //分配带标签的内存
	if (nStatus != NDIS_STATUS_SUCCESS)
	{
		return nStatus;
	}
	NdisZeroMemory(pMemory, 2048); //分配2KB内存
	
	if (pDebugRule->eType == TYPE_IP)
	{
		//Ethernet + ...
		writeEthHeaderForIP(pMemory, pDebugRule->eDstMAC, pDebugRule->eSrcMAC);
		if (pDebugRule->iType == TYPE_IP)
		{
			//(Ethernet +) IP + Ethernet + IP + Content
			writeIPHeader(pMemory + ETH_HEADER_LEN, pDebugRule->eDstIP, pDebugRule->eSrcIP, IP_HEADER_LEN + ETH_HEADER_LEN + IP_HEADER_LEN + CONTENT_LEN, TRUE);
			writeEthHeaderForIP(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN, pDebugRule->iDstMAC, pDebugRule->iSrcMAC);
			writeIPHeader(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN + ETH_HEADER_LEN, pDebugRule->iDstIP, pDebugRule->iSrcIP, IP_HEADER_LEN + CONTENT_LEN, FALSE);
			RtlMoveMemory(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN + ETH_HEADER_LEN + IP_HEADER_LEN, pDebugRule->content, CONTENT_LEN);
			nMemLen = ETH_HEADER_LEN + IP_HEADER_LEN + ETH_HEADER_LEN + IP_HEADER_LEN + CONTENT_LEN;
			sendMemory(pAdapt, pMemory, nMemLen);
			return nStatus;
		}
		else if (pDebugRule->iType == TYPE_ARP_REQUEST)
		{
			//(Ethernet +) IP + Ethernet + ARP Request
			writeIPHeader(pMemory + ETH_HEADER_LEN, pDebugRule->eDstIP, pDebugRule->eSrcIP, IP_HEADER_LEN + ETH_HEADER_LEN + ARP_HEADER_LEN, TRUE);
			writeEthHeaderForARP(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN, g_pMAC_FF, pDebugRule->iSrcMAC);
			writeARPHeader(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN + ETH_HEADER_LEN, g_pMAC_00, pDebugRule->iSrcMAC,
				pDebugRule->iDstIP, pDebugRule->iSrcIP, TRUE);
			//writeEthHeaderForARP(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN, pDebugRule->iSrcMAC, pDebugRule->iSrcMAC);
			//writeARPHeader(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN + ETH_HEADER_LEN, pDebugRule->iSrcMAC, pDebugRule->iSrcMAC,
			//	pDebugRule->iDstIP, pDebugRule->iSrcIP, TRUE);
			nMemLen = ETH_HEADER_LEN + IP_HEADER_LEN + ETH_HEADER_LEN + ARP_HEADER_LEN;
			sendMemory(pAdapt, pMemory, nMemLen);
			return nStatus;
		}
		else if (pDebugRule->iType == TYPE_ARP_REPLY)
		{
			//(Ethernet +) IP + Ethernet + ARP Reply
			writeIPHeader(pMemory + ETH_HEADER_LEN, pDebugRule->eDstIP, pDebugRule->eSrcIP, IP_HEADER_LEN + ETH_HEADER_LEN + ARP_HEADER_LEN, TRUE);
			writeEthHeaderForARP(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN, pDebugRule->iDstMAC, pDebugRule->iSrcMAC);
			writeARPHeader(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN + ETH_HEADER_LEN, pDebugRule->iDstMAC, pDebugRule->iSrcMAC,
				pDebugRule->iDstIP, pDebugRule->iSrcIP, FALSE);
			nMemLen = ETH_HEADER_LEN + IP_HEADER_LEN + ETH_HEADER_LEN + ARP_HEADER_LEN;
			sendMemory(pAdapt, pMemory, nMemLen);
			return nStatus;
		}
		else //if (pDebugRule->iType == TYPE_NONE)
		{
			//(Ethernet +) IP + Content
			writeIPHeader(pMemory + ETH_HEADER_LEN, pDebugRule->eDstIP, pDebugRule->eSrcIP, IP_HEADER_LEN + CONTENT_LEN, FALSE);
			RtlMoveMemory(pMemory + ETH_HEADER_LEN + IP_HEADER_LEN, pDebugRule->content, CONTENT_LEN);
			nMemLen = ETH_HEADER_LEN + IP_HEADER_LEN + CONTENT_LEN;
			sendMemory(pAdapt, pMemory, nMemLen);
			return nStatus;
		}
		
	}
	else if (pDebugRule->eType == TYPE_ARP_REQUEST)
	{
		//Ethernet + ARP Request
		writeEthHeaderForARP(pMemory, g_pMAC_FF, pDebugRule->eSrcMAC);
		writeARPHeader(pMemory + ETH_HEADER_LEN, g_pMAC_00, pDebugRule->eSrcMAC, pDebugRule->eDstIP, pDebugRule->eSrcIP,
			TRUE);
		nMemLen = ETH_HEADER_LEN + ARP_HEADER_LEN;
		sendMemory(pAdapt, pMemory, nMemLen);
		return nStatus;
	}
	else if (pDebugRule->eType == TYPE_ARP_REPLY)
	{
		//Ethernet + ARP Reply
		writeEthHeaderForARP(pMemory, pDebugRule->eDstMAC, pDebugRule->eSrcMAC);
		writeARPHeader(pMemory + ETH_HEADER_LEN, pDebugRule->eDstMAC, pDebugRule->eSrcMAC, pDebugRule->eDstIP, pDebugRule->eSrcIP,
			FALSE);
		nMemLen = ETH_HEADER_LEN + ARP_HEADER_LEN;
		sendMemory(pAdapt, pMemory, nMemLen);
		return nStatus;
	}
	else //if (pDebugRule->eType == TYPE_NONE)
	{
		KdPrint(("Passthru sendDebugPacket方法,数据包eType为TYPE_NONE,无法发送\n"));
		return NDIS_STATUS_FAILURE;
	}
}

PCHAR stringFromNetworkIP1(ULONG nIP)
{
	RtlStringCchPrintfA(g_strBuffer1, STR_BUFFER_LEN, "%d.%d.%d.%d", nIP & 0xFF, (nIP >> 8) & 0xFF, (nIP >> 16) & 0xFF, nIP >> 24);
	return g_strBuffer1;
}

PCHAR stringFromNetworkIP2(ULONG nIP)
{
	RtlStringCchPrintfA(g_strBuffer2, STR_BUFFER_LEN, "%d.%d.%d.%d", nIP & 0xFF, (nIP >> 8) & 0xFF, (nIP >> 16) & 0xFF, nIP >> 24);
	return g_strBuffer2;
}

PCHAR stringFromNetworkIP3(ULONG nIP)
{
	RtlStringCchPrintfA(g_strBuffer3, STR_BUFFER_LEN, "%d.%d.%d.%d", nIP & 0xFF, (nIP >> 8) & 0xFF, (nIP >> 16) & 0xFF, nIP >> 24);
	return g_strBuffer3;
}

PCHAR stringFromNetworkIP4(ULONG nIP)
{
	RtlStringCchPrintfA(g_strBuffer4, STR_BUFFER_LEN, "%d.%d.%d.%d", nIP & 0xFF, (nIP >> 8) & 0xFF, (nIP >> 16) & 0xFF, nIP >> 24);
	return g_strBuffer4;
}

PCHAR stringFromHostIP1(ULONG nIP)
{
	RtlStringCchPrintfA(g_strBuffer5, STR_BUFFER_LEN, "%d.%d.%d.%d", nIP >> 24, (nIP >> 16) & 0xFF, (nIP >> 8) & 0xFF, nIP & 0xFF);
	return g_strBuffer5;
}

PCHAR stringFromHostIP2(ULONG nIP)
{
	RtlStringCchPrintfA(g_strBuffer6, STR_BUFFER_LEN, "%d.%d.%d.%d", nIP >> 24, (nIP >> 16) & 0xFF, (nIP >> 8) & 0xFF, nIP & 0xFF);
	return g_strBuffer6;
}

PCHAR stringFromMAC1(PUCHAR pMac)
{
	RtlStringCchPrintfA(g_strBuffer7, STR_BUFFER_LEN, "%02X-%02X-%02X-%02X-%02X-%02X",
		*((PUCHAR) pMac), *((PUCHAR) (pMac + 1)), *((PUCHAR) (pMac + 2)), *((PUCHAR) (pMac + 3)), *((PUCHAR) (pMac + 4)),
		*((PUCHAR) (pMac + 5)));
	return g_strBuffer7;
}

PCHAR stringFromMAC2(PUCHAR pMac)
{
	RtlStringCchPrintfA(g_strBuffer8, STR_BUFFER_LEN, "%02X-%02X-%02X-%02X-%02X-%02X",
		*((PUCHAR) pMac), *((PUCHAR) (pMac + 1)), *((PUCHAR) (pMac + 2)), *((PUCHAR) (pMac + 3)), *((PUCHAR) (pMac + 4)),
		*((PUCHAR) (pMac + 5)));
	return g_strBuffer8;
}

PCHAR stringFromMAC3(PUCHAR pMac)
{
	RtlStringCchPrintfA(g_strBuffer9, STR_BUFFER_LEN, "%02X-%02X-%02X-%02X-%02X-%02X",
		*((PUCHAR) pMac), *((PUCHAR) (pMac + 1)), *((PUCHAR) (pMac + 2)), *((PUCHAR) (pMac + 3)), *((PUCHAR) (pMac + 4)),
		*((PUCHAR) (pMac + 5)));
	return g_strBuffer9;
}

PCHAR stringFromMAC4(PUCHAR pMac)
{
	RtlStringCchPrintfA(g_strBuffer10, STR_BUFFER_LEN, "%02X-%02X-%02X-%02X-%02X-%02X",
		*((PUCHAR) pMac), *((PUCHAR) (pMac + 1)), *((PUCHAR) (pMac + 2)), *((PUCHAR) (pMac + 3)), *((PUCHAR) (pMac + 4)),
		*((PUCHAR) (pMac + 5)));
	return g_strBuffer10;
}

FILTER_STATUS analyzeBuffer(PADAPT pAdapt, PUCHAR pBuffer, UINT *pnBufLen, int bRecvOrSend)
{
	//return FILTER_STATUS_PASS;
	analyzeBufferEx(pAdapt, pBuffer, pnBufLen, bRecvOrSend, NULL);
}

FILTER_STATUS analyzeBufferEx(PADAPT pAdapt, PUCHAR pBuffer, UINT *pnBufLen, int bRecvOrSend, PADAPT *ppForwardAdapt)
{
	PUCHAR pPacketContent = pBuffer;
	PEth_Header pEthHeader = (PEth_Header) pBuffer;
	UCHAR *dstMAC = pEthHeader->eth_DstAddr;
	UCHAR *srcMAC = pEthHeader->eth_SrcAddr;

	PNT_Mapping_Rule pMappingRule = NULL;
	PNT_Core_Rule pCoreRule = NULL;
	PUCHAR pMac = pAdapt->macAddr;

	KdPrint(("==> Passthru analyzeBuffer方法,pAdapt: 0x%p, pAdapt->macAddr: %02X-%02X-%02X-%02X-%02X-%02X\n", pAdapt,
		(UCHAR) pMac[0], (UCHAR) pMac[1], (UCHAR) pMac[2], (UCHAR) pMac[3], (UCHAR) pMac[4], (UCHAR) pMac[5]));

	if (*pnBufLen < 34 || *pnBufLen > 1700)
	{
		KdPrint(("Passthru analyzeBuffer方法,包长度小于34B或大于1700B,数据包已丢弃\n"));
		return FILTER_STATUS_PASS;
	}


	if (checkIPProtocol(pEthHeader) || checkARPProtocol(pEthHeader)) //是IPv4报文或者ARP报文
	{
		if (checkIPProtocol(pEthHeader)) //是IPv4报文
		{
			PIP_Header pIPHeader = (PIP_Header) ((PUCHAR) pEthHeader + ETH_HEADER_LEN);
			ULONG dstIP = pIPHeader->ip_Dst;
			ULONG srcIP = pIPHeader->ip_Src;
			KdPrint(("<== Passthru analyzeBuffer方法,正在分析[***IPv4***]报文:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
				stringFromMAC1(dstMAC), stringFromMAC2(srcMAC), stringFromNetworkIP1(dstIP), stringFromNetworkIP2(srcIP)));
		}
		else if (checkARPProtocol(pEthHeader) == 1) //是ARP请求报文
		{
			PARP_Header pARPHeader = (PARP_Header) ((PUCHAR) pEthHeader + ETH_HEADER_LEN);
			ULONG dstIP = pARPHeader->arp_dstProtAddr;
			ULONG srcIP = pARPHeader->arp_srcProtAddr;
			KdPrint(("<== Passthru analyzeBuffer方法,正在分析[***ARP请求***]报文:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
				stringFromMAC1(dstMAC), stringFromMAC2(srcMAC), stringFromNetworkIP1(dstIP), stringFromNetworkIP2(srcIP)));
		}
		else //if (checkARPProtocol(pEthHeader) == 2) //是ARP应答报文
		{
			PARP_Header pARPHeader = (PARP_Header) ((PUCHAR) pEthHeader + ETH_HEADER_LEN);
			ULONG dstIP = pARPHeader->arp_dstProtAddr;
			ULONG srcIP = pARPHeader->arp_srcProtAddr;
			KdPrint(("<== Passthru analyzeBuffer方法,正在分析[***ARP应答***]报文:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
				stringFromMAC1(dstMAC), stringFromMAC2(srcMAC), stringFromNetworkIP1(dstIP), stringFromNetworkIP2(srcIP)));
		}

		if (g_nStatus == STATUS_STOP)
		{
			KdPrint(("Passthru analyzeBuffer方法,Stop模式下,数据包已默认通过\n"));
			return FILTER_STATUS_PASS;
		}
		else if (g_nMode == MODE_CENTER)
		{
			if (!g_nCoreRuleReady)
			{
				KdPrint(("Passthru analyzeBuffer方法,Center模式下,Core Rule未准备好,数据包已丢弃\n"));
				return FILTER_STATUS_PASS;
			}
		}
		else if (g_nMode == MODE_MARGIN)
		{
			if (!g_nCoreRuleReady)
			{
				KdPrint(("Passthru analyzeBuffer方法,Margin模式下,Core Rule未准备好,数据包已丢弃\n"));
				return FILTER_STATUS_PASS;
			}
			else if (!g_nMappingRuleReady)
			{
				KdPrint(("Passthru analyzeBuffer方法,Margin模式下,Mapping Rule未准备好,数据包已丢弃\n"));
				return FILTER_STATUS_PASS;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//************************Center主机收包************************
		if (g_nMode == MODE_CENTER && bRecvOrSend == MODE_RECEIVE)
		{
			//A.接收网卡MAC地址属于IMAC（从虚拟网络向物理网络转发的数据包）：
			//	a.源MAC为自身IMAC地址，丢弃此数据包。
			//	b.进行VEP协议封装：
			//		源IP：CIP；
			//		源MAC：CMAC；
			//		目的IP：PIP；
			//		目的MAC：getTargetMacOrGateMac(PIP)，存放在EMAC处。
			//	c.计算IP报文长度，填入后，再计算IP报头校验和，填入后从CMAC网卡转发数据包。
			if (pMappingRule = getMappingRuleByIMAC(pAdapt->macAddr))
			{
				USHORT nIPTotalLen;
				pCoreRule = getCoreRule();

				if (checkMacAddrEqual(srcMAC, pAdapt->macAddr) == TRUE)
				{
					KdPrint(("<== Passthru analyzeBuffer方法,从IMAC网卡由于回环特性收到了自己发出的数据包,直接丢弃\n"));
					return FILTER_STATUS_DROP;
				}

				//nIPTotalLen = getShortFromNetBytes((PUCHAR) pEthHeader + ETH_HEADER_LEN + 2) + ETH_HEADER_LEN + IP_HEADER_LEN; //此算法在ARP时失效
				nIPTotalLen = *pnBufLen + IP_HEADER_LEN;
				prepare34Bytes(pBuffer, pnBufLen);
				writeEthHeaderForIP(pBuffer, pMappingRule->eMAC, pCoreRule->cMAC);
				writeIPHeader(pBuffer + ETH_HEADER_LEN, pMappingRule->pIP, pCoreRule->cIP, nIPTotalLen, TRUE);
				if (ppForwardAdapt)
				{
					*ppForwardAdapt = pCoreRule->cAdapt;
				}

				KdPrint(("<== Passthru analyzeBuffer方法,从IMAC网卡收到报文,加入如下封装后由网卡%s发出:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
					stringFromMAC1(pCoreRule->cAdapt->macAddr), stringFromMAC2(pMappingRule->eMAC), stringFromMAC3(pCoreRule->cMAC),
					stringFromNetworkIP1(pMappingRule->pIP), stringFromNetworkIP2(pCoreRule->cIP)));
				return FILTER_STATUS_MODIFY_REDIRECT;
			}
			//B.接收网卡MAC地址属于CMAC（从物理网络向虚拟网络转发的数据包）：
			//		a.若是IP报文，且IP协议字段为0x20，则为VEP协议封装，判断源IP为哪个PIP：
			//			1）若源IP对应到某一PIP，解封装后，需要判断内部是IP报文还是ARP报文：
			//				①是IP报文，则修改源MAC为IMAC，从对应的IMAC网卡转发数据包；
			//				②是ARP请求报文，则修改两处源MAC为IMAC，从对应的IMAC网卡转发数据包；
			//				③是ARP应答报文，则修改两处源MAC为IMAC，从对应的IMAC网卡转发数据包；
			//				④是其它类型报文，则丢弃。
			//			2）若源IP不对应某一PIP，则丢弃。
			//		b.不是VEP协议，则让所有数据包通过。
			else if (pCoreRule = getCoreRuleByCMAC(pAdapt->macAddr))
			{
				if (checkIPProtocol(pEthHeader) && checkVEPProtocol(pEthHeader)) //是VEP报文
				{
					PIP_Header pIPHeader = (PIP_Header) ((PUCHAR) pEthHeader + ETH_HEADER_LEN);
					ULONG dstIP = pIPHeader->ip_Dst;
					ULONG srcIP = pIPHeader->ip_Src;

					if (pMappingRule = getMappingRuleByPIP(srcIP))
					{
						PEth_Header pEthHeader2;
						UCHAR *dstMAC2;
						UCHAR *srcMAC2;
						shrink34Bytes(pBuffer, pnBufLen);
						pEthHeader2 = (PEth_Header) pBuffer; //Ethernet
						dstMAC2 = pEthHeader2->eth_DstAddr;
						srcMAC2 = pEthHeader2->eth_SrcAddr;

						if (checkIPProtocol(pEthHeader2)) //是IP报文
						{
							PIP_Header pIPHeader2 = (PIP_Header) ((PUCHAR) pEthHeader2 + ETH_HEADER_LEN);
							ULONG dstIP2 = pIPHeader2->ip_Dst;
							ULONG srcIP2 = pIPHeader2->ip_Src;
							if (ppForwardAdapt)
							{
								*ppForwardAdapt = pMappingRule->iAdapt;
							}

							KdPrint(("<== Passthru analyzeBuffer方法,从CMAC网卡收到报文,去掉封装后由网卡%s发出,去封装后为IP报文[修改前]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
								stringFromMAC1(pMappingRule->iAdapt->macAddr), stringFromMAC2(dstMAC2), stringFromMAC3(srcMAC2),
								stringFromNetworkIP1(dstIP2), stringFromNetworkIP2(srcIP2)));
							
							copyMacAddr(srcMAC2, pMappingRule->iMAC);
							
							KdPrint(("<== Passthru analyzeBuffer方法,从CMAC网卡收到报文,去掉封装后由网卡%s发出,去封装后为IP报文[修改后]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
								stringFromMAC1(pMappingRule->iAdapt->macAddr), stringFromMAC2(dstMAC2), stringFromMAC3(srcMAC2),
								stringFromNetworkIP1(dstIP2), stringFromNetworkIP2(srcIP2)));

							return FILTER_STATUS_MODIFY_REDIRECT;
						}
						else if (checkARPProtocol(pEthHeader2) == 1) //是ARP请求报文
						{
							PARP_Header pARPHeader2 = (PARP_Header) ((PUCHAR) pEthHeader2 + ETH_HEADER_LEN);
							if (ppForwardAdapt)
							{
								*ppForwardAdapt = pMappingRule->iAdapt;
							}

							KdPrint(("<== Passthru analyzeBuffer方法,从CMAC网卡收到报文,去掉封装后由网卡%s发出,去封装后为ARP请求报文[修改前]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
								stringFromMAC1(pMappingRule->iAdapt->macAddr), stringFromMAC2(dstMAC2), stringFromMAC3(srcMAC2),
								stringFromNetworkIP1(pARPHeader2->arp_dstProtAddr), stringFromNetworkIP2(pARPHeader2->arp_srcProtAddr)));

							copyMacAddr(srcMAC2, pMappingRule->iMAC);
							copyMacAddr(pARPHeader2->arp_srcHardAddr, pMappingRule->iMAC);
							
							KdPrint(("<== Passthru analyzeBuffer方法,从CMAC网卡收到报文,去掉封装后由网卡%s发出,去封装后为ARP请求报文[修改后]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
								stringFromMAC1(pMappingRule->iAdapt->macAddr), stringFromMAC2(dstMAC2), stringFromMAC3(srcMAC2),
								stringFromNetworkIP1(pARPHeader2->arp_dstProtAddr), stringFromNetworkIP2(pARPHeader2->arp_srcProtAddr)));

							return FILTER_STATUS_MODIFY_REDIRECT;
						}
						else if (checkARPProtocol(pEthHeader2) == 2) //是ARP应答报文
						{
							PARP_Header pARPHeader2 = (PARP_Header) ((PUCHAR) pEthHeader2 + ETH_HEADER_LEN);
							if (ppForwardAdapt)
							{
								*ppForwardAdapt = pMappingRule->iAdapt;
							}

							KdPrint(("<== Passthru analyzeBuffer方法,从CMAC网卡收到报文,去掉封装后由网卡%s发出,去封装后为ARP应答报文[修改前]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
								stringFromMAC1(pMappingRule->iAdapt->macAddr), stringFromMAC2(dstMAC2), stringFromMAC3(srcMAC2),
								stringFromNetworkIP1(pARPHeader2->arp_dstProtAddr), stringFromNetworkIP2(pARPHeader2->arp_srcProtAddr)));

							copyMacAddr(srcMAC2, pMappingRule->iMAC);
							copyMacAddr(pARPHeader2->arp_srcHardAddr, pMappingRule->iMAC);

							KdPrint(("<== Passthru analyzeBuffer方法,从CMAC网卡收到报文,去掉封装后由网卡%s发出,去封装后为ARP应答报文[修改后]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
								stringFromMAC1(pMappingRule->iAdapt->macAddr), stringFromMAC2(dstMAC2), stringFromMAC3(srcMAC2),
								stringFromNetworkIP1(pARPHeader2->arp_dstProtAddr), stringFromNetworkIP2(pARPHeader2->arp_srcProtAddr)));
							
							return FILTER_STATUS_MODIFY_REDIRECT;
						}
						else //是其它类型报文，如RARP、IPv6，丢弃
						{
							KdPrint(("<== Passthru analyzeBuffer方法,从CMAC网卡收到报文,去掉封装后由网卡%s发出,去封装后为RARP、IPv6等类型报文,故丢弃"));
							return FILTER_STATUS_DROP;
						}
					}
					else //找不到对应的PIP，丢弃
					{
						KdPrint(("<== Passthru analyzeBuffer方法,从CMAC网卡收到报文,找不到对应的PIP,故丢弃"));
						return FILTER_STATUS_DROP;
					}
				}
				else //不是VEP报文，则让所有数据包通过。
				{
					return FILTER_STATUS_PASS;
				}
			}
			//C.接收网卡MAC地址既不属于IMAC也不属于CMAC，则让所有数据包通过。
			else
			{
				return FILTER_STATUS_PASS;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//************************Center主机发包************************
		else if (g_nMode == MODE_CENTER && bRecvOrSend == MODE_SEND)
		{
			//A.发送网卡MAC地址属于IMAC：
			//把数据包丢弃。
			//B.发送网卡MAC地址属于CMAC或其它：
			//让所有数据包通过。
			if (pMappingRule = getMappingRuleByIMAC(pAdapt->macAddr))
			{
				KdPrint(("<== Passthru analyzeBuffer方法,从IMAC网卡试图主动发出的报文,直接丢弃\n"));
				return FILTER_STATUS_DROP;
			}
			else
			{
				return FILTER_STATUS_PASS;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//************************Margin主机收包************************
		else if (g_nMode == MODE_MARGIN && bRecvOrSend == MODE_RECEIVE)
		{
			pMappingRule = getMappingRule();
			//A.若是ARP报文，若目的IP为PIP，则必须由驱动程序直接进行ARP应答，并丢弃原数据包；若目的IP不为PIP，直接丢弃数据包。
			//B.若不是ARP报文，但是是IP报文，且IP协议字段为0x20，则为VEP协议封装，判断目的IP是否为PIP：
			//	a.目的IP为PIP，解封装后，再判断内部是IP报文还是ARP报文：
			//		①是IP报文，则修改目的MAC为EMAC，接收数据包；
			//		②是ARP请求报文，则接收数据包；
			//		③是ARP应答报文，则修改两处目的MAC为EMAC，接收数据包；
			//		④是其它类型报文，则丢弃。
			//	b.目的IP不为PIP，则丢弃。
			//C.若不是ARP报文，也不是VEP报文，则丢弃。
			
			if (checkARPProtocol(pEthHeader)) //是ARP报文
			{
				PARP_Header pARPHeader = (PARP_Header) ((PUCHAR) pEthHeader + ETH_HEADER_LEN);
				//sendARPResponse(PADAPT pAdapt, PUCHAR pDstAddr, PUCHAR pSrcAddr, ULONG nDstIP, ULONG nSrcIP)
				if (pARPHeader->arp_dstProtAddr == pMappingRule->pIP)
				{
					sendARPResponse(pAdapt, pARPHeader->arp_srcHardAddr, pMappingRule->eMAC, pARPHeader->arp_srcProtAddr,
						pMappingRule->pIP);
					KdPrint(("<== Passthru analyzeBuffer方法,从EMAC网卡收到针对本机:%s的ARP请求报文,直接由驱动程序进行ARP应答,内容如下:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
						stringFromNetworkIP1(pARPHeader->arp_dstProtAddr),
						stringFromMAC1(pARPHeader->arp_srcHardAddr), stringFromMAC2(pMappingRule->eMAC),
						stringFromNetworkIP2(pARPHeader->arp_srcProtAddr), stringFromNetworkIP3(pMappingRule->pIP)));
					return FILTER_STATUS_DROP_SEND;
				}
				else
				{
					KdPrint(("<== Passthru analyzeBuffer方法,从EMAC网卡收到针对其它机器:%s的ARP请求报文,直接丢弃\n",
						stringFromNetworkIP1(pARPHeader->arp_dstProtAddr)));
					return FILTER_STATUS_DROP;
				}
			}
			else if (checkIPProtocol(pEthHeader) && checkVEPProtocol(pEthHeader)) //是VEP报文
			{
				PIP_Header pIPHeader = (PIP_Header) ((PUCHAR) pEthHeader + ETH_HEADER_LEN);
				ULONG dstIP = pIPHeader->ip_Dst;
				ULONG srcIP = pIPHeader->ip_Src;
				if (dstIP == pMappingRule->pIP) //目的IP == PIP
				{
					PEth_Header pEthHeader2;
					UCHAR *dstMAC2;
					UCHAR *srcMAC2;
					shrink34Bytes(pBuffer, pnBufLen);
					pEthHeader2 = (PEth_Header) pBuffer; //Ethernet
					dstMAC2 = pEthHeader2->eth_DstAddr;
					srcMAC2 = pEthHeader2->eth_SrcAddr;
					
					if (checkIPProtocol(pEthHeader2)) //是IP报文
					{
						PIP_Header pIPHeader2 = (PIP_Header) ((PUCHAR) pEthHeader2 + ETH_HEADER_LEN);

						KdPrint(("<== Passthru analyzeBuffer方法,从EMAC网卡收到报文,去掉封装后接收,去封装后为IP报文[修改前]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
							stringFromMAC1(dstMAC2), stringFromMAC2(srcMAC2),
							stringFromNetworkIP1(pIPHeader2->ip_Dst), stringFromNetworkIP2(pIPHeader2->ip_Src)));

						copyMacAddr(dstMAC2, pMappingRule->eMAC);

						KdPrint(("<== Passthru analyzeBuffer方法,从EMAC网卡收到报文,去掉封装后接收,去封装后为IP报文[修改后]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
							stringFromMAC1(dstMAC2), stringFromMAC2(srcMAC2),
							stringFromNetworkIP1(pIPHeader2->ip_Dst), stringFromNetworkIP2(pIPHeader2->ip_Src)));

						return FILTER_STATUS_MODIFY_RECEIVE;
					}
					else if (checkARPProtocol(pEthHeader2) == 1) //是ARP请求报文
					{
						PARP_Header pARPHeader2 = (PARP_Header) ((PUCHAR) pEthHeader2 + ETH_HEADER_LEN);

						KdPrint(("<== Passthru analyzeBuffer方法,从EMAC网卡收到报文,去掉封装后接收,去封装后为ARP请求报文[无需修改]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
							stringFromMAC1(dstMAC2), stringFromMAC2(srcMAC2),
							stringFromNetworkIP1(pARPHeader2->arp_dstProtAddr), stringFromNetworkIP2(pARPHeader2->arp_srcProtAddr)));
						return FILTER_STATUS_MODIFY_RECEIVE;
					}
					else if (checkARPProtocol(pEthHeader2) == 2) //是ARP应答报文
					{
						PARP_Header pARPHeader2 = (PARP_Header) ((PUCHAR) pEthHeader2 + ETH_HEADER_LEN);

						KdPrint(("<== Passthru analyzeBuffer方法,从EMAC网卡收到报文,去掉封装后接收,去封装后为ARP应答报文[修改前]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
							stringFromMAC1(dstMAC2), stringFromMAC2(srcMAC2),
							stringFromNetworkIP1(pARPHeader2->arp_dstProtAddr), stringFromNetworkIP2(pARPHeader2->arp_srcProtAddr)));
						
						copyMacAddr(dstMAC2, pMappingRule->eMAC);
						copyMacAddr(pARPHeader2->arp_dstHardAddr, pMappingRule->eMAC);

						KdPrint(("<== Passthru analyzeBuffer方法,从EMAC网卡收到报文,去掉封装后接收,去封装后为ARP应答报文[修改后]:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
							stringFromMAC1(dstMAC2), stringFromMAC2(srcMAC2),
							stringFromNetworkIP1(pARPHeader2->arp_dstProtAddr), stringFromNetworkIP2(pARPHeader2->arp_srcProtAddr)));
						
						return FILTER_STATUS_MODIFY_RECEIVE;
					}
					else //是其它类型报文，如RARP、IPv6，丢弃
					{
						KdPrint(("<== Passthru analyzeBuffer方法,从EMAC网卡收到报文,去掉封装后为RARP、IPv6等类型报文,故丢弃"));
						return FILTER_STATUS_DROP;
					}
				}
				else //目的IP != PIP，则丢弃
				{
					KdPrint(("<== Passthru analyzeBuffer方法,从EMAC网卡收到报文,去掉封装后目的IP不为本机IP,故丢弃"));
					return FILTER_STATUS_DROP;
				}
			}
			else //既不是ARP也不是VEP，比如是普通IP，RARP，IPv6，则丢弃
			{
				return FILTER_STATUS_DROP;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//************************Margin主机发包************************
		else //if (g_nMode == MODE_START_MARGIN && bRecvOrSend == MODE_SEND)
		{
			USHORT nIPTotalLen;
			pCoreRule = getCoreRule();
			pMappingRule = getMappingRule();
			//A.进行VEP协议封装；
			//	源IP：PIP；
			//	源MAC：EMAC；
			//	目的IP：CIP；
			//	目的MAC：getTargetMacOrGateMac(CIP)，存放在CMAC处。
			//B.计算IP报文长度，填入后，再计算IP报头校验和，填入后发送。

			//nIPTotalLen = getShortFromNetBytes((PUCHAR) pEthHeader + ETH_HEADER_LEN + 2) + ETH_HEADER_LEN + IP_HEADER_LEN; //此算法在ARP时失效
			nIPTotalLen = *pnBufLen + IP_HEADER_LEN;
			prepare34Bytes(pBuffer, pnBufLen);
			writeEthHeaderForIP(pBuffer, pCoreRule->cMAC, pMappingRule->eMAC);
			writeIPHeader(pBuffer + ETH_HEADER_LEN, pCoreRule->cIP, pMappingRule->pIP, nIPTotalLen, TRUE);

			KdPrint(("<== Passthru analyzeBuffer方法,从上层驱动收到报文,加入如下封装后由本机网卡发出:dstMAC: %s, srcMAC: %s, dstIP: %s, srcIP: %s\n",
				stringFromMAC1(pCoreRule->cMAC), stringFromMAC2(pMappingRule->eMAC),
					stringFromNetworkIP1(pCoreRule->cIP), stringFromNetworkIP2(pMappingRule->pIP)));
			return FILTER_STATUS_MODIFY_SEND;
		}
	}
	else if (checkRARPProtocol(pEthHeader))
	{
		KdPrint(("<== Passthru analyzeBuffer方法,此报文为[***RARP***]报文:dstMAC: %s, srcMAC: %s,直接允许其通过\n", stringFromMAC1(dstMAC),
			stringFromMAC2(srcMAC)));
		return FILTER_STATUS_PASS;
	}
	else if (checkIPv6Protocol(pEthHeader))
	{
		KdPrint(("<== Passthru analyzeBuffer方法,此报文为[***IPv6***]报文:dstMAC: %s, srcMAC: %s,直接允许其通过\n", stringFromMAC1(dstMAC),
			stringFromMAC2(srcMAC)));
		return FILTER_STATUS_PASS;
	}
	else //Unknown protocol
	{
		KdPrint(("<== Passthru analyzeBuffer方法,此报文类型[***未知***]:dstMAC: %s, srcMAC: %s,直接允许其通过\n", stringFromMAC1(dstMAC),
			stringFromMAC2(srcMAC)));
		return FILTER_STATUS_PASS;
	}
}