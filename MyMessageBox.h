#ifndef _CLASS_PMPARAM
#define  _CLASS_PMPARAM

class PSShellcode;

class PMParam
{
public:
	CString strParam1;
	CString strParam2;
	CString strParam3;
	void *pObject1;
	UINT nEvent;
public:
	PMParam(CString strParam1);
	PMParam(CString strParam1, CString strParam2);
	PMParam(void *pObject1);
	PMParam(void *pObject1, CString strParam1);
	PMParam(void *pObject1, UINT nEvent);
};

#endif

void MyMessageBox_Error(CString strText, CString strCaption);
void MyMessageBox_Error(CString strFunction);
void Output_Console_List(CString strNo, CString strContent);
void Output_A_List(CString strNo, CString strContent);
void Output_B_List(CString strNo, CString strContent);

void Output_Policy_EditBox(CString strContent);
void Clear_And_Output_Policy_EditBox(CString strContent);
void Output_Console_EditBox(CString strContent);
void Output_Update_Connection(UINT nEvent, void *pThis);
