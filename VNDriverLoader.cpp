#include "stdafx.h"
#include "VNDriverLoader.h"


VNDriverLoader::VNDriverLoader()
{
	m_pDriverHandle = NULL;
	   
	m_bRemovable = TRUE;
	
	m_pszServiceName = NULL;
	m_pszDriverPathName = NULL;
	m_pszDriverDosName = NULL;

	m_bInitialized = FALSE;
	m_bLoaded = FALSE;
	m_bStarted = FALSE;

	initDataDir();
	m_strDriverName = _T("DrvFltIp.sys");
	m_strDriverLinkName = _T("DrvFltIp");
	m_strServiceName = getServiceNameFromDriverName(m_strDriverName);
}

VNDriverLoader::VNDriverLoader(CString strDriverName, CString strDriverLinkName)
{
	m_pDriverHandle = NULL;
	   
	m_bRemovable = TRUE;
	
	m_pszServiceName = NULL;
	m_pszDriverPathName = NULL;
	m_pszDriverDosName = NULL;
	
	m_bInitialized = FALSE;
	m_bLoaded = FALSE;
	m_bStarted = FALSE;
	
	initDataDir();
	m_strDriverName = strDriverName;
	m_strDriverLinkName = strDriverLinkName;
	m_strServiceName = getServiceNameFromDriverName(m_strDriverName);
}

VNDriverLoader::~VNDriverLoader()
{
	closeDevice();
	unloadDriver();
}

void VNDriverLoader::setRemovable(BOOL bRemovable)
{
	m_bRemovable = bRemovable;
}

void VNDriverLoader::setJustUnload()
{
	m_bStarted = TRUE;
}

BOOL VNDriverLoader::isInitialized()
{
	return m_bInitialized;
}

BOOL VNDriverLoader::isLoaded()
{
	return m_bLoaded;
}

BOOL VNDriverLoader::isStarted()
{
	return m_bStarted;
}

DWORD VNDriverLoader::initDriverNames(LPCTSTR strServiceName, LPCTSTR strDriverPathName, LPCTSTR strDriverDosName)
{	
	if (m_bInitialized)
	{
		if (unloadDriver() != DRV_SUCCESS)
			return DRV_ERROR_ALREADY_INITIALIZED;
	}

	LPTSTR strBuffer;

	if (strDriverPathName != NULL)
	{
		DWORD dwLength = (DWORD)(strlen(strServiceName) + strlen(strDriverPathName) + 1);
		strBuffer = (LPTSTR) malloc(dwLength);
		strcpy(strBuffer, strDriverPathName);
	}
	else
	{
		LPTSTR strCurrentDir;
        DWORD dwLength = GetCurrentDirectory(0, NULL);
      
		strCurrentDir = (LPTSTR) malloc (dwLength);
		        
        if (GetCurrentDirectory(dwLength, strCurrentDir) != 0) 
		{
			dwLength = (DWORD) (strlen(strCurrentDir) + strlen(strServiceName) + 6);
			strBuffer = (LPTSTR) malloc(dwLength);

			//Path = "directorio\driverName.sys"
			sprintf(strBuffer, "%s\\%s.sys", strCurrentDir, strServiceName);

			if (GetFileAttributes(strBuffer) == 0xFFFFFFFF)
			{
				free(strCurrentDir);
				free(strBuffer);

				LPCTSTR strPartSysDir = "\\system32\\Drivers\\";
				LPTSTR strFullSysDir;
	    	    
				DWORD dwLength = GetWindowsDirectory(NULL, 0);
     			strFullSysDir = (LPTSTR) malloc(dwLength + strlen(strPartSysDir));

				if (GetWindowsDirectory(strFullSysDir, dwLength) == 0) 
				{
					free(strFullSysDir);
					return DRV_ERROR_UNKNOWN;
				}
	
				strcat(strFullSysDir, strPartSysDir);

				dwLength = (DWORD) (strlen(strFullSysDir) + strlen(strServiceName) + 5);
				strBuffer = (LPTSTR) malloc(dwLength);

				sprintf(strBuffer, "%s%s.sys", strFullSysDir, strServiceName);

				free(strFullSysDir);

				// Si el fichero no existe, salgo
				if (GetFileAttributes(strBuffer) == 0xFFFFFFFF)
				{
					free(strBuffer);
					return DRV_ERROR_INVALID_PATH_OR_FILE;
				}
			}

			free(strCurrentDir);

        }

		else
		{
			free(strCurrentDir);
			return DRV_ERROR_UNKNOWN;
		}
	}
	
	m_pszDriverPathName = strBuffer;

	m_pszServiceName = (LPTSTR) malloc(strlen(strServiceName) + 1);
	strcpy(m_pszServiceName, strServiceName);
	
	LPCTSTR strTempDriverDosName;
	if (strDriverDosName != NULL)
	{
        strTempDriverDosName = strDriverDosName;
	}
	else
	{
		strTempDriverDosName = strServiceName;
	}

	//dosName="\\.\driverName"
	if (strTempDriverDosName[0] != '\\' && strTempDriverDosName[1] != '\\')
	{
		m_pszDriverDosName = (LPTSTR) malloc(strlen(strTempDriverDosName) + 5);
		sprintf(m_pszDriverDosName, "\\\\.\\%s", strTempDriverDosName);
	}
	else
	{
		m_pszDriverDosName = (LPTSTR)malloc(strlen(strTempDriverDosName));
		strcpy(m_pszDriverDosName, strTempDriverDosName);
	}

	m_bInitialized = TRUE;
	return DRV_SUCCESS;
}

