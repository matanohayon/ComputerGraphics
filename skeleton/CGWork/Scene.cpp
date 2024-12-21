#include "Scene.h"

#include <algorithm>
#include <limits>

// Constructor
Scene::Scene()
    : sceneTransform(Matrix4()),
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
// Applying a transformation to the whole scene
void Scene::applyTransform(const Matrix4& transform) {

    Matrix4 normalTransform = transform.inverse().transpose(); // Inverse-transpose for transforming normals

    // Apply transformation to all polygons in the scene
    for (Poly* poly : *polygons) {
        // Apply the full transformation to all vertices
        for (Vertex& vertex : poly->getVertices()) {
            // Transform vertex position
            vertex = transform.transform(vertex);

            // Transform normals if the vertex has a normal
            if (vertex.getHasNormal()) {
                const Vector4 direction = vertex.getNormalEnd() - vertex.getNormalStart();
                const Vector4 transformedDirection = normalTransform.transform(direction).normalize() * 16.0;
                const Vector4 transformedStart = transform.transform(vertex.getNormalStart());
                vertex.setNormal(transformedStart, transformedStart + transformedDirection, vertex.isNormalProvidedFromFile());                // Keep the flag indicating whether the normal came from the file
            }
        }

        // Apply the transformation to polygon normals if they exist
        if (poly->hasPolyNormalDefined()) {
            Vector4 direction = poly->getPolyNormal().end - poly->getPolyNormal().start;
            Vector4 transformedDirection = normalTransform.transform(direction).normalize() * 16.0;

            Vector4 transformedStart = transform.transform(poly->getPolyNormal().start);
            const bool originalFlag = poly->getPolyNormal().wasProvidedFromFile;

            // Correctly instantiate the PolyNormal object
            PolyNormal transformedPolyNormal(transformedStart, transformedStart + transformedDirection, originalFlag);
            poly->setPolyNormal(transformedPolyNormal);
        }
    }

    // Apply the transformation to the bounding box
    applyTransformToBoundingBox(transform);
}


void Scene::applyTransformToBoundingBox(const Matrix4& transform) {
    boundingBox.min = transform.transform(boundingBox.min);
    boundingBox.max = transform.transform(boundingBox.max);
    }

// Calculate the bounding box of the scene
void Scene::calculateBoundingBox() {
    calculateBoundingBoxFromVertices();
    hasBoundingBox = true;
}

// Calculate the bounding box from all vertices
void Scene::calculateBoundingBoxFromVertices() {
    if (polygons->empty()) {
        boundingBox.min = Vector4(0, 0, 0, 1);
        boundingBox.max = Vector4(0, 0, 0, 1);
        return;
    }
    Vector4 newMin(DBL_MAX, DBL_MAX, DBL_MAX, 1);
    Vector4 newMax(-DBL_MAX, -DBL_MAX, -DBL_MAX, 1);

    for (Poly* poly : *polygons) {
        for (const Vertex& vertex : poly->getVertices()) {
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

// for vertices with no normal in the data file, lets compute those
void Scene::calculateVertexNormals() {
    for (auto it = vertexConnectivity.begin(); it != vertexConnectivity.end(); ++it) {
        std::size_t vertexHash = it->first;
        const std::vector<Poly*>& polygons = it->second;

        Vector4 averageNormal(0.0, 0.0, 0.0, 0.0);

        // Compute average normal
        for (Poly* polygon : polygons) {
            if (polygon->hasPolyNormalDefined()) {
                Vector4 normal = polygon->getPolyNormal().end - polygon->getPolyNormal().start;
                averageNormal = averageNormal + normal.normalize();
            }
        }

        averageNormal = averageNormal.normalize()*16.0;

        // Assign the computed normal
        for (Poly* polygon : polygons) {
            for (Vertex& vertex : polygon->getVertices()) {
                if (hashVertex(vertex) == vertexHash && !vertex.isNormalProvidedFromFile()) {
                    const Vector4 ns(vertex.x, vertex.y, vertex.z, 1.0); // Create ns as a Vector4
                    vertex.setNormal(ns, ns + averageNormal,false);
                }
            }
        }
    }
}
