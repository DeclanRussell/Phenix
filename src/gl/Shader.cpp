#include "gl/Shader.h"
#include "gl/ShaderUtils.h"

Shader::Shader(std::string _path, GLenum _type){
   m_shaderID = shaderUtils::createShaderFromFile(_path.c_str(), _type);
}

Shader::~Shader(){
   glDeleteShader(m_shaderID);
}

GLuint Shader::getShaderID(){
   return m_shaderID;
}

