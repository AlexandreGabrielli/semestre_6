#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<Vec3f> texts_;
    std::vector<Vec3f> norms_;
    std::vector<std::vector<int> > faces_;
    std::vector<std::vector<int> > face_texts_;
    std::vector<std::vector<int> > face_norms_;
public:
	Model(const char *filename);
	~Model();
	int nverts() const;
	int nfaces() const;
    const Vec3f& vert(int i) const;
    const Vec3f& texture(int i) const;
    const Vec3f& normal(int i) const;
    const std::vector<int>& face(int idx) const;
    const std::vector<int>& face_texts(int idx) const;
    const std::vector<int>& face_normals(int idx) const;
};

#endif //__MODEL_H__
