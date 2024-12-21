#pragma once


// PolygonFineness dialog

class PolygonFineness : public CDialog
{
	DECLARE_DYNAMIC(PolygonFineness)

public:
	PolygonFineness(CWnd* pParent = nullptr);   // standard constructor
	virtual ~PolygonFineness();
	BOOL PolygonFineness::OnInitDialog();
	void PolygonFineness::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FINENESS_CONTROL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl sliderForPolyFineness;
};
