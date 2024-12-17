// CGWorkView.cpp : implementation of the CCGWorkView class
//
#include "stdafx.h"
#include "CGWork.h"

#include "CGWorkDoc.h"
#include "CGWorkView.h"
#include <algorithm> // Required for std::min
#undef min           // Prevent conflicts with Windows macros

#include <iostream>
using std::cout;
using std::endl;
#include "MaterialDlg.h"
#include "LightDialog.h"
#include "PerspectiveDlg.h"
#include "MouseSensitivityDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PngWrapper.h"
#include "iritSkel.h"
#include "LineDrawer.h"


// For Status Bar access
#include "MainFrm.h"

// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))

/*lets use the global scene container*/
#include "Scene.h"
extern Scene scene;



/////////////////////////////////////////////////////////////////////////////
// CCGWorkView

IMPLEMENT_DYNCREATE(CCGWorkView, CView)

BEGIN_MESSAGE_MAP(CCGWorkView, CView)
	//{{AFX_MSG_MAP(CCGWorkView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_VIEW_ORTHOGRAPHIC, OnViewOrthographic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORTHOGRAPHIC, OnUpdateViewOrthographic)
	ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewPerspective)
	ON_COMMAND(ID_ACTION_ROTATE, OnActionRotate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ROTATE, OnUpdateActionRotate)
	ON_COMMAND(ID_ACTION_SCALE, OnActionScale)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SCALE, OnUpdateActionScale)
	ON_COMMAND(ID_ACTION_TRANSLATE, OnActionTranslate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TRANSLATE, OnUpdateActionTranslate)
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)
	ON_COMMAND(ID_LIGHT_SHADING_FLAT, OnLightShadingFlat)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_FLAT, OnUpdateLightShadingFlat)
	ON_COMMAND(ID_LIGHT_SHADING_GOURAUD, OnLightShadingGouraud)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_GOURAUD, OnUpdateLightShadingGouraud)
	ON_COMMAND(ID_LIGHT_CONSTANTS, OnLightConstants)

	ON_COMMAND(ID_OPTIONS_PERSPECTIVECONTROL, OnPerspectiveParameters)
	ON_COMMAND(ID_OPTIONS_MOUSESENSITIVITY, &CCGWorkView::OnOptionsMousesensitivity)

	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()

	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad, GLU_FILL);
        gluCylinder(quad, radius, 0.0, height, 20, 20);
        gluDeleteQuadric(quad);
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView construction/destruction

CCGWorkView::CCGWorkView()
{
	m_draw_poly_normals = false;
	m_draw_vertex_normals = false;

	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;
	m_nView = ID_VIEW_ORTHOGRAPHIC;	

	m_bIsPerspective = false;

	m_nLightShading = ID_LIGHT_SHADING_FLAT;

	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;

	//init the first light to be enabled
	m_lights[LIGHT_ID_1].enabled=true;
	m_pDbBitMap = NULL;
	m_pDbDC = NULL;
	m_isDragging = false;
}

CCGWorkView::~CCGWorkView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView diagnostics

#ifdef _DEBUG
void CCGWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CCGWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGWorkDoc* CCGWorkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGWorkDoc)));
	return (CCGWorkDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView Window Creation - Linkage of windows to CGWork

BOOL CCGWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// An CGWork window must be created with the following
	// flags and must NOT include CS_PARENTDC for the
	// class style.

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}



int CCGWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeCGWork();

	return 0;
}


// This method initialized the CGWork system.
BOOL CCGWorkView::InitializeCGWork()
{
	m_pDC = new CClientDC(this);
	
	if ( NULL == m_pDC ) { // failure to get DC
		::AfxMessageBox(CString("Couldn't get a valid DC."));
		return FALSE;
	}

	CRect r;
	GetClientRect(&r);
	m_pDbDC = new CDC();
	m_pDbDC->CreateCompatibleDC(m_pDC);
	SetTimer(1, 1, NULL);
	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.right, r.bottom);	
	m_pDbDC->SelectObject(m_pDbBitMap);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView message handlers


void CCGWorkView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if ( 0 >= cx || 0 >= cy ) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth/(GLdouble)m_WindowHeight;

	CRect r;
	GetClientRect(&r);
	DeleteObject(m_pDbBitMap);
    	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.right, r.bottom);	
	m_pDbDC->SelectObject(m_pDbBitMap);
}


