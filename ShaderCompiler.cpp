//
//  ShaderUtils.cpp
//  ThreeDeeKit
//
//  Created by Matthew Whiteside on 5/25/13.
//  Copyright (c) 2013 General Covariance. All rights reserved.
//

#include "ShaderCompiler.h"


ShaderCompiler::ShaderCompiler(const string& basePath) : baseShaderPath(basePath){};


bool ShaderCompiler::link(GLuint *program){
  GLint status;
  glLinkProgram(*program);
  
#if defined(DEBUG)
  GLint logLength;
  glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    GLchar *log = (GLchar *)malloc(logLength);
    glGetProgramInfoLog(*program, logLength, &logLength, log);
    printf("%s",log);
    free(log);
  }
#endif
  
  glGetProgramiv(*program, GL_LINK_STATUS, &status);
  if (status == 0) {
    return false;
  }
  
  return true;
}

bool ShaderCompiler::validate(GLuint *program){
  GLint logLength, status;
  
  glValidateProgram(*program);
  glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    GLchar *log = (GLchar *)malloc(logLength);
    glGetProgramInfoLog(*program, logLength, &logLength, log);
    printf("validate log = %s",log);
    free(log);
  }
  
  glGetProgramiv(*program, GL_VALIDATE_STATUS, &status);
  if (status == 0) {
    return false;
  }
  
  return true;
  
}

bool ShaderCompiler::compileStage(GLuint *shader, GLenum stage){
  GLint status;
  
  *shader = glCreateShader(stage);
  string ext;
  if (stage == GL_VERTEX_SHADER)
    ext = ".vsh";
  else if (stage == GL_GEOMETRY_SHADER)
    ext = ".gsh";
  else
    ext = ".fsh";
  const string fullpath = baseShaderPath + ext;
  ifstream file;
  file.open(fullpath.c_str());
  std::stringstream buffer;
  buffer << file.rdbuf();
  const string source = buffer.str();
  const GLchar* _source = source.c_str();
  glShaderSource(*shader, 1, &_source, NULL);
  glCompileShader(*shader);
#if defined(DEBUG)
  GLint logLength;
  glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    GLchar *log = (GLchar *)malloc(logLength);
    glGetShaderInfoLog(*shader, logLength, &logLength, log);
    //std::cout << "Shader compile log:\n" << log << std::endl;
    printf("compile log %s\n",log);
    free(log);
  }
#endif
  
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
  if (status == 0) {
    glDeleteShader(*shader);
    return false;
  }
  
  return true;
  
}

bool ShaderCompiler::compile(GLuint *program, bool validate){
  GLuint  vertShader, geomShader, fragShader;
  
  GLuint vertexArrayObject;
#if TARGET_OS_IPHONE
  glGenVertexArraysOES(1, &vertexArrayObject);
  glBindVertexArrayOES(vertexArrayObject);
#else
  glGenVertexArrays(1, &vertexArrayObject);
  glBindVertexArray(vertexArrayObject);
#endif
  
  
  
  
  
  if (*program == 0) {
    cout << "Shader program is invalid." << endl;
    return false;
  }
  
  if (!compileStage(&vertShader, GL_VERTEX_SHADER)) {
    cout << "failed to compile vertex shader!" << endl;
    return false;
  }
#ifdef GL_GEOMETRY_SHADER
  if (!compileStage(&geomShader, GL_GEOMETRY_SHADER)) {
    cout << "Failed to compile geometry shader!" << endl;
    return false;
  }
#endif
  
  if (!compileStage(&fragShader, GL_FRAGMENT_SHADER)) {
    cout << "Failed to compile fragment shader!" << endl;
    return false;
  }
  
  
  glAttachShader(*program, vertShader);
#ifdef GL_GEOMETRY_SHADER
  
  glAttachShader(*program, geomShader);
#endif
  glAttachShader(*program, fragShader);
  
  
  // Link program.
  if (!link(program)) {
    
    
    if (vertShader) {
      glDeleteShader(vertShader);
      vertShader = 0;
    }
#ifdef GL_GEOMETRY_SHADER
    if (geomShader) {
      glDeleteShader(geomShader);
      geomShader = 0;
    }
#endif
    if (fragShader) {
      glDeleteShader(fragShader);
      fragShader = 0;
    }
    cout << "failed to link program!" << program << endl;
    return false;
  }
  
  if (validate && !ShaderCompiler::validate(program)){
    cout << "Failed to validate program" << endl;
    return false;
  }
  
  
  // Release vertex and fragment shaders.
  if (vertShader) {
    glDetachShader(*program, vertShader);
    glDeleteShader(vertShader);
  }
#ifdef GL_GEOMETRY_SHADER
  if (geomShader){
    glDetachShader(*program, geomShader);
    glDeleteShader(geomShader);
  }
#endif
  if (fragShader) {
    glDetachShader(*program, fragShader);
    glDeleteShader(fragShader);
  }
  
#if TARGET_OS_IPHONE
  glBindVertexArrayOES(0);
#else
  glBindVertexArray(0);
#endif
  
  
  return true;
}


const string ShaderCompiler::getName(){
  string ret;
  size_t index = this->baseShaderPath.rfind("/");
  if (index != string::npos) {
    ret = baseShaderPath.substr(index + 1, baseShaderPath.size()-1);
  }
  return ret;
}



