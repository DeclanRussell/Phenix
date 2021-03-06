#ifndef ABSTRACTOPTIXOBJECT_H
#define ABSTRACTOPTIXOBJECT_H

#include <optix.h>
#include <optixu/optixpp_namespace.h>

#include <string>

class AbstractOptixObject
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our default constructor
    //----------------------------------------------------------------------------------------------------------------------
    AbstractOptixObject():m_contextSet(false),m_ptxPathSet(false){}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Constructor that sets the context
    /// @param _context optix context (optix::Context)
    //----------------------------------------------------------------------------------------------------------------------
    AbstractOptixObject(optix::Context &_context):m_ptxPathSet(false){setContext(_context);}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our defualt destructor
    //----------------------------------------------------------------------------------------------------------------------
    ~AbstractOptixObject(){}
    //----------------------------------------------------------------------------------------------------------------------
    /// @breif set the optix context
    /// @param _context - optix context to set (optix::Context)
    //----------------------------------------------------------------------------------------------------------------------
    inline void setContext(optix::Context &_context){m_context = _context; m_contextSet = true;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief accessor to the optix context
    //----------------------------------------------------------------------------------------------------------------------
    inline optix::Context getContext(){return m_context;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the ptx path
    /// @param _path - path to ptx file (std::string)
    //----------------------------------------------------------------------------------------------------------------------
    inline void setPtxPath(std::string &_path){m_ptxPath = _path; m_ptxPathSet = true;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set the ptx path
    /// @param _path - path to ptx file (const char*)
    //----------------------------------------------------------------------------------------------------------------------
    inline void setPtxPath(const char *_path){m_ptxPath = _path; m_ptxPathSet=true;}
    //----------------------------------------------------------------------------------------------------------------------
    /// Accessor to our ptx path
    //----------------------------------------------------------------------------------------------------------------------
    inline std::string getPtxPath(){return m_ptxPath;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns if the context is set
    /// @returns true if context set (bool)
    //----------------------------------------------------------------------------------------------------------------------
    inline bool contextSet(){return m_contextSet;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns if the ptx path is set
    /// @returns true if ptx path set (bool)
    //----------------------------------------------------------------------------------------------------------------------
    inline bool ptxPathSet(){return m_ptxPathSet;}
    //----------------------------------------------------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief The optix context to create our object within
    //----------------------------------------------------------------------------------------------------------------------
    optix::Context m_context;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief location to ptx file
    //----------------------------------------------------------------------------------------------------------------------
    std::string m_ptxPath;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief bool to define if the context has been set
    //----------------------------------------------------------------------------------------------------------------------
    bool m_contextSet;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief bool to define if the ptx path is set
    //----------------------------------------------------------------------------------------------------------------------
    bool m_ptxPathSet;
    //----------------------------------------------------------------------------------------------------------------------

};

#endif // ABSTRACTOPTIXOBJECT_H
