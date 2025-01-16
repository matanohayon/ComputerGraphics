#include "stdafx.h"
#include "iritSkel.h"
/*****************************************************************************
* Skeleton for an interface to a parser to read IRIT data files.			 *
******************************************************************************
* (C) Gershon Elber, Technion, Israel Institute of Technology                *
******************************************************************************
* Written by:  Gershon Elber				Ver 1.0, Feb 2002				 *
* Minimal changes made by Amit Mano			November 2008					 *
******************************************************************************/

/*data structure for the entire scene

*/
#include "Vector4.h" // Include Vector4
#include "Vertex.h" // Include Vector4
#include "Normal.h"
#include "Poly.h"    // Include Poly (renamed from Polygon)
#include "Scene.h"  // Include the Scene class definition
Scene scene; // Global scene object to hold all polygons


IPFreeformConvStateStruct CGSkelFFCState = {
	FALSE,          /* Talkative */
	FALSE,          /* DumpObjsAsPolylines */
	TRUE,           /* DrawFFGeom */
	FALSE,          /* DrawFFMesh */
	{ 10, 10, 10 }, /* 10 isocurves peru/v/w direction. */
	100,            /* 100 point samples along a curve. */
	SYMB_CRV_APPROX_UNIFORM,  /* CrvApproxMethod */
	FALSE,   /* ShowIntrnal */
	FALSE,   /* CubicCrvsAprox */
	20,      /* Polygonal FineNess */
	FALSE,   /* ComputeUV */
	TRUE,    /* ComputeNrml */
	FALSE,   /* FourPerFlat */
	0,       /* OptimalPolygons */
	FALSE,   /* BBoxGrid */
	TRUE,    /* LinearOnePolyFlag */
	FALSE
};

//CGSkelProcessIritDataFiles(argv + 1, argc - 1);


/*****************************************************************************
* DESCRIPTION:                                                               *
* Main module of skeleton - Read command line and do what is needed...	     *
*                                                                            *
* PARAMETERS:                                                                *
*   FileNames:  Files to open and read, as a vector of strings.              *
*   NumFiles:   Length of the FileNames vector.								 *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelProcessIritDataFiles(CString &FileNames, int NumFiles) // parser process wrapper
{
	IPObjectStruct *PObjects;
	IrtHmgnMatType CrntViewMat;
	IPTraverseObjHierarchyStruct TraversState;
	/* Get the data files: */
	IPSetFlattenObjects(FALSE);
	CStringA CStr(FileNames);
	if ((PObjects = IPGetDataFiles((const char* const *)&CStr, 1/*NumFiles*/, TRUE, FALSE)) == NULL)
		return false;
	PObjects = IPResolveInstances(PObjects);

	if (IPWasPrspMat)
		MatMultTwo4by4(CrntViewMat, IPViewMat, IPPrspMat);
	else
		IRIT_GEN_COPY(CrntViewMat, IPViewMat, sizeof(IrtHmgnMatType));

	/* Here some useful parameters to play with in tesselating freeforms: */
	//CGSkelFFCState.FineNess = 22;   /* Res. of tesselation, larger is finer. */
	CGSkelFFCState.ComputeUV = TRUE;   /* Wants UV coordinates for textures. */
	CGSkelFFCState.FourPerFlat = TRUE;/* 4 poly per ~flat patch, 2 otherwise.*/
	CGSkelFFCState.LinearOnePolyFlag = TRUE;    /* Linear srf gen. one poly. */

	/* Traverse ALL the parsed data, recursively. */
	IPTraverseObjHierarchyInitState(&TraversState);
	TraversState.ApplyFunc = (IPApplyObjFuncType) CGSkelDumpOneTraversedObject;//call the traverse to parse
	IRIT_GEN_COPY(TraversState.Mat, CrntViewMat, sizeof(IrtHmgnMatType));
	IPTraverseObjListHierarchy(PObjects, &TraversState);
	return true;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Call back function of IPTraverseObjListHierarchy. Called on every non    *
