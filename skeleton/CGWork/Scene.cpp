#include "Scene.h"

#include <algorithm>
#include <limits>

// Constructor
Scene::Scene()
    : sceneTransform(Matrix4()), //we are not using
    boundingBox{ Vector4(DBL_MAX, DBL_MAX, DBL_MAX, 1.0), Vector4(DBL_MIN, DBL_MIN, DBL_MIN, 1.0) },

    wireframeColor(RGB(255, 255, 255)),  // Default white
    normalColor(RGB(0, 255, 0)),        // Default green
    backgroundColor(RGB(0, 0, 0)),      // Default black

    sensitivity(1.0),                   // Default sensitivity
    showNormals(false),                 // Default: don't show normals
    hasVertexNormals(false),            // Default: no vertex normals
    isFirstDraw(true),
    hasBoundingBox(false),
    showBoundingBox(false){
    polygons = new std::vector<Poly*>;
    //edges : {{poly1,poly2}}

}           // Default: don't show bounding box

// Add a polygon to the scene
void Scene::addPolygon(Poly* poly) {
    polygons->push_back(poly);
    calculateBoundingBox(); // Recalculate bounding box
}

// Get all polygons in the scene
const std::vector<Poly*>* Scene::getPolygons() const {
    return polygons;
}

// Get the total number of polygons
size_t Scene::getPolygonCount() const {
    return polygons->size();
}

//applying a transformation to the whole scene
void Scene::applyTransform(const Matrix4& transform) {
    Matrix4 normalTransform = transform.inverse().transpose(); // Inverse-transpose for transforming normals

    // Apply transformation to all polygons in the scene
    for (Poly* poly : *polygons) {
        poly->applyTransform(transform, normalTransform); // Delegate transformation to Poly
    }

    // Apply the transformation to the bounding box
    boundingBox.applyTransform(transform);

    updateObjectCenter();
}

// Calculate the bounding box of the scene
void Scene::calculateBoundingBox() {
    calculateBoundingBoxFromVertices();
    hasBoundingBox = true;
}

// Calculate the bounding box from all vertices
void Scene::calculateBoundingBoxFromVertices() {
    if (polygons->empty()) {
        // Set all corners to (0, 0, 0) if there are no polygons
        boundingBox.TLF = Vector4(0, 0, 0, 1);
        boundingBox.TLB = Vector4(0, 0, 0, 1);
        boundingBox.TRF = Vector4(0, 0, 0, 1);
        boundingBox.TRB = Vector4(0, 0, 0, 1);
        boundingBox.BLF = Vector4(0, 0, 0, 1);
        boundingBox.BLB = Vector4(0, 0, 0, 1);
        boundingBox.BRF = Vector4(0, 0, 0, 1);
        boundingBox.BRB = Vector4(0, 0, 0, 1);
        return;
    }

    // Initialize min and max to extreme values
    Vector4 newMin(DBL_MAX, DBL_MAX, DBL_MAX, 1);
    Vector4 newMax(-DBL_MAX, -DBL_MAX, -DBL_MAX, 1);

    // Find the min and max values from all vertices
    for (Poly* poly : *polygons) {
        for (const Vertex& vertex : poly->getVertices()) {
            newMin.updateMin(vertex); // Update the minimum corner
            newMax.updateMax(vertex); // Update the maximum corner
        }
    }

    // Set the bounding box corners based on min and max
    boundingBox.TLF = Vector4(newMin.x, newMax.y, newMax.z, 1.0); // Top-Left-Forward
    boundingBox.TLB = Vector4(newMin.x, newMax.y, newMin.z, 1.0); // Top-Left-Backward
    boundingBox.TRF = Vector4(newMax.x, newMax.y, newMax.z, 1.0); // Top-Right-Forward
    boundingBox.TRB = Vector4(newMax.x, newMax.y, newMin.z, 1.0); // Top-Right-Backward
    boundingBox.BLF = Vector4(newMin.x, newMin.y, newMax.z, 1.0); // Bottom-Left-Forward
    boundingBox.BLB = Vector4(newMin.x, newMin.y, newMin.z, 1.0); // Bottom-Left-Backward
    boundingBox.BRF = Vector4(newMax.x, newMin.y, newMax.z, 1.0); // Bottom-Right-Forward
    boundingBox.BRB = Vector4(newMax.x, newMin.y, newMin.z, 1.0); // Bottom-Right-Backward
}

