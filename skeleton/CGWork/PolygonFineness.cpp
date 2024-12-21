// PolygonFineness.cpp : implementation file
//
#include "StdAfx.h"
#include "CGWork.h"
#include "PolygonFineness.h"
#include "afxdialogex.h"
#include "iritSkel.h"


// PolygonFineness dialog

IMPLEMENT_DYNAMIC(PolygonFineness, CDialog)

PolygonFineness::PolygonFineness(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_FINENESS_CONTROL, pParent)
{

}

PolygonFineness::~PolygonFineness()
{
}

void PolygonFineness::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_FINENESS, sliderForPolyFineness);
}

void PolygonFineness::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	if (pScrollBar->GetSafeHwnd() == sliderForPolyFineness.GetSafeHwnd())
	{
		// Update slider value

		pApp->p_slider_polyFineness_value = sliderForPolyFineness.GetPos();
		CGSkelFFCState.FineNess = sliderForPolyFineness.GetPos();

	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL PolygonFineness::OnInitDialog() {
	CDialog::OnInitDialog();

	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();

	sliderForPolyFineness.SetRange(2, 60);
	sliderForPolyFineness.SetPos(pApp->p_slider_polyFineness_value);


	return TRUE;

}


BEGIN_MESSAGE_MAP(PolygonFineness, CDialog)
END_MESSAGE_MAP()


// PolygonFineness message handlers
