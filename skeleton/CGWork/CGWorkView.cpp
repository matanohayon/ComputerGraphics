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
extern IPFreeformConvStateStruct CGSkelFFCState; // access to the polygon tesselation finess
#include "LineDrawer.h"


// For Status Bar access
#include "MainFrm.h"

// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))

/*lets use the global scene container*/
#include "Scene.h"
#include "Vertex.h"


#define MOUSE_FACTOR 5

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

	ON_COMMAND(ID_VIEW_BOUNDINGBOX, OnBoundingBox)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BOUNDINGBOX, OnUpdateBoundingBox)

	ON_COMMAND(ID_VIEW_VERTEXNORMAL, OnVertexNormal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VERTEXNORMAL, OnUpdateVertexNormal)

	ON_COMMAND(ID_VIEW_POLYGONNORMALS, OnPolyNormal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POLYGONNORMALS, OnUpdatePolyNormal)

	ON_COMMAND(ID_OPTIONS_POLYGONFINENESS, &CCGWorkView::OnOptionsPolygonFineness)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_POLYGONFINENESS, &CCGWorkView::OnUpdateOptionsPolygonFineness)

	//vertexNormal and poly normal from file and not from file:

	//poly from file
// Polygon normals not from file
	ON_COMMAND(ID_VIEW_POLYGONNORMALSNOTFROM, OnPolyNormalsNotFrom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POLYGONNORMALSNOTFROM, OnUpdatePolyNormalsNotFrom)
	// Polygon normals from file
	ON_COMMAND(ID_VIEW_POLYGONNORMALSFROM, OnPolyNormalsFrom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POLYGONNORMALSFROM, OnUpdatePolyNormalsFrom)
	// Vertex normals from file
	ON_COMMAND(ID_VIEW_VERTEXNORMALSFROM, OnVertexNormalsFrom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VERTEXNORMALSFROM, OnUpdateVertexNormalsFrom)
	// Vertex normals not from file
	ON_COMMAND(ID_VIEW_VERTEXNORMALSNOTFROM, OnVertexNormalsNotFrom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VERTEXNORMALSNOTFROM, OnUpdateVertexNormalsNotFrom)




	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()

	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
	GLUquadric* quad = gluNewQuadric();
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
	m_draw_bounding_box = false;
	m_uniform_color = false;
	m_draw_poly_normals_from = false; //poly normals form file
	m_draw_poly_normals_not_from = false; // poly normals not from file
	m_draw_vertex_normals_from = false; //vertex normals from file
	m_draw_vertex_normals_not_from = false; // vertex normal nor from file

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
	m_lights[LIGHT_ID_1].enabled = true;
	m_pDbBitMap = NULL;
	m_pDbDC = NULL;
	m_isDragging = false;
	prev_start = CPoint(0, 0);
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

	if (NULL == m_pDC) { // failure to get DC
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

	if (0 >= cx || 0 >= cy) {
		return; // Prevent resizing to zero or negative dimensions
	}

	// Save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// Compute the aspect ratio
	m_AspectRatio = (GLdouble)m_WindowWidth / (GLdouble)m_WindowHeight;

	CRect r;
	GetClientRect(&r);
	DeleteObject(m_pDbBitMap); // Delete the old bitmap
	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.right, r.bottom); // Create a new bitmap
	m_pDbDC->SelectObject(m_pDbBitMap); // Attach the new bitmap to the device context

	// Apply the transformation to center the object to the screen
	Matrix4 centerMatrix = getMatrixToCenterObject();
	scene.applyTransform(centerMatrix);

	// Trigger a redraw
	Invalidate();
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

// Helper function to draw a single line
void CCGWorkView::DrawLineHelper(CDC* pDC, const Vector4& start, const Vector4& end, double screenHeight, COLORREF color) {
	LineDrawer::DrawLine(
		pDC->m_hDC,
		Vector4(static_cast<double>(start.x), static_cast<double>(screenHeight - start.y), 0.0),
		Vector4(static_cast<double>(end.x), static_cast<double>(screenHeight - end.y), 0.0),
		color
	);
}


/*
	bool m_draw_poly_normals_from ; //poly normals form file
	bool m_draw_poly_normals_not_from ; // poly normals not from file
	bool m_draw_vertex_normals_from ; //vertex normals from file
	bool m_draw_vertex_normals_not_from ; // vertex normal nor from file
*/


