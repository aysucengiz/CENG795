//
// Created by vicy on 10/21/25.
//

#include "Mesh.h"

#include <complex.h>

#include "../../functions/helpers.h"
#include "../../functions/overloads.h"

////////////////////////////////////////////////
//////////////////// MESH //////////////////////
////////////////////////////////////////////////

Mesh::Mesh(uint32_t id, std::string st, Material &m, std::string s, bool read_from_file, std::deque<CVertex> &vertices, bool v, uint32_t start_index, bool computeVNormals)
    : Object(m,id,Vertex(-INFINITY,-INFINITY,-INFINITY),Vertex(INFINITY,INFINITY,INFINITY),v ){
        if (st == "smooth")    shadingtype = ShadingType::SMOOTH;
        else if (st == "flat") shadingtype = ShadingType::FLAT;
        else                   shadingtype = ShadingType::NONE;
    // std::cout << st << std::endl;

    globalBbox.vMax = Vertex(-INFINITY,-INFINITY,-INFINITY);
    globalBbox.vMin = Vertex(INFINITY,INFINITY,INFINITY);
        if (read_from_file)
        {
            happly::PLYData plyIn(s);
            std::vector<std::vector<int>> f = plyIn.getElement("face").getListProperty<int>("vertex_indices");
            //std::cout << "First face indices: " << f[0][0] << " " << f[0][1] << " " << f[0][2] << std::endl;
            start_index -= f[0][0];
            for (int i = 0; i < f.size(); i++)
            {
                if (vertices.size() <= start_index+f[i][0])std::cout << vertices.size() <<" : " << start_index-1+f[i][0]<< std::endl;
                if (vertices.size() <= start_index+f[i][1])std::cout << vertices.size() <<" : " << start_index-1+f[i][1]<< std::endl;
                if (vertices.size() <= start_index+f[i][2])std::cout << vertices.size() <<" : " << start_index-1+f[i][2]<< std::endl;

                if (f[i].size() == 3 && f[i][0] != f[i][1] && f[i][0] != f[i][2] && f[i][1] != f[i][2] )
                {
                    Vertex minv = minVert3(vertices[start_index+f[i][0]].v,vertices[start_index+f[i][1]].v,vertices[start_index+f[i][2]].v);
                    Vertex maxv = maxVert3(vertices[start_index+f[i][0]].v,vertices[start_index+f[i][1]].v,vertices[start_index+f[i][2]].v);
                    globalBbox.vMax = maxVert2(maxv,globalBbox.vMax);
                    globalBbox.vMin = minVert2(minv,globalBbox.vMin);
                    Faces.push_back(new Triangle(Faces.size(),
                                                vertices[start_index+f[i][0]], vertices[start_index+f[i][1]], vertices[start_index+f[i][2]],
                                                m,
                                                shadingtype,
                                                v,
                                                computeVNormals));
                }
            }

        }
        else
        {
            std::istringstream verticesStream(s);
            uint32_t vert[3];
            while (verticesStream >>vert[0]>>vert[1]>> vert[2]) {
                vert[0]--; vert[1]--; vert[2]--;
                if (vert[0] != vert[1] && vert[0] != vert[2] && vert[1] !=vert[2])
                {
                    Vertex minv = minVert3(vertices[vert[0]].v,vertices[vert[1]].v,vertices[vert[2]].v);
                    Vertex maxv = maxVert3(vertices[vert[0]].v,vertices[vert[1]].v,vertices[vert[2]].v);
                    globalBbox.vMax = maxVert2(maxv,globalBbox.vMax);
                    globalBbox.vMin = minVert2(minv,globalBbox.vMin);

                    Faces.push_back(new Triangle(Faces.size(),
                        vertices[vert[0]], vertices[vert[1]], vertices[vert[2]],
                        m, shadingtype,
                        v,
                        computeVNormals));
                }

            }
        }

    main_center.x = (globalBbox.vMax.x + globalBbox.vMin.x) / 2.0;
    main_center.y = (globalBbox.vMax.y + globalBbox.vMin.y) / 2.0;
    main_center.z = (globalBbox.vMax.z + globalBbox.vMin.z) / 2.0;
    if (ACCELERATE) bvh.getScene(Faces);

}

ObjectType Mesh::getObjectType() const{ return ObjectType::MESH; }


Vec3r Mesh::getNormal(const Vertex &v, uint32_t triID) const
{
    return Faces[triID]->getNormal(v,triID);
}

Object::intersectResult Mesh::checkIntersection(const Ray& ray, const real& t_min, bool shadow_test, bool back_cull) const
{
    intersectResult result;
    result.currTri = 0;
    result.obj = nullptr;
    result.t_min = t_min;
    if (!visible) return result;
    if (globalBbox.intersects(ray))
    {
        if (ACCELERATE)
        {
            return bvh.traverse(ray,t_min,Faces,shadow_test,back_cull);
        }
        else
        {
            int numFaces = Faces.size();
            intersectResult temp;
            temp.obj = result.obj;
            temp.t_min = result.t_min;
            for (int i=0; i< numFaces; i++)
            {

                temp = Faces[i]->checkIntersection(ray, temp.t_min, shadow_test,back_cull);
                result.t_min = temp.t_min;
                if (temp.obj != nullptr)
                {
                    result.obj = temp.obj;
                    if (shadow_test) return result;
                    result.currTri = i;
                }
            }
            return result;
        }
    }
    return result;

}
