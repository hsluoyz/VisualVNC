

class VNDriverLoaderEx
{
public:
	CString m_strDataDirectory;
	BOOL m_bInstalled;
	CString m_strDriverLinkName;
	HANDLE m_hDevice;

public:
	VNDriverLoaderEx();
	~VNDriverLoaderEx();
	setJustUnload();

	BOOL FolderExist(CString strPath);
	BOOL FileExist(CString strPathName);
	void initDataDir();
	LRESULT MyMessageBox(CString strText, CString strCaption, UINT iType);
	LRESULT MyMessageBox_YESNO(CString strText, CString strCaption);

	CString getServiceInfFilePath_Passthru();
	CString getServiceInfFilePath_PassthruMP();
	DWORD getServiceInfFilePath_Passthru(LPTSTR lpFilename, DWORD nSize);
	DWORD getServiceInfFilePath_PassthruMP(LPTSTR lpFilename, DWORD nSize);
	DWORD installDriver_Passthru();
	DWORD uninstallDriver_Passthru();

	BOOL openDeviceLink();
	BOOL closeDeviceLink();
	BOOL writeAndReadFromNdis(ULONG nCTLCode, UCHAR* pInData, int nInDataLen, UCHAR* pOutData, int nOutDataLen,
		int &nRetDataLen);
	BOOL writeToNdis(ULONG nCTLCode, UCHAR* pData, int nDataLen, int &nRetDataLen);
	BOOL readFromNdis(ULONG nCTLCode, UCHAR* pData, int nDataLen, int &nRetDataLen);

	BOOL queryAlive();
};