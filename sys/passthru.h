/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    passthru.h

Abstract:

    Ndis Intermediate Miniport driver sample. This is a passthru driver.

Author:

Environment:


Revision History:

 
--*/

#include "../CommonIO.h"

#define NTDDI_VERSION NTDDI_WIN7

#ifdef NDIS51_MINIPORT
#define PASSTHRU_MAJOR_NDIS_VERSION            5
#define PASSTHRU_MINOR_NDIS_VERSION            1
#else
#define PASSTHRU_MAJOR_NDIS_VERSION            4
#define PASSTHRU_MINOR_NDIS_VERSION            0
#endif

#ifdef NDIS51
#define PASSTHRU_PROT_MAJOR_NDIS_VERSION    5
#define PASSTHRU_PROT_MINOR_NDIS_VERSION    0
#else
#define PASSTHRU_PROT_MAJOR_NDIS_VERSION    4
#define PASSTHRU_PROT_MINOR_NDIS_VERSION    0
#endif

#define MAX_BUNDLEID_LENGTH 50

#define TAG 'ImPa'
#define WAIT_INFINITE 0



//advance declaration
typedef struct _ADAPT ADAPT, *PADAPT;

DRIVER_INITIALIZE DriverEntry;
extern
NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT            DriverObject,
    IN PUNICODE_STRING           RegistryPath
    );

DRIVER_DISPATCH PtDispatch;
NTSTATUS
PtDispatch(
    IN PDEVICE_OBJECT            DeviceObject,
    IN PIRP                      Irp
    );

NDIS_STATUS
PtRegisterDevice(
    VOID
    );

NDIS_STATUS
PtDeregisterDevice(
    VOID
   );

DRIVER_UNLOAD PtUnload;
VOID
PtUnloadProtocol(
    VOID
    );

//
// Protocol proto-types
//
extern
VOID
PtOpenAdapterComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_STATUS                Status,
    IN NDIS_STATUS                OpenErrorStatus
    );

extern
VOID
PtCloseAdapterComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_STATUS                Status
    );

extern
VOID
PtResetComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_STATUS                Status
    );

extern
VOID
PtRequestComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNDIS_REQUEST              NdisRequest,
    IN NDIS_STATUS                Status
    );

extern
VOID
PtStatus(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_STATUS                GeneralStatus,
    IN PVOID                      StatusBuffer,
    IN UINT                       StatusBufferSize
    );

extern
VOID
PtStatusComplete(
    IN NDIS_HANDLE                ProtocolBindingContext
    );

extern
VOID
PtSendComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNDIS_PACKET               Packet,
    IN NDIS_STATUS                Status
    );

extern
VOID
PtTransferDataComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNDIS_PACKET               Packet,
    IN NDIS_STATUS                Status,
    IN UINT                       BytesTransferred
    );

extern
NDIS_STATUS
PtReceive(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_HANDLE                MacReceiveContext,
    IN PVOID                      HeaderBuffer,
    IN UINT                       HeaderBufferSize,
    IN PVOID                      LookAheadBuffer,
    IN UINT                       LookaheadBufferSize,
    IN UINT                       PacketSize
    );

extern
VOID
PtReceiveComplete(
    IN NDIS_HANDLE                ProtocolBindingContext
    );

extern
INT
PtReceivePacket(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNDIS_PACKET               Packet
    );

extern
VOID
PtBindAdapter(
    OUT PNDIS_STATUS              Status,
    IN  NDIS_HANDLE               BindContext,
    IN  PNDIS_STRING              DeviceName,
    IN  PVOID                     SystemSpecific1,
    IN  PVOID                     SystemSpecific2
    );

extern
VOID
PtUnbindAdapter(
    OUT PNDIS_STATUS              Status,
    IN  NDIS_HANDLE               ProtocolBindingContext,
    IN  NDIS_HANDLE               UnbindContext
    );
    
VOID
PtUnload(
    IN PDRIVER_OBJECT             DriverObject
    );



extern 
NDIS_STATUS
PtPNPHandler(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNET_PNP_EVENT             pNetPnPEvent
    );




NDIS_STATUS
PtPnPNetEventReconfigure(
    IN PADAPT            pAdapt,
    IN PNET_PNP_EVENT    pNetPnPEvent
    );    

NDIS_STATUS 
PtPnPNetEventSetPower (
    IN PADAPT                    pAdapt,
    IN PNET_PNP_EVENT            pNetPnPEvent
    );
    

