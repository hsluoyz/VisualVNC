#include "StdAfx.h"

#include "VNDriverLoaderEx.h"
#include "bindview.h"

#include "CommonIO.h"
#include <WinIoCtl.h>

VNDriverLoaderEx::VNDriverLoaderEx()
{
	m_bInstalled = FALSE;
	initDataDir();
	m_strDriverLinkName = _T("\\\\.\\VNCPassThru");
	m_hDevice = NULL;
}

VNDriverLoaderEx::~VNDriverLoaderEx()
{
	if (m_bInstalled)
	{
		uninstallDriver_Passthru();
	}
	closeDeviceLink();
}

VNDriverLoaderEx::setJustUnload()
{
	m_bInstalled = TRUE;
}

BOOL VNDriverLoaderEx::FolderExist(CString strPath)
{
	WIN32_FIND_DATA wfd;
	BOOL rValue = FALSE;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = TRUE;
	}
	FindClose(hFind);
	return rValue;
}

BOOL VNDriverLoaderEx::FileExist(CString strPathName)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(strPathName, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	else
	{
		FindClose(hFind);
		return true;
	}
	return false;
}

void VNDriverLoaderEx::initDataDir()
{
	CString cstrDir;
	TCHAR cBuffer[260];
	GetCurrentDirectory(MAX_PATH, cBuffer);
	CString cstrPath(cBuffer);
	if (FolderExist(cstrPath + _T("\\data")))
	{
		cstrDir = cstrPath;
	}
	else
	{
		int nCount = cstrPath.ReverseFind(_T('\\'));
		cstrPath.Delete(nCount, cstrPath.GetLength() - nCount);

		if (FolderExist(cstrPath + _T("\\data")))
		{
			cstrDir = cstrPath;
		}
		else
		{
			MyMessageBox_Error(_T("initDataDir"));
			return;
		}
	}
	cstrDir += _T("\\data\\");
	m_strDataDirectory = cstrDir;
}

LRESULT VNDriverLoaderEx::MyMessageBox(CString strText, CString strCaption, UINT iType)
{
	BCGP_MSGBOXPARAMS params;
	
	params.lpszCaption = strCaption;
	params.lpszText = strText;
	params.bDrawButtonsBanner = TRUE;
	params.bUseNativeControls = FALSE;
	
	params.dwStyle = iType;
	params.bIgnoreStandardButtons = FALSE;
	params.bShowCheckBox = FALSE;
	
	params.dwStyle |= MB_ICONEXCLAMATION;
	
	int result = BCGPMessageBoxIndirect(&params);
	return result;
}

LRESULT VNDriverLoaderEx::MyMessageBox_YESNO(CString strText, CString strCaption)
{
	BCGP_MSGBOXPARAMS params;
	
	params.lpszCaption = strCaption;
	params.lpszText = strText;
	params.bDrawButtonsBanner = TRUE;
	params.bUseNativeControls = FALSE;
	
	params.dwStyle = MB_YESNO;
	params.bIgnoreStandardButtons = FALSE;
	params.bShowCheckBox = FALSE;
	
	params.dwStyle |= MB_ICONEXCLAMATION;
	
	int result = BCGPMessageBoxIndirect(&params);
	return result;
}

CString VNDriverLoaderEx::getServiceInfFilePath_Passthru()
{
	CString strPathName = m_strDataDirectory + _T("netsf.inf");
	return strPathName;
}

CString VNDriverLoaderEx::getServiceInfFilePath_PassthruMP()
{
	CString strPathName = m_strDataDirectory + _T("netsf_m.inf");
	return strPathName;
}

DWORD VNDriverLoaderEx::getServiceInfFilePath_Passthru(LPTSTR lpFilename, DWORD nSize)
{
	// Get Path to This Module
	DWORD nResult;
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];

	nResult = GetModuleFileName(NULL, lpFilename, nSize);

	if (nResult == 0)
	{
		return 0;
	}

	_tsplitpath(lpFilename, szDrive, szDir, NULL, NULL);

	_tmakepath(lpFilename, szDrive, szDir, _T("netsf"), _T(".inf"));

	return (DWORD)_tcslen(lpFilename);
}

DWORD VNDriverLoaderEx::getServiceInfFilePath_PassthruMP(LPTSTR lpFilename, DWORD nSize)
{
	// Get Path to This Module
	DWORD nResult;
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];

	nResult = GetModuleFileName(NULL, lpFilename, nSize);

	if (nResult == 0)
	{
		return 0;
	}

	_tsplitpath(lpFilename, szDrive, szDir, NULL, NULL);

	_tmakepath(lpFilename, szDrive, szDir, _T("netsf_m"), _T(".inf"));

	return (DWORD)_tcslen(lpFilename);
}



