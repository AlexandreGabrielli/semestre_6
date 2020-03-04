#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

Model *model = NULL;
const int width = 500;
const int height = 500;


std::array<Vec2i, 2> boite_englobante(const TGAImage &image, Vec3f *t, int n = 3) {
    std::array<Vec2i, 2> box{Vec2i{image.get_width() - 1, image.get_height() - 1}, Vec2i{0, 0}};
    for (int i = 0; i < n; ++i) {
        if (t[i].x < box[0].x) box[0].x = t[i].x;
        if (t[i].y < box[0].y) box[0].y = t[i].y;
        if (t[i].x > box[1].x) box[1].x = t[i].x;
        if (t[i].y > box[1].y) box[1].y = t[i].y;
    }
    if(box[1].y >= image.get_height()) box[1].y = image.get_height()-1;

    return box;
}

Vec3f barycentriques(Vec3f *t, Vec2i p) {
    Vec3f x{static_cast<float>(t[1].x - t[0].x),
            static_cast<float>(t[2].x - t[0].x),
            static_cast<float>(t[0].x - p.x)};
    Vec3f y{static_cast<float>(t[1].y - t[0].y),
            static_cast<float>(t[2].y - t[0].y),
            static_cast<float>(t[0].y - p.y)};
    Vec3f u = x ^y;
    if (abs(u.z) < 1)
        return Vec3f(-1, 1, 1);
    else
        return Vec3f{1.f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z};
}


void triangle(Vec3f *t, TGAImage &image, TGAColor color,float buffer [width*height]) {
    auto bbox = boite_englobante(image, t);
    for (Vec2i p{0, bbox[0].y}; p.y <= bbox[1].y; ++p.y) {
        for (p.x = bbox[0].x; p.x <= bbox[1].x; ++p.x) {
            Vec3f b = barycentriques(t, p);
            if (b.x >= 0 and b.y >= 0 and b.z >= 0 ) {
                float z = t[0].z * b.x + t[1].z * b.y +t[2].z * b.z;
                if ( z >  buffer[p.y*width +p.x] ){
                    buffer[p.y*width +p.x] = z;
                    image.set(p.x, p.y, color);}
            }}
    }
}

int main(int argc, char **argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../../obj/african_head.obj");
    }
    float pixels[width * height] = {0};
    std::fill_n(pixels,width*height,std::numeric_limits<float>::lowest());

    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);

        Vec3f t[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++) {
            world_coords[j] = model->vert(face[j]);
            t[j] = Vec3f((world_coords[j].x + 1.) * width / 2., (world_coords[j].y + 1.) * height / 2.,world_coords[j].z);
        }

        Vec3f n = (world_coords[2]-world_coords[0]) ^(world_coords[1]-world_coords[0]);

        n.normalize();
       Vec3f camera = {0, 0, -1};

        float I = (n * camera);
        if (I > 0) {
            I*=256;
            triangle(t, image, TGAColor(I, I, I, 255),pixels);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("random.tga");
    delete model;
    return 0;
}