void CCGWorkView::DrawPolygonEdgesAndVertexNormals(CDC* pDC, Poly* poly, double screenHeight, COLORREF color) {
	const std::vector<Vertex>& vertices = poly->getVertices();
	const size_t vertexCount = vertices.size();

	for (size_t i = 0; i < vertexCount; ++i) {
		const Vertex& start = vertices[i];
		const Vertex& end = vertices[(i + 1 < vertexCount) ? i + 1 : 0];

		// Draw polygon edge
		DrawLineHelper(pDC, start, end, screenHeight, color);

		// Draw vertex normals based on global flags
		if (start.getHasNormal()) {
			if (m_draw_vertex_normals_from && start.isNormalProvidedFromFile()) {
				// Draw normals provided from file
				DrawLineHelper(pDC, start.getNormalStart(), start.getNormalEnd(), screenHeight, RGB(0, 255, 0)); // Green
			}
			if (m_draw_vertex_normals_not_from && !start.isNormalProvidedFromFile()) {
				// Draw calculated normals
				DrawLineHelper(pDC, start.getNormalStart(), start.getNormalEnd(), screenHeight, RGB(255, 0, 0)); // Red
			}
		}
	}
}



void CCGWorkView::DrawPolygonNormal(CDC* pDC, Poly* poly, double screenHeight, COLORREF color) {
	if (!poly->hasPolyNormalDefined()) return;

	const PolyNormal& polyNormal = poly->getPolyNormal();

	// Draw polygon normals based on global flags
	if (m_draw_poly_normals_from && polyNormal.wasProvidedFromFile) {
		// Draw normals provided from file
		DrawLineHelper(pDC, polyNormal.start, polyNormal.end, screenHeight, RGB(0, 0, 255)); // Blue
	}
	if (m_draw_poly_normals_not_from && !polyNormal.wasProvidedFromFile) {
		// Draw calculated normals
		DrawLineHelper(pDC, polyNormal.start, polyNormal.end, screenHeight, RGB(255, 165, 0)); // Orange
	}
}