DWORD VNDriverLoader::innerLoadDriver(LPCTSTR strServiceName, LPCTSTR strDriverPathName, LPCTSTR strDriverDosName, BOOL bStart)
{
	DWORD dwRetCode = initDriverNames(strServiceName, strDriverPathName, strDriverDosName);

	if (dwRetCode == DRV_SUCCESS)
		dwRetCode = innerLoadDriver(bStart);

	return dwRetCode;
}

DWORD VNDriverLoader::innerLoadDriver(BOOL bStart)
{
	DWORD dwRetCode = DRV_SUCCESS;

	if (m_bLoaded)
		return DRV_SUCCESS;
	if (!m_bInitialized)
	{
		MyMessageBox_Error(_T("innerLoadDriver"));
		return DRV_ERROR_NO_INITIALIZED;
	}

	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (SCManager == NULL) 
	{
		MyMessageBox_Error(_T("innerLoadDriver"));
		return DRV_ERROR_SCM;
	}

    SC_HANDLE SCService = CreateService(SCManager,			  // SCManager database
									     m_pszServiceName,            // nombre del servicio
							    		 m_pszServiceName,            // nombre a mostrar
										 SERVICE_ALL_ACCESS,    // acceso total
										 SERVICE_KERNEL_DRIVER, // driver del kernel
										 SERVICE_DEMAND_START,  // comienzo bajo demanda
										 SERVICE_ERROR_NORMAL,  // control de errores normal
										 m_pszDriverPathName,	          // path del driver
										 NULL,                  // no pertenece a un grupo
										 NULL,                  // sin tag
										 NULL,                  // sin dependencias
										 NULL,                  // cuenta local del sistema
										 NULL                   // sin password
										 );
    
	if (SCService == NULL) 
	{
		SCService = OpenService(SCManager, m_pszServiceName, SERVICE_ALL_ACCESS);
		if (SCService == NULL) 
		{
			MyMessageBox_Error(_T("innerLoadDriver"));
			dwRetCode = DRV_ERROR_SERVICE;
		}
	}

	if (SCService)
	{
		CloseServiceHandle(SCService);
		SCService=NULL;
	}

	if (SCManager)
	{
		CloseServiceHandle(SCManager);
		SCManager = NULL;
	}

	if (dwRetCode == DRV_SUCCESS)
	{
		m_bLoaded = TRUE;
		if (bStart)
			dwRetCode = startDriver();
	}

	return dwRetCode;
}

