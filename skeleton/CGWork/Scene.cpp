#include "Scene.h"

#include <algorithm>
#include <limits>

float SCALE_FACTOR = 17.0;

// Constructor
Scene::Scene()
    : sceneTransform(Matrix4()),
    boundingBox{ Vector4(FLT_MAX, FLT_MAX, FLT_MAX, 1.0), Vector4(FLT_MIN, FLT_MIN, FLT_MIN, 1.0) },
    wireframeColor(RGB(255, 255, 255)),  // Default white
    normalColor(RGB(0, 255, 0)),        // Default green
    backgroundColor(RGB(0, 0, 0)),      // Default black
    sensitivity(1.0),                   // Default sensitivity
    showNormals(false),                 // Default: don't show normals
    hasVertexNormals(false),            // Default: no vertex normals
    isFirstDraw(true),
    hasBoundingBox(false),
    showBoundingBox(false) {}           // Default: don't show bounding box

// Add a polygon to the scene
void Scene::addPolygon(const Poly& poly) {
    polygons.push_back(poly);
    calculateBoundingBox(); // Recalculate bounding box
}

// Get all polygons in the scene
const std::vector<Poly>& Scene::getPolygons() const {
    return polygons;
}

// Get the total number of polygons
size_t Scene::getPolygonCount() const {
    return polygons.size();
}


void Scene::applyTransform(const Matrix4& transform) {


    sceneTransform = transform * sceneTransform; // Pre-multiply to apply new transform first
    Matrix4 normalTransform = sceneTransform.inverse().transpose(); // Inverse-transpose for normals

    // Apply transformation to all polygons in the scene
    for (Poly& poly : polygons) {

        // Apply the full transformation to all vertices
        for (Vertex& vertex : poly.getVertices()) {
            // Transform vertex position
            vertex = transform.transform(vertex);

            if (vertex.getHasNormal()) {
                // Update normal start to match the transformed vertex position
                Vector4 transformedStart = transform.transform(vertex.getNormalStart());

                // Transform the normal direction using the inverse-transpose matrix
                Vector4 direction = vertex.getNormalEnd() - vertex.getNormalStart();
                Vector4 transformedDirection = normalTransform.transform(direction).normalize();


                // Update the vertex normal with the transformed start and transformed direction
                vertex.setNormal(transformedStart, transformedStart + transformedDirection * SCALE_FACTOR);
            }
        }

        // Apply the transformation to polygon normals
        if (poly.hasPolyNormalDefined()) {
            Vector4 direction = poly.getPolyNormal().end - poly.getPolyNormal().start;
            Vector4 transformedDirection = normalTransform.transform(direction).normalize();
            Vector4 transformedStart = transform.transform(poly.getPolyNormal().start);

            poly.setPolyNormal(PolyNormal(transformedStart, transformedStart + transformedDirection* SCALE_FACTOR));
        }
    }

    // Apply the transformation to the bounding box
    applyTransformToBoundingBox(transform); // Use the full transformation for the bounding box

}



void Scene::applyTransformToBoundingBox(const Matrix4& transform) {
    Vector4 corners[8] = {
        boundingBox.min,
        Vector4(boundingBox.min.x, boundingBox.min.y, boundingBox.max.z, 1),
        Vector4(boundingBox.min.x, boundingBox.max.y, boundingBox.min.z, 1),
        Vector4(boundingBox.min.x, boundingBox.max.y, boundingBox.max.z, 1),
        Vector4(boundingBox.max.x, boundingBox.min.y, boundingBox.min.z, 1),
        Vector4(boundingBox.max.x, boundingBox.min.y, boundingBox.max.z, 1),
        Vector4(boundingBox.max.x, boundingBox.max.y, boundingBox.min.z, 1),
        boundingBox.max
    };

    Vector4 newMin(FLT_MAX, FLT_MAX, FLT_MAX, 1);
    Vector4 newMax(FLT_MIN, FLT_MIN, FLT_MIN, 1);

    for (const Vector4& corner : corners) {
        Vector4 transformedCorner = transform.transform(corner);
        newMin.updateMin(transformedCorner);
        newMax.updateMax(transformedCorner);
    }

    boundingBox.min = newMin;
    boundingBox.max = newMax;
}

// Calculate the bounding box of the scene
void Scene::calculateBoundingBox() {
    calculateBoundingBoxFromVertices();
    hasBoundingBox = true;
}

// Calculate the bounding box from all vertices
void Scene::calculateBoundingBoxFromVertices() {
    if (polygons.empty()) {
        boundingBox.min = Vector4(0, 0, 0, 1);
        boundingBox.max = Vector4(0, 0, 0, 1);
        return;
    }
    boundingBox.min = Vector4(FLT_MAX, FLT_MAX, FLT_MAX, 1.0);
    boundingBox.max = Vector4(FLT_MIN, FLT_MIN, FLT_MIN, 1.0);

    for (const Poly& poly : polygons) {
        for (const Vertex& vertex : poly.getVertices()) {
            boundingBox.min.updateMin(vertex);
            boundingBox.max.updateMax(vertex);
        }
    }
}

// Get the bounding box of the scene
const BoundingBox& Scene::getBoundingBox() const {
    return boundingBox;
}

// Set custom colors for wireframe, normals, and background
void Scene::setColors(COLORREF wireframe, COLORREF normal, COLORREF background) {
    wireframeColor = wireframe;
    normalColor = normal;
    backgroundColor = background;
}

// Get custom colors
COLORREF Scene::getWireframeColor() const {
    return wireframeColor;
}

COLORREF Scene::getNormalColor() const {
    return normalColor;
}

COLORREF Scene::getBackgroundColor() const {
    return backgroundColor;
}

// Set sensitivity for transformations
void Scene::setSensitivity(double newSensitivity) {
    sensitivity = newSensitivity;
}

// Get sensitivity value
double Scene::getSensitivity() const {
    return sensitivity;
}

// Flags for rendering options
void Scene::setShowNormals(bool show) {
    showNormals = show;
}

bool Scene::isShowNormals() const {
    return showNormals;
}

void Scene::setShowBoundingBox(bool show) {
    showBoundingBox = show;
}

bool Scene::isShowBoundingBox() const {
    return showBoundingBox;
}

// Update `isFirstDraw` flag
void Scene::updateIsFirstDraw(bool status) {
    isFirstDraw = status;
}

// Getter for `isFirstDraw`
bool Scene::getIsFirstDraw() const {
    return isFirstDraw;
}

// Update if the scene has vertex normals
void Scene::updateHasVertexNormals(bool status) {
    hasVertexNormals = status;
}

// Check if the scene has vertex normals
bool Scene::hasVertexNormalsAttribute() const {
    return hasVertexNormals;
}

// Clear the scene
void Scene::clear() {
    polygons.clear();
    sceneTransform = Matrix4(); // Reset to identity matrix
    boundingBox = { Vector4(FLT_MAX, FLT_MAX, FLT_MAX, 1.0), Vector4(FLT_MIN, FLT_MIN, FLT_MIN, 1.0) };
    hasVertexNormals = false;
}