void CCGWorkView::DrawBoundingBox(CDC* pDC, const BoundingBox& bbox, double screenHeight, COLORREF color) {
	// Extract min and max points
	const Vector4& min = bbox.min;
	const Vector4& max = bbox.max;

	// Define the 8 corners of the bounding box
	std::vector<Vector4> corners = {
		{min.x, min.y, min.z, 1.0}, {max.x, min.y, min.z, 1.0},
		{max.x, max.y, min.z, 1.0}, {min.x, max.y, min.z, 1.0},
		{min.x, min.y, max.z, 1.0}, {max.x, min.y, max.z, 1.0},
		{max.x, max.y, max.z, 1.0}, {min.x, max.y, max.z, 1.0}
	};

	// Define the 12 edges of the bounding box
	std::vector<std::pair<int, int>> edges = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
		{4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
		{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
	};

	// Draw each edge using the helper function
	for (const auto& edge : edges) {
		DrawLineHelper(pDC, corners[edge.first], corners[edge.second], screenHeight, color);
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
	scene.setColors(pApp->Object_color, RGB(0, 255, 0), pApp->Background_color); // Set colors
	pDCToUse->FillSolidRect(&r, scene.getBackgroundColor()); // Fill background color

	const double screenHeight = static_cast<double>(r.Height());

	if (!scene.getPolygons()->empty()) {
		for (Poly* poly : *scene.getPolygons()) {
			const std::vector<Vertex>& vertices = poly->getVertices();
			COLORREF color = poly->getColor();

			if (m_uniform_color) {
				color = RGB(255, 255, 255);
			}

			// Draw polygon edges and vertex normals
			DrawPolygonEdgesAndVertexNormals(pDCToUse, poly, screenHeight, pApp->Object_color);

			// Draw polygon normals
			DrawPolygonNormal(pDCToUse, poly, screenHeight, color);
		}

		// Draw bounding box if flag is set
		if (scene.hasBoundingBox && m_draw_bounding_box) {
			DrawBoundingBox(pDCToUse, scene.getBoundingBox(), screenHeight, RGB(0, 0, 255)); // Blue for bounding box
		}

		if (pDCToUse != m_pDC) {
			m_pDC->BitBlt(r.left, r.top, r.Width(), r.Height(), pDCToUse, r.left, r.top, SRCCOPY);
		}
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





Matrix4 CCGWorkView::getMatrixToCenterObject() {
	// Calculate bounding box and determine initial transformation
	scene.calculateBoundingBox();

	const BoundingBox& bbox = scene.getBoundingBox();
	const Vector4& min = bbox.min;
	const Vector4& max = bbox.max;

	const Vector4 center = Vector4(
		(min.x + max.x) / 2.0,
		(min.y + max.y) / 2.0,
		(min.z + max.z) / 2.0,
		1.0 // Maintain consistent w for homogeneous coordinates
	);
	CRect r;
	GetClientRect(&r);

	const double screenWidth = static_cast<double>(r.Width());
	const double screenHeight = static_cast<double>(r.Height());
	const double marginFactor = 0.25; // initial load will onlu go from 0.25 to 0.75 of the screen in x and y. meaning the center of the window

	const double sceneWidth = max.x - min.x;
	const double sceneHeight = max.y - min.y;

	const double targetWidth = screenWidth * (1.0 - marginFactor);
	const double targetHeight = screenHeight * (1.0 - marginFactor);

	const double scaleX = sceneWidth > 1e-6 ? targetWidth / sceneWidth : 1.0;
	const double scaleY = sceneHeight > 1e-6 ? targetHeight / sceneHeight : 1.0;

	const double sceneScale = (scaleX < scaleY) ? scaleX : scaleY;

	Matrix4 t; // Starts as the identity matrix

	const Matrix4 translateToOrigin = Matrix4::translate(-center.x, -center.y, -center.z);
	const Matrix4 scaling = Matrix4::scale(sceneScale, sceneScale, sceneScale);
	const Matrix4 translateToScreen = Matrix4::translate(screenWidth / 2.0, screenHeight / 2.0, 0.0);

	// Translate to origin (center the scene)
	t = t * translateToScreen;
	// Scale the scene to fit within the target screen area
	t = t * scaling;
	// Translate to the screen center
	t = t * translateToOrigin;
	return t;
}



void CCGWorkView::OnFileLoad() {
	TCHAR szFilters[] = _T("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal() == IDOK) {
		m_strItdFileName = dlg.GetPathName(); // Full path and filename
		scene.clear(); // Clear the existing scene data
		//scene.hasBoundingBox = true;

		// Load and process the IRIT file
		CGSkelProcessIritDataFiles(m_strItdFileName, 1);

		// Calculate bounding box and determine initial transformation
		Matrix4 t = getMatrixToCenterObject();

		scene.calculateVertexNormals();


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

	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
	{
		dlg.SetDialogData((LightID)id, m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT, m_ambientLight);

	if (dlg.DoModal() == IDOK)
	{
		for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
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

	CString str;
	str.Format(_T("Perspective Settings"));
	STATUS_BAR_TEXT(str);
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
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();

	if (dlg.DoModal() == IDOK)
	{
		//	t_slider_value = dlg.TranslationSensitivity;

			// Handle OK click if needed
	}
}

void CCGWorkView::OnBoundingBox()
{
	//scene.hasBoundingBox = !scene.hasBoundingBox;
	m_draw_bounding_box = !m_draw_bounding_box;
	Invalidate();

}
void CCGWorkView::OnUpdateBoundingBox(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_draw_bounding_box);
}


void CCGWorkView::OnVertexNormal()
{
	//scene.hasBoundingBox = !scene.hasBoundingBox;
	m_draw_vertex_normals = !m_draw_vertex_normals;
	Invalidate();

}
void CCGWorkView::OnUpdateVertexNormal(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_draw_vertex_normals);
}

void CCGWorkView::OnPolyNormal()
{
	//scene.hasBoundingBox = !scene.hasBoundingBox;
	m_draw_poly_normals = !m_draw_poly_normals;
	Invalidate();

}

void CCGWorkView::OnUpdatePolyNormal(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_draw_poly_normals);
}

void CCGWorkView::OnPolyNormalsNotFrom()
{
	m_draw_poly_normals_not_from = !m_draw_poly_normals_not_from;
	Invalidate();
}

void CCGWorkView::OnUpdatePolyNormalsNotFrom(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_draw_poly_normals_not_from);
}

void CCGWorkView::OnPolyNormalsFrom()
{
	m_draw_poly_normals_from = !m_draw_poly_normals_from;
	Invalidate();
}

void CCGWorkView::OnUpdatePolyNormalsFrom(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_draw_poly_normals_from);
}

void CCGWorkView::OnVertexNormalsFrom()
{
	m_draw_vertex_normals_from = !m_draw_vertex_normals_from;
	Invalidate();
}

void CCGWorkView::OnUpdateVertexNormalsFrom(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_draw_vertex_normals_from);
}

void CCGWorkView::OnVertexNormalsNotFrom()
{
	m_draw_vertex_normals_not_from = !m_draw_vertex_normals_not_from;
	Invalidate();
}

void CCGWorkView::OnUpdateVertexNormalsNotFrom(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_draw_vertex_normals_not_from);
}