DWORD VNDriverLoader::unloadDriver(BOOL bForceClearData)
{
	DWORD dwRetCode = DRV_SUCCESS;

	if (m_bStarted)
	{
		if ((dwRetCode = stopDriver()) == DRV_SUCCESS) 
		{
			if (m_bRemovable)
			{
				SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
				if (SCManager == NULL) 
				{
					MyMessageBox_Error(_T("unloadDriver"));
					return DRV_ERROR_SCM;
				}

				SC_HANDLE SCService = OpenService(SCManager, m_pszServiceName, SERVICE_ALL_ACCESS);
				if (SCService != NULL)
				{
					if (!DeleteService(SCService))
					{
						MyMessageBox_Error(_T("unloadDriver"));
						dwRetCode = DRV_ERROR_REMOVING;
					}
					else
					{
						dwRetCode = DRV_SUCCESS;
					}
				}
				else
				{
					MyMessageBox_Error(_T("unloadDriver"));
					dwRetCode = DRV_ERROR_SERVICE;
				}

				if (SCService)
				{
					CloseServiceHandle(SCService);
					SCService = NULL;
				}
				if (SCManager)
				{
					CloseServiceHandle(SCManager);
					SCManager = NULL;
				}

				if (dwRetCode == DRV_SUCCESS)
					m_bLoaded = FALSE;
			}
		}
	}

	if (m_bInitialized) 
	{
		if(dwRetCode != DRV_SUCCESS && bForceClearData == FALSE)
			return dwRetCode;
		
		m_bInitialized = FALSE;
				
		if (m_pszDriverPathName != NULL)
		{
			free(m_pszDriverPathName);
			m_pszDriverPathName = NULL;
		}

		if (m_pszDriverDosName != NULL)
		{
			free(m_pszDriverDosName);
			m_pszDriverDosName = NULL;
		}

		if (m_pszServiceName != NULL)
		{
			free(m_pszServiceName);
			m_pszServiceName = NULL;
		}

	}

	return dwRetCode;
}

DWORD VNDriverLoader::startDriver()
{
	if (m_bStarted)
		return DRV_SUCCESS;

	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	DWORD dwRetCode;
	
	if (SCManager == NULL)
	{
		MyMessageBox_Error(_T("startDriver"));
		return DRV_ERROR_SCM;
	}

    SC_HANDLE SCService = OpenService(SCManager,
		                              m_pszServiceName,
				                      SERVICE_ALL_ACCESS);
	if (SCService == NULL)
	{
		MyMessageBox_Error(_T("startDriver"));
        return DRV_ERROR_SERVICE;
	}

    
    if (!StartService(SCService, 0, NULL)) 
	{
		DWORD dwErrCode = GetLastError();
        if (dwErrCode == ERROR_SERVICE_ALREADY_RUNNING) 
		{
			//m_bRemovable = FALSE;
			dwRetCode = DRV_SUCCESS;
		}
		else
		{
			MyMessageBox_Error(_T("startDriver"));
			dwRetCode = DRV_ERROR_STARTING;
		}
    }
	else
	{
		dwRetCode = DRV_SUCCESS;
	}

	if (SCService)
	{
		CloseServiceHandle(SCService);
		SCService = NULL;
	}

	if (SCManager)
	{
		CloseServiceHandle(SCManager);
		SCManager = NULL;
	}

	if (dwRetCode == DRV_SUCCESS)
	{
		m_bStarted = TRUE;
		dwRetCode = openDevice();
	}

    return dwRetCode;
}

DWORD VNDriverLoader::stopDriver()
{
	if (!m_bStarted)
		return DRV_SUCCESS;

	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	DWORD dwRetCode;
	
	if (SCManager == NULL) 
	{
		MyMessageBox_Error(_T("stopDriver"));
		return DRV_ERROR_SCM;
	}

    SERVICE_STATUS dwStatus;
    SC_HANDLE SCService = OpenService(SCManager, m_pszServiceName, SERVICE_ALL_ACCESS);
    
	if (SCService != NULL)
	{
		if (m_pDriverHandle)
		{
			CloseHandle(m_pDriverHandle);
			m_pDriverHandle = NULL;
		}

		if (!ControlService(SCService, SERVICE_CONTROL_STOP, &dwStatus))
		{
			MyMessageBox_Error(_T("stopDriver"));
			dwRetCode = DRV_ERROR_STOPPING;
		}
		else
		{
			dwRetCode = DRV_SUCCESS;
		}
	}
	else
	{
		MyMessageBox_Error(_T("stopDriver"));
		dwRetCode = DRV_ERROR_SERVICE;
	}

	if (SCService)
	{
		CloseServiceHandle(SCService);
		SCService = NULL;
	}
	if (SCManager)
	{
		CloseServiceHandle(SCManager);
		SCManager = NULL;
	}

	if (dwRetCode == DRV_SUCCESS)
		m_bStarted = FALSE;

    return dwRetCode;
}

