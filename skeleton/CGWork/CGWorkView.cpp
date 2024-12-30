// CGWorkView.cpp : implementation of the CCGWorkView class
//
#include "stdafx.h"
#include "CGWork.h"
#include "ScanConvertZBuffer.h"
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
#include "PolygonFineness.h"
#include <thread>


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

	ON_COMMAND(ID_VIEW_VIEW, &CCGWorkView::OnViewView)
	ON_COMMAND(ID_VIEW_OBJECT, &CCGWorkView::OnViewObject)

	ON_UPDATE_COMMAND_UI(ID_VIEW_VIEW, &CCGWorkView::OnUpdateViewView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJECT, &CCGWorkView::OnUpdateViewObject)

	ON_COMMAND(ID_VIEW_DRAW_SILS, &CCGWorkView::OnDrawSilhouettes)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAW_SILS, &CCGWorkView::OnUpdateDrawSilhouettes)
	
	ON_COMMAND(ID_DRAWS_FLIP_NORMALS, &CCGWorkView::OnFlipNormals)
	ON_UPDATE_COMMAND_UI(ID_DRAWS_FLIP_NORMALS, &CCGWorkView::OnUpdateFlipNormals)

	ON_COMMAND(ID_RENDER_TOFILE, &CCGWorkView::OnRenderToFile)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TOFILE, &CCGWorkView::OnUpdateRenderToFile)

	ON_COMMAND(ID_RENDER_TOSCREEN, &CCGWorkView::OnRenderToScreen)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TOSCREEN, &CCGWorkView::OnUpdateRenderToScreen)

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
	m_is_object_space_transform = 1;//default to transform relative to object center and not to 0,0
	m_flip_normals = 0;
	m_draw_silhouettes = 0;
	m_render_to_screen = true;
	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;
	m_nView = ID_VIEW_ORTHOGRAPHIC;

	m_draw_to_screen = true;

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



void CCGWorkView::DrawPolygonEdgesAndVertexNormals(CDC* pDC, Poly* poly, double screenHeight, COLORREF color, COLORREF c2) {
	const std::vector<Vertex>& vertices = poly->getVertices();
	const size_t vertexCount = vertices.size();

	for (size_t i = 0; i < vertexCount; ++i) {
		const Vertex& start = vertices[i];
		const Vertex& end = vertices[(i + 1 < vertexCount) ? i + 1 : 0];

		// Draw polygon edge
		DrawLineHelper(pDC, start, end, screenHeight, color);

		// Draw vertex normals from file
		if (m_draw_vertex_normals_from ) {
			// Draw normals provided from file
			const Normal& normalFromFile = start.getNormalFromFile();
			DrawLineHelper(pDC, normalFromFile.start, normalFromFile.end, screenHeight, c2);
		}
		//draw vertex normals calculated
		if (m_draw_vertex_normals_not_from) {
			// Draw calculated normals
			const Normal& normalCalculated = start.getNormalCalculated();
			DrawLineHelper(pDC, normalCalculated.start, normalCalculated.end, screenHeight, c2);
		}
		
	}
}





void CCGWorkView::DrawPolygonNormal(CDC* pDC, Poly* poly, double screenHeight, COLORREF color) {
	// Draw polygon normals from file
	if (m_draw_poly_normals_from) {
		const Normal& normalFromFile = poly->getPolyNormalFromFile();
		DrawLineHelper(pDC, normalFromFile.start, normalFromFile.end, screenHeight, color); // Blue
	}

	// Draw calculated polygon normals
	if (m_draw_poly_normals_not_from) {
		const Normal& normalCalculated = poly->getPolyNormalCalculated();
		DrawLineHelper(pDC, normalCalculated.start, normalCalculated.end, screenHeight, color); // Orange
	}
}