/////////////////////////////// MOUSE CONTROL //////////////////////////////////////////////

void CCGWorkView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Begin dragging
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	m_isDragging = true;
	SetCapture();  // Capture mouse events globally for this window

	//pApp->start = point;
	// Log or process the starting point
	TRACE(_T("Mouse Down at: X=%d, Y=%d\n"), point.x, point.y);

	prev_start = point;

	CView::OnLButtonDown(nFlags, point);
}

void CCGWorkView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_isDragging)
	{
		// Handle dragging logic (e.g., drawing, updating UI, etc.)
		//TRACE(_T("Mouse Move at: X=%d, Y=%d\n"), point.x, point.y);


		// If needed, convert to screen coordinates
		CPoint screenPoint = point;
		ClientToScreen(&screenPoint);
		//TRACE(_T("Mouse Move (Screen): X=%d, Y=%d\n"), screenPoint.x, screenPoint.y);


		//CString str;
		//str.Format(_T("Mouse Move (Screen): X=%d, Y=%d\n"), screenPoint.x, screenPoint.y);
		//STATUS_BAR_TEXT(str);

		int diffrence = prev_start.x - point.x;
		prev_start = point;
		CString str;
		str.Format(_T("raw diff= %d    calculated diff %d\n"), diffrence, diffrence / MOUSE_FACTOR);
		STATUS_BAR_TEXT(str);
		diffrence = diffrence / MOUSE_FACTOR;

		MapMouseMovement(diffrence);


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

		//pApp->end = point;
		//ApplyXRotation();
		//Invalidate();

		// Log or process the final point
		TRACE(_T("Mouse Up at: X=%d, Y=%d\n"), point.x, point.y);
		CString str;
		str.Format(_T("mouse bn up"));
		//STATUS_BAR_TEXT(str);
	}

	CView::OnLButtonUp(nFlags, point);
}


void CCGWorkView::ApplyXRotation(int d) {


	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 tranformation;
	Matrix4 t = tranformation.rotateX(d);
	CString str;
	str.Format(_T("deg = %d, sens = %d"), d, pApp->r_slider_value);
	STATUS_BAR_TEXT(str);
	ApplyTransformation(t);


}

void CCGWorkView::ApplyYRotation(int d) {


	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 tranformation;
	Matrix4 t = tranformation.rotateY(d);

	ApplyTransformation(t);

}

void CCGWorkView::ApplyZRotation(int d) {


	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 tranformation;
	Matrix4 t = tranformation.rotateZ(d);

	ApplyTransformation(t);

}

void CCGWorkView::ApplyXTranslation(int d) {


	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 tranformation;
	Matrix4 t = tranformation.translate(d, 1, 1);

	ApplyTransformation(t);

}

void CCGWorkView::ApplyYTranslation(int d) {


	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 tranformation;
	Matrix4 t = tranformation.translate(1, d, 1);

	ApplyTransformation(t);

}

void CCGWorkView::ApplyZTranslation(int d) {


	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 tranformation;
	Matrix4 t = tranformation.translate(1, 1, d);

	ApplyTransformation(t);

}

void CCGWorkView::ApplyXScale(int d) {


	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 tranformation;
	Matrix4 t = tranformation.scale(d, 1, 1);

	ApplyTransformation(t);

}

void CCGWorkView::ApplyYScale(int d) {


	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 tranformation;
	Matrix4 t = tranformation.scale(1, d, 1);

	ApplyTransformation(t);

}

