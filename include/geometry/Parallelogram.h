#ifndef PARALLELOGRAM_H
#define PARALLELOGRAM_H

#include "geometry/AbstractOptixGeometry.h"

class Parallelogram : public AbstractOptixGeometry
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Constructor that sets the context
    /// @param _context - optix context (optix::Context)
    //----------------------------------------------------------------------------------------------------------------------
    Parallelogram(optix::Context &_context);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default destructor
    //----------------------------------------------------------------------------------------------------------------------
    ~Parallelogram(){}
    //----------------------------------------------------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our default constructor removed from public use as we need a context to do pretty much anything!
    //----------------------------------------------------------------------------------------------------------------------
    Parallelogram();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our static parallelogram intersection program. This is created to remove the need to recompile every time a class is created
    //----------------------------------------------------------------------------------------------------------------------
    static optix::Program m_parallelogramIntersect;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our static parallelogram intersection program. This is created to remove the need to recompile every time a class is created
    //----------------------------------------------------------------------------------------------------------------------
    static optix::Program m_parallelogramBB;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief static bool to declare if we have created our static intersect and bb programs
    //----------------------------------------------------------------------------------------------------------------------
    static bool m_init;
    //----------------------------------------------------------------------------------------------------------------------
};

#endif // PARALLELOGRAM_H
