#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Poly.h"
#include "Vertex.h"
#include "Matrix4.h"
#include "Vector4.h"
#include <Windows.h>

struct BoundingBox {
    Vector4 min; // Minimum coordinates
    Vector4 max; // Maximum coordinates
};

class Scene {
private:
    std::vector<Poly*> *polygons;   // List of polygons in the scene
    Matrix4 sceneTransform;       // Transformation matrix for the entire scene
    BoundingBox boundingBox;      // Scene bounding box

    COLORREF wireframeColor;      // Custom wireframe color
    COLORREF normalColor;         // Custom normal color
    COLORREF backgroundColor;     // Custom background color

    double sensitivity;           // Sensitivity factor for transformations

    bool showNormals;             // Flag to show normals
    bool showBoundingBox;         // Flag to show bounding box

    bool hasVertexNormals;        // Indicates if the scene has vertex normals

    bool isFirstDraw;             // Indicates if it is the first drawing

public:
    Scene();
    

    bool hasBoundingBox;

    // Add a polygon to the scene
    void addPolygon(Poly* poly);

    // Get all polygons in the scene
    const std::vector<Poly*>* getPolygons() const;

    // Get the total number of polygons
    size_t getPolygonCount() const;

    // Apply a transformation to the entire scene
    void applyTransform(const Matrix4& transform);

    // Apply transformation to the bounding box
    void applyTransformToBoundingBox(const Matrix4& transform);

    // Calculate the bounding box of the scene
    void calculateBoundingBox();

    // Calculate the bounding box from all vertices
    void calculateBoundingBoxFromVertices();

    // Get the bounding box of the scene
    const BoundingBox& getBoundingBox() const;

    // Set custom colors for wireframe, normals, and background
    void setColors(COLORREF wireframe, COLORREF normal, COLORREF background);

    // Get custom colors
    COLORREF getWireframeColor() const;
    COLORREF getNormalColor() const;
    COLORREF getBackgroundColor() const;

    // Set sensitivity for transformations
    void setSensitivity(double newSensitivity);

    // Get sensitivity value
    double getSensitivity() const;

    void setShowNormals(bool show);
    bool isShowNormals() const;

    void setShowBoundingBox(bool show);
    bool isShowBoundingBox() const;

    // Attributes for normals
    void updateHasVertexNormals(bool status);
    bool hasVertexNormalsAttribute() const;

    void updateIsFirstDraw(bool status);
    bool getIsFirstDraw() const;

    // Clear the scene (reset the polygons, transformations, and bounding box)
    void clear();
};

#endif // SCENE_H