//
// Miniport proto-types
//
NDIS_STATUS
MPInitialize(
    OUT PNDIS_STATUS             OpenErrorStatus,
    OUT PUINT                    SelectedMediumIndex,
    IN PNDIS_MEDIUM              MediumArray,
    IN UINT                      MediumArraySize,
    IN NDIS_HANDLE               MiniportAdapterHandle,
    IN NDIS_HANDLE               WrapperConfigurationContext
    );

VOID
MPSendPackets(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN PPNDIS_PACKET              PacketArray,
    IN UINT                       NumberOfPackets
    );

NDIS_STATUS
MPSend(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN PNDIS_PACKET               Packet,
    IN UINT                       Flags
    );

NDIS_STATUS
MPQueryInformation(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN NDIS_OID                   Oid,
    IN PVOID                      InformationBuffer,
    IN ULONG                      InformationBufferLength,
    OUT PULONG                    BytesWritten,
    OUT PULONG                    BytesNeeded
    );

NDIS_STATUS
MPSetInformation(
    IN NDIS_HANDLE                                      MiniportAdapterContext,
    IN NDIS_OID                                         Oid,
    __in_bcount(InformationBufferLength) IN PVOID       InformationBuffer,
    IN ULONG                                            InformationBufferLength,
    OUT PULONG                                          BytesRead,
    OUT PULONG                                          BytesNeeded
    );

VOID
MPReturnPacket(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN PNDIS_PACKET               Packet
    );

NDIS_STATUS
MPTransferData(
    OUT PNDIS_PACKET              Packet,
    OUT PUINT                     BytesTransferred,
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN NDIS_HANDLE                MiniportReceiveContext,
    IN UINT                       ByteOffset,
    IN UINT                       BytesToTransfer
    );

VOID
MPHalt(
    IN NDIS_HANDLE                MiniportAdapterContext
    );


VOID
MPQueryPNPCapabilities(  
    OUT PADAPT                    MiniportProtocolContext, 
    OUT PNDIS_STATUS              Status
    );


#ifdef NDIS51_MINIPORT

VOID
MPCancelSendPackets(
    IN NDIS_HANDLE            MiniportAdapterContext,
    IN PVOID                  CancelId
    );

VOID
MPAdapterShutdown(
    IN NDIS_HANDLE                MiniportAdapterContext
    );

VOID
MPDevicePnPEvent(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN NDIS_DEVICE_PNP_EVENT      DevicePnPEvent,
    IN PVOID                      InformationBuffer,
    IN ULONG                      InformationBufferLength
    );

#endif // NDIS51_MINIPORT

VOID
MPFreeAllPacketPools(
    IN PADAPT                    pAdapt
    );


VOID
MPProcessSetPowerOid(
    IN OUT PNDIS_STATUS                             pNdisStatus,
    IN PADAPT                                       pAdapt,
    __in_bcount(InformationBufferLength) IN PVOID   InformationBuffer,
    IN ULONG                                        InformationBufferLength,
    OUT PULONG                                      BytesRead,
    OUT PULONG                                      BytesNeeded
    );

VOID
PtReferenceAdapt(
    IN PADAPT     pAdapt
    );

BOOLEAN
PtDereferenceAdapt(
    IN PADAPT     pAdapt
    );

//
// There should be no DbgPrint's in the Free version of the driver
//
#if DBG

#define DBGPRINT(Fmt)                                        \
    {                                                        \
        KdPrint(("Passthru: "));                                \
        KdPrint(Fmt);                                        \
    }

#else // if DBG

#define DBGPRINT(Fmt)                                            

#endif // if DBG 

#define    NUM_PKTS_IN_POOL    256


//
// Protocol reserved part of a sent packet that is allocated by us.
//
typedef struct _SEND_RSVD
{
    PNDIS_PACKET    OriginalPkt;
} SEND_RSVD, *PSEND_RSVD;

//
// Miniport reserved part of a received packet that is allocated by
// us. Note that this should fit into the MiniportReserved space
// in an NDIS_PACKET.
//
typedef struct _RECV_RSVD
{
    PNDIS_PACKET    OriginalPkt;
} RECV_RSVD, *PRECV_RSVD;

C_ASSERT(sizeof(RECV_RSVD) <= sizeof(((PNDIS_PACKET)0)->MiniportReserved));

//
// Event Codes related to the PassthruEvent Structure
//

typedef enum 
{
    Passthru_Invalid,
    Passthru_SetPower,
    Passthru_Unbind

} PASSSTHRU_EVENT_CODE, *PPASTHRU_EVENT_CODE; 

// 
// 过滤结果变量
//
typedef enum{
	FILTER_STATUS_PASS,
	FILTER_STATUS_DROP,
	FILTER_STATUS_DROP_SEND,
	FILTER_STATUS_MODIFY_REDIRECT,
	FILTER_STATUS_MODIFY_SEND,
	FILTER_STATUS_MODIFY_RECEIVE,
}FILTER_STATUS;

