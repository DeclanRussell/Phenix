#include "ui/OpenGLWidget.h"
#include <QGuiApplication>
#include <QFileDialog>
#include <QBuffer>
#include <QImageWriter>
#include <iostream>
#include <optixu/optixpp_namespace.h>

const static float INCREMENT=0.15;
//------------------------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//------------------------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.3;
OpenGLWidget::OpenGLWidget(const QGLFormat _format, QWidget *_parent) : QGLWidget(_format,_parent){
    // set this widget to have the initial keyboard focus
    setFocus();
    setFocusPolicy(Qt::StrongFocus);
    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    m_rotate=false;
    // mouse rotation values set to 0
    m_spinXFace=0;
    m_spinYFace=0;
    m_spinXFaceEnvironment=0;
    m_spinYFaceEnvironment=0;
    m_moveRenderReduction = 4;
    m_timedOut = 0;
    m_cameraMovRayDepth = 2;
    m_modelPos = glm::vec3(0);
    m_mouseGlobalTX = glm::mat4();
    m_translateEnvironment = false;
    m_drawHud = true;
    m_renderer = 0;
    m_render = true;
    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    this->resize(_parent->size());
}
//----------------------------------------------------------------------------------------------------------------------
OpenGLWidget::~OpenGLWidget(){
    //delete m_statsLineEdit;
    delete m_shaderProgram;
    delete m_cam;
    delete m_textDrawer;
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(2, m_VBO);
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::initializeGL(){
#ifndef DARWIN
    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if(error != GLEW_OK){
        std::cerr<<"GLEW IS NOT OK!!! "<<std::endl;
    }
#endif
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // as re-size is not explicitly called we need to do this.
    glViewport(0,0,width(),height());

    if(!m_renderer)
    {
        std::cerr<<"No ray tracer set. Exiting application."<<std::endl;
        exit(-1);
    }

    m_renderer->setDevicePixelRatio(devicePixelRatio());
    m_renderer->initialize();
    //create our plane to project our scene onto
    float vertex[]={
        //bottom left
        -1.0f,-1.0f,
        //top left
        -1.0f,1.0f,
        //bottom right
        1.0f,-1.0f,
        //top right
        1.0f,1.0f
    };
    //notice our tex coords are flipped upside down becuase
    //the resulting image from a camera will be upside down
    float texCoords[]={
        //bottom left
        1.0,1.0f,
        //top left
        1.0f,0.0f,
        //bottom right
        0.0f,1.0f,
        //top right
        0.0f,0.0f
    };

    //gen our VAO
    glGenVertexArrays(1,&m_VAO);
    glBindVertexArray(m_VAO);
    //put our data into our vbo's
    glGenBuffers(2,m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER,m_VBO[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertex),vertex,GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*2.0,(GLvoid*)(0*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,m_VBO[1]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(texCoords),texCoords,GL_STATIC_DRAW);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(float)*2.0,(GLvoid*)(0*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    //now lets generate our texture for later use
    glGenTextures(1,&m_texID);
    glBindTexture(GL_TEXTURE_2D, m_texID);

    // Change these to GL_LINEAR for super- or sub-sampling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // GL_CLAMP_TO_EDGE for linear filtering, not relevant for nearest.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture( GL_TEXTURE_2D, 0);
    //all good!

    // Create a shader program
    m_shaderProgram = new ShaderProgram();
    Shader *vertexShader = new Shader("shaders/pathTraceVert.vert", GL_VERTEX_SHADER);
    Shader *fragmentShader = new Shader("shaders/pathTraceFrag.frag", GL_FRAGMENT_SHADER);

    m_shaderProgram->attachShader(vertexShader);
    m_shaderProgram->attachShader(fragmentShader);
    m_shaderProgram->bindFragDataLocation(0, "fragColour");
    m_shaderProgram->link();
    m_shaderProgram->use();

    delete vertexShader;
    delete fragmentShader;

    m_modelViewProjectionLoc = m_shaderProgram->getUniformLoc("MVP");
    m_texLoc = m_shaderProgram->getUniformLoc("pathTraceTex");
    glUniform1i(m_texLoc,0);

    m_cam = new Camera(glm::vec3(0.0, 0.0, -20.0));

    //start our render time out
    m_timeOutStart = m_timeOutStart.currentTime();

    //create our HUD
    m_textDrawer = new Text(QFont("Arial",14));
    m_textDrawer->setColour(255,0,0);
    m_textDrawer->setScreenSize(width(),height());

    //start our FPS counter
    m_FPSTimer = QTime::currentTime();

    startTimer(0);

}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::resizeGL(const int _w, const int _h){
    //some error handling cannot have an aspect ratio of 0/infinity
    if(_w==0||_h==0)return;
    // set the viewport for openGL
    glViewport(0,0,_w,_h);
    m_renderer->resize(_w,_h);
    m_cam->setShape(width(), height());
    m_textDrawer->setScreenSize(width(),height());
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::timerEvent(QTimerEvent *){
    updateGL();
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shaderProgram->use();
    bool timedOut;
    QTime currentTime = m_timeOutStart.currentTime();
    int secsPassed = m_timeOutStart.secsTo(currentTime);
    //if we haven't timed out then render another frame with our path tracer
    if(secsPassed<m_timedOut||m_timedOut==0){
        timedOut = false;
        if(m_render)
        {
            m_renderer->trace();
        }
    }
    else
    {
        timedOut = true;
    }
    GLuint vboId = m_renderer->getOutputBuffer()->getGLBOId();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, m_texID);
    // send pbo to texture
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vboId);

    RTsize elementSize = m_renderer->getOutputBuffer()->getElementSize();
    if      ((elementSize % 8) == 0) glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
    else if ((elementSize % 4) == 0) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    else if ((elementSize % 2) == 0) glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
    else                             glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, m_renderer->getWidth(), m_renderer->getHeight(), 0, GL_RGBA, GL_FLOAT, 0);

    loadMatricesToShader(glm::mat4(1.0), m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
    glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);


    QTime updateTime = QTime::currentTime();
    if(m_drawHud)
    {
        //calc the update time
        int msecsto = m_FPSTimer.msecsTo(updateTime);
        QString FPS;
        (msecsto==0)?FPS = "FPS: Too fast to calculate" : FPS = QString("FPS: %1").arg(1000.f/(float)msecsto);
        int textIndent  = (width()-height())/2;
        if(textIndent<0) textIndent = 0;
        if(timedOut)
        {
            m_textDrawer->renderText(textIndent,5,QString("Render Timed Out"));
            m_textDrawer->renderText(textIndent,20, FPS);
        }
        else
        {
            m_textDrawer->renderText(textIndent,5,QString("Rendering"));
            m_textDrawer->renderText(textIndent,20, FPS);
        }
    }

    //restart ouf FPS timere
    m_FPSTimer = QTime::currentTime();

}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::resizeGL(QResizeEvent *_event)
{
    //some error handling cannot have an aspect ratio of 0/infinity
    if(_event->size().width()==0||_event->size().height()==0)return;
    int _w = _event->size().width();
    int _h = _event->size().height();
    // set the viewport for openGL
    glViewport(0,0,_w,_h);
    m_renderer->resize(_w,_h);
    m_cam->setShape(width(), height());
    m_textDrawer->setScreenSize(width(),height());
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::loadMatricesToShader(glm::mat4 _modelMatrix, glm::mat4 _viewMatrix, glm::mat4 _perspectiveMatrix){
    GLuint MVPLoc = m_shaderProgram->getUniformLoc("MVP");

    glm::mat4 MVP = _perspectiveMatrix;

    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVP));
}
//------------------------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::mouseMoveEvent (QMouseEvent *_event){
  if(m_rotate && _event->buttons() == Qt::LeftButton){
    float diffx=_event->x()-m_origX;
    float diffy=_event->y()-m_origY;
    m_spinXFace -= (float) 0.01f * diffy;
    m_spinYFace += (float) 0.01f * diffx;
    glm::mat4 rotx,roty,finalRot;
    rotx = glm::rotate(rotx, m_spinXFace,glm::vec3(1.0,0.0,0.0));
    roty = glm::rotate(roty, m_spinYFace,glm::vec3(0.0,1.0,0.0));
    finalRot = rotx*roty;
    m_mouseGlobalTX = finalRot;
    m_mouseGlobalTX[3][0] = m_modelPos.x;
    m_mouseGlobalTX[3][1] = m_modelPos.y;
    m_mouseGlobalTX[3][2] = m_modelPos.z;
    // identity matrix to init our transformation
    float m[16];
    m[ 0] = m_mouseGlobalTX[0][0];  m[ 1] = m_mouseGlobalTX[1][0];  m[ 2] = m_mouseGlobalTX[2][0];  m[ 3] = m_mouseGlobalTX[3][0];
    m[ 4] = m_mouseGlobalTX[0][1];  m[ 5] = m_mouseGlobalTX[1][1];  m[ 6] = m_mouseGlobalTX[2][1];  m[ 7] = m_mouseGlobalTX[3][1];
    m[ 8] = m_mouseGlobalTX[0][2];  m[ 9] = m_mouseGlobalTX[1][2];  m[ 10] = m_mouseGlobalTX[2][2];  m[ 11] = m_mouseGlobalTX[3][2];
    m[ 12] = m_mouseGlobalTX[0][3];  m[ 13] = m_mouseGlobalTX[1][3];  m[ 14] = m_mouseGlobalTX[2][3];  m[ 15] = m_mouseGlobalTX[3][3];
    // create our inverse transform
    float invM[16];
    glm::mat4 inv = glm::inverse(m_mouseGlobalTX);
    invM[ 0] = inv[0][0];  invM[ 1] = inv[1][0];  invM[ 2] = inv[2][0];  invM[ 3] = inv[3][0];
    invM[ 4] = inv[0][1];  invM[ 5] = inv[1][1];  invM[ 6] = inv[2][1];  invM[ 7] = inv[3][1];
    invM[ 8] = inv[0][2];  invM[ 9] = inv[1][2];  invM[ 10] = inv[2][2];  invM[ 11] = inv[3][2];
    invM[ 12] = inv[0][3];  invM[ 13] = inv[1][3];  invM[ 14] = inv[2][3];  invM[ 15] = inv[3][3];

    m_renderer->setTransform(m,invM,false);
    m_render = true;

    m_origX = _event->x();
    m_origY = _event->y();
    // our scene has cahnged so reset our timeout
  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton){
    float diffX = (_event->x() - m_origXPos) * INCREMENT;
    float diffY = (_event->y() - m_origYPos) * INCREMENT;
    diffX*=-1.0;
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.x += INCREMENT * diffX;
    m_modelPos.y -= INCREMENT * diffY;
    m_mouseGlobalTX[3][0] = m_modelPos.x;
    m_mouseGlobalTX[3][1] = m_modelPos.y;
    // identity matrix to init our transformation
    float m[16];
    m[ 0] = m_mouseGlobalTX[0][0];  m[ 1] = m_mouseGlobalTX[1][0];  m[ 2] = m_mouseGlobalTX[2][0];  m[ 3] = m_mouseGlobalTX[3][0];
    m[ 4] = m_mouseGlobalTX[0][1];  m[ 5] = m_mouseGlobalTX[1][1];  m[ 6] = m_mouseGlobalTX[2][1];  m[ 7] = m_mouseGlobalTX[3][1];
    m[ 8] = m_mouseGlobalTX[0][2];  m[ 9] = m_mouseGlobalTX[1][2];  m[ 10] = m_mouseGlobalTX[2][2];  m[ 11] = m_mouseGlobalTX[3][2];
    m[ 12] = m_mouseGlobalTX[0][3];  m[ 13] = m_mouseGlobalTX[1][3];  m[ 14] = m_mouseGlobalTX[2][3];  m[ 15] = m_mouseGlobalTX[3][3];
    // create our inverse transform
    float invM[16];
    glm::mat4 inv = glm::inverse(m_mouseGlobalTX);
    invM[ 0] = inv[0][0];  invM[ 1] = inv[1][0];  invM[ 2] = inv[2][0];  invM[ 3] = inv[3][0];
    invM[ 4] = inv[0][1];  invM[ 5] = inv[1][1];  invM[ 6] = inv[2][1];  invM[ 7] = inv[3][1];
    invM[ 8] = inv[0][2];  invM[ 9] = inv[1][2];  invM[ 10] = inv[2][2];  invM[ 11] = inv[3][2];
    invM[ 12] = inv[0][3];  invM[ 13] = inv[1][3];  invM[ 14] = inv[2][3];  invM[ 15] = inv[3][3];

    m_renderer->setTransform(m,invM,false);
    m_render = true;

   }
  else if(m_translateEnvironment && _event->buttons() == Qt::MiddleButton){
      float diffx=_event->x()-m_origX;
      float diffy=_event->y()-m_origY;
      m_spinYFaceEnvironment += (float) 0.01f * diffx;
      glm::mat4 rotx,roty,finalRot;
      rotx = glm::rotate(rotx, m_spinXFaceEnvironment,glm::vec3(1.0,0.0,0.0));
      roty = glm::rotate(roty, m_spinYFaceEnvironment,glm::vec3(0.0,1.0,0.0));
      finalRot = rotx*roty;
      m_mouseGlobalTXEnvironment = finalRot;
      m_spinXFace -= (float) 0.01f * diffy;
      m_spinYFace += (float) 0.01f * diffx;
      rotx = glm::mat4(1.0);
      roty = glm::mat4(1.0);
      rotx = glm::rotate(rotx, m_spinXFaceEnvironment,glm::vec3(1.0,0.0,0.0));
      roty = glm::rotate(roty, m_spinYFaceEnvironment,glm::vec3(0.0,1.0,0.0));
      finalRot = rotx*roty;
      m_mouseGlobalTX = finalRot;
      m_mouseGlobalTX[3][0] = m_modelPos.x;
      m_mouseGlobalTX[3][1] = m_modelPos.y;
      m_mouseGlobalTX[3][2] = m_modelPos.z;
      m_origX = _event->x();
      m_origY = _event->y();
      m_render = true;

  }
}
//------------------------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::mousePressEvent ( QMouseEvent * _event){
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate = true;
    // resize our pathtracer for more responsive movement controls
    // resize amount set in general settings widget
    m_renderer->resize(width()/m_moveRenderReduction,height()/m_moveRenderReduction);

  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate = true;
    m_renderer->resize(width()/m_moveRenderReduction,height()/m_moveRenderReduction);
  }
  // right mouse translate mode
  else if(_event->button() == Qt::MiddleButton)
  {
      m_origX = _event->x();
      m_origY = _event->y();
      m_translateEnvironment = true;
  }

}
//------------------------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::mouseReleaseEvent ( QMouseEvent * _event ){
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
    m_renderer->resize(width()*devicePixelRatio(),height()*devicePixelRatio());

  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
    m_renderer->resize(width()*devicePixelRatio(),height()*devicePixelRatio());

  }
  else if(_event->button() == Qt::MiddleButton)
  {
     m_translateEnvironment = false;
  }
}
//------------------------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::wheelEvent(QWheelEvent *_event)
{

    // identity matrix to init our transformation
    float m[16];
    m[ 0] = m_mouseGlobalTX[0][0];  m[ 1] = m_mouseGlobalTX[1][0];  m[ 2] = m_mouseGlobalTX[2][0];  m[ 3] = m_mouseGlobalTX[3][0];
    m[ 4] = m_mouseGlobalTX[0][1];  m[ 5] = m_mouseGlobalTX[1][1];  m[ 6] = m_mouseGlobalTX[2][1];  m[ 7] = m_mouseGlobalTX[3][1];
    m[ 8] = m_mouseGlobalTX[0][2];  m[ 9] = m_mouseGlobalTX[1][2];  m[ 10] = m_mouseGlobalTX[2][2];  m[ 11] = m_mouseGlobalTX[3][2];
    m[ 12] = m_mouseGlobalTX[0][3];  m[ 13] = m_mouseGlobalTX[1][3];  m[ 14] = m_mouseGlobalTX[2][3];  m[ 15] = m_mouseGlobalTX[3][3];
    // create our inverse transform
    float invM[16];
    glm::mat4 inv = glm::inverse(m_mouseGlobalTX);
    invM[ 0] = inv[0][0];  invM[ 1] = inv[1][0];  invM[ 2] = inv[2][0];  invM[ 3] = inv[3][0];
    invM[ 4] = inv[0][1];  invM[ 5] = inv[1][1];  invM[ 6] = inv[2][1];  invM[ 7] = inv[3][1];
    invM[ 8] = inv[0][2];  invM[ 9] = inv[1][2];  invM[ 10] = inv[2][2];  invM[ 11] = inv[3][2];
    invM[ 12] = inv[0][3];  invM[ 13] = inv[1][3];  invM[ 14] = inv[2][3];  invM[ 15] = inv[3][3];
    if(_event->delta() > 0)
    {
        m_modelPos.z-=ZOOM;
        m_mouseGlobalTX[3][2]-=ZOOM;
        m_renderer->setTransform(m,invM,false);
    }
    else if(_event->delta() <0 )
    {
        m_modelPos.z+=ZOOM;
        m_mouseGlobalTX[3][2]+=ZOOM;
        m_renderer->setTransform(m,invM,false);
    }
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::keyPressEvent(QKeyEvent *_event)
{
    switch(_event->key())
    {
    case Qt::Key_Escape:
        QGuiApplication::exit();
    break;
    case Qt::Key_S:
        saveImage();
    break;
    case Qt::Key_Space:
        toggleRender();
    break;
    default:
    break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::keyReleaseEvent(QKeyEvent *_event)
{
    switch(_event->key())
    {
        default:
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::saveImage()
{
    m_render = false;
    QImage img(m_renderer->getWidth(),m_renderer->getHeight(),QImage::Format_RGB32);
    QColor color;
    // as we're using a openGL buffer rather than optix we must map it with openGL calls
    GLint vboId = m_renderer->getOutputBuffer()->getGLBOId();
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    typedef struct { float r; float g; float b; float a;} rgb;
    rgb* rgb_data = (rgb*)data;

    int x;
    int y;
    int h = m_renderer->getWidth()*m_renderer->getHeight();
    for(int i=0; i<h; i++)
    {
        float red = rgb_data[h-i].r; if(red>1.0) red=1.0;
        float green = rgb_data[h-i].g; if(green>1.0) green=1.0;
        float blue = rgb_data[h-i].b; if(blue>1.0) blue=1.0;
        float alpha = rgb_data[h-i].a; if(alpha>1.0) alpha=1.0;
        color.setRgbF(red,green,blue,alpha);
        y = floor((float)i/m_renderer->getWidth());
        x = m_renderer->getWidth() - i + y*m_renderer->getWidth() - 1;
        img.setPixel(x, y, color.rgb());

    }
    //remember to unmap the buffer after or you're going to have a bad time!
    glUnmapBuffer(GL_ARRAY_BUFFER);

    QFileDialog fileDialog(this);
    fileDialog.setDefaultSuffix(".png");
    QString selectedType;
    QString saveFile = fileDialog.getSaveFileName(this, tr("Save Image File"), "./images/",tr("PNG (*.png);;JPEG (*.jpeg);;BMP (*.bmp);;GIF (*.gif)"),&selectedType);
    std::cout<<"Selected name filter is "<<selectedType.toStdString()<<std::endl;
    QString format;
    if(selectedType== "PNG (*.png)")
    {
        format = "PNG";
    }
    else if(selectedType== "JPEG (*.jpeg)")
    {
        format = "JPEG";
    }
    else if(selectedType== "BMP (*.bmp)")
    {
        format = "BMP";
    }
    else if(selectedType== "GIF (*.gif)")
    {
        format = "GIF";
    }
    img.save(saveFile, format.toStdString().c_str());
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::resetGlobalTrans(){
    m_mouseGlobalTX = glm::mat4(1.0);
    float m[16];
    m[ 0] = 1.0f;  m[ 1] = 0.0f;  m[ 2] = 0.0f;  m[ 3] = 0.0f;
    m[ 4] = 0.0f;  m[ 5] = 1.0f;  m[ 6] = 0.0f;  m[ 7] = 0.0f;
    m[ 8] = 0.0f;  m[ 9] = 0.0f;  m[10] = 1.0f;  m[11] = 0.0f;
    m[12] = 0.0f;  m[13] = 0.0f;  m[14] = 0.0f;  m[15] = 1.0f;
    m_renderer->setTransform(m, m, false);
    m_render = true;

}
