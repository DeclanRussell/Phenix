#ifndef ABSTRACTOPTIXGEOMETRY_H
#define ABSTRACTOPTIXGEOMETRY_H

#define DtoR (float)M_PI/180.f
#include "common/AbstractOptixObject.h"
#include <optixu/optixu_math_namespace.h>

class AbstractOptixGeometry : public AbstractOptixObject
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Constructor that sets the context
    /// @param _context - optix context (optix::Context)
    //----------------------------------------------------------------------------------------------------------------------
    AbstractOptixGeometry(optix::Context &_context);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our defualt destructor
    //----------------------------------------------------------------------------------------------------------------------
    ~AbstractOptixGeometry(){}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief creates intersection program from set ptx file
    /// @param _name - name of intersection program
    //----------------------------------------------------------------------------------------------------------------------
    void createIntersectionProgram(std::string &_name);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets intersection program
    /// @param _p - interesection program (optix::Program)
    //----------------------------------------------------------------------------------------------------------------------
    void setIntersectionProgram(optix::Program &_p);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief creates bounding box program from set ptx file
    /// @param _name - name of bounding box program
    //----------------------------------------------------------------------------------------------------------------------
    void createBBProgram(std::string &_name);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets bounding box program
    /// @param _p - bounding box program (optix::Program)
    //----------------------------------------------------------------------------------------------------------------------
    void setBBProgram(optix::Program &_p);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns an instance of the geometry
    //----------------------------------------------------------------------------------------------------------------------
    virtual AbstractOptixGeometry instance();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the primative count in our geometry
    /// @param _n - primitive count (uint)
    //----------------------------------------------------------------------------------------------------------------------
    inline void setPrimCount(unsigned int _n){m_geometry->setPrimitiveCount(_n);}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the material applied to our geometry
    /// @param _mat - material to apply (optix::geometry)
    //----------------------------------------------------------------------------------------------------------------------
    void setMaterial(optix::Material &_mat);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the transformation of our geometry
    /// @param _m - transformation matrix (float*)
    /// @param _transpose - if we want to transpose our matrix (bool)
    /// @param _invM - inverse matrix of _m (float*)
    //----------------------------------------------------------------------------------------------------------------------
    void setTransform(float* _m, float* _invM, bool _transpose = false);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief accessor to model with transform applied
    //----------------------------------------------------------------------------------------------------------------------
    inline optix::Transform getGeomAndTrans(){return m_transform;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief accessor to the geometry instance
    //----------------------------------------------------------------------------------------------------------------------
    inline optix::GeometryInstance getGeometryInstance(){return m_geometryInstance;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set the postion of our geometry
    //----------------------------------------------------------------------------------------------------------------------
    inline void setPos(float _x, float _y, float _z){m_pos = optix::make_float3(_x,_y,_z); applyTransforms();}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set the rotation of our geometry
    //----------------------------------------------------------------------------------------------------------------------
    inline void setRot(float _x, float _y, float _z){m_rot = optix::make_float3(_x*DtoR,_y*DtoR,_z*DtoR); applyTransforms();}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set the scale of our geometry
    //----------------------------------------------------------------------------------------------------------------------
    inline void setScale(float _x, float _y, float _z){m_scale = optix::make_float3(_x,_y,_z); applyTransforms();}
    //----------------------------------------------------------------------------------------------------------------------
protected:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Constructor for instancing
    /// @param _context - optix context (optix::Context)
    /// @param _instance - Geometry to instance (AbstractOptixGeometry)
    //----------------------------------------------------------------------------------------------------------------------
    AbstractOptixGeometry(optix::Context &_context, AbstractOptixGeometry &_instance);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our optix geometry
    //----------------------------------------------------------------------------------------------------------------------
    optix::Geometry m_geometry;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our default constructor removed from public use as we need a context to do pretty much anything!
    //----------------------------------------------------------------------------------------------------------------------
    AbstractOptixGeometry();
    //----------------------------------------------------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our geometry intersection program
    //----------------------------------------------------------------------------------------------------------------------
    optix::Program m_intersectionProgram;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our bounding box program
    //----------------------------------------------------------------------------------------------------------------------
    optix::Program m_BBProgram;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our optix geometry group
    //----------------------------------------------------------------------------------------------------------------------
    optix::GeometryGroup m_geometryGroup;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our optix geomtery instance
    //----------------------------------------------------------------------------------------------------------------------
    optix::GeometryInstance m_geometryInstance;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our optix Transformation on our geometry
    //----------------------------------------------------------------------------------------------------------------------
    optix::Transform m_transform;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the position of our geometry
    //----------------------------------------------------------------------------------------------------------------------
    optix::float3 m_pos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the rotation of our geometry
    //----------------------------------------------------------------------------------------------------------------------
    optix::float3 m_rot;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the scale of our geometry
    //----------------------------------------------------------------------------------------------------------------------
    optix::float3 m_scale;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief applies our geometry transformations
    //----------------------------------------------------------------------------------------------------------------------
    void applyTransforms(bool _transpose = false);
    //----------------------------------------------------------------------------------------------------------------------

};

#endif // ABSTRACTOPTIXGEOMETRY_H