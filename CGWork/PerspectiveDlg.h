#pragma once


// PerspectiveDlg dialog

class PerspectiveDlg : public CDialog
{
	DECLARE_DYNAMIC(PerspectiveDlg)

public:
	PerspectiveDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~PerspectiveDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PERSPCTIVE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double d;
	double fovy;
	afx_msg void OnBnClickedDefaultsButton();
};