//
// Passthru Event with  a code to state why they have been state
//

typedef struct _PASSTHRU_EVENT
{
    NDIS_EVENT Event;
    PASSSTHRU_EVENT_CODE Code;

} PASSTHRU_EVENT, *PPASSTHRU_EVENT;


typedef NTSTATUS 
(*AddDeviceFunc)(
				 IN PDRIVER_OBJECT  DriverObject,
				 IN PDEVICE_OBJECT  PhysicalDeviceObject 
				 );// 定义AddDevice函数指针变量类型

// 定义一个函数指针变量类型
typedef NTSTATUS 
(*DispatchFunc)(
				IN PDEVICE_OBJECT DeviceObject, 
				IN PIRP Irp
				);// 定义分发函数指针变量类型

extern AddDeviceFunc systemAddDevice; 
extern DispatchFunc systemCreate;
extern DispatchFunc systemWrite;
extern DispatchFunc systemRead;
extern DispatchFunc systemDeviceControl;

extern PDEVICE_OBJECT gDeviceObject;
//
// Structure used by both the miniport as well as the protocol part of the intermediate driver
// to represent an adapter and its corres. lower bindings
//
typedef struct _ADAPT
{
    struct _ADAPT *                Next;
    
    NDIS_HANDLE                    BindingHandle;    // To the lower miniport
    NDIS_HANDLE                    MiniportHandle;    // NDIS Handle to for miniport up-calls
    NDIS_HANDLE                    SendPacketPoolHandle;
    NDIS_HANDLE                    RecvPacketPoolHandle;
    NDIS_STATUS                    Status;            // Open Status
    NDIS_EVENT                     Event;            // Used by bind/halt for Open/Close Adapter synch.
    NDIS_MEDIUM                    Medium;
    NDIS_REQUEST                   Request;        // This is used to wrap a request coming down
                                                // to us. This exploits the fact that requests
                                                // are serialized down to us.
    PULONG                         BytesNeeded;
    PULONG                         BytesReadOrWritten;
    BOOLEAN                        ReceivedIndicationFlags[32];
    
    BOOLEAN                        OutstandingRequests;      // TRUE iff a request is pending
                                                        // at the miniport below
    BOOLEAN                        QueuedRequest;            // TRUE iff a request is queued at
                                                        // this IM miniport

    BOOLEAN                        StandingBy;                // True - When the miniport or protocol is transitioning from a D0 to Standby (>D0) State
    BOOLEAN                        UnbindingInProcess;
    NDIS_SPIN_LOCK                 Lock;
                                                        // False - At all other times, - Flag is cleared after a transition to D0

    NDIS_DEVICE_POWER_STATE        MPDeviceState;            // Miniport's Device State 
    NDIS_DEVICE_POWER_STATE        PTDeviceState;            // Protocol's Device State 
    NDIS_STRING                    DeviceName;                // For initializing the miniport edge
    NDIS_EVENT                     MiniportInitEvent;        // For blocking UnbindAdapter while
                                                        // an IM Init is in progress.
    BOOLEAN                        MiniportInitPending;    // TRUE iff IMInit in progress
    NDIS_STATUS                    LastIndicatedStatus;    // The last indicated media status
    NDIS_STATUS                    LatestUnIndicateStatus; // The latest suppressed media status
    ULONG                          OutstandingSends;
    LONG                           RefCount;
    BOOLEAN                        MiniportIsHalted;

	UCHAR							macAddr[6];
	BOOLEAN							bMac;
} ADAPT, *PADAPT;

extern    NDIS_HANDLE                        ProtHandle, DriverHandle;
extern    NDIS_MEDIUM                        MediumArray[21];
extern    PADAPT                             pAdaptList;
extern    NDIS_SPIN_LOCK                     GlobalLock;


#define ADAPT_MINIPORT_HANDLE(_pAdapt)    ((_pAdapt)->MiniportHandle)
#define ADAPT_DECR_PENDING_SENDS(_pAdapt)     \
    {                                         \
        NdisAcquireSpinLock(&(_pAdapt)->Lock);   \
        (_pAdapt)->OutstandingSends--;           \
        NdisReleaseSpinLock(&(_pAdapt)->Lock);   \
    }

//
// Custom Macros to be used by the passthru driver 
//
/*
BOOLEAN
IsIMDeviceStateOn(
   PADAPT 
   )

*/
#define IsIMDeviceStateOn(_pP)        ((_pP)->MPDeviceState == NdisDeviceStateD0 && (_pP)->PTDeviceState == NdisDeviceStateD0 ) 