DWORD VNDriverLoader::openDevice()
{
	if (m_pDriverHandle != NULL) 
	{
		CloseHandle(m_pDriverHandle);
	}

    m_pDriverHandle = CreateFile(m_pszDriverDosName,
							  GENERIC_READ | GENERIC_WRITE,
							  0,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);


    if (m_pDriverHandle == INVALID_HANDLE_VALUE)
	{
		MyMessageBox_Error(_T("openDevice"));
		return DRV_ERROR_INVALID_HANDLE;
	}
	
	return DRV_SUCCESS;
}

void VNDriverLoader::closeDevice()
{
	if (m_pDriverHandle != NULL) 
		CloseHandle(m_pDriverHandle);
	m_pDriverHandle = NULL;
}

HANDLE VNDriverLoader::getDriverHandle(void)
{
	return m_pDriverHandle;
}

DWORD VNDriverLoader::writeIo(DWORD dwCode, PVOID pBuffer, DWORD cbCount)
{
	if (m_pDriverHandle == NULL)
	{
		MyMessageBox_Error(_T("writeIo"));
		return DRV_ERROR_INVALID_HANDLE;
	}

	DWORD dwBytesRet;
	BOOL bRetCode = DeviceIoControl(m_pDriverHandle,
								      dwCode,
								      pBuffer,
								      cbCount,
								      NULL,
								      0,
								      &dwBytesRet,
								      NULL);
	if (!bRetCode)
	{
		MyMessageBox_Error(_T("writeIo"));
		return DRV_ERROR_IO;
	}
	else
	{
		return DRV_SUCCESS;
	}
}

DWORD VNDriverLoader::readIo(DWORD dwCode, PVOID pBuffer, DWORD cbCount)
{
	if (m_pDriverHandle == NULL)
	{
		MyMessageBox_Error(_T("readIo"));
		return DRV_ERROR_INVALID_HANDLE;
	}

	DWORD dwBytesRet;
	BOOL bRetCode = DeviceIoControl(m_pDriverHandle,
								   dwCode,
								   NULL,
								   0,
								   pBuffer,
								   cbCount,
								   &dwBytesRet,
								   NULL);

	if (!bRetCode)
	{
		MyMessageBox_Error(_T("readIo"));
		return DRV_ERROR_IO;
	}
	else
	{
		return dwBytesRet;
	}
}

DWORD VNDriverLoader::rawIo(DWORD dwCode, PVOID pInBuffer, DWORD cbInCount, PVOID pOutBuffer, DWORD cbOutCount)
{
	if (m_pDriverHandle == NULL)
	{
		MyMessageBox_Error(_T("rawIo"));
		return DRV_ERROR_INVALID_HANDLE;
	}

	DWORD dwBytesRet;
	BOOL dwRetCode = DeviceIoControl(m_pDriverHandle,
								   dwCode,
								   pInBuffer,
								   cbInCount,
								   pOutBuffer,
								   cbOutCount,
								   &dwBytesRet,
								   NULL);

	if (!dwRetCode)
	{
		MyMessageBox_Error(_T("rawIo"));
		return DRV_ERROR_IO;
	}
	else
	{
		return dwBytesRet;
	}
}

BOOL VNDriverLoader::FolderExist(CString strPath)
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

BOOL VNDriverLoader::FileExist(CString strPathName)
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

void VNDriverLoader::initDataDir()
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

CString VNDriverLoader::getFileNameWithoutExt(CString strFileName)
{
	int iPos = strFileName.Find(_T('.'));
	if (iPos == -1)
	{
		return strFileName;
	}
	else
	{
		return strFileName.Left(iPos);
	}
}

CString VNDriverLoader::getServiceNameFromDriverName(CString strDriverName)
{
	CString strServiceName;
	strServiceName = _T("my_");
	strServiceName += getFileNameWithoutExt(strDriverName);
	strServiceName += _T("_service");
	return strServiceName;
}

void VNDriverLoader::loadDriver()
{
	if (!FileExist(m_strDataDirectory + m_strDriverName))
	{
		MyMessageBox_Error(_T("myLoadDriver"));
		return;
	}
	
	innerLoadDriver(m_strServiceName, m_strDataDirectory + m_strDriverName, m_strDriverLinkName, TRUE);
}