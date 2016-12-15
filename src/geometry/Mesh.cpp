#include "geometry/Mesh.h"
#include <iostream>
#include <sstream>

// Declare our static variables
optix::Program Mesh::m_meshIntersect;
optix::Program Mesh::m_meshBB;
bool Mesh::m_init;

//----------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(optix::Context &_context) : AbstractOptixGeometry(_context)
{
    setPtxPath("ptx/triangle_mesh.cu.ptx");
    if(!m_init)
    {
        m_meshIntersect = getContext()->createProgramFromPTXFile(getPtxPath(),"mesh_intersect");
        m_meshBB = getContext()->createProgramFromPTXFile(getPtxPath(),"mesh_bounds");
        m_init = true;
    }
    setIntersectionProgram(m_meshIntersect);
    setBBProgram(m_meshBB);
    m_numPolygons = 0;
}

Mesh::Mesh(std::string _path, optix::Context &_context) : AbstractOptixGeometry(_context)
{
    setPtxPath("ptx/triangle_mesh.cu.ptx");
    if(!m_init)
    {
        m_meshIntersect = getContext()->createProgramFromPTXFile(getPtxPath(),"mesh_intersect");
        m_meshBB = getContext()->createProgramFromPTXFile(getPtxPath(),"mesh_bounds");
        m_init = true;
    }
    setIntersectionProgram(m_meshIntersect);
    setBBProgram(m_meshBB);
    m_numPolygons = 0;
    importGeometry(_path);
}
//----------------------------------------------------------------------------------------------------------------------
Mesh::~Mesh(){
    // Remove our GPU buffers
    m_vertexBuffer->destroy();
    m_normalBuffer->destroy();
    m_texCoordsBuffer->destroy();
    m_tangentsBuffer->destroy();
    m_bitangentsBuffer->destroy();
}
//----------------------------------------------------------------------------------------------------------------------
void Mesh::importGeometry(std::string _loc){
    if(!contextSet())
    {
        std::cerr<<"No context set cannot create geometry."<<std::endl;
        return;
    }
    //import our mesh
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_loc.c_str(), aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate);
    if(scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr<<"The file was not successfully opened: "<<_loc.c_str()<<std::endl;
        return;
    }

    extractMeshData(scene->mRootNode, scene);

    std::cout<<"Buffer sizes"<<std::endl;
    std::cout<<"Pos: "<<m_vertices.size()<<std::endl;
    std::cout<<"Normals: "<<m_normals.size()<<std::endl;
    std::cout<<"TexCoords: "<<m_texCoords.size()<<std::endl;
    std::cout<<"Tangents: "<<m_tangents.size()<<std::endl;
    std::cout<<"Bitangents: "<<m_bitangents.size()<<std::endl;
    std::cout<<"NumPolys: "<<m_numPolygons<<std::endl;

    createBuffers();
}
//----------------------------------------------------------------------------------------------------------------------
void Mesh::extractMeshData(const aiNode *_node, const aiScene *_scene)
{
    for (unsigned int i=0; i<_node->mNumMeshes; i++){
            aiMesh *mesh = _scene->mMeshes[_node->mMeshes[i]];
            m_numPolygons+=mesh->mNumFaces;
            processMesh(mesh);
    }

    for (unsigned int i=0; i<_node->mNumChildren; i++){
        extractMeshData(_node->mChildren[i], _scene);
    }
}
//----------------------------------------------------------------------------------------------------------------------
void Mesh::processMesh(const aiMesh *_mesh){

    if(_mesh->HasPositions())
        std::cout<<"mesh has pos"<<std::endl;
    if(_mesh->HasNormals())
        std::cout<<"mesh has normals"<<std::endl;
    if(_mesh->HasTangentsAndBitangents())
        std::cout<<"mesh has tangents and bitangents"<<std::endl;
    if(_mesh->HasTextureCoords(0))
        std::cout<<"mesh has texturecoords"<<std::endl;
    for(unsigned int i = 0; i <_mesh->mNumVertices; i++)
    {
        optix::float3 tempVec;

        if(_mesh->HasPositions())
        {
            // position
            tempVec.x = _mesh->mVertices[i].x;
            tempVec.y = _mesh->mVertices[i].y;
            tempVec.z = _mesh->mVertices[i].z;
            //std::cout<<"pos "<<tempVec.x<<","<<tempVec.y<<","<<tempVec.z<<std::endl;
            m_vertices.push_back(tempVec);
        }


        if(_mesh->HasNormals())
        {
            // normals
            tempVec.x = _mesh->mNormals[i].x;
            tempVec.y = _mesh->mNormals[i].y;
            tempVec.z = _mesh->mNormals[i].z;
            //std::cout<<"pos "<<tempVec.x<<","<<tempVec.y<<","<<tempVec.z<<std::endl;
            m_normals.push_back(tempVec);
        }


        // UV
        if(_mesh->HasTextureCoords(0)){
            tempVec.x = _mesh->mTextureCoords[0][i].x;
            tempVec.y = _mesh->mTextureCoords[0][i].y;
            tempVec.z = 0.f;
            m_texCoords.push_back(tempVec);
            //std::cout<<"tex "<<tempVec.x<<","<<tempVec.y<<std::endl;
        }

        // tangent
        if(_mesh->HasTangentsAndBitangents()){
            tempVec.x = _mesh->mTangents[i].x;
            tempVec.y = _mesh->mTangents[i].y;
            tempVec.z = _mesh->mTangents[i].z;
            m_tangents.push_back(tempVec);

            // bitangent
            tempVec.x = _mesh->mBitangents[i].x;
            tempVec.y = _mesh->mBitangents[i].y;
            tempVec.z = _mesh->mBitangents[i].z;
            //std::cout<<"bitan "<<tempVec.x<<","<<tempVec.y<<","<<tempVec.z<<std::endl;

            m_bitangents.push_back(tempVec);
        }
    }


}
//----------------------------------------------------------------------------------------------------------------------
void Mesh::createBuffers(){

    // Create vertex, normal, and texture_coordinate buffers
    m_vertexBuffer = getContext()->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, m_vertices.size() );
    m_vertexBuffer->validate();
    m_normalBuffer = getContext()->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, m_normals.size() );
    m_normalBuffer->validate();
    m_texCoordsBuffer = getContext()->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, m_texCoords.size());
    m_texCoordsBuffer->validate();
    m_tangentsBuffer = getContext()->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, m_tangents.size());
    m_tangentsBuffer->validate();
    m_bitangentsBuffer = getContext()->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, m_bitangents.size());
    m_bitangentsBuffer->validate();
    //now lets write our information to our buffers
    void *vertPtr = m_vertexBuffer->map();

    optix::float3* vertData = (optix::float3*)vertPtr;
    unsigned int i;
    for(i=0; i<m_vertices.size();i++){
        vertData[i].x = m_vertices[i].x;
        vertData[i].y = m_vertices[i].y;
        vertData[i].z = m_vertices[i].z;
    }
    m_vertexBuffer->unmap();

    vertPtr = m_normalBuffer->map();
    vertData = (optix::float3*)vertPtr;
    for(i=0; i<m_vertices.size();i++){
        vertData[i].x = m_normals[i].x;
        vertData[i].y = m_normals[i].y;
        vertData[i].z = m_normals[i].z;
    }
    m_normalBuffer->unmap();

    void *texPtr = m_texCoordsBuffer->map();

    optix::float3* texData = (optix::float3*)texPtr;

    for(i=0; i<m_vertices.size();i++){
        if (m_texCoords.size() > 0.0){
            texData[i].x = m_texCoords[i].x;
            texData[i].y = m_texCoords[i].y;
            texData[i].z = 0.f;
        }
        else{
            texData[i].x = texData[i].y = texData[i].z = 0.0;
        }
    }
    m_texCoordsBuffer->unmap();

    void *tangPtr = m_tangentsBuffer->map();
    optix::float3* tangData = (optix::float3*)tangPtr;
    for(i=0; i<m_tangents.size(); i++){
        tangData[i].x = m_tangents[i].x;
        tangData[i].y = m_tangents[i].y;
        tangData[i].z = m_tangents[i].z;
    }
    m_tangentsBuffer->unmap();

    void *bitangPtr = m_bitangentsBuffer->map();
    optix::float3* bitangData = (optix::float3*)bitangPtr;
    for(i=0; i<m_bitangents.size(); i++){
        bitangData[i].x = m_bitangents[i].x;
        bitangData[i].y = m_bitangents[i].y;
        bitangData[i].z = m_bitangents[i].z;
    }
    m_bitangentsBuffer->unmap();


    //set our buffers for our geomtry
    m_geometry["vertex_buffer"]->setBuffer( m_vertexBuffer );
    m_geometry["normal_buffer"]->setBuffer( m_normalBuffer );
    m_geometry["texcoord_buffer"]->setBuffer( m_texCoordsBuffer );
    m_geometry["tangent_buffer"]->setBuffer( m_tangentsBuffer );
    m_geometry["bitangent_buffer"]->setBuffer( m_bitangentsBuffer );

    setPrimCount(m_numPolygons);

    m_geometry->validate();

    rebuildAcceleration();
}
//----------------------------------------------------------------------------------------------------------------------
