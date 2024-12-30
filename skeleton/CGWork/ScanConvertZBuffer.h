#ifndef SCAN_CONVERT_ZBUFFER_H
#define SCAN_CONVERT_ZBUFFER_H

#include "Point.h"
#include <vector> // For std::vector
#include <cstddef> // For size_t

// Function to initialize a Z-buffer as a 1D dynamically allocated array
Point* initZBuffer(size_t width, size_t height);

// Free the Z-buffer
void freeZBuffer(Point* zBuffer);

// Render a polygon using scan conversion and Z-buffering
void renderPolygon(Point* zBuffer, size_t width, size_t height, const std::vector<Point>& polygon);

#endif // SCAN_CONVERT_ZBUFFER_H