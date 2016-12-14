#include "renderer/AbstractOptixRenderer.h"

//----------------------------------------------------------------------------------------------------------------------
AbstractOptixRenderer::AbstractOptixRenderer()
{
    // create an instance of our OptiX engine
    m_context = optix::Context::create();
    m_devicePixelRatio = 1;
}
//----------------------------------------------------------------------------------------------------------------------
AbstractOptixRenderer::~AbstractOptixRenderer()
{
    // Free our output buffer
    m_outputBuffer->destroy();
    // Destroy our optix instance
    m_context->destroy();
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixRenderer::initialize()
{

}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixRenderer::resize(unsigned int _width, unsigned int _height)
{
    m_width = _width;
    m_height = _height;
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixRenderer::trace()
{
    m_context->launch(0,m_width,m_height);
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixRenderer::setRayGenProgram(std::string _ptxPath, std::string _name, unsigned int _entryPointIndex)
{
    optix::Program rg = m_context->createProgramFromPTXFile(_ptxPath,_name);
    m_context->setRayGenerationProgram(_entryPointIndex,rg);
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixRenderer::setRayGenProgram(optix::Program &_rayGenProg, unsigned int _entryPointIndex)
{
    m_context->setRayGenerationProgram(_entryPointIndex,_rayGenProg);
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixRenderer::setExceptionProgram(std::string _ptxPath, std::string _name, unsigned int _entryPointIndex)
{
    optix::Program exep = m_context->createProgramFromPTXFile(_ptxPath,_name);
    m_context->setExceptionProgram(_entryPointIndex,exep);
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixRenderer::setExceptionProgram(optix::Program &_excepProg, unsigned int _entryPointIndex)
{
    m_context->setExceptionProgram(_entryPointIndex,_excepProg);
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixRenderer::setMissProgram(std::string _ptxPath, std::string _name, unsigned int _entryPointIndex)
{
    optix::Program miss = m_context->createProgramFromPTXFile(_ptxPath,_name);
    m_context->setMissProgram(_entryPointIndex,miss);
}
//----------------------------------------------------------------------------------------------------------------------
void AbstractOptixRenderer::setMissProgram(optix::Program &_missProg, unsigned int _entryPointIndex)
{
    m_context->setMissProgram(_entryPointIndex,_missProg);
}
//----------------------------------------------------------------------------------------------------------------------
