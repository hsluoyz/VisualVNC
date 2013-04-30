// SocketManager.cpp: implementation of the CSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlconv.h>
#include "SocketManager.h"

#include "resource.h"
#include "MainFrm.h"

#include "MyRichEditCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
const UINT EVT_CONSUCCESS = 0x0000;	// Connection established
const UINT EVT_CONFAILURE = 0x0001;	// General failure - Wait Connection failed
const UINT EVT_CONDROP	  = 0x0002;	// Connection dropped
const UINT EVT_ZEROLENGTH = 0x0003;	// Zero length message
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSocketManager::CSocketManager(int iIndex)
: m_pMsgCtrl(NULL)
{
	m_iIndex = iIndex;
}

CSocketManager::~CSocketManager()
{

}


void CSocketManager::DisplayData(const LPBYTE lpData, DWORD dwCount, const SockAddrIn& sfrom)
{
	CString strData;
	memcpy(strData.GetBuffer(dwCount), A2CT((LPSTR)lpData), dwCount);
	strData.ReleaseBuffer();
	if (!sfrom.IsNull())
	{
		LONG  uAddr = sfrom.GetIPAddr();
		BYTE* sAddr = (BYTE*) &uAddr;
		short nPort = ntohs( sfrom.GetPort() );	// show port in host format...
		CString strAddr;
		// Address is stored in network format...
		strAddr.Format(_T("%u.%u.%u.%u (%d)>"),
					(UINT)(sAddr[0]), (UINT)(sAddr[1]),
					(UINT)(sAddr[2]), (UINT)(sAddr[3]), nPort);

		strData = strAddr + strData;
	}

	AppendMessage( strData );
}


void CSocketManager::AppendMessage(CString strText)
{
	if (NULL == m_pMsgCtrl)
		return;
	/*
	if (::IsWindow( m_pMsgCtrl->GetSafeHwnd() ))
	{
		int nLen = m_pMsgCtrl->GetWindowTextLength();
		m_pMsgCtrl->SetSel(nLen, nLen);
		m_pMsgCtrl->ReplaceSel( strText );
	}
	*/
	//HWND hWnd = m_pMsgCtrl->GetSafeHwnd();


	PMParam *pParam = new PMParam(m_pMsgCtrl, strText);
	CMainFrame *pMainFrm = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainFrm->GetActiveView()->PostMessage(ID_GUI_MESSAGE + 11, (WPARAM) pParam, NULL);


	/*
	DWORD dwResult = 0;
	if (SendMessageTimeout(hWnd, WM_GETTEXTLENGTH, 0, 0, SMTO_NORMAL, 1000L, &dwResult) != 0)
	{
		int nLen = (int) dwResult;
		if (SendMessageTimeout(hWnd, EM_SETSEL, nLen, nLen, SMTO_NORMAL, 1000L, &dwResult) != 0)
		{
			if (SendMessageTimeout(hWnd, EM_REPLACESEL, FALSE, (LPARAM)strText, SMTO_NORMAL, 1000L, &dwResult) != 0)
			{
			}
		}

	}
	*/
}


void CSocketManager::SetMessageWindow(CMyRichEditCtrl* pMsgCtrl)
{
	m_pMsgCtrl = pMsgCtrl;
}


void CSocketManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	LPBYTE lpData = lpBuffer;
	SockAddrIn origAddr;
	stMessageProxy msgProxy;
	if (IsSmartAddressing())
	{
		dwCount = __min(sizeof(msgProxy), dwCount);
		memcpy(&msgProxy, lpBuffer, dwCount);
		origAddr = msgProxy.address;
		if (IsServer())
		{
			// broadcast message to all
			msgProxy.address.sin_addr.s_addr = htonl(INADDR_BROADCAST);
			WriteComm((const LPBYTE)&msgProxy, dwCount, 0L);
		}
		dwCount -= sizeof(msgProxy.address);
		lpData = msgProxy.byData;
	}

	// Display data to message list
	DisplayData( lpData, dwCount, origAddr );
}

CString CSocketManager::itos(int i)
{
	CString strTemp;
	strTemp.Format(_T("%d"), i);
	return strTemp;
}

///////////////////////////////////////////////////////////////////////////////
// OnEvent
// Send message to parent window to indicate connection status
void CSocketManager::OnEvent(UINT uEvent)
{
	if (NULL == m_pMsgCtrl)
		return;

	CWnd* pParent = m_pMsgCtrl->GetParent();
	if (!::IsWindow( pParent->GetSafeHwnd()))
		return;

	CString strServer = _T("Server No. ") + itos(m_iIndex) + _T(": ");
	switch( uEvent )
	{
		case EVT_CONSUCCESS:
			AppendMessage(strServer + _T("Connection Established\r\n") );
			m_bConnected = TRUE;
			break;
		case EVT_CONFAILURE:
			AppendMessage(strServer + _T("Connection Failed\r\n") );
			m_bConnected = FALSE;
			break;
		case EVT_CONDROP:
			AppendMessage(strServer + _T("Connection Abandonned\r\n") );
			m_bConnected = FALSE;
			break;
		case EVT_ZEROLENGTH:
			AppendMessage(strServer + _T("Zero Length Message\r\n") );
			m_bConnected = FALSE;
			break;
		default:
			TRACE("Unknown Socket event\n");
			m_bConnected = FALSE;
			break;
	}

	Output_Update_Connection(uEvent, this);
	//pParent->PostMessage(WM_UPDATE_CONNECTION, uEvent, (LPARAM) this);

}