BOOL CCGWorkView::SetupViewingFrustum(void)
{
    return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cube.
BOOL CCGWorkView::SetupViewingOrthoConstAspect(void)
{
	return TRUE;
}



BOOL CCGWorkView::OnEraseBkgnd(CDC* pDC) 
{
	// Windows will clear the window with the background color every time your window 
	// is redrawn, and then CGWork will clear the viewport with its own background color.
	return true;
}



void CCGWorkView::DrawPolygonEdges(CDC* pDC, const Poly& poly, double screenHeight, COLORREF color) {
	const std::vector<Vector4>& vertices = poly.getVertices();

	for (size_t i = 0; i < vertices.size(); ++i) {
		const Vector4& start = vertices[i];
		const Vector4& end = vertices[(i + 1) % vertices.size()]; // Wrap to first vertex

		LineDrawer::DrawLine(
			pDC->m_hDC,
			Vector4(static_cast<double>(start.x), static_cast<double>(screenHeight - start.y), 0.0),
			Vector4(static_cast<double>(end.x), static_cast<double>(screenHeight - end.y), 0.0),
			color
		);
	}
}

void CCGWorkView::DrawPolygonNormal(CDC* pDC, const Poly& poly, double screenHeight, COLORREF color) {
	const Vector4& normalStart = poly.getNormalStart();
	const Vector4& normalEnd = poly.getNormalEnd();

	LineDrawer::DrawLine(
		pDC->m_hDC,
		Vector4(static_cast<double>(normalStart.x), static_cast<double>(screenHeight - normalStart.y), 0.0),
		Vector4(static_cast<double>(normalEnd.x), static_cast<double>(screenHeight - normalEnd.y), 0.0),
		color
	);
}
void CCGWorkView::DrawVertexNormals(CDC* pDC, const Poly& poly, double screenHeight, COLORREF color) {
	if (!poly.hasPredefinedNormal()) return;

	const std::vector<Vector4>& vertices = poly.getVertices();
	const std::vector<Vector4>& vertexNormals = poly.getVertexNormals();

	if (vertices.size() != vertexNormals.size()) return; // Ensure sizes match
	int extendNorm = 30;
	for (size_t i = 0; i < vertices.size(); ++i) {
		const Vector4& vertex = vertices[i];
		const Vector4 normalEnd = vertex + vertexNormals[i]*extendNorm; // Scale the normal if needed

		LineDrawer::DrawLine(
			pDC->m_hDC,
			Vector4(static_cast<double>(vertex.x), static_cast<double>(screenHeight - vertex.y), 0.0),
			Vector4(static_cast<double>(normalEnd.x), static_cast<double>(screenHeight - normalEnd.y), 0.0),
			color
		);
	}
}




/////////////////////////////////////////////////////////////////////////////
// CCGWorkView drawing
/////////////////////////////////////////////////////////////////////////////
void CCGWorkView::OnDraw(CDC* pDC) {

	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();

	CCGWorkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	CRect r;
	GetClientRect(&r);

	// Use the double-buffered DC to avoid flickering
	CDC* pDCToUse = m_pDbDC;

	scene.setColors(pApp->Object_color, RGB(0, 255, 0), pApp->Background_color); // setting the object color and the backgrond
	pDCToUse->FillSolidRect(&r, scene.getBackgroundColor()); // Use scene's background color

	double screenWidth = static_cast<double>(r.Width());
	double screenHeight = static_cast<double>(r.Height());

	// Iterate through the polygons in the scene and draw them
	for (const Poly& poly : scene.getPolygons()) {
		const std::vector<Vector4>& vertices = poly.getVertices();
		COLORREF color = poly.getColor(); // Get the color for the polygon

		// Draw polygon edges
		DrawPolygonEdges(pDCToUse, poly, screenHeight, color);

		// Draw polygon normals if the flag is set
		if (!m_draw_poly_normals) {
			DrawPolygonNormal(pDCToUse, poly, screenHeight, RGB(255, 0, 0)); // Red color for normals
		}

		// Draw vertex normals if the flag is set
		if (!m_draw_vertex_normals) {
			if (!scene.hasVertexNormalsAttribute()) {
				AfxMessageBox(_T("The Object does not have vertex normals!"));
			}
			else {
				DrawVertexNormals(pDCToUse, poly, screenHeight, RGB(0, 255, 0)); // Green color for normals
			}
		}
	}
	if (pDCToUse != m_pDC) {
		m_pDC->BitBlt(r.left, r.top, r.Width(), r.Height(), pDCToUse, r.left, r.top, SRCCOPY);
	}
}



/////////////////////////////////////////////////////////////////////////////
// User Defined Functions

void CCGWorkView::RenderScene() {
	// do nothing. This is supposed to be overriden...
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView CGWork Finishing and clearing...

void CCGWorkView::OnDestroy()
{
	CView::OnDestroy();

	// delete the DC
	if (m_pDC) {
		delete m_pDC;
	}

	if (m_pDbDC) {
		delete m_pDbDC;
	}
}









void CCGWorkView::OnFileLoad() {
	TCHAR szFilters[] = _T("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal() == IDOK) {
		m_strItdFileName = dlg.GetPathName(); // Full path and filename
		scene.clear(); // Clear the existing scene data

		// Load and process the IRIT file
		CGSkelProcessIritDataFiles(m_strItdFileName, 1);

		// Calculate bounding box and determine initial transformation
		scene.calculateBoundingBox();

		const BoundingBox& bbox = scene.getBoundingBox();
		const Vector4& min = bbox.min;
		const Vector4& max = bbox.max;

		Vector4 center = Vector4(
			(min.x + max.x) / 2.0,
			(min.y + max.y) / 2.0,
			(min.z + max.z) / 2.0,
			1.0 // Maintain consistent w for homogeneous coordinates
		);
		CRect r;
		GetClientRect(&r);

		double screenWidth = static_cast<double>(r.Width());
		double screenHeight = static_cast<double>(r.Height());
		double marginFactor = 0.25;

		double sceneWidth = max.x - min.x;
		double sceneHeight = max.y - min.y;

		double targetWidth = screenWidth * (1.0 - marginFactor);
		double targetHeight = screenHeight * (1.0 - marginFactor);

		double scaleX = sceneWidth > 1e-6 ? targetWidth / sceneWidth : 1.0;
		double scaleY = sceneHeight > 1e-6 ? targetHeight / sceneHeight : 1.0;

		double sceneScale = (scaleX < scaleY) ? scaleX : scaleY;

		Matrix4 t; // Starts as the identity matrix

		Matrix4 translateToOrigin = Matrix4::translate(-center.x, -center.y, -center.z);
		Matrix4 scaling = Matrix4::scale(sceneScale, sceneScale, sceneScale);
		Matrix4 translateToScreen = Matrix4::translate(screenWidth / 2.0, screenHeight / 2.0, 0.0);
		
		// Translate to origin (center the scene)
		t = t * translateToScreen;
		// Scale the scene to fit within the target screen area
		t = t * scaling;
		// Translate to the screen center
		t = t * translateToOrigin;

		scene.applyTransform(t);

		scene.updateIsFirstDraw(false);

		Invalidate(); // Trigger WM_PAINT for redraw
	}
}





// VIEW HANDLERS ///////////////////////////////////////////

// Note: that all the following Message Handlers act in a similar way.
// Each control or command has two functions associated with it.

void CCGWorkView::OnViewOrthographic() 
{
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	Invalidate();		// redraw using the new view.
}

void CCGWorkView::OnUpdateViewOrthographic(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_ORTHOGRAPHIC);
}

void CCGWorkView::OnViewPerspective() 
{
	m_nView = ID_VIEW_PERSPECTIVE;
	m_bIsPerspective = true;
	Invalidate();
}

void CCGWorkView::OnUpdateViewPerspective(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_PERSPECTIVE);
}




