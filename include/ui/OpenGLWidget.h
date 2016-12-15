#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#ifdef DARWIN
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
    #ifndef WIN32
        #include <GL/gl.h>
    #endif
#endif
#include <QGLWidget>
#include <QEvent>
#include <QResizeEvent>
#include <QMessageBox>
#include <glm/glm.hpp>
#include <QTime>
#include <QGridLayout>
#include <QLineEdit>

#define GLM_FORCE_RADIANS
#include "gl/Camera.h"
#include "gl/ShaderProgram.h"
#include "gl/Shader.h"
#include "gl/Text.h"
#include "renderer/AbstractOptixRenderer.h"



class OpenGLWidget : public QGLWidget
{
    Q_OBJECT //must include to gain access to qt stuff
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    explicit OpenGLWidget(const QGLFormat _format, QWidget *_parent=0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~OpenGLWidget();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the virtual initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initializeGL();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void paintGL();
    //----------------------------------------------------------------------------------------------------------------------
    // Qt 5.5.1 must have this implemented and uses it
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(QResizeEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    // Qt 5.x uses this instead! http://doc.qt.io/qt-5/qopenglwindow.html#resizeGL
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(int _w, int _h);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief mouse move
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent(QMouseEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a timer event function from the Q_object
    //----------------------------------------------------------------------------------------------------------------------
    void timerEvent(QTimerEvent *);//
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function called when a mouse button is pressed
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent(QMouseEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Called when the mouse button is released
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent(QMouseEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Scroll wheel event
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent(QWheelEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Key press event
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Key release event
    //----------------------------------------------------------------------------------------------------------------------
    void keyReleaseEvent(QKeyEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Returns a string containing the environment map
    //----------------------------------------------------------------------------------------------------------------------
    inline QString getEnvironmentMap(){return m_environmentMap;}
    //----------------------------------------------------------------------------------------------------------------------
public slots:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief saves render to image file
    //----------------------------------------------------------------------------------------------------------------------
    void saveImage();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mutator for our movement render reduction
    //----------------------------------------------------------------------------------------------------------------------
    void setMoveRenderReduction(int _reductionAmount){m_moveRenderReduction = _reductionAmount;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mutator for our timeout duration
    //----------------------------------------------------------------------------------------------------------------------
    inline void setTimeOutDur(int _timeout){m_timedOut = _timeout;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to set the max depth we wish rays to travers while moving our scene camera
    /// @param _depth - desired ray depth
    //----------------------------------------------------------------------------------------------------------------------
    inline void setCamMovRayDepth(int _depth){m_cameraMovRayDepth = _depth;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Reset Global Trans
    //----------------------------------------------------------------------------------------------------------------------
    void resetGlobalTrans();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to toggle HUD
    //----------------------------------------------------------------------------------------------------------------------
    inline void drawHUD(bool _toggle){m_drawHud = _toggle;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets our optix renderer
    /// @param _renderer - the optix renderer we wish to use
    //----------------------------------------------------------------------------------------------------------------------
    inline void setOptiXRenderer(AbstractOptixRenderer *_renderer){m_renderer = _renderer;}
    //----------------------------------------------------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our renderer
    //----------------------------------------------------------------------------------------------------------------------
    AbstractOptixRenderer *m_renderer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief timer for the FPS count
    //----------------------------------------------------------------------------------------------------------------------
    QTime m_FPSTimer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a bool to define if we want to draw the HUD
    //----------------------------------------------------------------------------------------------------------------------
    bool m_drawHud;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our text drawing class
    //----------------------------------------------------------------------------------------------------------------------
    Text *m_textDrawer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the max depth we wish rays to travers while moving our scene camera
    //----------------------------------------------------------------------------------------------------------------------
    int m_cameraMovRayDepth;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our current ray depth
    //----------------------------------------------------------------------------------------------------------------------
    int m_curMaxRayDepth;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief out vao for our plane to project our texture on to
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_VAO;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our buffer object for our vao, one for vertex postions and one for texture coordinates
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_VBO[2];
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief handle to the texture we will project onto our plane
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_texID;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Shader Program
    //----------------------------------------------------------------------------------------------------------------------
    ShaderProgram *m_shaderProgram;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the location of our texture uniform in our shader
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_texLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ModelViewProjection matrix location
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_modelViewProjectionLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Height of the window
    //----------------------------------------------------------------------------------------------------------------------
    int m_height;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Width of the window
    //----------------------------------------------------------------------------------------------------------------------
    int m_width;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Mouse transforms
    //----------------------------------------------------------------------------------------------------------------------
    glm::mat4 m_mouseGlobalTX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Mouse transforms environment map
    //----------------------------------------------------------------------------------------------------------------------
    glm::mat4 m_mouseGlobalTXEnvironment;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief model pos
    //----------------------------------------------------------------------------------------------------------------------
    glm::vec3 m_modelPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Spin face x
    //----------------------------------------------------------------------------------------------------------------------
    float m_spinXFace;
    float m_spinXFaceEnvironment;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Sping face y
    //----------------------------------------------------------------------------------------------------------------------
    float m_spinYFace;
    float m_spinYFaceEnvironment;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief rotate bool
    //----------------------------------------------------------------------------------------------------------------------
    bool m_rotate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief translate bool
    //----------------------------------------------------------------------------------------------------------------------
    bool m_translate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief
    //----------------------------------------------------------------------------------------------------------------------
    int m_origX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief
    //----------------------------------------------------------------------------------------------------------------------
    int m_origY;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief
    //----------------------------------------------------------------------------------------------------------------------
    int m_origXPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief
    //----------------------------------------------------------------------------------------------------------------------
    int m_origYPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our opengl camera
    //----------------------------------------------------------------------------------------------------------------------
    Camera *m_cam;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Load our perspective matrix to the shader
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToShader(glm::mat4 _modelMatrix, glm::mat4 _viewMatrix, glm::mat4 _perspectiveMatrix);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the render reduction we want while moving for increased interactibilty speed
    //----------------------------------------------------------------------------------------------------------------------
    int m_moveRenderReduction;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief render timeout in seconds
    //----------------------------------------------------------------------------------------------------------------------
    int m_timedOut;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used for calculating the timeout
    //----------------------------------------------------------------------------------------------------------------------
    QTime m_timeOutStart;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief The environment map location
    //----------------------------------------------------------------------------------------------------------------------
    QString m_environmentMap;
    //----------------------------------------------------------------------------------------------------------------------
    bool m_translateEnvironment;

};

#endif // OPENGLWIDGET_H
