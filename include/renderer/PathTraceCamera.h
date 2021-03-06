#ifndef PATHTRACECAMERA_H
#define PATHTRACECAMERA_H

/// @class PathTraceCamera
/// @brief this in a simple camera for path tracing. This is converted code from NVidia's demo's
/// @author Declan Russell
/// @date 27/01/2015

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_matrix_namespace.h>
#define DtoR (float)M_PI/180.f

typedef optix::float3 float3;
typedef optix::float4 float4;
typedef optix::Matrix4x4 Matrix4x4;

class PathTraceCamera
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our default contructor that does all our calculations
    /// @param _eye - the eye of our pinhole camera
    /// @param _lookat - the direction that our camera is facing
    /// @param _up - which direction up is
    /// @param _hfov - the horizontal field of view of our camera
    /// @param _vfov - the virtical field of view of our camera
    //----------------------------------------------------------------------------------------------------------------------
    PathTraceCamera(float3 _eye, float3 _lookat, float3 _up, float _hfov, float _vfov);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mutator for our paramters
    /// @param _eye - the eye of our pinhole camera
    /// @param _lookat - the direction that our camera is facing
    /// @param _up - which direction up is
    /// @param _hfov - the horizontal field of view of our camera
    /// @param _vfov - the virtical field of view of our camera
    //----------------------------------------------------------------------------------------------------------------------
    void setParameters(float3 _eye, float3 _lookat, float3 _up, float _hfov, float _vfov);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function to return
    /// @param _eye call by reference to set the eye postion of our camera
    /// @param _U call by reference to set the U vector of of our camera
    /// @param _V call by reference to set the V vector of of our camera
    /// @param _W call by reference to set the W vector of of our camera
    //----------------------------------------------------------------------------------------------------------------------
    void getEyeUVW(float3 &_eye, float3 &_U, float3 &_V, float3 &_W);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our eye location
    //----------------------------------------------------------------------------------------------------------------------
    float3 m_eye;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our U vector of our camera
    //----------------------------------------------------------------------------------------------------------------------
    float3 m_U;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our V vector of our camera
    //----------------------------------------------------------------------------------------------------------------------
    float3 m_V;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our W vector of our camera
    //----------------------------------------------------------------------------------------------------------------------
    float3 m_W;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our up vector
    //----------------------------------------------------------------------------------------------------------------------
    float3 m_up;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our camera lookat
    //----------------------------------------------------------------------------------------------------------------------
    float3 m_lookat;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our horizontal field of view
    //----------------------------------------------------------------------------------------------------------------------
    float m_hfov;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our verticle field of view
    //----------------------------------------------------------------------------------------------------------------------
    float m_vfov;
    //----------------------------------------------------------------------------------------------------------------------
protected:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief basic constructor, we dont want this to be used.
    //----------------------------------------------------------------------------------------------------------------------
    PathTraceCamera();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief calculates our Eye U V W vectors
    /// @brief this is called by our defualt constructor and setParamiters functions
    //----------------------------------------------------------------------------------------------------------------------
    void calcVectors(float3 _eye, float3 _lookat, float3 _up, float _hfov, float _vfov);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Initialises our matrix basis
    //----------------------------------------------------------------------------------------------------------------------
    Matrix4x4 initWithBasis( const float3& u, const float3& v,const float3& w,const float3& t );
    //----------------------------------------------------------------------------------------------------------------------

};

#endif // PATHTRACECAMERA_H
