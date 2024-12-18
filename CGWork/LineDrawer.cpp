#include "LineDrawer.h"
//the midpoint algorithm
void LineDrawer::DrawLine(HDC hdc, const Vector4& start, const Vector4& end, COLORREF color) {
    int x1 = static_cast<int>(start.x);
    int y1 = static_cast<int>(start.y);
    int x2 = static_cast<int>(end.x);
    int y2 = static_cast<int>(end.y);

    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        SetPixel(hdc, x1, y1, color); // Plot the pixel
        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}
