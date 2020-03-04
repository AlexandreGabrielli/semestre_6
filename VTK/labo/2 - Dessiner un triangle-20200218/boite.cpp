//
// Created by Alexandre on 25.02.2020.
//
#include <vector>
#include <cmath>
#include <iostream>
#include <array>
#include "tgaimage.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const int width = 200;
const int height = 200;

std::array<Vec2i, 2> boite_englobante(Vec2i *t, int n = 3) {
    int miny = std::min(std::min(t[0].y, t[1].y), t[2].y);
    int minx = std::min(std::min(t[0].x, t[1].x), t[2].x);
    int maxy = std::max(std::max(t[0].y, t[1].y), t[2].y);
    int maxx = std::max(std::max(t[0].x, t[1].x), t[2].x);

    return std::array<Vec2i, 2>{Vec2i{minx, miny}, Vec2i{maxx, maxy}};
}

bool est_dans_le_triangle(Vec2i pt, Vec2i *t) {
    Vec2i A = t[0];
    Vec2i B = t[1];
    Vec2i C = t[2];
    Vec3f a = {static_cast<float>(B.x - A.x), static_cast<float>(C.x - A.x), static_cast<float>(A.x - pt.x)};
    Vec3f b = {static_cast<float>(B.y - A.y), static_cast<float>(C.y - A.y), static_cast<float>(A.y - pt.y)};

    Vec3f c = a ^b;

    return !(c.x >= 0 or c.y >= 0)  && ( abs(c.z) > 1);
}


void triangle(Vec2i *t, TGAImage &image, TGAColor color) {
    auto bbox = boite_englobante(t);
    for (Vec2i p{0, bbox[0].y}; p.y <= bbox[1].y; ++p.y) {
        for (p.x = bbox[0].x; p.x <= bbox[1].x; ++p.x) {
            if (est_dans_le_triangle(p, t)) {
                image.set(p.x, p.y, color);
            }
        }
    }


}


int main(int argc, char **argv) {
    TGAImage image(width, height, TGAImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0, image, red);
    triangle(t1, image, white);
    triangle(t2, image, green);


    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("triangle.tga");
    return 0;
}

