
#if !defined(VNDriverLoader_CLASS)
#define VNDriverLoader_CLASS

#pragma once

#include "winsvc.h"


//ERROR CODES
#define DRV_SUCCESS						 (DWORD)0		// Todo bien

#define DRV_ERROR_SCM					 (DWORD)-1		// ERROR al abrir el service manager
#define DRV_ERROR_SERVICE				 (DWORD)-2		// ERROR al crear el servicio
#define DRV_ERROR_MEMORY				 (DWORD)-3		// ERROR al reservar memoria
#define DRV_ERROR_INVALID_PATH_OR_FILE	 (DWORD)-4		// ERROR, Path no valido
#define DRV_ERROR_INVALID_HANDLE		 (DWORD)-5		// ERROR, driver handle no valido
#define DRV_ERROR_STARTING				 (DWORD)-6		// ERROR al poner en Start el driver
#define DRV_ERROR_STOPPING				 (DWORD)-7		// ERROR al parar el driver
#define DRV_ERROR_REMOVING				 (DWORD)-8		// ERROR eliminando el "servicio"
#define DRV_ERROR_IO					 (DWORD)-9		// ERROR en operacion de E/S
#define DRV_ERROR_NO_INITIALIZED		 (DWORD)-10		// ERROR, clase no inicializada
#define DRV_ERROR_ALREADY_INITIALIZED	 (DWORD)-11		// ERROR, clase ya inicializada
#define DRV_ERROR_NULL_POINTER			 (DWORD)-12		// ERROR, puntero a null como parametro
#define DRV_ERROR_UNKNOWN				 (DWORD)-13		// ERROR desconocido

class VNDriverLoader
{
public:
	VNDriverLoader();		//constructor
	VNDriverLoader(CString strDriverName, CString strDriverLinkName);
	~VNDriverLoader();		//destructor
	
	DWORD initDriverNames(LPCTSTR strServiceName, LPCTSTR strDriverPathName, LPCTSTR strDriverDosName = NULL);
	DWORD innerLoadDriver(LPCTSTR strServiceName, LPCTSTR strDriverPathName, LPCTSTR strDriverDosName = NULL, BOOL bStart = TRUE);
	DWORD innerLoadDriver(BOOL bStart = TRUE);
	DWORD unloadDriver(BOOL bForceClearData = FALSE);
	
	DWORD startDriver();
	DWORD stopDriver();
	DWORD openDevice();
	void closeDevice();
	HANDLE getDriverHandle();
	
	void setRemovable(BOOL bRemovable);
	void setJustUnload();
	
	BOOL isInitialized();
	BOOL isStarted();
	BOOL isLoaded();

	DWORD writeIo(DWORD dwCode, PVOID pBuffer, DWORD cbCount);
	DWORD readIo(DWORD dwCode, PVOID pBuffer, DWORD cbCount);
	DWORD rawIo(DWORD dwCode, PVOID pInBuffer, DWORD cbInCount, PVOID pOutBuffer, DWORD cbOutCount);
	
private:
	HANDLE m_pDriverHandle;	// driver handle
	
	LPTSTR m_pszServiceName;
	LPTSTR m_pszDriverPathName;
	LPTSTR m_pszDriverDosName;
	
	BOOL m_bInitialized;
	BOOL m_bStarted;
	BOOL m_bLoaded;
	BOOL m_bRemovable;
///////////////////////////////////////////////////////////
public:
	CString m_strDataDirectory;
	CString m_strServiceName;
	CString m_strDriverName;
	CString m_strDriverLinkName;

public:
	BOOL FolderExist(CString strPath);
	BOOL FileExist(CString strPathName);
	void initDataDir();

	CString getFileNameWithoutExt(CString strFileName);
	CString getServiceNameFromDriverName(CString strDriverName);
	void loadDriver();
};

#endif