void CCGWorkView::DrawBoundingBox(CDC* pDC, const BoundingBox& bbox, double screenHeight, COLORREF color) {
	std::vector<Vector4> corners = bbox.getCorners();

	std::vector<std::pair<int, int>> edges = {
		{0, 1}, {1, 3}, {3, 2}, {2, 0}, // Top face
		{4, 5}, {5, 7}, {7, 6}, {6, 4}, // Bottom face
		{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
	};

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
	const size_t width = static_cast<size_t>(r.Width());
	const size_t height = static_cast<size_t>(r.Height());
	const COLORREF green = RGB(0, 255, 0);

	// Initialize Z-buffer
	Point* zBuffer = initZBuffer(width, height);

	// Draw edges, normals, and interiors using Z-buffer
	if (!scene.getPolygons()->empty()) {
		for (Poly* poly : *scene.getPolygons()) {
			const std::vector<Vertex>& vertices = poly->getVertices();
			COLORREF color = poly->getColor();

			// Convert vertices to Point objects for Z-buffer rendering
			std::vector<Point> polygonPoints;
			for (const Vertex& vertex : vertices) {
				polygonPoints.emplace_back(vertex.x, vertex.y, vertex.z, 1.0f, color);
			}

			// Render the polygon into the Z-buffer
			renderPolygon(zBuffer, width, height, polygonPoints);
			// Draw polygon edges and vertex normals
			DrawPolygonEdgesAndVertexNormals(pDCToUse, poly, screenHeight, pApp->Object_color, pApp->vertex_normals_color);
			// Draw polygon normals
			DrawPolygonNormal(pDCToUse, poly, screenHeight, pApp->poly_normals_color);

		}

		// Render Z-buffer contents onto the screen
		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				const Point& point = zBuffer[y * width + x];
				if (point.z < FLT_MAX) { // Ignore uninitialized pixels
					pDCToUse->SetPixel(x, static_cast<int>(screenHeight - y), point.getColor());
				}
			}
		}

		// Draw bounding box if flag is set
		if (scene.hasBoundingBox && m_draw_bounding_box) {
			DrawBoundingBox(pDCToUse, scene.getBoundingBox(), screenHeight, green); // Green for bounding box
		}
	}

	// Cleanup Z-buffer
	freeZBuffer(zBuffer);

	// Copy the double-buffered image to the screen
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





