#include "renderer/PathTracer.h"
#include <QColor>
#include <QTime>
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "geometry/Parallelogram.h"
#include "geometry/Sphere.h"

//Declare our static instance variable
PathTracerScene* PathTracerScene::m_instance;

//----------------------------------------------------------------------------------------------------------------------
PathTracerScene* PathTracerScene::getInstance(){
    if(!m_instance){
        m_instance = new PathTracerScene();
    }
    return m_instance;
}

//----------------------------------------------------------------------------------------------------------------------
PathTracerScene::PathTracerScene()  : AbstractOptixRenderer(),
                                    m_totalNumPolygons(0),
                                    m_cameraChanged(false),
                                    m_rr_begin_depth(1u),
                                    m_sqrt_num_samples( 2u ),
                                    m_frame(0),
                                    m_translateEnviroment(false)
{
    AbstractOptixRenderer::resize(512,512);
}
//----------------------------------------------------------------------------------------------------------------------
PathTracerScene::~PathTracerScene()
{
    delete m_camera;

    //m_enviSampler->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void PathTracerScene::initialize()
{
    // Grab our context
    optix::Context context = getContext();
    // how many ray types we have
    // we have our light ray, shadow ray and a bsdf shadow ray
    context->setRayTypeCount( 2 );
    // we only have one entry point which is our output buffer
    context->setEntryPointCount( 1 );
    // sets the stack size important for recursion
    // we want this to be as big as our hardware will allow us
    // so that we can send as many rays as the user desires
    context->setStackSize( 4000 );
    // set some variables
    context["scene_epsilon"]->setFloat( 1.e-3f );
    // set our ray types
    context["pathtrace_ray_type"]->setUint(0u);
    context["pathtrace_shadow_ray_type"]->setUint(1u);
    context["rr_begin_depth"]->setUint(m_rr_begin_depth);

    // Enable printing on the GPU
    rtContextSetPrintEnabled(context->get(), 1);

    // create our output buffer and set it in our engine
    optix::Variable output_buffer = context["output_buffer"];
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * m_width * m_height, 0, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    m_outputBuffer = context->createBufferFromGLBO(RT_BUFFER_OUTPUT,vbo);
    m_outputBuffer->setFormat(RT_FORMAT_FLOAT4);
    m_outputBuffer->setSize(m_width/m_devicePixelRatio,m_height/m_devicePixelRatio);
    output_buffer->set(m_outputBuffer);

    m_camera = new PathTraceCamera(optix::make_float3( 278.0f, 273.0f, -900.0f ),   //eye
                                 optix::make_float3( 278.0f, 273.0f,    0.0f  ),       //lookat
                                 optix::make_float3( 0.0f, 1.0f,  0.0f ),      //up
                                 35.0f,                                 //hfov
                                 35.0f);                                //vfov

    float3 eye,U,V,W;
    m_camera->getEyeUVW(eye,U,V,W);
    //set up our camera in our engine
    context["eye"]->setFloat( eye );
    context["U"]->setFloat( U );
    context["V"]->setFloat( V );
    context["W"]->setFloat( W);
    context["aperture_radius"]->setFloat(0.0);
    context["focal_point"]->setFloat(0.0, 0.0, 0.0);

    //set our max ray depth
    context["maxDepth"]->setUint(5);
    m_maxRayDepth = 5;
    context["sqrt_num_samples"]->setUint(m_sqrt_num_samples );
    context["bad_color"]->setFloat( 233.f, 5.0f, 150.0f );
    context["bg_color"]->setFloat( optix::make_float3(0.f,0.f,0.f) );
    const float3 default_color = optix::make_float3(1.0f, 1.0f, 1.0f);
    float m[16];
    m[ 0] = 1.0f;  m[ 1] = 0.0f;  m[ 2] = 0.0f;  m[ 3] = 0.0f;
    m[ 4] = 0.0f;  m[ 5] = 1.0f;  m[ 6] = 0.0f;  m[ 7] = 0.0f;
    m[ 8] = 0.0f;  m[ 9] = 0.0f;  m[10] = 1.0f;  m[11] = 0.0f;
    m[12] = 0.0f;  m[13] = 0.0f;  m[14] = 0.0f;  m[15] = 1.0f;
    context["cameraMatrix"]->setMatrix4x4fv(false, m);

    // Setup programs
    std::string ptx_path = "ptx/path_tracer.cu.ptx";
    setRayGenProgram(ptx_path,"pathtrace_camera");
    //optix::Program ray_gen_program = m_context->createProgramFromPTXFile( ptx_path, "depth_of_field_camera" );
    setExceptionProgram(ptx_path,"exception");
    setMissProgram(ptx_path,"miss");
    //m_context->setMissProgram( 0, m_context->createProgramFromPTXFile( ptx_path, "envi_miss" ) );


    //init our frame number
    context["frame_number"]->setUint(1);

    // Index of sampling_stategy (BSDF, light, MIS)
    m_sampling_strategy = 0;
    context["sampling_stategy"]->setInt(m_sampling_strategy);

    // Lights buffer
    //m_context["lights"]->setBuffer( LightManager::getInstance()->getLightsBuffer() );
    // Light buffer

    //create our top group and set it in our engine and add a matrix for our global translation
    m_globalTrans = context->createTransform();
    m_globalTrans->setMatrix(false,m,0);
    m_globalTransGroup = context->createGroup();
    m_globalTrans->setChild(m_globalTransGroup);
    m_globalTransGroup->setAcceleration(context->createAcceleration("Sbvh","Bvh"));
    m_topGroup = context->createGroup();
    m_topGroup->setAcceleration(context->createAcceleration("NoAccel","NoAccel"));
    m_topGroup->addChild(m_globalTrans);
    context["top_object"]->set(m_topGroup);

    // Finalize
    context->validate();
    context->compile();
}
//----------------------------------------------------------------------------------------------------------------------
void PathTracerScene::trace()
{
    //if our camera has changed then update it in our engine
    if(m_cameraChanged) updateCamera();

    //launch it
    getContext()["frame_number"]->setUint( m_frame++ );
    getContext()->launch(0,m_width,m_height);
}
//----------------------------------------------------------------------------------------------------------------------
void PathTracerScene::resize(unsigned int _width,unsigned int _height)
{
    m_width = _width/m_devicePixelRatio;
    m_height = _height/m_devicePixelRatio;

    unsigned int elementSize = m_outputBuffer->getElementSize();
    GLuint handleID = m_outputBuffer->getGLBOId();

    float aR = (float)_width/(float)_height;
    m_camera->setParameters(m_camera->m_eye,m_camera->m_lookat,m_camera->m_up,35.f*aR,35.f);
    updateCamera();

    m_outputBuffer->setSize(m_width,m_height);
    m_outputBuffer->unregisterGLBuffer();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, handleID);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, elementSize * m_width * m_height, 0, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    m_outputBuffer->registerGLBuffer();

    m_frame = 0;
}
//----------------------------------------------------------------------------------------------------------------------
void PathTracerScene::updateCamera()
{
    float3 eye,U,V,W;
    m_camera->getEyeUVW(eye,U,V,W);

    //set up our camera in our engine
    getContext()["eye"]->setFloat( eye );
    getContext()["U"]->setFloat( U );
    getContext()["V"]->setFloat( V );
    getContext()["W"]->setFloat( W);
    m_frame = 0;
    m_cameraChanged = false;
}
//----------------------------------------------------------------------------------------------------------------------
QImage PathTracerScene::saveImage()
{
    QImage img(m_width,m_height,QImage::Format_RGB32);
    QColor color;
    // as we're using a openGL buffer rather than optix we must map it with openGL calls
    GLint vboId = m_outputBuffer->getGLBOId();
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    typedef struct { float r; float g; float b; float a;} rgb;
    rgb* rgb_data = (rgb*)data;

    int x;
    int y;
    int h = m_width * m_height;
    for(unsigned int i=0; i<m_width*m_height; i++)
    {
        float red = rgb_data[h-i].r; if(red>1.0) red=1.0;
        float green = rgb_data[h-i].g; if(green>1.0) green=1.0;
        float blue = rgb_data[h-i].b; if(blue>1.0) blue=1.0;
        float alpha = rgb_data[h-i].a; if(alpha>1.0) alpha=1.0;
        color.setRgbF(red,green,blue,alpha);
        y = floor((float)i/m_width);
        x = m_width - i + y*m_width - 1;
        img.setPixel(x, y, color.rgb());

    }
    //remember to unmap the buffer after or you're going to have a bad time!
    glUnmapBuffer(GL_ARRAY_BUFFER);

    return img;
}
//----------------------------------------------------------------------------------------------------------------------
void PathTracerScene::setGlobalTrans(glm::mat4 _trans)
{
    // identity matrix to init our transformation
    float m[16];
    m[ 0] = _trans[0][0];  m[ 1] = _trans[1][0];  m[ 2] = _trans[2][0];  m[ 3] = _trans[3][0];
    m[ 4] = _trans[0][1];  m[ 5] = _trans[1][1];  m[ 6] = _trans[2][1];  m[ 7] = _trans[3][1];
    m[ 8] = _trans[0][2];  m[ 9] = _trans[1][2];  m[ 10] = _trans[2][2];  m[ 11] = _trans[3][2];
    m[ 12] = _trans[0][3];  m[ 13] = _trans[1][3];  m[ 14] = _trans[2][3];  m[ 15] = _trans[3][3];
    // create our inverse transform
    float invM[16];
    glm::mat4 inv = glm::inverse(_trans);
    invM[ 0] = inv[0][0];  invM[ 1] = inv[1][0];  invM[ 2] = inv[2][0];  invM[ 3] = inv[3][0];
    invM[ 4] = inv[0][1];  invM[ 5] = inv[1][1];  invM[ 6] = inv[2][1];  invM[ 7] = inv[3][1];
    invM[ 8] = inv[0][2];  invM[ 9] = inv[1][2];  invM[ 10] = inv[2][2];  invM[ 11] = inv[3][2];
    invM[ 12] = inv[0][3];  invM[ 13] = inv[1][3];  invM[ 14] = inv[2][3];  invM[ 15] = inv[3][3];
    // set our transform
    m_globalTrans->setMatrix(false,m,invM);

    //update our scene
    cleanTopAcceleration();
}
//----------------------------------------------------------------------------------------------------------------------
void PathTracerScene::setMaterial(PathTracerScene::GeometryInstance &gi, PathTracerScene::Material material, const std::string &color_name, const PathTracerScene::float3 &color)
{
    gi->addMaterial(material);
    gi[color_name]->setFloat(color);
}
//----------------------------------------------------------------------------------------------------------------------
optix::GeometryInstance PathTracerScene::createParallelogram(const float3& anchor, const float3& offset1, const float3& offset2)
{
    // Set up parallelogram programs
    std::string ptx_path = "ptx/parallelogram.cu.ptx" ;
    optix::Program pgram_bounding_box = getContext()->createProgramFromPTXFile( ptx_path, "bounds" );
    optix::Program pgram_intersection = getContext()->createProgramFromPTXFile( ptx_path, "intersect" );

    optix::Geometry parallelogram = getContext()->createGeometry();
    parallelogram->setPrimitiveCount( 1u );
    parallelogram->setIntersectionProgram( pgram_intersection );
    parallelogram->setBoundingBoxProgram( pgram_bounding_box );

    float3 normal = normalize( cross( offset1, offset2 ) );
    float d = dot( normal, anchor );
    float4 plane = optix::make_float4( normal, d );

    float3 v1 = offset1 / dot( offset1, offset1 );
    float3 v2 = offset2 / dot( offset2, offset2 );

    parallelogram["plane"]->setFloat( plane );
    parallelogram["anchor"]->setFloat( anchor );
    parallelogram["v1"]->setFloat( v1 );
    parallelogram["v2"]->setFloat( v2 );

    optix::GeometryInstance gi = getContext()->createGeometryInstance();
    gi->setGeometry(parallelogram);
    return gi;
}
//----------------------------------------------------------------------------------------------------------------------
void PathTracerScene::loadTestGeomtry()
{
    // Light buffer
    ParallelogramLight light;
    light.corner   = optix::make_float3( 343.0f, 548.6f, 227.0f);
    light.v1       = optix::make_float3( -130.0f, 0.0f, 0.0f);
    light.v2       = optix::make_float3( 0.0f, 0.0f, 105.0f);
    light.normal   = normalize( cross(light.v1, light.v2) );
    light.emission = optix::make_float3( 15.0f, 15.0f, 15.0f );

    optix::Context context = getContext();
    optix::Buffer light_buffer = context->createBuffer( RT_BUFFER_INPUT );
    light_buffer->setFormat( RT_FORMAT_USER );
    light_buffer->setElementSize( sizeof( ParallelogramLight ) );
    light_buffer->setSize( 1u );
    memcpy( light_buffer->map(), &light, sizeof( light ) );
    light_buffer->unmap();
    context["lights"]->setBuffer( light_buffer );


    // Set up material
    std::string ptx_path = "ptx/path_tracer.cu.ptx";
    Material diffuse = context->createMaterial();
    optix::Program diffuse_ch = context->createProgramFromPTXFile( ptx_path, "diffuse" );
    optix::Program diffuse_ah = context->createProgramFromPTXFile( ptx_path, "shadow" );
    diffuse->setClosestHitProgram( 0, diffuse_ch );
    diffuse->setAnyHitProgram( 1, diffuse_ah );
    Material reflection = context->createMaterial();
    optix::Program reflection_ch = context->createProgramFromPTXFile( ptx_path, "reflection" );
    reflection->setClosestHitProgram( 0, reflection_ch );
    reflection->setAnyHitProgram( 1, diffuse_ah );

    optix::Material diffuse_light = context->createMaterial();
    optix::Program diffuse_em = context->createProgramFromPTXFile( ptx_path, "diffuseEmitter" );
    diffuse_light->setClosestHitProgram( 0, diffuse_em );


    // create geometry instances
    std::vector<optix::GeometryInstance> gis;

    const float3 white = optix::make_float3( 0.9f, 0.9f, 0.9f );
    const float3 green = optix::make_float3( 0.05f, 0.8f, 0.05f );
    const float3 red   = optix::make_float3( 0.8f, 0.05f, 0.05f );
    const float3 light_em = optix::make_float3( 15.0f, 15.0f, 5.0f );

    // Floor
    Parallelogram floor(getContext());
    floor.setScale(556.f,1.f,559.2f);
    floor.setPos(556.f/2.f,0.f,559.2f/2.f);
    floor.setMaterial(diffuse);
    floor.getGeometryInstance()["diffuse_color"]->setFloat(white);
    m_globalTransGroup->addChild(floor.getGeomAndTrans());

    //setMaterial(gis.back(), diffuse, "diffuse_color", white);

    // Ceiling
    Parallelogram ceiling(getContext());
    ceiling.setScale(556.f,1.f,559.2f);
    ceiling.setPos(556.f/2.f,548.8f,559.2f/2.f);
    ceiling.setMaterial(diffuse);
    ceiling.getGeometryInstance()["diffuse_color"]->setFloat(white);
    m_globalTransGroup->addChild(ceiling.getGeomAndTrans());

    // Back wall
    Parallelogram Back(getContext());
    Back.setScale(556.f,1.f,559.2f);
    Back.setPos(556.f/2.f,559.2f/2.f,559.2f);
    Back.setRot(90.f,0.f,0.f);
    Back.setMaterial(diffuse);
    Back.getGeometryInstance()["diffuse_color"]->setFloat(white);
    m_globalTransGroup->addChild(Back.getGeomAndTrans());

    // Right wall
    Parallelogram Right(getContext());
    Right.setScale(556.f,1.f,559.2f);
    Right.setPos(0.f,548.8f/2.f,559.2f/2.f);
    Right.setRot(0.f,0.f,90.f);
    Right.setMaterial(diffuse);
    Right.getGeometryInstance()["diffuse_color"]->setFloat(green);
    m_globalTransGroup->addChild(Right.getGeomAndTrans());


    // Left wall
    Parallelogram Left(getContext());
    Left.setScale(556.f,1.f,559.2f);
    Left.setPos(556.0f,548.8f/2.f,559.2f/2.f);
    Left.setRot(0.f,0.f,90.f);
    Left.setMaterial(diffuse);
    Left.getGeometryInstance()["diffuse_color"]->setFloat(red);
    m_globalTransGroup->addChild(Left.getGeomAndTrans());

    // Sphere
    Sphere s(getContext());
    s.setScale(100.f,100.f,100.f);
    s.setPos(200.0f,100.f,559.2f/4.f);
    s.setMaterial(diffuse);
    s.getGeometryInstance()["diffuse_color"]->setFloat(red);
    m_globalTransGroup->addChild(s.getGeomAndTrans());

    Sphere s2(getContext());
    s2.setScale(150.f,150.f,150.f);
    s2.setPos((2.5f*559.2f)/4.f,150.f,(3.f*559.2f)/4.f);
    s2.setMaterial(reflection);
    s2.getGeometryInstance()["diffuse_color"]->setFloat(white);
    m_globalTransGroup->addChild(s2.getGeomAndTrans());


    // Light
    Parallelogram l(getContext());
    l.setScale(130.0f,1.f,105.0f);
    l.setPos(556.f/2.f,548.6f,559.2f/2.f);
    l.setMaterial(diffuse_light);
    l.getGeometryInstance()["emission_color"]->setFloat(light_em);
    m_globalTransGroup->addChild(l.getGeomAndTrans());

    // Mark our acceleration dirty so it rebuilds
    m_globalTransGroup->getAcceleration()->markDirty();
}
//----------------------------------------------------------------------------------------------------------------------
void PathTracerScene::cleanTopAcceleration()
{
    m_globalTransGroup->getAcceleration()->markDirty();
    m_frame = 0;
}
//----------------------------------------------------------------------------------------------------------------------

