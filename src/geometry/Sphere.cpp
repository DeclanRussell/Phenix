#include "geometry/Sphere.h"

// Declare our static variables
optix::Program Sphere::m_sphereIntersect;
optix::Program Sphere::m_sphereBB;
bool Sphere::m_init;

//----------------------------------------------------------------------------------------------------------------------
Sphere::Sphere(optix::Context &_context) : AbstractOptixGeometry(_context)
{
    // Set path to our sphere ptx file
    setPtxPath("ptx/sphere.cu.ptx");
    // If we havent initialized our intersect & BB programs lets do it now
    if(!m_init)
    {
        m_sphereIntersect = getContext()->createProgramFromPTXFile(getPtxPath(),"intersect_sphere");
        m_sphereBB = getContext()->createProgramFromPTXFile(getPtxPath(),"bounds_sphere");
        m_init = true;
    }
    setIntersectionProgram(m_sphereIntersect);
    setBBProgram(m_sphereBB);
    setPrimCount(1u);

    m_geometry["sphere"]->setFloat( optix::make_float4(0.f,0.f,0.f,1.f) );

}
//----------------------------------------------------------------------------------------------------------------------
Sphere::Sphere() : AbstractOptixGeometry(){}
//----------------------------------------------------------------------------------------------------------------------