Matrix4 CCGWorkView::getMatrixToCenterObject() {
	// Calculate bounding box and determine initial transformation
	scene.calculateBoundingBox();

	const BoundingBox& bbox = scene.getBoundingBox();
	std::vector<Vector4> corners = bbox.getCorners(); // Get all corners of the bounding box

	// Calculate min and max using appropriate corners
	const Vector4& min = corners[5]; // BLB: Bottom-Left-Backward corner
	const Vector4& max = corners[2]; // TRF: Top-Right-Forward corner

	// Compute the center of the bounding box
	const Vector4 center = Vector4(
		(min.x + max.x) / 2.0,
		(min.y + max.y) / 2.0,
		(min.z + max.z) / 2.0,
		1.0 // Maintain consistent w for homogeneous coordinates
	);

	// Get the client rectangle
	CRect r;
	GetClientRect(&r);

	// Screen dimensions
	const double screenWidth = static_cast<double>(r.Width());
	const double screenHeight = static_cast<double>(r.Height());

	// Margin factor for initial load (centered on the window)
	const double marginFactor = 0.47;

	// Scene dimensions
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
void PerspectiveParametersThread()
{
	// Create and open the dialog
	PerspectiveDlg paramDlg;
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();

	// Initialize default values
	paramDlg.d = pApp->d;                   // Camera distance
	paramDlg.fovy = pApp->fovy;             // Vertical field of view
	paramDlg.aspectRatio = pApp->aspectRatio; // Aspect ratio
	paramDlg.nearPlane = pApp->nearPlane;   // Near clipping plane
	paramDlg.farPlane = pApp->farPlane;     // Far clipping plane

	if (paramDlg.DoModal() == IDOK)
	{
		// Retrieve updated values after the dialog is closed
		pApp->d = paramDlg.d;
		pApp->fovy = paramDlg.fovy;
		pApp->aspectRatio = paramDlg.aspectRatio;
		pApp->nearPlane = paramDlg.nearPlane;
		pApp->farPlane = paramDlg.farPlane;

		// Apply the parameters if needed (e.g., to update a perspective matrix)
	}
}

void CCGWorkView::OnPerspectiveParameters()
{
	std::thread dialogThread(PerspectiveParametersThread);
	dialogThread.detach();
}

void MouseDlgThread() {
	// TODO: Add your command handler code here
	CDialog* dlg = new MouseSensitivityDlg();
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	if (dlg->DoModal() == IDOK)
	{
		//	t_slider_value = dlg.TranslationSensitivity;

			// Handle OK click if needed
	}

}

void CCGWorkView::OnOptionsMousesensitivity()
{
	std::thread dialogThread(MouseDlgThread);
	dialogThread.detach(); 


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
		//str.Format(_T("raw diff= %d calculated diff %d\n"), diffrence, diffrence / MOUSE_FACTOR);
		STATUS_BAR_TEXT(str);
		diffrence = diffrence / MOUSE_FACTOR;

		MapMouseMovement(diffrence);


	}

	CString str;
	str.Format(_T("mouse position = ( %d , %d )\n"), point.x, point.y);
	STATUS_BAR_TEXT(str);

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


////transformation matrices
Matrix4 CreateCenteredRotationMatrix(const Matrix4& rotationMatrix) {
	Vector4 center = scene.getObjectCenter();

	// Create translation matrices
	Matrix4 translateToOrigin = Matrix4::translate(-center.x, -center.y, -center.z);
	Matrix4 translateBack = Matrix4::translate(center.x, center.y, center.z);

	// Combine transformations: Translate to origin -> Rotate -> Translate back
	return translateBack * rotationMatrix * translateToOrigin;
}



Matrix4 CreateCenteredScalingMatrix(double scaleX, double scaleY, double scaleZ) {
	Vector4 center = scene.getObjectCenter();
	// Create translation matrices
	Matrix4 translateToOrigin = Matrix4::translate(-center.x, -center.y, -center.z);

	// Scaling matrix
	Matrix4 scaling = Matrix4::scale(scaleX, scaleY, scaleZ);

	// Translation back to the center
	Matrix4 translateBack = Matrix4::translate(center.x, center.y, center.z);

	// Combine transformations: Translate to origin -> Scale -> Translate back
	return translateBack * scaling * translateToOrigin;
}


// Apply rotation around the X-axis
void CCGWorkView::ApplyXRotation(int d) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();

	// Create the rotation matrix
	Matrix4 r = Matrix4::rotateX(d);

	if (m_is_object_space_transform) {
		 r = CreateCenteredRotationMatrix(r);
	}
	// Create the centered rotation matrix

	// Apply the transformation
	ApplyTransformation(r);

	// Update the status bar
	CString str;
	str.Format(_T("X-Rotation: deg = %d, sens = %d"), d, pApp->r_slider_value);
	STATUS_BAR_TEXT(str);
}

// Apply rotation around the Y-axis
void CCGWorkView::ApplyYRotation(int d) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();

	// Create the rotation matrix
	Matrix4 r = Matrix4::rotateY(d);

	if (m_is_object_space_transform) {
		r = CreateCenteredRotationMatrix(r);
	}
	// Create the centered rotation matrix

	// Apply the transformation
	ApplyTransformation(r);

	// Update the status bar
	CString str;
	str.Format(_T("Y-Rotation: deg = %d, sens = %d"), d, pApp->r_slider_value);
	STATUS_BAR_TEXT(str);
}

