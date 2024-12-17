// MouseSensitivityDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "CGWork.h"
#include "MouseSensitivityDlg.h"
#include "afxdialogex.h"


// MouseSensitivityDlg dialog

IMPLEMENT_DYNAMIC(MouseSensitivityDlg, CDialog)

int rotate = 1, translate = 1, scale = 1;

MouseSensitivityDlg::MouseSensitivityDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MOUSE, pParent)
	, TranslationSensitivity(translate)
	, r_slider_value(rotate)
	, S_slider_value(scale)
{

}

MouseSensitivityDlg::~MouseSensitivityDlg()
{
}

void MouseSensitivityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_SLIDER2, TranslationSensitivity);
	DDX_Control(pDX, IDC_SLIDER2, T_slider);
	DDX_Control(pDX, IDC_SLIDER1, R_slider);
	DDX_Slider(pDX, IDC_SLIDER1, r_slider_value);
	DDX_Control(pDX, IDC_SLIDER3, S_slider);
	DDX_Slider(pDX, IDC_SLIDER3, S_slider_value);
}

BOOL MouseSensitivityDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();


	T_slider.SetRange(0, 10);
	T_slider.SetPos(pApp->t_slider_value);
	R_slider.SetRange(0, 10);
	R_slider.SetPos(pApp->r_slider_value);
	S_slider.SetRange(0, 10);
	S_slider.SetPos(pApp->s_slider_value);

	return TRUE;

}

BEGIN_MESSAGE_MAP(MouseSensitivityDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_MOUSEDEFAULTS, &MouseSensitivityDlg::OnDefaultsButtonClicked)
END_MESSAGE_MAP()


// MouseSensitivityDlg message handlers


void MouseSensitivityDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	if (pScrollBar->GetSafeHwnd() == T_slider.GetSafeHwnd())
	{
		// Update slider value


		pApp->t_slider_value = T_slider.GetPos();

	}
	else if (pScrollBar->GetSafeHwnd() == R_slider.GetSafeHwnd())
	{
		// Update the value for Slider 2
		pApp->r_slider_value = R_slider.GetPos();
	}
	else if (pScrollBar->GetSafeHwnd() == S_slider.GetSafeHwnd())
	{
		// Update the value for Slider 3
		pApp->s_slider_value = S_slider.GetPos();
	}


	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void MouseSensitivityDlg::OnDefaultsButtonClicked() {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	pApp->t_slider_value = 1;
	T_slider.SetPos(pApp->t_slider_value);
	pApp->r_slider_value = 1;
	R_slider.SetPos(pApp->r_slider_value);
	pApp->s_slider_value = 1;
	S_slider.SetPos(pApp->s_slider_value);



}