DWORD VNDriverLoaderEx::installDriver_Passthru()
{
	if (m_bInstalled)
	{
		MyMessageBox_Error(_T("The driver has been installed."), _T("Run Info"));
		return 0;
	}

	DWORD nResult;

	//_tprintf( _T("Installing %s...\n"), NDISPROT_FRIENDLY_NAME );

	nResult = MyMessageBox(_T("你要安装 Passthru 网络驱动？"), _T("Passthru网络过滤驱动"), MB_OKCANCEL | MB_ICONINFORMATION);

	if (nResult != IDOK)
	{
		return 0;
	}

	// Get Path to Service INF File
	// ----------------------------
	// The INF file is assumed to be in the same folder as this application...
	TCHAR szFileFullPath[_MAX_PATH];


	//-----------------------------------------------------------------------
	//第一次在一个系统上使用这个程序安装Passthru时，会出现安装失败的情况，或者
	//安装成功，但passthru miniport的部分没有安装上去,在windows 目录下 的setupapi.log文件中可以看到
	//安装失败记录，错误是找不到文件。在“设备管理器”选择显示隐藏设备后
	//也不会在 网络适配器 下面看到 passthru miniport项。但手动安装在'本地网络属性"->"安装"->”服务“选择
	//硬盘上netsf.inf进行安装成功候，再用程序安装就可以了。
	//在网络上查了一下，这个问题应该是因为 Passthru这个驱动需要两个inf文件，而netsf_m.inf并不没有被复制到
	//系统的inf 目录（c:\windows\inf）去。虽然 netsf.inf 里面有[CopyInf   = netsf_m.inf]项要求复制netsf_m.inf
	//但这个不能 正常工作（The \"CopyINF\" directive, by design, is only observed if the original INF is 
	//not yet in the INF directory. So to work around the problem, you have to 
	//update your installation app (snetcfg) to also copy the Net class (miniport) 
	//inf using SetupCopyOEMInf when it comes to installing IM drivers. Make sure 
	//you specify a fully qualified path of the INF in the SetupCopyOEMInf 
	//arguments. 
	//）
	//
	//所以这个问题的解决就是自己把 netsf_m.inf这个文件放到c:\windows\inf目录去。这可以通过在 netsf.inf里面添加
	//copyfile项，像copy Passthru.sys一样添加一项copy   netsf_m.inf的项。另一种方法就是像下面这样添加调用
	//SetupCopyOEMInfW来复制netsf_m.inf的代码
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDirWithDrive[_MAX_DRIVE + _MAX_DIR];
	CString strMPInfPathName = getServiceInfFilePath_PassthruMP();
	if (!FileExist(strMPInfPathName))
	{
		//_tprintf( _T("Unable to get INF file path.\n") );
		MyMessageBox(_T("获取INF文件路径失败！"), _T("安装程序错误提示"), MB_OK);
		return 0;
	}
	//
	// Get the path where the INF file is.
	//
	_tcscpy(szFileFullPath, strMPInfPathName);

	_tsplitpath(szFileFullPath, szDrive, szDir, NULL, NULL);

	_tcscpy(szDirWithDrive, szDrive);
	_tcscat(szDirWithDrive, szDir);
	if (!SetupCopyOEMInf(szFileFullPath, szDirWithDrive, // Other files are in the
			 // same dir. as primary INF
			 SPOST_PATH,	 // First param is path to INF
			 0, 			 // Default copy style
			 NULL,  		 // Name of the INF after
			 // it's copied to %windir%\inf
			 0, 			 // Max buf. size for the above
			 NULL,  		 // Required size if non-null
			 NULL)  		 // Optionally get the filename
		// part of Inf name after it is copied.
	   )
	{
		MyMessageBox(_T("复制 PasstruMP 的inf安装文件到系统目录失败！"), _T("安装程序错误提示"), MB_OK);
	}
	//------------------------------------------------------------------------


	CString strInfPathName = getServiceInfFilePath_Passthru();

	if (!FileExist(strInfPathName))
	{
		//     _tprintf( _T("Unable to get INF file path.\n") );
		MyMessageBox(_T("获取INF文件路径失败！"), _T("安装程序错误提示"), MB_OK);
		return 0;
	}

	//_tprintf( _T("INF Path: %s\n"), szFileFullPath );

	HRESULT hr = S_OK;

	//_tprintf( _T("PnpID: %s\n"),   _T( "ms_passthru"));
	_tcscpy(szFileFullPath, strInfPathName);

	_bstr_t bstrFileFullPath = szFileFullPath;
	_bstr_t bstrPnpID = "ms_passthru";
	hr = InstallSpecifiedComponent(bstrFileFullPath,  				  //驱动安装的inf文件路径 , 适当修改吧
		 bstrPnpID, 				// NDISPROT_SERVICE_PNP_DEVICE_ID,    //这个也要适当修改的
		 &GUID_DEVCLASS_NETSERVICE  		  //NDIS Protocal 类型
	);

	if (hr != S_OK)
	{
		/*ErrMsg( hr, L"InstallSpecifiedComponent\n" );*/
		MyMessageBox(_T("安装驱动失败！"), _T("安装程序错误提示"), MB_OK);
	}
	else
	{
		m_bInstalled = TRUE;
		MessageBox(NULL, _T("安装驱动成功！"), _T("安装程序提示"), MB_OK);
	}

	return 0;
}