// ACTION HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnActionRotate() 
{
	m_nAction = ID_ACTION_ROTATE;
}

void CCGWorkView::OnUpdateActionRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_ROTATE);
}

void CCGWorkView::OnActionTranslate() 
{
	m_nAction = ID_ACTION_TRANSLATE;
}

void CCGWorkView::OnUpdateActionTranslate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_TRANSLATE);
}

void CCGWorkView::OnActionScale() 
{
	m_nAction = ID_ACTION_SCALE;
}

void CCGWorkView::OnUpdateActionScale(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_SCALE);
}




// AXIS HANDLERS ///////////////////////////////////////////


// Gets calles when the X button is pressed or when the Axis->X menu is selected.
// The only thing we do here is set the ChildView member variable m_nAxis to the 
// selected axis.
void CCGWorkView::OnAxisX() 
{
	m_nAxis = ID_AXIS_X;
}

// Gets called when windows has to repaint either the X button or the Axis pop up menu.
// The control is responsible for its redrawing.
// It sets itself disabled when the action is a Scale action.
// It sets itself Checked if the current axis is the X axis.
void CCGWorkView::OnUpdateAxisX(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_X);
}


void CCGWorkView::OnAxisY() 
{
	m_nAxis = ID_AXIS_Y;
}

void CCGWorkView::OnUpdateAxisY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Y);
}


