#include "StdAfx.h"

//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 2001.
//
//  File:   	B I N D V I E W . C P P
//
//  Contents:  
//
//  Notes:  	
//
//  Author: 	Alok Sinha    15-Amy-01
//
//----------------------------------------------------------------------------


#include "BindView.h"

//----------------------------------------------------------------------------
// Globals
//

//
// Image list for devices of various setup class.
//


// 
// SP_CLASSIMAGELIST_DATA ClassImageListData;
// 
// HINSTANCE hInstance;
// HMENU hMainMenu;
// HMENU hComponentSubMenu;
// HMENU hBindingPathSubMenu;
// 
// //
// // Network components whose bindings are enumerated.
// //
// 
// LPWSTR lpszNetClass[] =
// {
// 	L"All Clients", L"All Services", L"All Protocols"
// };
// 
// //
// // GUIDs of network components.
// //
// 
// const GUID* pguidNetClass[] =
// {
// 	&GUID_DEVCLASS_NETCLIENT, & GUID_DEVCLASS_NETSERVICE, & GUID_DEVCLASS_NETTRANS, & GUID_DEVCLASS_NET
// };
// 
// DWORD GetServiceInfFilePath_Passthru(LPTSTR lpFilename, DWORD nSize)
// {
// 
// 	// Get Path to This Module
// 	DWORD nResult;
// 	TCHAR szDrive[_MAX_DRIVE];
// 	TCHAR szDir[_MAX_DIR];
// 
// 	nResult = GetModuleFileName(NULL, lpFilename, nSize);
// 
// 	if (nResult == 0)
// 	{
// 		return 0;
// 	}
// 
// 	_wsplitpath(lpFilename, szDrive, szDir, NULL, NULL);
// 
// 	_wmakepath(lpFilename, szDrive, szDir, L"netsf", L".inf");
// 
// 	return (DWORD)wcslen(lpFilename);
// }
// 
// DWORD GetServiceInfFilePath_PassthruMP(LPTSTR lpFilename, DWORD nSize)
// {
// 	// Get Path to This Module
// 	DWORD nResult;
// 	TCHAR szDrive[_MAX_DRIVE];
// 	TCHAR szDir[_MAX_DIR];
// 
// 	nResult = GetModuleFileName(NULL, lpFilename, nSize);	
// 	if (nResult == 0)
// 	{
// 		return 0;
// 	}
// 
// 	_wsplitpath(lpFilename, szDrive, szDir, NULL, NULL);
// 
// 	_wmakepath(lpFilename, szDrive, szDir, L"netsf_m", L".inf");
// 
// 	return (DWORD)wcslen(lpFilename);
// }
// 
// DWORD InstallDriver_Passthru()
// {
// 	DWORD nResult;	
// 	// Get Path to Service INF File
// 	// ----------------------------
// 	// The INF file is assumed to be in the same folder as this application...
// 	TCHAR szFileFullPath[_MAX_PATH];
// 
// 
// 	//-----------------------------------------------------------------------
// 	//��һ����һ��ϵͳ��ʹ���������װPassthruʱ������ְ�װʧ�ܵ����������
// 	//��װ�ɹ�����passthru miniport�Ĳ���û�а�װ��ȥ,��windows Ŀ¼�� ��setupapi.log�ļ��п��Կ���
// 	//��װʧ�ܼ�¼���������Ҳ����ļ����ڡ��豸��������ѡ����ʾ�����豸��
// 	//Ҳ������ ���������� ���濴�� passthru miniport����ֶ���װ��'������������"->"��װ"->������ѡ��
// 	//Ӳ����netsf.inf���а�װ�ɹ������ó���װ�Ϳ����ˡ�
// 	//�������ϲ���һ�£��������Ӧ������Ϊ Passthru���������Ҫ����inf�ļ�����netsf_m.inf����û�б����Ƶ�
// 	//ϵͳ��inf Ŀ¼��c:\windows\inf��ȥ����Ȼ netsf.inf ������[CopyInf   = netsf_m.inf]��Ҫ����netsf_m.inf
// 	//��������� ����������The \"CopyINF\" directive, by design, is only observed if the original INF is 
// 	//not yet in the INF directory. So to work around the problem, you have to 
// 	//update your installation app (snetcfg) to also copy the Net class (miniport) 
// 	//inf using SetupCopyOEMInf when it comes to installing IM drivers. Make sure 
// 	//you specify a fully qualified path of the INF in the SetupCopyOEMInf 
// 	//arguments. 
// 	//��
// 	//
// 	//�����������Ľ�������Լ��� netsf_m.inf����ļ��ŵ�c:\windows\infĿ¼ȥ�������ͨ���� netsf.inf��������
// 	//copyfile���copy Passthru.sysһ������һ��copy   netsf_m.inf�����һ�ַ��������������������ӵ���
// 	//SetupCopyOEMInfW������netsf_m.inf�Ĵ���
// 	TCHAR szDrive[_MAX_DRIVE];
// 	TCHAR szDir[_MAX_DIR];
// 	TCHAR szDirWithDrive[_MAX_DRIVE + _MAX_DIR];
// 	nResult = GetServiceInfFilePath_PassthruMP(szFileFullPath, MAX_PATH);
// 	if (nResult == 0)
// 	{
// 		//_tprintf( "Unable to get INF file path.\n") );
// 		MessageBoxW(NULL, L"��ȡINF�ļ�·��ʧ�ܣ�", L"Passthru��װ���������ʾ", MB_OK);
// 		return 0;
// 	}
// 	//
// 	// Get the path where the INF file is.
// 	//
// 	_wsplitpath(szFileFullPath, szDrive, szDir, NULL, NULL);
// 
// 	wcscpy(szDirWithDrive, szDrive);
// 	wcscat(szDirWithDrive, szDir);
// 	if (!SetupCopyOEMInfW((PWSTR)szFileFullPath, (PWSTR)szDirWithDrive, // Other files are in the
// 		// same dir. as primary INF
// 		SPOST_PATH, 	// First param is path to INF
// 		0,  			// Default copy style
// 		NULL,   		// Name of the INF after
// 		// it's copied to %windir%\inf
// 		0,  			// Max buf. size for the above
// 		NULL,   		// Required size if non-null
// 		NULL)   		// Optionally get the filename
// 		// part of Inf name after it is copied.
// 	   )
// 	{
// 		MessageBoxW(NULL, L"���� PasstruMP ��inf��װ�ļ���ϵͳĿ¼ʧ�ܣ�", L"Passthru��װ���������ʾ", MB_OK);
// 	}
// 	//------------------------------------------------------------------------
// 
// 
// 	nResult = GetServiceInfFilePath_Passthru(szFileFullPath, MAX_PATH);
// 
// 	if (nResult == 0)
// 	{
// 		//     _tprintf( "Unable to get INF file path.\n" );
// 		MessageBoxW(NULL, L"��ȡINF�ļ�·��ʧ�ܣ�", L"Passthru��װ���������ʾ", MB_OK);
// 		return 0;
// 	}
// 
// 	//_tprintf( "INF Path: %s\n", szFileFullPath );
// 
// 	HRESULT hr = S_OK;
// 
// 	//_tprintf( "PnpID: %s\n"),    "ms_passthru");
// 
// 	WCHAR wszPassthru[] = L"ms_passthru" ;
// 	hr = InstallSpecifiedComponent((LPWSTR)szFileFullPath,  				  //������װ��inf�ļ�·�� , �ʵ��޸İ�
// 		(LPWSTR)wszPassthru,			   // NDISPROT_SERVICE_PNP_DEVICE_ID,    //���ҲҪ�ʵ��޸ĵ�
// 		pguidNetClass[1]			//NDIS Protocal ����
// 	);
// 
// 	if (hr != S_OK)
// 	{
// 		/*ErrMsg( hr, L"InstallSpecifiedComponent\n" );*/
// 		MessageBoxW(NULL, L"��װ����ʧ�ܣ�", L"Passthru��װ���������ʾ", MB_OK);
// 	}
// 	else
// 	{
// 		MessageBoxW(NULL, L"�ɹ���װ������", L"Passthru��װ������ʾ", MB_OK);
// 	}
// 
// 	return 0;
// }
// 
// 
// DWORD UninstallDriver_Passthru()
// {
// 	INetCfg* pnc;
// 	INetCfgComponent* pncc;
// 	INetCfgClass* pncClass;
// 	INetCfgClassSetup* pncClassSetup;
// 	LPTSTR lpszApp;
// 	GUID guidClass;
// 	OBO_TOKEN obo;
// 	HRESULT hr;
// 
// 	hr = HrGetINetCfg(TRUE, APP_NAME, &pnc, &lpszApp);
// 	if (hr == S_OK)
// 	{
// 		//
// 		// Get a reference to the network component to uninstall.
// 		//
// 		hr = pnc->FindComponent(L"ms_passthru", &pncc);	  
// 		if (hr == S_OK)
// 		{
// 			//
// 			// Get the class GUID.
// 			//
// 			hr = pncc->GetClassGuid(&guidClass);
// 
// 			if (hr == S_OK)
// 			{
// 				//
// 				// Get a reference to component's class.
// 				//
// 
// 				hr = pnc->QueryNetCfgClass(&guidClass, IID_INetCfgClass, (PVOID *)&pncClass);
// 				if (hr == S_OK)
// 				{
// 					//
// 					// Get the setup interface.
// 					//
// 
// 					hr = pncClass->QueryInterface(IID_INetCfgClassSetup, (LPVOID *)&pncClassSetup);
// 
// 					if (hr == S_OK)
// 					{
// 						//
// 						// Uninstall the component.
// 						//
// 
// 						ZeroMemory(&obo, sizeof(OBO_TOKEN));
// 
// 						obo.Type = OBO_USER;
// 
// 						hr = pncClassSetup->DeInstall(pncc, &obo, NULL);
// 						if ((hr == S_OK) || (hr == NETCFG_S_REBOOT))
// 						{
// 							hr = pnc->Apply();
// 
// 							if ((hr != S_OK) && (hr != NETCFG_S_REBOOT))
// 							{
// 								MessageBoxW(NULL, L"ж������֮���޷��ɹ�Ӧ��!", L"Passthru��װ���������ʾ", MB_OK);
// 							}
// 							else
// 							{
// 								MessageBoxW(NULL, L"�ɹ�ж������!", L"Passthru��װ������ʾ", MB_OK);
// 							}
// 						}
// 						else
// 						{
// 							MessageBoxW(NULL, L"ж���������ʧ��!", L"Passthru��װ���������ʾ", MB_OK);
// 						}
// 
// 						ReleaseRef(pncClassSetup);
// 					}
// 					else
// 					{
// 						MessageBoxW(NULL, L"�޷��õ���װ��ӿ�!", L"Passthru��װ���������ʾ", MB_OK);
// 					}
// 
// 					ReleaseRef(pncClass);
// 				}
// 				else
// 				{
// 					MessageBoxW(NULL, L"�޷��õ���װ��ӿ�!", L"Passthru��װ���������ʾ", MB_OK);
// 				}
// 			}
// 			else
// 			{
// 				MessageBoxW(NULL, L"�޷��õ���װ��ӿڵ� GUID!", L"Passthru��װ���������ʾ", MB_OK);
// 			}
// 
// 			ReleaseRef(pncc);
// 		}
// 		else
// 		{
// 			MessageBoxW(NULL, L"�޷��õ�һ���ӿ�ָ�룡", L"��װ���������ʾ", MB_OK);
// 		}
// 
// 		HrReleaseINetCfg(pnc, TRUE);
// 	}
// 	else
// 	{
// 		if ((hr == NETCFG_E_NO_WRITE_LOCK) && lpszApp)
// 		{
// 			//   ErrMsg( hr,
// 			//  	  L"%s currently holds the lock, try later.",
// 			//  	 lpszApp );
// 			MessageBoxW(NULL, L"�����������⣬�Ժ����ԣ�", L"Passthru��װ���������ʾ", MB_OK);
// 			CoTaskMemFree(lpszApp);
// 		}
// 		else
// 		{
// 			MessageBoxW(NULL, L"�޷��õ�֪ͨ����ӿڣ�", L"Passthru��װ���������ʾ", MB_OK);
// 		}
// 	}
// 
// 	return 0;
// }
// 
// 
// //
// // Program entry point.
// //
// 
// int __cdecl main()
// {
// 	UninstallDriver_Passthru() ;
// 
// 	return 0;
// }