// Get the bounding box of the scene
const BoundingBox& Scene::getBoundingBox() const {
    return boundingBox;
}

Vector4 Scene::getObjectCenter() {
    return objectCenter;
}

void Scene::updateObjectCenter() {
    const BoundingBox& bbox = getBoundingBox();
    std::vector<Vector4> corners = bbox.getCorners();

    // Calculate the center of the bounding box
    objectCenter = Vector4(
        (corners[0].x + corners[7].x) / 2.0,
        (corners[0].y + corners[7].y) / 2.0,
        (corners[0].z + corners[7].z) / 2.0,
        1.0 // Homogeneous coordinate
    );
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
void Scene::setSensitivity(float newSensitivity) {
    sensitivity = newSensitivity;
}

// Get sensitivity value
float Scene::getSensitivity() const {
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
    polygons->clear();
    sceneTransform = Matrix4(); // Reset to identity matrix
    boundingBox = { Vector4(DBL_MAX, DBL_MAX, DBL_MAX, 1.0), Vector4(DBL_MIN, DBL_MIN, DBL_MIN, 1.0) };
    hasVertexNormals = false;
}


std::size_t Scene::hashVertex(const Vertex& vertex, double accuracy) {
    auto round = [accuracy](double coord) -> int {
        return static_cast<int>(std::round(coord / accuracy));
    };
    std::size_t hx = std::hash<int>()(round(vertex.x));
    std::size_t hy = std::hash<int>()(round(vertex.y));
    std::size_t hz = std::hash<int>()(round(vertex.z));
    return hx ^ (hy << 1) ^ (hz << 2); // Combine hashes
}


void Scene::addPolygonToConnectivity(const Vertex& vertex, Poly* polygon) {
    std::size_t vertexHash = hashVertex(vertex);
    vertexConnectivity[vertexHash].push_back(polygon);
}


const std::vector<Poly*>& Scene::getIncidentPolygons(const Vertex& vertex) const {
    static std::vector<Poly*> empty;
    auto it = vertexConnectivity.find(hashVertex(vertex));
    return it != vertexConnectivity.end() ? it->second : empty;
}


// For vertices with no normal in the data file, calculate them
void Scene::calculateVertexNormals() {
    for (auto it = vertexConnectivity.begin(); it != vertexConnectivity.end(); ++it) {
        std::size_t vertexHash = it->first;
        const std::vector<Poly*>& polygons = it->second;

        Vector4 averageNormal(0.0, 0.0, 0.0, 0.0);

        // Compute the average normal for the vertex
        for (Poly* polygon : polygons) {
                Vector4 normal = polygon->getPolyNormalCalculated().end - polygon->getPolyNormalCalculated().start;
                averageNormal = averageNormal + normal.normalize();
        }

        averageNormal = averageNormal.normalize() * 16.0; // Scale the normal

        // Assign the computed normal to the vertex
        for (Poly* polygon : polygons) {
            for (Vertex& vertex : polygon->getVertices()) {
                if (hashVertex(vertex) == vertexHash && !vertex.isNormalProvidedFromFile()) {
                    const Vector4 normalStart(vertex.x, vertex.y, vertex.z, 1.0); // Normal starts at vertex position
                    vertex.setNormalCalculated(Normal(normalStart, normalStart + averageNormal)); // Set the calculated normal
                }
            }
        }
    }
}
