#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } else if(!line.compare(0, 3, "vt "))
        {
            iss >> trash >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            texts_.push_back(v);
        }
        else if(!line.compare(0, 3, "vn "))
        {
            iss >> trash >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            norms_.push_back(v);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f, ft, fn;
            int idx, txt_idx, n_idx;
            iss >> trash;
            while (iss >> idx >> trash >> txt_idx >> trash >> n_idx) {
                f.push_back(--idx);       // in wavefront obj all indices start at 1, not zero
                ft.push_back(--txt_idx);
                fn.push_back(--n_idx);
            }
            faces_.push_back(f);
            face_texts_.push_back(ft);
            face_norms_.push_back(fn);
        }
    }
    std::cerr << "# v# " << verts_.size() << " vt# " << texts_.size() << " vn# " << norms_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() const {
    return (int)verts_.size();
}

int Model::nfaces() const {
    return (int)faces_.size();
}

const std::vector<int>& Model::face(int idx) const {
    return faces_[idx];
}

const std::vector<int>& Model::face_texts(int idx) const {
    return face_texts_[idx];
}

const std::vector<int>& Model::face_normals(int idx) const {
    return face_norms_[idx];
}

const Vec3f& Model::vert(int i) const {
    return verts_[i];
}

const Vec3f& Model::texture(int i) const {
    return texts_[i];
}

const Vec3f& Model::normal(int i) const {
    return norms_[i];
}