* list object found in hierarchy.                                            *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:       Non list object to handle.                                   *
*   Mat:        Transformation matrix to apply to this object.               *
*   Data:       Additional data.                                             *
*                                                                            *
* RETURN VALUE:                                                              *
*   void									                                 *
*****************************************************************************/
void CGSkelDumpOneTraversedObject(IPObjectStruct *PObj,
                                  IrtHmgnMatType Mat,
                                  void *Data)
{
	IPObjectStruct *PObjs;

	if (IP_IS_FFGEOM_OBJ(PObj))
		PObjs = IPConvertFreeForm(PObj, &CGSkelFFCState);
	else
		PObjs = PObj;

	for (PObj = PObjs; PObj != NULL; PObj = PObj -> Pnext)
		if (!CGSkelStoreData(PObj)) 
			exit(1);
}


bool areNormalsSimilar(const Vector4& normal1, const Vector4& normal2, double epsilon = 0.001) {
	return (std::abs(normal1.x - normal2.x) <= epsilon &&
		std::abs(normal1.y - normal2.y) <= epsilon &&
		std::abs(normal1.z - normal2.z) <= epsilon);
}


/*****************************************************************************
* DESCRIPTION:                                                               *
*   Prints the data from given geometry object.								 *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:       Object to print.                                             *
*   Indent:     Column of indentation.                                       *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelStoreData(IPObjectStruct* PObj) {//we are calling this func once per "OBJECT" in the .itd file
	const char* Str;
	double RGB[3], Transp;
	IPPolygonStruct* PPolygon;
	IPVertexStruct* PVertex;
	const IPAttributeStruct* Attrs = AttrTraceAttributes(PObj->Attr, PObj->Attr);

	if (PObj->ObjType != IP_OBJ_POLY) {
		AfxMessageBox(_T("Non-polygonal object detected and ignored"));
		return true;
	}

	// Retrieve object color
	COLORREF polyColor = RGB(220, 220, 220); // Default to white
	if (CGSkelGetObjectColor(PObj, RGB)) {
		polyColor = RGB(static_cast<int>(RGB[0] * 255),static_cast<int>(RGB[1] * 255),static_cast<int>(RGB[2] * 255));
	}

	if (CGSkelGetObjectTransp(PObj, &Transp)) {
		// Handle transparency if needed
	}

	if ((Str = CGSkelGetObjectTexture(PObj)) != NULL) {
		// Handle volumetric texture if needed
	}

	if ((Str = CGSkelGetObjectPTexture(PObj)) != NULL) {
		// Handle parametric texture if needed
	}

	if (Attrs != NULL) {
		printf("[OBJECT\n");
		while (Attrs) {
			// Handle additional attributes if needed
			Attrs = AttrTraceAttributes(Attrs, NULL);
		}
	}

	// Iterate over polygons
	for (PPolygon = PObj->U.Pl; PPolygon != NULL; PPolygon = PPolygon->Pnext) {
		if (PPolygon->PVertex == NULL) {
			AfxMessageBox(_T("Attempt to dump empty polygon"));
			return false;
		}

		Poly* poly = new Poly; // Create a new Poly object for this polygon
		poly->setColor(polyColor); // Set the color for the polygon

		// Process vertices and calculate centroid
		Vector4 centroid(0.0, 0.0, 0.0, 0.0);
		int vertexCount = 0;

		PVertex = PPolygon->PVertex;



		do {

			// Create vertex from file coordinates
			Vertex vertex(PVertex->Coord[0], PVertex->Coord[1], PVertex->Coord[2]); 
			vertexCount++; // Update count of vertices
			centroid = centroid + vertex; // Update the centroid (later will divide by vertex count)


			// Process vertex normals (if available) comes from file
			if (IP_HAS_NORMAL_VRTX(PVertex)) {
				const Vector4 vertexNormal = Vector4(PVertex->Normal[0], PVertex->Normal[1], PVertex->Normal[2]).normalize(); // Normalize the vertex normal
				const Normal normal = Normal(vertex, vertex + vertexNormal);
				vertex.setNormalFromFile(normal);
			}

			//"vertex normal calculated"
			//using the next block we will add the vertex to the connectivity and later find the normal based on the polygons which are touching the vertex
			poly->addVertex(vertex); // Assign the vertex to the polygon
			PVertex = PVertex->Pnext; // Advance the pointer to the next vertex
			scene.addPolygonToConnectivity(vertex, poly); // Add connectivity. //in the CGWORKVIEW.cpp script, on file load we will handle the hashing

		} while (PVertex != PPolygon->PVertex && PVertex != NULL);


		centroid = centroid / static_cast<float>(vertexCount); // Finalize centroid. If the polygon is convex, the centroid should be inside.

		// add poly normal from file
		if (IP_HAS_PLANE_POLY(PPolygon)) {
			const Vector4 pn = Vector4(PPolygon->Plane[0], PPolygon->Plane[1], PPolygon->Plane[2]).normalize(); // Normal to the polygon plane -- start
			poly->setPolyNormalFromFile(Normal(centroid, centroid + pn)); // Normal provided by file
		}

		// add poly normal calculated
		const std::vector<Vertex>& vertices = poly->getVertices(); 
		const Vector4 edge1 = vertices[1] - vertices[0];
		const Vector4 edge2 = vertices[2] - vertices[0];
		const Vector4 calculatedNormal = edge1.cross(edge2).normalize(); // Cross the edges to get the normal to the plane
		poly->setPolyNormalCalculated(Normal(centroid, centroid + calculatedNormal)); // Normal calculated programmatically


		// Add the completed polygon to the global Scene
		scene.addPolygon(poly);
	}
	return true;
}





/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the color of an object.                                          *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its color.                                         *
*   RGB:    as 3 floats in the domain [0, 1].                                *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has color, FALSE otherwise.                       *
*****************************************************************************/
int CGSkelGetObjectColor(IPObjectStruct *PObj, double RGB[3])
{
	static int TransColorTable[][4] = {
		{ /* BLACK	*/   0,    0,   0,   0 },
		{ /* BLUE	*/   1,    0,   0, 255 },
		{ /* GREEN	*/   2,    0, 255,   0 },
		{ /* CYAN	*/   3,    0, 255, 255 },
		{ /* RED	*/   4,  255,   0,   0 },
		{ /* MAGENTA 	*/   5,  255,   0, 255 },
		{ /* BROWN	*/   6,   50,   0,   0 },
		{ /* LIGHTGRAY	*/   7,  127, 127, 127 },
		{ /* DARKGRAY	*/   8,   63,  63,  63 },
		{ /* LIGHTBLUE	*/   9,    0,   0, 255 },
		{ /* LIGHTGREEN	*/   10,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   11,   0, 255, 255 },
		{ /* LIGHTRED	*/   12, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 13, 255,   0, 255 },
		{ /* YELLOW	*/   14, 255, 255,   0 },
		{ /* WHITE	*/   15, 255, 255, 255 },
		{ /* BROWN	*/   20,  50,   0,   0 },
		{ /* DARKGRAY	*/   56,  63,  63,  63 },
		{ /* LIGHTBLUE	*/   57,   0,   0, 255 },
		{ /* LIGHTGREEN	*/   58,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   59,   0, 255, 255 },
		{ /* LIGHTRED	*/   60, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 61, 255,   0, 255 },
		{ /* YELLOW	*/   62, 255, 255,   0 },
		{ /* WHITE	*/   63, 255, 255, 255 },
		{		     -1,   0,   0,   0 }
	};
	int i, j, Color, RGBIColor[3];

	if (AttrGetObjectRGBColor(PObj,
		&RGBIColor[0], &RGBIColor[1], &RGBIColor[2])) {
			for (i = 0; i < 3; i++)
				RGB[i] = RGBIColor[i] / 255.0;

			return TRUE;
	}
	else if ((Color = AttrGetObjectColor(PObj)) != IP_ATTR_NO_COLOR) {
		for (i = 0; TransColorTable[i][0] >= 0; i++) {
			if (TransColorTable[i][0] == Color) {
				for (j = 0; j < 3; j++)
					RGB[j] = TransColorTable[i][j+1] / 255.0;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the volumetric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of volumetric texture map to apply, NULL if none.        *
*****************************************************************************/
const char *CGSkelGetObjectTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "texture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the parametric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its parametric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of parametric texture map to apply, NULL if none.        *
*****************************************************************************/
const char *CGSkelGetObjectPTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "ptexture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the transparency level of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*   Transp: Transparency level between zero and one.                         *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has transparency, FALSE otherwise.                *
*****************************************************************************/
int CGSkelGetObjectTransp(IPObjectStruct *PObj, double *Transp)
{
	*Transp = AttrGetObjectRealAttrib(PObj, "transp");

	return !IP_ATTR_IS_BAD_REAL(*Transp);
}