// Apply rotation around the Z-axis
void CCGWorkView::ApplyZRotation(int d) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();

	// Create the rotation matrix
	Matrix4 r = Matrix4::rotateZ(d);

	// Create the centered rotation matrix
	if (m_is_object_space_transform) {
		r = CreateCenteredRotationMatrix(r);
	}
	// Apply the transformation
	ApplyTransformation(r);

	// Update the status bar
	CString str;
	str.Format(_T("Z-Rotation: deg = %d, sens = %d"), d, pApp->r_slider_value);
	STATUS_BAR_TEXT(str);
}


void CCGWorkView::ApplyXTranslation(int d) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 transformation = Matrix4::translate(d, 0, 0);

	ApplyTransformation(transformation);

	// Update status bar
	CString str;
	str.Format(_T("Translated along X: %d"), d);
	STATUS_BAR_TEXT(str);
}

void CCGWorkView::ApplyYTranslation(int d) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 transformation = Matrix4::translate(0, d, 0);

	ApplyTransformation(transformation);

	// Update status bar
	CString str;
	str.Format(_T("Translated along Y: %d"), d);
	STATUS_BAR_TEXT(str);
}

void CCGWorkView::ApplyZTranslation(int d) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	Matrix4 transformation = Matrix4::translate(0, 0, d);

	ApplyTransformation(transformation);

	// Update status bar
	CString str;
	str.Format(_T("Translated along Z: %d"), d);
	STATUS_BAR_TEXT(str);
}


double validateFactor(double factor) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	if (factor == 0) {
		return 1;
	}
	else if (factor >= 0) {
		return 1.2 * pApp->s_slider_value;
	}
	else {
		return 0.81 / pApp->s_slider_value;;
	}
	/*
	const double threshold = 0.01;
	if (std::abs(factor) < threshold) {
		// Return the threshold value with the same sign as the original factor
		return (factor < 0) ? -threshold : threshold;
	}
	return factor; // Return the original factor if it's above the threshold
	*/
	}

void CCGWorkView::ApplyXScale(double factor) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	const double  f = validateFactor(factor);
	Matrix4 scalingMatrix = CreateCenteredScalingMatrix(f, 1.0, 1.0);
	ApplyTransformation(scalingMatrix);

	// Update status bar
	CString str;
	str.Format(_T("Scaled along X by factor: %.3f"), f);
	STATUS_BAR_TEXT(str);
}

void CCGWorkView::ApplyYScale(double factor) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	const double f = validateFactor(factor);

	Matrix4 scalingMatrix = CreateCenteredScalingMatrix(1.0, f, 1.0);
	ApplyTransformation(scalingMatrix);

	// Update status bar
	CString str;
	str.Format(_T("Scaled along Y by factor: %.3f"), f);
	STATUS_BAR_TEXT(str);
}

void CCGWorkView::ApplyZScale(double factor) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();
	const double f = validateFactor(factor);

	Matrix4 scalingMatrix = CreateCenteredScalingMatrix(1.0, 1.0, f); // Use factor, not 1.0 / factor
	ApplyTransformation(scalingMatrix);

	// Update status bar
	CString str;
	str.Format(_T("Scaled along Z by factor: %.3f"), f);
	STATUS_BAR_TEXT(str);
}

void CCGWorkView::ApplyTransformation(Matrix4& t)
{
	scene.applyTransform(t); // Directly apply the transformation to the scene
	Invalidate(); // Trigger a redraw
}

