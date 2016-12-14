#include "geometry/Parallelogram.h"

// Declare our static variables
optix::Program Parallelogram::m_parallelogramIntersect;
optix::Program Parallelogram::m_parallelogramBB;
bool Parallelogram::m_init;

//----------------------------------------------------------------------------------------------------------------------
Parallelogram::Parallelogram(optix::Context &_context) : AbstractOptixGeometry(_context)
{
    // Set path to our parallelogram ptx file
    setPtxPath("ptx/parallelogram.cu.ptx");
    // If we havent initialized our intersect & BB programs lets do it now
    if(!m_init)
    {
        m_parallelogramIntersect = getContext()->createProgramFromPTXFile(getPtxPath(),"intersect");
        m_parallelogramBB = getContext()->createProgramFromPTXFile(getPtxPath(),"bounds");
        m_init = true;
    }
    setIntersectionProgram(m_parallelogramIntersect);
    setBBProgram(m_parallelogramBB);
    setPrimCount(1u);

    optix::float3 offset1 = optix::make_float3(0.f,0.f,1.f);
    optix::float3 offset2 = optix::make_float3(1.f,0.f,0.f);
    optix::float3 anchor = optix::make_float3(-.5f,0.f,-.5f);

    optix::float3 normal = normalize( cross( offset1, offset2 ) );
    float d = optix::dot( normal, anchor );
    optix::float4 plane = optix::make_float4( normal, d );

    optix::float3 v1 = offset1 / dot( offset1, offset1 );
    optix::float3 v2 = offset2 / dot( offset2, offset2 );

    m_geometry["plane"]->setFloat( plane );
    m_geometry["anchor"]->setFloat( anchor );
    m_geometry["v1"]->setFloat( v1 );
    m_geometry["v2"]->setFloat( v2 );
}
//----------------------------------------------------------------------------------------------------------------------
Parallelogram::Parallelogram() : AbstractOptixGeometry(){}
//----------------------------------------------------------------------------------------------------------------------