void CCGWorkView::OnAxisZ() 
{
	m_nAxis = ID_AXIS_Z;
}

void CCGWorkView::OnUpdateAxisZ(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Z);
}





// OPTIONS HANDLERS ///////////////////////////////////////////




// LIGHT SHADING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnLightShadingFlat() 
{
	m_nLightShading = ID_LIGHT_SHADING_FLAT;
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_FLAT);
}


void CCGWorkView::OnLightShadingGouraud() 
{
	m_nLightShading = ID_LIGHT_SHADING_GOURAUD;
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_GOURAUD);
}

// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::OnLightConstants() 
{
	CLightDialog dlg;

	for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	{	    
	    dlg.SetDialogData((LightID)id,m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT,m_ambientLight);

	if (dlg.DoModal() == IDOK) 
	{
	    for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	    {
		m_lights[id] = dlg.GetDialogData((LightID)id);
	    }
	    m_ambientLight = dlg.GetDialogData(LIGHT_ID_AMBIENT);
	}	
	Invalidate();
}

void CCGWorkView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnTimer(nIDEvent);
	if (nIDEvent == 1)
		Invalidate();
}

/////////////////////////////////////////////////////////////////////////////////////////
void CCGWorkView::OnPerspectiveParameters()
{
	// Create and open the dialog
	PerspectiveDlg paramDlg;
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();

	// Optionally initialize values
	paramDlg.d = pApp->d; // Example default value
	paramDlg.fovy = pApp->fovy; // Example default value

	if (paramDlg.DoModal() == IDOK)
	{
		// Retrieve updated values after the dialog is closed
		pApp->d = paramDlg.d;
		pApp->fovy = paramDlg.fovy;

		// Use the parameters (e.g., store or apply them)

	}
}


void CCGWorkView::OnOptionsMousesensitivity()
{
	// TODO: Add your command handler code here
	MouseSensitivityDlg dlg;

	if (dlg.DoModal() == IDOK)
	{

		// Handle OK click if needed
	}
}


/////////////////////////////// MOUSE CONTROL //////////////////////////////////////////////

void CCGWorkView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Begin dragging
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	m_isDragging = true;
	SetCapture();  // Capture mouse events globally for this window

	pApp->start = point;
	// Log or process the starting point
	TRACE(_T("Mouse Down at: X=%d, Y=%d\n"), point.x, point.y);

	CView::OnLButtonDown(nFlags, point);
}

void CCGWorkView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_isDragging)
	{
		// Handle dragging logic (e.g., drawing, updating UI, etc.)
		TRACE(_T("Mouse Move at: X=%d, Y=%d\n"), point.x, point.y);

		// If needed, convert to screen coordinates
		CPoint screenPoint = point;
		ClientToScreen(&screenPoint);
		TRACE(_T("Mouse Move (Screen): X=%d, Y=%d\n"), screenPoint.x, screenPoint.y);
	}

	CView::OnMouseMove(nFlags, point);
}

void CCGWorkView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_isDragging)
	{
		CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
		// End dragging
		m_isDragging = false;
		ReleaseCapture();  // Release mouse capture

		pApp->end = point;

		// Log or process the final point
		TRACE(_T("Mouse Up at: X=%d, Y=%d\n"), point.x, point.y);
	}

	CView::OnLButtonUp(nFlags, point);
}