void CCGWorkView::MapMouseMovement(int deg) {
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();

	// Scale deg based on the selected action
	if (m_nAction == ID_ACTION_ROTATE) {
		deg *= pApp->r_slider_value;
	}
	else if (m_nAction == ID_ACTION_TRANSLATE) {
		deg *= pApp->t_slider_value;
	}
	else if (m_nAction == ID_ACTION_SCALE) {
		deg *= pApp->s_slider_value;
	}
	else {
		return; // Invalid action, exit early
	}

	// Perform the transformation based on the selected axis
	switch (m_nAxis) {
	case ID_AXIS_X:
		if (m_nAction == ID_ACTION_ROTATE) ApplyXRotation(deg);
		else if (m_nAction == ID_ACTION_TRANSLATE) ApplyXTranslation(deg);
		else if (m_nAction == ID_ACTION_SCALE) ApplyXScale(deg);
		break;
	case ID_AXIS_Y:
		if (m_nAction == ID_ACTION_ROTATE) ApplyYRotation(deg);
		else if (m_nAction == ID_ACTION_TRANSLATE) ApplyYTranslation(deg);
		else if (m_nAction == ID_ACTION_SCALE) ApplyYScale(deg);
		break;
	case ID_AXIS_Z:
		if (m_nAction == ID_ACTION_ROTATE) ApplyZRotation(deg);
		else if (m_nAction == ID_ACTION_TRANSLATE) ApplyZTranslation(deg);
		else if (m_nAction == ID_ACTION_SCALE) ApplyZScale(deg);
		break;
	default:
		// Invalid axis, do nothing
		break;
	}

}
void CheckMenuItem(UINT menuID, bool checked) {
	CMenu* pMenu = AfxGetMainWnd()->GetMenu(); // Get the main menu
	if (pMenu) {
		pMenu->CheckMenuItem(menuID, checked ? MF_CHECKED | MF_BYCOMMAND : MF_UNCHECKED | MF_BYCOMMAND);
	}
}


void CCGWorkView::OnUpdateViewView(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_is_object_space_transform == 0);
}

void CCGWorkView::OnUpdateViewObject(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_is_object_space_transform == 1);
}

void CCGWorkView::OnViewView() {
	m_is_object_space_transform = 0; // Set flag to View
	CheckMenuItem(ID_VIEW_VIEW, true);
	CheckMenuItem(ID_VIEW_OBJECT, false);
}

void CCGWorkView::OnViewObject() {
	m_is_object_space_transform = 1; // Set flag to World
	CheckMenuItem(ID_VIEW_VIEW, false);
	CheckMenuItem(ID_VIEW_OBJECT, true);
}


void CCGWorkView::OnDrawSilhouettes() {
	m_draw_silhouettes = 1 - m_draw_silhouettes; // Set flag to View
	CheckMenuItem(ID_VIEW_DRAW_SILS, true);
}
void CCGWorkView::OnUpdateDrawSilhouettes(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_draw_silhouettes == 1);
}

void CCGWorkView::OnFlipNormals() {
	m_flip_normals = 1 - m_flip_normals;
	scene.flipNormals();

}
void CCGWorkView::OnUpdateFlipNormals(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_flip_normals == 1);
}

void CCGWorkView::OnRenderToFile() {
	m_render_to_screen = !m_render_to_screen;
}

void CCGWorkView::OnUpdateRenderToFile(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_render_to_screen == false);
}

void CCGWorkView::OnRenderToScreen() {
	m_render_to_screen = !m_render_to_screen;
}

void CCGWorkView::OnUpdateRenderToScreen(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_render_to_screen == true);
}


////////////polygon finess:


////////////polygon finess:
void CCGWorkView::OnUpdateOptionsPolygonFineness(CCmdUI* pCmdUI)
{
	// Enable the menu item
	pCmdUI->Enable(TRUE);
}


void PolygonFinenessThread()
{
	// Create and display the dialog
	PolygonFineness Dlg;
	CCGWorkApp* pApp = (CCGWorkApp*)AfxGetApp();




	if (Dlg.DoModal() == IDOK)
	{
		// Retrieve updated values after the dialog is closed

		// Use the parameters (e.g., store or apply them)

	}
}


void CCGWorkView::OnOptionsPolygonFineness()
{
	std::thread dialogThread(PolygonFinenessThread);
	dialogThread.detach();
}