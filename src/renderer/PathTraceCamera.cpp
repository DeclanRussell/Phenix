#include "renderer/PathTraceCamera.h"

PathTraceCamera::PathTraceCamera()
{
}
//----------------------------------------------------------------------------------------------------------------------
PathTraceCamera::PathTraceCamera(float3 _eye, float3 _lookat, float3 _up, float _hfov, float _vfov){
    calcVectors(_eye,_lookat, _up, _hfov,_vfov);
}
//----------------------------------------------------------------------------------------------------------------------
void PathTraceCamera::setParameters(float3 _eye, float3 _lookat, float3 _up, float _hfov, float _vfov){
    calcVectors(_eye,_lookat, _up, _hfov,_vfov);
}
//----------------------------------------------------------------------------------------------------------------------
void PathTraceCamera::calcVectors(float3 _eye, float3 _lookat, float3 _up, float _hfov, float _vfov){
    m_lookat = _lookat;
    m_eye = _eye;
    m_up= _up;
    m_W = _lookat - _eye;
    float lookdir_len = optix::length(m_W);
    m_U = optix::normalize(optix::cross(m_W,_up));
    m_V = optix::normalize(optix::cross(m_U,m_W));
    m_U*= lookdir_len * tanf(DtoR(_hfov*0.5f));
    m_V*= lookdir_len * tanf(DtoR(_vfov*0.5f));
    m_hfov = _hfov;
    m_vfov = _vfov;
}
//----------------------------------------------------------------------------------------------------------------------
void PathTraceCamera::getEyeUVW(float3 &_eye, float3 &_U, float3 &_V, float3 &_W){
    _eye = m_eye;
    _U = m_U;
    _V = m_V;
    _W = m_W;
}
//----------------------------------------------------------------------------------------------------------------------
Matrix4x4 PathTraceCamera::initWithBasis(const float3 &u, const float3 &v, const float3 &w, const float3 &t){
    float m[16];
    m[0] = u.x;
    m[1] = v.x;
    m[2] = w.x;
    m[3] = t.x;

    m[4] = u.y;
    m[5] = v.y;
    m[6] = w.y;
    m[7] = t.y;

    m[8] = u.z;
    m[9] = v.z;
    m[10] = w.z;
    m[11] = t.z;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
    return Matrix4x4( m );
}
//----------------------------------------------------------------------------------------------------------------------
