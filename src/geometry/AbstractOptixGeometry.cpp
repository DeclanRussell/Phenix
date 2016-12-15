#include "geometry/AbstractOptixGeometry.h"
#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
//----------------------------------------------------------------------------------------------------------------------
AbstractOptixGeometry::AbstractOptixGeometry() : AbstractOptixObject() {}
//----------------------------------------------------------------------------------------------------------------------
AbstractOptixGeometry::AbstractOptixGeometry(optix::Context &_context) : AbstractOptixObject(_context)
{
    // Create our optix objects
    m_geometry = _context->createGeometry();
    // Add it as an instance
    m_geometryInstance = _context->createGeometryInstance();
    m_geometryInstance->setGeometry(m_geometry);
    // We need a geometry group such that we can pair our geometry with a transform
    m_geometryGroup = _context->createGeometryGroup();
    m_geometryGroup->setChildCount(1);
    m_geometryGroup->setChild(0,m_geometryInstance);
    //create our acceleration method, in this case none becuase we only have one peice of geomtry
    //set this acceleration in our geometry group
    m_geometryGroup->setAcceleration(_context->createAcceleration("Sbvh","Bvh"));
    m_geometryGroup->getAcceleration()->markDirty();
    //Set the acceleration structure as dirty
    m_transform = _context->createTransform();
    // identity matrix to init our transformation
    float m[16];
    m[ 0] = 1.0f;  m[ 1] = 0.0f;  m[ 2] = 0.0f;  m[ 3] = 0.0f;
    m[ 4] = 0.0f;  m[ 5] = 1.0f;  m[ 6] = 0.0f;  m[ 7] = 0.0f;
    m[ 8] = 0.0f;  m[ 9] = 0.0f;  m[10] = 1.0f;  m[11] = 0.0f;
    m[12] = 0.0f;  m[13] = 0.0f;  m[14] = 0.0f;  m[15] = 1.0f;
    m_transform->setMatrix(false,m,m);
    // Add our geometry group to our transform
    m_transform->setChild(m_geometryGroup);

    m_pos = optix::make_float3(0.f);
    m_scale = optix::make_float3(1.f);
    m_rot = optix::make_float3(0.f);
}
//----------------------------------------------------------------------------------------------------------------------
AbstractOptixGeometry::AbstractOptixGeometry(optix::Context &_context, AbstractOptixGeometry &_instance) : AbstractOptixObject(_context)
{
    // Create our optix objects
    m_geometry = _instance.m_geometry;
    // Add it as an instance
    m_geometryInstance = _context->createGeometryInstance();
    m_geometryInstance->setGeometry(m_geometry);
    // We need a geometry group such that we can pair our geometry with a transform
    m_geometryGroup = _context->createGeometryGroup();
    m_geometryGroup->setChildCount(1);
    m_geometryGroup->setChild(0,m_geometryInstance);
    //create our acceleration method, in this case none becuase we only have one peice of geomtry
    //set this acceleration in our geometry group
    m_geometryGroup->setAcceleration(_context->createAcceleration("Sbvh","Bvh"));
    m_geometryGroup->getAcceleration()->markDirty();
    //Set the acceleration structure as dirty
    m_transform = _context->createTransform();
    // identity matrix to init our transformation
    float m[16];
    m[ 0] = 1.0f;  m[ 1] = 0.0f;  m[ 2] = 0.0f;  m[ 3] = 0.0f;
    m[ 4] = 0.0f;  m[ 5] = 1.0f;  m[ 6] = 0.0f;  m[ 7] = 0.0f;
    m[ 8] = 0.0f;  m[ 9] = 0.0f;  m[10] = 1.0f;  m[11] = 0.0f;
    m[12] = 0.0f;  m[13] = 0.0f;  m[14] = 0.0f;  m[15] = 1.0f;
    m_transform->setMatrix(false,m,m);
    // Add our geometry group to our transform
    m_transform->setChild(m_geometryGroup);
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixGeometry::createIntersectionProgram(std::string &_name)
{
    // some error handling
    if(!contextSet())
    {
        std::cerr<<"No context set. Cannot create intersection program!"<<std::endl;
        return;
    }
    if(!ptxPathSet())
    {
        std::cerr<<"No ptx path set. Cannot create intersection program!"<<std::endl;
        return;
    }
    if(_name.empty())
    {
        std::cerr<<"No Intersection program name set. Cannot create intersection program!"<<std::endl;
        return;
    }

    // If we have made it this far we're in good form. Lets hope it doesn't go wrong here!
    m_intersectionProgram= getContext()->createProgramFromPTXFile(getPtxPath(),_name);
    m_geometry->setIntersectionProgram(m_intersectionProgram);

    std::cerr<<"Intersection program "<<_name<<" created"<<std::endl;

}

void AbstractOptixGeometry::createIntersectionProgram(const char *_name)
{
    // some error handling
    if(!contextSet())
    {
        std::cerr<<"No context set. Cannot create intersection program!"<<std::endl;
        return;
    }
    if(!ptxPathSet())
    {
        std::cerr<<"No ptx path set. Cannot create intersection program!"<<std::endl;
        return;
    }
    if(!_name)
    {
        std::cerr<<"No Intersection program name set. Cannot create intersection program!"<<std::endl;
        return;
    }

    // If we have made it this far we're in good form. Lets hope it doesn't go wrong here!
    m_intersectionProgram= getContext()->createProgramFromPTXFile(getPtxPath(),_name);
    m_geometry->setIntersectionProgram(m_intersectionProgram);

    std::cerr<<"Intersection program "<<_name<<" created"<<std::endl;

}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixGeometry::setIntersectionProgram(optix::Program &_p)
{
    m_intersectionProgram = _p;
    m_geometry->setIntersectionProgram(m_intersectionProgram);
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixGeometry::createBBProgram(std::string &_name)
{
    // some error handling
    if(!contextSet())
    {
        std::cerr<<"No context set. Cannot create bounding box program!"<<std::endl;
        return;
    }
    if(!ptxPathSet())
    {
        std::cerr<<"No ptx path set. Cannot create bounding box program!"<<std::endl;
        return;
    }
    if(_name.empty())
    {
        std::cerr<<"No Bounding box name set. Cannot create bounding box program!"<<std::endl;
        return;
    }

    // If we have made it this far we're in good form. Lets hope it doesn't go wrong here!
    m_BBProgram = getContext()->createProgramFromPTXFile(getPtxPath(),_name);
    m_geometry->setBoundingBoxProgram(m_BBProgram);

    std::cerr<<"Bounding box program "<<_name<<" created"<<std::endl;

}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixGeometry::createBBProgram(const char *_name)
{
    // some error handling
    if(!contextSet())
    {
        std::cerr<<"No context set. Cannot create bounding box program!"<<std::endl;
        return;
    }
    if(!ptxPathSet())
    {
        std::cerr<<"No ptx path set. Cannot create bounding box program!"<<std::endl;
        return;
    }
    if(!_name)
    {
        std::cerr<<"No Bounding box name set. Cannot create bounding box program!"<<std::endl;
        return;
    }

    // If we have made it this far we're in good form. Lets hope it doesn't go wrong here!
    m_BBProgram = getContext()->createProgramFromPTXFile(getPtxPath(),_name);
    m_geometry->setBoundingBoxProgram(m_BBProgram);

    std::cerr<<"Bounding box program "<<_name<<" created"<<std::endl;

}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixGeometry::setBBProgram(optix::Program &_p)
{
    m_BBProgram = _p;
    m_geometry->setBoundingBoxProgram(m_BBProgram);
}
//----------------------------------------------------------------------------------------------------------------------
AbstractOptixGeometry AbstractOptixGeometry::instance()
{
    // some error handling
    if(!contextSet())
    {
        std::cerr<<"Context not set. Cannot instance so returning blank geometry. This is not good!"<<std::endl;
        return AbstractOptixGeometry();
    }
    else
    {
        return AbstractOptixGeometry(getContext(),*this);
    }
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixGeometry::setMaterial(optix::Material &_mat)
{
    if(m_geometryInstance->getMaterialCount()==0){
        m_geometryInstance->addMaterial(_mat);
    }
    else{
        m_geometryInstance->setMaterial(0,_mat);
    }
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixGeometry::setTransform(float *_m, float *_invM, bool _transpose)
{
    m_transform->setMatrix(_transpose,_m,_invM);
    m_geometryGroup->getAcceleration()->markDirty();
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixGeometry::applyTransforms(bool _transpose)
{
    // Create our transform matrix
    glm::mat4 trans;
    glm::mat4 t,rx,ry,rz,s;
    s = glm::scale(s,glm::vec3(m_scale.x,m_scale.y,m_scale.z));
    rx = glm::rotate(rx, m_rot.x,glm::vec3(1.f,0.f,0.f));
    ry = glm::rotate(ry, m_rot.y,glm::vec3(0.f,1.f,0.f));
    rz = glm::rotate(rz, m_rot.z,glm::vec3(0.f,0.f,1.f));
    t = glm::translate(t,glm::vec3(m_pos.x,m_pos.y,m_pos.z));
    trans = t*rx*ry*rz*s;

    // identity matrix to init our transformation
    float m[16];
    m[ 0] = trans[0][0];  m[ 1] = trans[1][0];  m[ 2] = trans[2][0];  m[ 3] = trans[3][0];
    m[ 4] = trans[0][1];  m[ 5] = trans[1][1];  m[ 6] = trans[2][1];  m[ 7] = trans[3][1];
    m[ 8] = trans[0][2];  m[ 9] = trans[1][2];  m[ 10] = trans[2][2];  m[ 11] = trans[3][2];
    m[ 12] = trans[0][3];  m[ 13] = trans[1][3];  m[ 14] = trans[2][3];  m[ 15] = trans[3][3];

    // create our inverse transform
    float invM[16];
    glm::mat4 inv = glm::inverse(trans);
    invM[ 0] = inv[0][0];  invM[ 1] = inv[1][0];  invM[ 2] = inv[2][0];  invM[ 3] = inv[3][0];
    invM[ 4] = inv[0][1];  invM[ 5] = inv[1][1];  invM[ 6] = inv[2][1];  invM[ 7] = inv[3][1];
    invM[ 8] = inv[0][2];  invM[ 9] = inv[1][2];  invM[ 10] = inv[2][2];  invM[ 11] = inv[3][2];
    invM[ 12] = inv[0][3];  invM[ 13] = inv[1][3];  invM[ 14] = inv[2][3];  invM[ 15] = inv[3][3];
    // set our transform
    setTransform(m,invM,_transpose);
}
//----------------------------------------------------------------------------------------------------------------------
