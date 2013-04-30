// VisualVNCDoc.h : interface of the CVisualVNCDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALVNCDOC_H__D5C07970_E3E7_4AFA_BB46_E1CD1C796E34__INCLUDED_)
#define AFX_VISUALVNCDOC_H__D5C07970_E3E7_4AFA_BB46_E1CD1C796E34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVisualVNCDoc : public CDocument
{
protected: // create from serialization only
	CVisualVNCDoc();
	DECLARE_DYNCREATE(CVisualVNCDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualVNCDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVisualVNCDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVisualVNCDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALVNCDOC_H__D5C07970_E3E7_4AFA_BB46_E1CD1C796E34__INCLUDED_)