//参考资料
//http://bbs.csdn.net/topics/350097612
//http://bbs.csdn.net/topics/300055754
//http://blog.chinaunix.net/uid-9195812-id-2006661.html


NDIS_STATUS getMemoryCopyFromPacket(PNDIS_PACKET Packet, PUCHAR *ppBuffer, UINT *pnBufferLen);
void freeMemory(PCHAR pBuffer);
NDIS_STATUS sendMemory(PADAPT adapt, PUCHAR pBuffer, ULONG nBufLen);
PNDIS_PACKET addPacketWithMemory_ReceivePool(PADAPT adapt, PNDIS_PACKET ndisPacket, PUCHAR pBuffer, ULONG nBufLen);
PNDIS_PACKET createPacketFromMemory_ReceivePool(PADAPT adapt, PUCHAR pBuffer, ULONG nBufLen);
PNDIS_PACKET createPacketFromMemory_SendPool(PADAPT adapt, PUCHAR pBuffer, ULONG nBufLen);

void freePacketBufferAndMemory(PNDIS_PACKET Packet);
void dprFreePacketBufferAndMemory(PNDIS_PACKET Packet);
void freeBufferAndMemory(PNDIS_PACKET Packet);

BOOLEAN checkMacAddrEqual(PUCHAR pMacAddr1, PUCHAR pMacAddr2);
void copyMacAddr(PUCHAR pMacAddrDst, PUCHAR pMacAddrSrc);

PADAPT getAdapterByMAC(PUCHAR pMAC);
PNT_Core_Rule getCoreRuleByCMAC(PUCHAR pCMAC);
PNT_Mapping_Rule getMappingRuleByIMAC(PUCHAR pIMAC);
PNT_Mapping_Rule getMappingRuleByPIP(ULONG nPIP);
PNT_Core_Rule getCoreRule();
PNT_Mapping_Rule getMappingRule();

USHORT getShortFromNetBytes(PUCHAR pData);
USHORT checkSum(USHORT *pBuf, UINT nBufLen);
void prepare34Bytes(PUCHAR pBuffer, UINT *pnBufLen);
void shrink34Bytes(PUCHAR pBuffer, UINT *pnBufLen);

BOOLEAN checkIPProtocol(PUCHAR pBuffer);
BOOLEAN checkVEPProtocol(PUCHAR pBuffer);
int checkARPProtocol(PUCHAR pBuffer);
BOOLEAN checkRARPProtocol(PUCHAR pBuffer);
BOOLEAN checkIPv6Protocol(PUCHAR pBuffer);

void writeEthHeaderForIP(PUCHAR pBuffer, PUCHAR pDstAddr, PUCHAR pSrcAddr);
void writeEthHeaderForARP(PUCHAR pBuffer, PUCHAR pDstAddr, PUCHAR pSrcAddr);
void writeIPHeader(PUCHAR pBuffer, ULONG nDstIP, ULONG nSrcIP, USHORT nTotalLen, BOOLEAN bVEPFlag);
void writeARPHeader(PUCHAR pBuffer, PUCHAR pDstAddr, PUCHAR pSrcAddr, ULONG nDstIP, ULONG nSrcIP, BOOLEAN bRequestOrReply);
NDIS_STATUS sendARPResponse(PADAPT pAdapt, PUCHAR pDstAddr, PUCHAR pSrcAddr, ULONG nDstIP, ULONG nSrcIP);
NDIS_STATUS sendDebugPacket(NT_Debug_Rule *pDebugRule);

PCHAR stringFromNetworkIP1(ULONG nIP);
PCHAR stringFromNetworkIP2(ULONG nIP);
PCHAR stringFromNetworkIP3(ULONG nIP);
PCHAR stringFromNetworkIP4(ULONG nIP);
PCHAR stringFromHostIP1(ULONG nIP);
PCHAR stringFromHostIP2(ULONG nIP);
PCHAR stringFromMAC1(PUCHAR pMac);
PCHAR stringFromMAC2(PUCHAR pMac);
PCHAR stringFromMAC3(PUCHAR pMac);
PCHAR stringFromMAC4(PUCHAR pMac);

FILTER_STATUS analyzeBuffer(PADAPT pAdapt, PUCHAR pBuffer, UINT *pnBufLen, int bRecvOrSend);
FILTER_STATUS analyzeBufferEx(PADAPT pAdapt, PUCHAR pBuffer, UINT *pnBufLen, int bRecvOrSend, PADAPT *ppForwardAdapt);

#define NdisTestPacketFlags(_Packet, _Flags)     (_Packet)->Private.Flags & (_Flags)