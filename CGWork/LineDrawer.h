#pragma once
#ifndef LINEDRAWER_H
#define LINEDRAWER_H

#include <windows.h> // For drawing to the screen
#include "Vector4.h"

class LineDrawer {
public:
    // Bresenham's line-drawing algorithm
    static void DrawLine(HDC hdc, const Vector4& start, const Vector4& end, COLORREF color);
};

#endif // LINEDRAWER_H