DWORD VNDriverLoaderEx::uninstallDriver_Passthru()
{
	if (!m_bInstalled)
	{
		MyMessageBox_Error(_T("The driver hasn't been installed."), _T("Run Info"));
		return 0;
	}
	//_tprintf( _T("Uninstalling %s...\n"), NDISPROT_FRIENDLY_NAME );

	int nResult = MyMessageBox(_T("你要卸载Passthru网络驱动？"), _T("Passthru网络过滤驱动"), MB_OKCANCEL | MB_ICONINFORMATION);

	if (nResult != IDOK)
	{
		return 0;
	}

	INetCfg* pnc;
	INetCfgComponent* pncc;
	INetCfgClass* pncClass;
	INetCfgClassSetup* pncClassSetup;
	LPWSTR lpszApp;
	GUID guidClass;
	OBO_TOKEN obo;
	HRESULT hr;

	hr = HrGetINetCfg(TRUE, APP_NAME, &pnc, &lpszApp);

	if (hr == S_OK)
	{
		//
		// Get a reference to the network component to uninstall.
		//
		hr = pnc->FindComponent(L"ms_passthru", &pncc);

		if (hr == S_OK)
		{
			//
			// Get the class GUID.
			//
			hr = pncc->GetClassGuid(&guidClass);

			if (hr == S_OK)
			{
				//
				// Get a reference to component's class.
				//

				hr = pnc->QueryNetCfgClass(&guidClass, IID_INetCfgClass, (PVOID *)&pncClass);
				if (hr == S_OK)
				{
					//
					// Get the setup interface.
					//

					hr = pncClass->QueryInterface(IID_INetCfgClassSetup, (LPVOID *)&pncClassSetup);

					if (hr == S_OK)
					{
						//
						// Uninstall the component.
						//

						ZeroMemory(&obo, sizeof(OBO_TOKEN));

						obo.Type = OBO_USER;

						hr = pncClassSetup->DeInstall(pncc, &obo, NULL);
						if ((hr == S_OK) || (hr == NETCFG_S_REBOOT))
						{
							hr = pnc->Apply();

							if ((hr != S_OK) && (hr != NETCFG_S_REBOOT))
							{
								//ErrMsg( hr,
								//     L"Couldn't apply the changes after"
								//     L" uninstalling %s.",
								//     _T ("ms_passthru" ));
								MyMessageBox(_T("卸载驱动之后无法成功应用！"), _T("安装程序错误提示"), MB_OK);
							}
							else
							{
								m_bInstalled = FALSE;
								MyMessageBox(_T("成功卸载驱动！"), _T("安装程序提示"), MB_OK);
							}
						}
						else
						{
							//ErrMsg( hr,
							//     L"Failed to uninstall %s.",
							//     _T("ms_passthru" ));
							MyMessageBox(_T("卸载网络组件失败！"), _T("安装程序错误提示"), MB_OK);
						}

						ReleaseRef(pncClassSetup);
					}
					else
					{
						//ErrMsg( hr,
						//     L"Couldn't get an interface to setup class." );
						MyMessageBox(_T("无法得到安装类接口！"), _T("安装程序错误提示"), MB_OK);
					}

					ReleaseRef(pncClass);
				}
				else
				{
					//ErrMsg( hr,
					//     L"Couldn't get a pointer to class interface "
					//     L"of %s.",
					//     _T ("ms_passthru") );
					MessageBox(NULL, _T("无法得到安装类接口！"), _T("安装程序错误提示"), MB_OK);
				}
			}
			else
			{
				//ErrMsg( hr,
				//     L"Couldn't get the class guid of %s.",
				//     _T("ms_passthru") );
				MyMessageBox(_T("无法得到安装类接口的 GUID ！"), _T("安装程序错误提示"), MB_OK);
			}

			ReleaseRef(pncc);
		}
		else
		{
			//ErrMsg( hr,
			//     L"Couldn't get an interface pointer to %s.",
			//     _T ("ms_passthru") );

			//MyMessageBox(_T("无法得到一个接口指针！"), _T("安装程序错误提示"), MB_OK);
			MyMessageBox(_T("驱动不存在或已处于卸载状态！"), _T("安装程序错误提示"), MB_OK);
		}

		HrReleaseINetCfg(pnc, TRUE);
	}
	else
	{
		if ((hr == NETCFG_E_NO_WRITE_LOCK) && lpszApp)
		{
			//   ErrMsg( hr,
			//  	  L"%s currently holds the lock, try later.",
			//  	 lpszApp );
			MyMessageBox(_T("碰到死锁问题，稍后再试！"), _T("安装程序错误提示"), MB_OK);
			CoTaskMemFree(lpszApp);
		}
		else
		{
			// ErrMsg( hr, L"Couldn't get the notify object interface." );
			MyMessageBox(_T("无法得到通知对象接口！"), _T("安装程序错误提示"), MB_OK);
		}
	}

	return 0;
}

