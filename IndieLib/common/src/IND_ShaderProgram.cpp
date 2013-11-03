/*********************************** The zlib License ************************************
 *
 * Copyright (c) 2013 Indielib-crossplatform Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 *****************************************************************************************/

#include "IND_ShaderProgram.h"

#include "Defines.h"
#include "Global.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

struct IND_ShaderProgramImpl {
    
    IND_ShaderProgramImpl() : _program(0), _vertexShader(0), _fragmentShader (0) {}
    
    bool compileShader(const char* codeBuffer, GLuint type, GLuint *handle) {
        if (!codeBuffer || !handle) return false;
        
        *handle = glCreateShader(type);
        glShaderSource(*handle, 1, &codeBuffer, NULL);
        glCompileShader(*handle);
        
        GLint status;
        glGetShaderiv(*handle, GL_COMPILE_STATUS, &status);
        
        return status;
    }
    
    string compileLog(GLuint handle) {
        GLint logLength = 0;
        string log;
        
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);
        
        if (logLength >= 1) {
            char *logBytes = (char*)malloc(logLength);
            glGetShaderInfoLog(handle, logLength, NULL, logBytes);
            log = string(logBytes);
            free(logBytes);
        }
        
        return log;
    }
    
    string shaderSource(GLuint handle) {
        GLsizei length;
        glGetShaderiv(handle, GL_SHADER_SOURCE_LENGTH, &length);
        GLchar src[length];
        glGetShaderSource(handle, length, NULL, src);
        return string(src);
    }
    
    GLuint _program;
    GLuint _vertexShader;
    GLuint _fragmentShader;
};


void IND_ShaderProgram::init() {
    _impl = new IND_ShaderProgramImpl();
}

void IND_ShaderProgram::end() {
    if (_impl->_program) {
        glDeleteProgram(_impl->_program);
    }
    
    DISPOSE(_impl);
}

bool IND_ShaderProgram::compile(const char* vertexShader, const char* fragmentShader) {
    if (!vertexShader || !fragmentShader) return false;
    
    bool success = false;
    
    g_debug->header("Compiling vertex shader", DebugApi::LogHeaderBegin);
    success = _impl->compileShader(vertexShader, GL_VERTEX_SHADER, &_impl->_vertexShader);
    if (!success) {
        g_debug->header("Error compiling vertex shader", DebugApi::LogHeaderError);
        g_debug->dataChar(_impl->shaderSource(_impl->_vertexShader), true);
        g_debug->dataChar(_impl->compileLog(_impl->_vertexShader), true);
    }
    g_debug->header("Compiling vertex shader finished", DebugApi::LogHeaderEnd);
    
    g_debug->header("Compiling fragment shader", DebugApi::LogHeaderBegin);
    success = success && _impl->compileShader(vertexShader, GL_FRAGMENT_SHADER, &_impl->_fragmentShader);
    if (!success) {
        g_debug->header("Error compiling fragment shader", DebugApi::LogHeaderError);
        g_debug->dataChar(_impl->shaderSource(_impl->_vertexShader), true);
        g_debug->dataChar(_impl->compileLog(_impl->_vertexShader), true);
    }
    g_debug->header("Compiling fragment shader finished", DebugApi::LogHeaderEnd);
    
    return success;
}

bool  IND_ShaderProgram::link() {
    if (!_impl->_vertexShader || !_impl->_fragmentShader ) return false;
    if (_impl->_program) return true;
    
    _impl->_program = glCreateProgram();
    if (!_impl->_program) return false;
    
    glAttachShader(_impl->_program, _impl->_vertexShader);
    glAttachShader(_impl->_program, _impl->_fragmentShader);
    
    glLinkProgram(_impl->_program);
    GLint status;
    glGetProgramiv(_impl->_program, GL_LINK_STATUS, &status);
    
    if (!status) {
        g_debug->header("Error linking shader program", DebugApi::LogHeaderError);
        g_debug->dataChar(_impl->compileLog(_impl->_program), true);
        return false;
    }
    
    return true;
}

void IND_ShaderProgram::use() {
    glUseProgram(_impl->_program);
}



