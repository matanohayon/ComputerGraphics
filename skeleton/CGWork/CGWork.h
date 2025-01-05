// CGWORK.h : main header file for the CGWORK application
//

#if !defined(AFX_CGWORK_H__58573165_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
#define AFX_CGWORK_H__58573165_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Light.h"
/////////////////////////////////////////////////////////////////////////////
// CCGWORKApp:
// See CGWORK.cpp for the implementation of this class
//

class CCGWorkApp : public CWinApp
{
public:
	CCGWorkApp();

	int t_slider_value;
	int r_slider_value;
	int s_slider_value;
	int p_slider_polyFineness_value;

	COLORREF Object_color;
	COLORREF Background_color;
	COLORREF vertex_normals_color;
	COLORREF poly_normals_color;
	float d;
	float fovy;
	float nearPlane;
	float farPlane;
	float aspectRatio;
	CPoint start;
	CPoint end;

double m_lMaterialAmbient;		// The Ambient in the scene
	double m_lMaterialDiffuse;		// The Diffuse in the scene
	double m_lMaterialSpecular;		// The Specular in the scene
	double m_lMaterialShininess;

	LightParams m_lights[MAX_LIGHT];	//configurable lights array
	LightParams m_ambientLight;		//ambient light (only RGB is used)


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCGWorkApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCGWorkApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGWORK_H__58573165_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
