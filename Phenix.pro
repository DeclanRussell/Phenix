TARGET=Phenix
OBJECTS_DIR=obj
# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
        cache()
        DEFINES +=QT5BUILD
}
MOC_DIR=moc
#Just something to add our exe icon
RC_FILE = myapp.rc
CONFIG-=app_bundle
QT+=gui opengl core
SOURCES += \
    src/gl/Camera.cpp \
    #src/common/HDRLoader.cpp \
    src/ui/mainwindow.cpp \
    src/ui/OpenGLWidget.cpp \
    src/renderer/PathTraceCamera.cpp \
    src/gl/Shader.cpp \
    src/gl/ShaderProgram.cpp \
    src/gl/ShaderUtils.cpp \
    src/gl/Text.cpp \
    src/gl/Texture.cpp \
    src/gl/TextureLoader.cpp \
    src/gl/TextureUtils.cpp \
    src/main.cpp \
    src/geometry/AbstractOptixGeometry.cpp \
    src/geometry/Parallelogram.cpp \
    src/geometry/Sphere.cpp \
    src/renderer/PathTracer.cpp \
    src/renderer/AbstractOptixRenderer.cpp \
    src/geometry/Mesh.cpp \
    src/ui/InspectorMenu.cpp \
    src/ui/OptixQListWidgetItem.cpp \
    src/ui/GeometryAttribEditor.cpp
    #src/lights/Light.cpp \
    #src/lights/LightManager.cpp



HEADERS += \
    include/gl/Camera.h \
    #include/common/HDRLoader.h \
    include/ui/mainwindow.h \
    include/ui/OpenGLWidget.h \
    include/renderer/PathTraceCamera.h \
    include/common/random.h \
    include/gl/Shader.h \
    include/gl/ShaderProgram.h \
    include/gl/ShaderUtils.h \
    include/gl/Text.h \
    include/gl/Texture.h \
    include/gl/TextureLoader.h \
    include/gl/TextureUtils.h \
    include/common/helpers.h \
    #include/lights/Light.h \
    #include/lights/LightManager.h
    include/common/AbstractOptixObject.h \
    include/geometry/AbstractOptixGeometry.h \
    include/geometry/Parallelogram.h \
    include/geometry/Sphere.h \
    include/lights/ParallelogramLight.h \
    include/renderer/PathTracer.h \
    include/renderer/AbstractOptixRenderer.h \
    include/geometry/Mesh.h \
    include/ui/InspectorMenu.h \
    include/ui/OptixQListWidgetItem.h \
    include/ui/GeometryAttribEditor.h


INCLUDEPATH +=./include
unix: INCLUDEPATH+= /opt/local/include
unix:LIBS += -L/opt/local/lib -lIL -lassimp
DESTDIR=./

CONFIG += console
CONFIG -= app_bundle

# use this to suppress some warning from boost
unix:QMAKE_CXXFLAGS_WARN_ON += "-Wno-unused-parameter"
QMAKE_CXXFLAGS+= -msse -msse2 -msse3
macx:QMAKE_CXXFLAGS+= -arch x86_64
macx:INCLUDEPATH+=/usr/local/include/
# define the _DEBUG flag for the graphics lib

unix:LIBS += -L/usr/local/lib

# now if we are under unix and not on a Mac (i.e. linux) define GLEW
linux-*{
                linux-*:QMAKE_CXXFLAGS +=  -march=native
                linux-*:DEFINES+=GL42
                DEFINES += LINUX
                LIBS += -lGLEW
}
win32:{
    DEFINES+=WIN32
    DEFINES+=_WIN32
    DEFINES += GLEW_STATIC
    INCLUDEPATH+=C:/boost \
                $$(ASSIMP_DIR)\include
    LIBS+= -lopengl32 -lglew32s
    LIBS+= -L$$(ASSIMP_DIR)\lib\Debug -lassimp
    DEFINES += _USE_MATH_DEFINES
    DEFINES += NOMINMAX
}

#On windows we must define if we are in debug mode or not
CONFIG(debug, debug|release) {
#DEBUG
    # MSVCRT link option (static or dynamic, it must be the same with your Qt SDK link option)
    win32:MSVCRT_LINK_FLAG_DEBUG = "/MDd"
    win32:NVCCFLAGS += -D_DEBUG -Xcompiler $$MSVCRT_LINK_FLAG_DEBUG
}
else{
#Release UNTESTED!!!
    win32:MSVCRT_LINK_FLAG_RELEASE = "/MD"
    win32:NVCCFLAGS += -Xcompiler $$MSVCRT_LINK_FLAG_RELEASE
}
DEPENDPATH+=include
# if we are on a mac define DARWIN
macx:DEFINES += DARWIN


