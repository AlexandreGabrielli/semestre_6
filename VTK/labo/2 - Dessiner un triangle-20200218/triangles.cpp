#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const int width = 200;
const int height = 200;

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x > p1.x) {
        std::swap(p0, p1);
    }

    for (int x = p0.x; x <= p1.x; x++) {
        float t = (x - p0.x) / (float) (p1.x - p0.x);
        int y = p0.y * (1. - t) + p1.y * t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

void triangle2(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {

    int miny = std::min(std::min(t0.y, t1.y), t2.y);
    Vec2i startPoint;
//les deux autre point;
    Vec2i pointgauche;
    Vec2i pointDroite;
    if (t0.y == miny) {
        startPoint = t0;
        pointgauche = t2;
        pointDroite = t1;
    } else if (t1.y == miny) {
        startPoint = t1;
        pointgauche = t0;
        pointDroite = t2;
    } else {
        startPoint = t2;
        pointgauche = t1;
        pointDroite = t0;
    }
    if (pointgauche.x > pointDroite.x) {
        std::swap(pointDroite, pointgauche);
    }
    float mGauche = (float) (pointgauche.y - startPoint.y) / (float) (pointgauche.x - startPoint.x);
    float mDroite = (float) (pointDroite.y - startPoint.y) / (float) (pointDroite.x - startPoint.x);
    float mhaut = (float) (pointgauche.y - pointDroite.y) / (float) (pointgauche.x - pointDroite.x);
    bool specific = mGauche < 0 && mDroite < 0;
    for (int hauteur = startPoint.y; hauteur <= std::max(std::max(t0.y, t1.y), t2.y); hauteur++) {

        float interceptionXDroiteBas = (hauteur - 1 + startPoint.x * mDroite - startPoint.y) / mDroite;
        float interceptionXGaucheBas = (hauteur - 1 + startPoint.x * mGauche - startPoint.y) / mGauche;

        float interceptionXHauteur = (hauteur - 1 + pointDroite.x * mhaut - pointDroite.y) / mhaut;

        for (int i = std::ceil(interceptionXGaucheBas); i <= std::floor(interceptionXDroiteBas); i++) {
            if (((!specific && i <= std::ceil(interceptionXHauteur)) or (specific && i >= std::ceil(interceptionXHauteur)))) {
                image.set(i, hauteur, color);
            }
        }
    }
}
//
//int main(int argc, char **argv) {
//    TGAImage image(width, height, TGAImage::RGB);
//
//    Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
//    Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
//    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
//
//    triangle2(t0[0], t0[1], t0[2], image, red);
//    triangle2(t1[0], t1[1], t1[2], image, white);
//    triangle2(t2[0], t2[1], t2[2], image, green);
//
//
//    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
//    image.write_tga_file("triangle.tga");
//    return 0;
//}
//
