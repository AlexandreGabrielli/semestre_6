#include <vector>
#include <string>
#include <array>
#include <cmath>
#include <limits>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

using namespace std;

const int width = 600;
const int height = 600;

std::array<Vec2i, 2> boite_englobante(const TGAImage &image, Vec3f *t, int n = 3) {
    std::array<Vec2i, 2> box{Vec2i{image.get_width() - 1, image.get_height() - 1}, Vec2i{0, 0}};
    for (int i = 0; i < n; ++i) {
        if (t[i].x < box[0].x) box[0].x = t[i].x;
        if (t[i].y < box[0].y) box[0].y = t[i].y;
        if (t[i].x > box[1].x) box[1].x = t[i].x;
        if (t[i].y > box[1].y) box[1].y = t[i].y;
    }
    if (box[0].x < 0) box[0].x = 0;
    if (box[0].y < 0) box[0].y = 0;
    if (box[1].x > image.get_width()) box[1].x = image.get_width();
    if (box[1].y > image.get_height()) box[1].y = image.get_height();

    return box;
}

Vec3f barycentriques(Vec3f *t, Vec3f p) {
    Vec3f x{(t[1].x - t[0].x),
            (t[2].x - t[0].x),
            (t[0].x - p.x)};
    Vec3f y{(t[1].y - t[0].y),
            (t[2].y - t[0].y),
            (t[0].y - p.y)};
    Vec3f u = x ^y;
    if (abs(u.z) < 0.01)
        return Vec3f(-1, 1, 1);
    else
        return Vec3f{1.f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z};
}

bool est_dans_le_triangle(Vec3f *t, Vec3f pt) {
    Vec3f b = barycentriques(t, pt);
    return (b.x >= 0 and b.y >= 0 and b.z >= 0);
}

void triangle(Vec3f *t, Vec3f *tc, float *zbuffer, TGAImage &image, TGAImage &texture, Vec3f intensity) {
    auto bbox = boite_englobante(image, t);
    for (int y = bbox[0].y; y <= bbox[1].y; ++y) {
        for (int x = bbox[0].x; x <= bbox[1].x; ++x) {
            Vec3f p{float(x), float(y), 0};
            Vec3f b = barycentriques(t, p);
            if (b.x >= 0 and b.y >= 0 and b.z >= 0) {
                p.z = b.x * t[0].z + b.y * t[1].z + b.z * t[2].z;
                if (zbuffer[x + image.get_width() * y] < p.z) {
                    zbuffer[x + image.get_width() * y] = p.z;

                    Vec3f ptc = tc[0] * b.x + tc[1] * b.y + tc[2] * b.z;
                    Vec2i pc(ptc.x * texture.get_width(), ptc.y * texture.get_height());
                    TGAColor color = texture.get(pc.x, pc.y);
                    float gouraud = intensity * b;
                    if (gouraud < 0) gouraud = 0;
                    for (int i = 0; i < 3; ++i) color.raw[i] *= gouraud;

                    image.set(x, y, color);
                }
            }
        }
    }
}

Matrix4x4 make_translate(Vec3f matrix) {
    Matrix4x4 m = Matrix4x4::identity();
    for (int i = 0; i < 3; ++i) {
        m[i][3] = matrix.raw[i];
    }
    return m;
}

Matrix4x4 make_scale(Vec3f matrix) {
    Matrix4x4 m = Matrix4x4::identity();
    for (int i = 0; i < 3; ++i) {
        m[i][i] = matrix.raw[i];
    }
    return m;
}

Matrix4x4 make_viewport(int x, int y, size_t w, size_t h) {
    Matrix4x4 t = make_translate(Vec3f{x + w / 2.f, y + h / 2.f, 100});
    Matrix4x4 s = make_scale(Vec3f{w / 2.f, h / 2.f, 100});
    return t * s;
}

VecH xyzw(Vec3f coordonner) {
    VecH m(1.f);
    for (size_t i = 0; i < 3; ++i) {
        m[i][0] = coordonner.raw[i];
    }
    return m;
}

Vec3f xyz(VecH m) {
    Vec3f v;
    for (size_t i = 0; i < 3; ++i)
        v.raw[i] = m[i][0] / m[3][0];
    return v;
}

Matrix4x4 make_cam(int d) {
    Matrix4x4 m = Matrix4x4::identity();
    m[3][2] = -1.f / d;
    return m;
}

int main(int argc, char **argv) {

    string modelFileName = "../african_head.obj";
    Model model(modelFileName.c_str());
    string textureFileName = "../african_head_diffuse.tga";
    TGAImage texture;
    Matrix4x4 viewport = make_viewport(0, 0, 500, 500);
    Matrix4x4 cam = make_cam(2);
    texture.read_tga_file(textureFileName.c_str());
    texture.flip_vertically();

    const Vec3f light(0, 0, -1);

    TGAImage image(width, height, TGAImage::RGB);

    float zbuffer[width * height];
    for (unsigned i = 0; i < width * height; ++i)
        zbuffer[i] = std::numeric_limits<float>::lowest();

    for (int i = 0; i < model.nfaces(); i++) {
        Vec3f screen[3];
        Vec3f world[3];
        Vec3f text[3];
        Vec3f intensity;

        for (int j = 0; j < 3; j++) {
            world[j] = model.vert(model.face(i)[j]);
            screen[j] = xyz(viewport * cam * xyzw(world[j]));
            text[j] = model.texture(model.face_texts(i)[j]);
            Vec3f n = model.normal(model.face_normals(i)[j]) * -1;
            intensity.raw[j] = n * light;
        }

        triangle(screen, text, zbuffer, image, texture, intensity);

    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("texture.tga");
    return 0;
}