OTHER_FILES += shaders/*.glsl \
               shaders/*.vert \
               shaders/*.frag

#Optix Stuff
message(The project will be installed in $$PWD)
CUDA_SOURCES +=  "$$PWD"/optixSrc/parallelogram.cu \
                 "$$PWD"/optixSrc/path_tracer.cu \
                 "$$PWD"/optixSrc/sphere.cu \
                 "$$PWD"/optixSrc/triangle_mesh.cu
message(CUDA sources are $$CUDA_SOURCES)

# Path to cuda SDK install
macx:CUDA_DIR = /Developer/NVIDIA/CUDA-6.5
linux:CUDA_DIR = /usr/local/cuda-6.5
win32:CUDA_DIR = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0"
# Path to cuda toolkit install
macx:CUDA_SDK = /Developer/NVIDIA/CUDA-6.5/samples
linux:CUDA_SDK = /usr/local/cuda-6.5/samples
win32:CUDA_SDK = "C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0"

# include paths
macx:INCLUDEPATH += /Developer/OptiX/SDK/sutil
macx:INCLUDEPATH += /Developer/OptiX/SDK
linux:INCLUDEPATH += /usr/local/OptiX/SDK/sutil
linux:INCLUDEPATH += /usr/local/OptiX/SDK
win32:INCLUDEPATH += "C:\ProgramData\NVIDIA Corporation\OptiX SDK 4.0.2\SDK\sutil"
win32:INCLUDEPATH += "C:\ProgramData\NVIDIA Corporation\OptiX SDK 4.0.2\SDK"
INCLUDEPATH += $$CUDA_DIR/include
INCLUDEPATH += $$CUDA_DIR/common/inc/
INCLUDEPATH += $$CUDA_DIR/../shared/inc/
macx:INCLUDEPATH += /Developer/OptiX/include
linux:INCLUDEPATH += /usr/local/OptiX/include
win32:INCLUDEPATH += "C:\ProgramData\NVIDIA Corporation\OptiX SDK 4.0.2\include"
win32:INCLUDEPATH += $$CUDA_SDK\common\inc
win32:INCLUDEPATH += "C:\Program Files (x86)\IL\include"
# lib dirs
#QMAKE_LIBDIR += $$CUDA_DIR/lib64
macx:QMAKE_LIBDIR += $$CUDA_DIR/lib
linux:QMAKE_LIBDIR += $$CUDA_DIR/lib64
win32:QMAKE_LIBDIR += $$CUDA_DIR\lib\x64
linux|macx:QMAKE_LIBDIR += $$CUDA_SDK/common/lib
win32:QMAKE_LIBDIR +=$$CUDA_SDK\common\lib\x64
macx:QMAKE_LIBDIR += /Developer/OptiX/lib64
linux:QMAKE_LIBDIR += /usr/local/OptiX/lib64
win32:QMAKE_LIBDIR += "C:\ProgramData\NVIDIA Corporation\OptiX SDK 4.0.2\lib64"
LIBS += -lcudart -lcudadevrt
unix: LIBS += -loptix
win32: LIBS += -loptix.1

# nvcc flags (ptxas option verbose is always useful)
# add the PTX flags to compile optix files
NVCCFLAGS = --compiler-options -fno-strict-aliasing -use_fast_math --ptxas-options=-v -ptx

#set our ptx directory
PTX_DIR = ptx

# join the includes in a line
# We need quotations for windows because windows is dumb!
CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')

# Prepare the extra compiler configuration (taken from the nvidia forum - i'm not an expert in this part)
optix.input = CUDA_SOURCES
message(optix input is $$optix.input)

#cuda.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}_cuda.o
optix.output = $$PTX_DIR/${QMAKE_FILE_BASE}.cu.ptx

# Tweak arch according to your hw's compute capability
#for optix you can only have one architechture when using the PTX flags when using the -ptx flag you dont want to have the -c flag for compiling
optix.commands = $$CUDA_DIR/bin/nvcc -m64 -gencode arch=compute_52,code=sm_52 $$NVCCFLAGS $$CUDA_INC $$LIBS  \"$$PWD/${QMAKE_FILE_NAME}\" -o \"$$PWD/${QMAKE_FILE_OUT}\"
#use this line for debug code
#optix.commands = $$CUDA_DIR/bin/nvcc -m64 -g -G -gencode arch=compute_52,code=sm_52 $$NVCCFLAGS $$CUDA_INC $$LIBS  ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
#Declare that we wnat to do this before compiling the C++ code
optix.CONFIG = target_predeps
#now declare that we dont want to link these files with gcc
optix.CONFIG += no_link
optix.dependency_type = TYPE_C
# Tell Qt that we want add our optix compiler
QMAKE_EXTRA_COMPILERS += optix