BOOL VNDriverLoaderEx::openDeviceLink()
{
	if (m_hDevice)
	{
		return TRUE;
		/*
		if (queryAlive())
		{
			return TRUE;
		}
		else
		{
			m_hDevice = NULL;
		}
		*/
	}

	// 打开到驱动程序所控制设备的句柄
	m_hDevice = ::CreateFile(m_strDriverLinkName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		m_hDevice = NULL;
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL VNDriverLoaderEx::closeDeviceLink()
{
	if (m_hDevice)
	{
		BOOL bRet = ::CloseHandle(m_hDevice);
		if (bRet)
		{
			m_hDevice = NULL;
			return TRUE;
		}
		else
		{
			MyMessageBox_Error(_T("closeDeviceLink"));
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}
}

BOOL VNDriverLoaderEx::writeAndReadFromNdis(ULONG nCTLCode, UCHAR* pInData, int nInDataLen, UCHAR* pOutData, int nOutDataLen,
											int &nRetDataLen)
{
	if (!openDeviceLink())
	{
		//MyMessageBox_Error(_T("writeAndReadFromNdis"));
		return FALSE;
	}

	ULONG ulRetDataLen;
	BOOL bRet = ::DeviceIoControl(m_hDevice,
		nCTLCode,					// 功能号
		pInData,					// 输入缓冲，要传递的信息，预先填好
		nInDataLen,					// 输入缓冲长度
		pOutData,					// 没有输出缓冲
		nOutDataLen,				// 输出缓冲的长度为0
		&ulRetDataLen,				// 返回的长度	
		NULL);
	nRetDataLen = ulRetDataLen;

	if (bRet == 0)
	{
		MyMessageBox_Error(_T("writeAndReadFromNdis"));
		closeDeviceLink();
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL VNDriverLoaderEx::writeToNdis(ULONG nCTLCode, UCHAR* pData, int nDataLen, int &nRetDataLen)
{
	if (!openDeviceLink())
	{
		//MyMessageBox_Error(_T("writeToNdis"));
		return FALSE;
	}
	
	ULONG ulRetDataLen;
	BOOL bRet = ::DeviceIoControl(m_hDevice,
		nCTLCode,					// 功能号
		NULL,						// 
		0,							// 
		pData,						// 
		nDataLen,					// 
		&ulRetDataLen,				// 返回的长度	
		NULL);
	nRetDataLen = ulRetDataLen;
	
	if (bRet == 0)
	{
		MyMessageBox_Error(_T("writeToNdis"));
		closeDeviceLink();
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL VNDriverLoaderEx::readFromNdis(ULONG nCTLCode, UCHAR* pData, int nDataLen, int &nRetDataLen)
{
	if (!openDeviceLink())
	{
		//MyMessageBox_Error(_T("readFromNdis"));
		return FALSE;
	}

	ULONG ulRetDataLen;
	BOOL bRet = ::DeviceIoControl(m_hDevice,
		nCTLCode,					// 功能号
		NULL,						// 
		0,							// 
		pData,						// 
		nDataLen,					// 
		&ulRetDataLen,				// 返回的长度	
		NULL);
	nRetDataLen = ulRetDataLen;

	if (bRet == 0)
	{
		MyMessageBox_Error(_T("readFromNdis"));
		closeDeviceLink();
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL VNDriverLoaderEx::queryAlive()
{
	UCHAR pInputBuffer[100];
	int iRetBufLen = 0;
	readFromNdis(IOCTL_VNCIO_SIMPLE_READ, pInputBuffer, 100, iRetBufLen);
	if (iRetBufLen > 0)
	{
		CString strHelloWord = pInputBuffer; //"hello from vncpassthru"
		if (strHelloWord == _T("hello from vncpassthru"))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}