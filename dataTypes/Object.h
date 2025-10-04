//
// Created by Aysu on 04-Oct-25.
//

#ifndef OBJECT_H
#define OBJECT_H
/*
typedef enum ObjectType
{
    NO_OBJ_TYPE,
    TRIANGLE,
    SPHERE
} ObjectType;

class Object
{
private:
    uint32_t _id;
    uint32_t material;

public:
    Object(uint32_t material,uint32_t _id) : material(material), _id(_id) {};
    virtual ObjectType getObjectType() = 0;
    unsigned int getId(){return _id;}
    unsigned int getMaterial(){return material;}

};*/

typedef struct Triangle{
    uint32_t _id;
    uint32_t indices[3];
    uint32_t material;

    Triangle(uint32_t id, std::string ind, uint32_t material) :
            _id(id), material(material){
        std::istringstream ss(ind);
        ss >> indices[0] >> indices[1] >> indices[2];
        if (ss.fail()) {
            throw std::invalid_argument("Invalid triangle indices string: " + ind);
        }
    }
}Triangle;

typedef struct  Sphere{
    uint32_t _id;
    uint32_t center;
    double radius;
    uint32_t material;
    Sphere(uint32_t id, uint32_t c, double r, uint32_t m)
    : _id(id), center(c), radius(r), material(m) {}
}Sphere;


#endif //OBJECT_H
