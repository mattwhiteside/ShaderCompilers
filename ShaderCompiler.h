//THIS FILE SHOULD BE KEPT TO THE C++ 98 standard

//
//  ShaderUtils.h
//  ThreeDeeKit
//
//  Created by Matthew Whiteside on 5/25/13.
//  Copyright (c) 2013 General Covariance. All rights reserved.
//

#ifndef __ThreeDeeKit__ShaderUtils__
#define __ThreeDeeKit__ShaderUtils__

#include <iostream>
#include <OpenGL/gl3.h>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>


using namespace std;

class ShaderCompiler {
private:

  bool link(GLuint *program);
  bool validate(GLuint *program);
  bool compileStage(GLuint *program, GLenum stage);

protected:
  const string baseShaderPath;
    
public:
  ShaderCompiler(const string& basePath);
  bool compile(GLuint *program, bool validate = true);
  const string getName();
};



#endif /* defined(__ThreeDeeKit__ShaderUtils__) */

