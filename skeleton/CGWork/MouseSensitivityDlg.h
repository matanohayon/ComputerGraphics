#pragma once


// MouseSensitivityDlg dialog

class MouseSensitivityDlg : public CDialog
{
	DECLARE_DYNAMIC(MouseSensitivityDlg);

public:
	MouseSensitivityDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~MouseSensitivityDlg();

	BOOL OnInitDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP();
public:
	int TranslationSensitivity = 1;
	CSliderCtrl T_slider;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl R_slider;
	int r_slider_value = 1;
	CSliderCtrl S_slider;
	int S_slider_value = 1;
	afx_msg void OnDefaultsButtonClicked();
};
