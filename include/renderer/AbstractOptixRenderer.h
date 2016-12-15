#ifndef ABSTRACTOPTIXRENDERER_H
#define ABSTRACTOPTIXRENDERER_H

#ifdef DARWIN
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
    #ifndef WIN32
        #include <GL/gl.h>
    #endif
#endif

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_matrix_namespace.h>

class AbstractOptixRenderer
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default constructor. Creates our optix context.
    //----------------------------------------------------------------------------------------------------------------------
    AbstractOptixRenderer();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default destructor. Frees optix context and other alocated memory
    //----------------------------------------------------------------------------------------------------------------------
    ~AbstractOptixRenderer();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief initalize function. This is to prepare anything that the renderer needs before begining.
    //----------------------------------------------------------------------------------------------------------------------
    virtual void initialize();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief resizes the resolution of our render.
    /// @param _width - resolution width
    /// @param _height - resolution height
    //----------------------------------------------------------------------------------------------------------------------
    virtual void resize(unsigned int _width, unsigned int _height);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Launches our ray tracer
    //----------------------------------------------------------------------------------------------------------------------
    virtual void trace();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets a transformation to the scene. Normally global transformation to geometry.
    /// @param _trans - transform matrix (float*)
    /// @param _invTrans - inverse transform (float*)
    /// @param _transpose - if you wish to transpose the matrixs (bool)
    //----------------------------------------------------------------------------------------------------------------------
    virtual void setTransform(float* _trans, float* _invTrans, bool _transpose){}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief accessor to the optix context
    //----------------------------------------------------------------------------------------------------------------------
    inline optix::Context getContext(){return m_context;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief mutator for our device pixel ratio
    //----------------------------------------------------------------------------------------------------------------------
    inline void setDevicePixelRatio(int _ratio = 1){m_devicePixelRatio = _ratio;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns our output buffer
    /// @returns output buffer (optix::Buffer)
    //----------------------------------------------------------------------------------------------------------------------
    inline optix::Buffer getOutputBuffer(){return m_outputBuffer;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief an accessor to the width of our scene
    /// @returns resolution width (unsigned int)
    //----------------------------------------------------------------------------------------------------------------------
    inline unsigned int getWidth(){return m_width;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief an accessor to the height of our scene
    /// @returns resolution height (unsigned int)
    //----------------------------------------------------------------------------------------------------------------------
    inline unsigned int getHeight(){return m_height;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the ray generation program
    /// @param _ptxPath - path to file containing ray gen program (std::string)
    /// @param _name - name of ray gen program (std::string)
    /// @param _entryPointIndex - index of entry point (unsigned int)
    //----------------------------------------------------------------------------------------------------------------------
    void setRayGenProgram(std::string _ptxPath, std::string _name, unsigned int _entryPointIndex = 0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the ray generation program
    /// @param _rayGenProg - ray generation program (optix::Program)
    /// @param _entryPointIndex - index of entry point (unsigned int)
    //----------------------------------------------------------------------------------------------------------------------
    void setRayGenProgram(optix::Program &_rayGenProg, unsigned int _entryPointIndex = 0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the exception program
    /// @param _ptxPath - path to file containing exception program (std::string)
    /// @param _name - name of ray gen program (std::string)
    /// @param _entryPointIndex - index of entry point (unsigned int)
    //----------------------------------------------------------------------------------------------------------------------
    void setExceptionProgram(std::string _ptxPath, std::string _name, unsigned int _entryPointIndex = 0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the exception program
    /// @param _excepProg - exception program (optix::Program)
    /// @param _entryPointIndex - index of entry point (unsigned int)
    //----------------------------------------------------------------------------------------------------------------------
    void setExceptionProgram(optix::Program &_excepProg, unsigned int _entryPointIndex = 0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the miss program (std::string)
    /// @param _ptxPath - path to file containing miss program (std::string)
    /// @param _name - name of ray gen program (std::string)
    /// @param _entryPointIndex - index of entry point (unsigned int)
    //----------------------------------------------------------------------------------------------------------------------
    void setMissProgram(std::string _ptxPath, std::string _name, unsigned int _entryPointIndex = 0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the exception program
    /// @param _missProg - miss program (optix::Program)
    /// @param _entryPointIndex - index of entry point (unsigned int)
    //----------------------------------------------------------------------------------------------------------------------
    void setMissProgram(optix::Program &_missProg, unsigned int _entryPointIndex = 0);
    //----------------------------------------------------------------------------------------------------------------------
protected:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our output buffer
    //----------------------------------------------------------------------------------------------------------------------
    optix::Buffer m_outputBuffer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief render resolution width
    //----------------------------------------------------------------------------------------------------------------------
    unsigned int m_width;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief render resolution height
    //----------------------------------------------------------------------------------------------------------------------
    unsigned int m_height;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our device pixel ratio default set to 1 but for mac this could be different
    //----------------------------------------------------------------------------------------------------------------------
    int m_devicePixelRatio;
    //----------------------------------------------------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief An intance of the optix engine
    //----------------------------------------------------------------------------------------------------------------------
    optix::Context m_context;
    //----------------------------------------------------------------------------------------------------------------------
};

#endif // ABSTRACTOPTIXRENDERER_H
