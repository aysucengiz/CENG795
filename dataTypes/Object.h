//
// Created by Aysu on 04-Oct-25.
//

#ifndef OBJECT_H
#define OBJECT_H

typedef enum ObjectType
{
    NO_OBJ_TYPE,
    TRIANGLE,
    SPHERE
}

class Object
{
private:
    uint8_t _id;
    uint8_t material;

public:
    Object(uint8_t material,uint8_t _id) : material(material), _id(_id) {};
    virtual ObjectType getObjectType() = 0;
    unsigned int getId(){return _id;}
    unsigned int getMaterial(){return material;}

}

class Triangle : public Object {
public:
    ObjectType getObjectType() {return TRIANGLE;}
    uint8_t indices[3];
}

class Sphere : public Object {
public:
    ObjectType getObjectType() {return SPHERE;}
    uint8_t center;
    double radius;
}


#endif //OBJECT_H