void CCGWorkView::ApplyZScale(int d) {


	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 tranformation;
	Matrix4 t = tranformation.scale(1, 1, d);

	ApplyTransformation(t);

}


void CCGWorkView::ApplyTransformation(Matrix4& t)
{
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

	const double screenWidth = static_cast<double>(r.Width());
	const double screenHeight = static_cast<double>(r.Height());
	const double marginFactor = 0.25; // initial load will onlu go from 0.25 to 0.75 of the screen in x and y. meaning the center of the window

	const double sceneWidth = max.x - min.x;
	const double sceneHeight = max.y - min.y;

	const double targetWidth = screenWidth * (1.0 - marginFactor);
	const double targetHeight = screenHeight * (1.0 - marginFactor);

	const double scaleX = sceneWidth > 1e-6 ? targetWidth / sceneWidth : 1.0;
	const double scaleY = sceneHeight > 1e-6 ? targetHeight / sceneHeight : 1.0;

	const double sceneScale = (scaleX < scaleY) ? scaleX : scaleY;


	const Matrix4 translateToOrigin = Matrix4::translate(-center.x, -center.y, -center.z);
	const Matrix4 scaling = Matrix4::scale(sceneScale, sceneScale, sceneScale);
	const Matrix4 translateToScreen = Matrix4::translate(screenWidth / 2.0, screenHeight / 2.0, 0.0);

	// Translate to origin (center the scene)
	t = t * translateToScreen;
	// Scale the scene to fit within the target screen area
	t = t * scaling;
	// Translate to the screen center
	t = t * translateToOrigin;

	scene.applyTransform(t);
}

void CCGWorkView::MapMouseMovement(int deg)
{
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	if (m_nAction == ID_ACTION_ROTATE)
	{
		deg = deg * pApp->r_slider_value;
		if (m_nAxis == ID_AXIS_X) ApplyXRotation(deg);
		if (m_nAxis == ID_AXIS_Y) ApplyYRotation(deg);
		if (m_nAxis == ID_AXIS_Z) ApplyZRotation(deg);
	}
	if (m_nAction == ID_ACTION_TRANSLATE)
	{
		deg = deg * pApp->t_slider_value;
		if (m_nAxis == ID_AXIS_X) ApplyXTranslation(deg);
		if (m_nAxis == ID_AXIS_Y)  ApplyYTranslation(deg);
		if (m_nAxis == ID_AXIS_Z)  ApplyZTranslation(deg);
	}
	if (m_nAction == ID_ACTION_SCALE)
	{
		deg = deg * pApp->s_slider_value;
		if (m_nAxis == ID_AXIS_X) ApplyXScale(deg);
		if (m_nAxis == ID_AXIS_Y) ApplyYScale(deg);
		if (m_nAxis == ID_AXIS_Z) ApplyZScale(deg);
	}
}






////////////polygon finess:
void CCGWorkView::OnUpdateOptionsPolygonFineness(CCmdUI* pCmdUI)
{
	// Enable the menu item
	pCmdUI->Enable(TRUE);
}


void CCGWorkView::OnOptionsPolygonFineness()
{
	// Create and display the dialog
	CDialog finenessDialog(IDD_FINENESS_CONTROL);
	if (finenessDialog.DoModal() == IDOK)
	{
		// Handle the dialog's OK action if needed
	}
}



//controlling the slider for the polygon tessetation finess
void CCGWorkView::InitializeFinenessSlider()
{
	// Get the slider control by its ID
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FINENESS);
	if (pSlider != nullptr)
	{
		pSlider->SetRange(2, 100);                  // Set the range from 2 to 100
		pSlider->SetPos(CGSkelFFCState.FineNess);  // Initialize with the current fineness value
		m_finenessSlider.Attach(pSlider->m_hWnd);  // Attach the slider to the member variable
	}
}

void CCGWorkView::OnFinenessSliderChanged()
{
	// Get the current value from the slider
	int fineness = m_finenessSlider.GetPos();

	// Update the global tessellation fineness variable
	CGSkelFFCState.FineNess = fineness;

	// Update the scene with the new fineness
	UpdateSceneForFineness();
}

void CCGWorkView::UpdateSceneForFineness()
{
	// Trigger a redraw of the view
	Invalidate();
	UpdateWindow();
}