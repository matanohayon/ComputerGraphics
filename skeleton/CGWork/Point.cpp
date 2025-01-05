#include "Point.h"

// Default constructor
Point::Point() : Vector4(), color(RGB(255, 255, 255)) {
    poly = nullptr;
} // Default white color

// Constructor with coordinates and color
Point::Point(float x, float y, float z, float w, COLORREF color)
    : Vector4(x, y, z, w), color(color) {
    poly = nullptr;
}

Point::Point(float x, float y, float z, float w, COLORREF color,const Poly* p)
    : Vector4(x, y, z, w), color(color),poly(p) {}

// Constructor with Vector4 and color
Point::Point(const Vector4& vector, COLORREF color)
    : Vector4(vector), color(color) {
    poly = nullptr;
}

// Set the color
void Point::setColor(COLORREF newColor) {
    color = newColor;
}

// Get the color
COLORREF Point::getColor() const {
    return color;
}

void Point::setPolygon(Poly* p)
{
    poly = p;
}

const Poly* Point::getPolygon()
{
    return poly;
}
