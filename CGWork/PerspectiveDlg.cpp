// PerspectiveDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "CGWork.h"
#include "PerspectiveDlg.h"
#include "afxdialogex.h"


// PerspectiveDlg dialog

IMPLEMENT_DYNAMIC(PerspectiveDlg, CDialog)

PerspectiveDlg::PerspectiveDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PERSPECTIVE, pParent)
	, d(0)
	, fovy(0)
{

}

PerspectiveDlg::~PerspectiveDlg()
{
}

void PerspectiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, d);
	DDX_Text(pDX, IDC_EDIT2, fovy);
}


BEGIN_MESSAGE_MAP(PerspectiveDlg, CDialog)
	ON_BN_CLICKED(IDC_PERSPECTIVEDEFAULTS, &PerspectiveDlg::OnBnClickedDefaultsButton)
END_MESSAGE_MAP()


// PerspectiveDlg message handlers


void PerspectiveDlg::OnBnClickedDefaultsButton()
{
	// TODO: Add your control notification handler code here
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	pApp->d = 15;
	pApp->fovy = 50;
	d = pApp->d; // Example default value
	fovy = pApp->fovy; // Example default value
	UpdateData(FALSE);
	//EndDialog(IDD_PERSPECTIVE);


}
