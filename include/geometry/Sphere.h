#ifndef SPHERE_H
#define SPHERE_H

#include "geometry/AbstractOptixGeometry.h"

class Sphere : public AbstractOptixGeometry
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Constructor that sets the context
    /// @param _context - optix context (optix::Context)
    //----------------------------------------------------------------------------------------------------------------------
    Sphere(optix::Context &_context);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default destructor
    //----------------------------------------------------------------------------------------------------------------------
    ~Sphere(){}
    //----------------------------------------------------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our default constructor removed from public use as we need a context to do pretty much anything!
    //----------------------------------------------------------------------------------------------------------------------
    Sphere();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our static sphere intersection program. This is created to remove the need to recompile every time a class is created
    //----------------------------------------------------------------------------------------------------------------------
    static optix::Program m_sphereIntersect;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our static sphere intersection program. This is created to remove the need to recompile every time a class is created
    //----------------------------------------------------------------------------------------------------------------------
    static optix::Program m_sphereBB;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief static bool to declare if we have created our static intersect and bb programs
    //----------------------------------------------------------------------------------------------------------------------
    static bool m_init;
    //----------------------------------------------------------------------------------------------------------------------
};

#endif // SPHERE_H
