#include "line.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    if (x0 > x1) {
        std::swap(x1, x0);
        std::swap(y1, y0);
    }
    int dy = y1 - y0;
    int dx = x1 - x0;
    int y = y0;
    float e = 0.0; //ecart
    float e1 = std::abs((float) dy / (float) dx); //pente
    int dysign = (y1 > y0) ? 1 : -1;
    float e2 = -1.0; // diminution
    for (int x = x0; x < x1; x++) {
        e = e + e1;
        if (e >= 0.5) {
            y += dysign;
            e = e + e2;
        }
        image.set(x, y, color);
    }
}