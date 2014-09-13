#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "gl_core_3_2.h"
	
#if defined(__APPLE__)
#include <mach-o/dyld.h>

static void* AppleGLGetProcAddress (const GLubyte *name)
{
  static const struct mach_header* image = NULL;
  NSSymbol symbol;
  char* symbolName;
  if (NULL == image)
  {
    image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR);
  }
  /* prepend a '_' for the Unix C symbol mangling convention */
  symbolName = malloc(strlen((const char*)name) + 2);
  strcpy(symbolName+1, (const char*)name);
  symbolName[0] = '_';
  symbol = NULL;
  /* if (NSIsSymbolNameDefined(symbolName))
	 symbol = NSLookupAndBindSymbol(symbolName); */
  symbol = image ? NSLookupSymbolInImage(image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) : NULL;
  free(symbolName);
  return symbol ? NSAddressOfSymbol(symbol) : NULL;
}
#endif /* __APPLE__ */

#if defined(__sgi) || defined (__sun)
#include <dlfcn.h>
#include <stdio.h>

static void* SunGetProcAddress (const GLubyte* name)
{
  static void* h = NULL;
  static void* gpa;

  if (h == NULL)
  {
    if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL) return NULL;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != NULL)
    return ((void*(*)(const GLubyte*))gpa)(name);
  else
    return dlsym(h, (const char*)name);
}
#endif /* __sgi || __sun */

#if defined(_WIN32)

#ifdef _MSC_VER
#pragma warning(disable: 4055)
#pragma warning(disable: 4054)
#endif

static int TestPointer(const PROC pTest)
{
	ptrdiff_t iTest;
	if(!pTest) return 0;
	iTest = (ptrdiff_t)pTest;
	
	if(iTest == 1 || iTest == 2 || iTest == 3 || iTest == -1) return 0;
	
	return 1;
}

static PROC WinGetProcAddress(const char *name)
{
	HMODULE glMod = NULL;
	PROC pFunc = wglGetProcAddress((LPCSTR)name);
	if(TestPointer(pFunc))
	{
		return pFunc;
	}
	glMod = GetModuleHandleA("OpenGL32.dll");
	return (PROC)GetProcAddress(glMod, (LPCSTR)name);
}

#define IntGetProcAddress(name) WinGetProcAddress(name)
#else
	#if defined(__APPLE__)
		#define IntGetProcAddress(name) AppleGLGetProcAddress(name)
    #else
		#if defined(__sgi) || defined(__sun)
			#define IntGetProcAddress(name) SunGetProcAddress(name)
		#else /* GLX */
		    #include <GL/glx.h>

			#define IntGetProcAddress(name) (*glXGetProcAddressARB)((const GLubyte*)name)
		#endif
	#endif
#endif

void (CODEGEN_FUNCPTR *blocks_ptrc_glBlendFunc)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glClear)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glClearDepth)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glClearStencil)(GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCullFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDepthFunc)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDepthMask)(GLboolean) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDepthRange)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDisable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glEnable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFinish)() = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFlush)() = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFrontFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetBooleanv)(GLenum, GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetDoublev)(GLenum, GLdouble *) = NULL;
GLenum (CODEGEN_FUNCPTR *blocks_ptrc_glGetError)() = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetFloatv)(GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetIntegerv)(GLenum, GLint *) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *blocks_ptrc_glGetString)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetTexImage)(GLenum, GLint, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetTexLevelParameterfv)(GLenum, GLint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetTexLevelParameteriv)(GLenum, GLint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetTexParameterfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetTexParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glHint)(GLenum, GLenum) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsEnabled)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glLineWidth)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glLogicOp)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPixelStoref)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPixelStorei)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPointSize)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPolygonMode)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glReadBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glScissor)(GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glStencilFunc)(GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glStencilMask)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glStencilOp)(GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexImage1D)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexParameterf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexParameterfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexParameteri)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexParameteriv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glViewport)(GLint, GLint, GLsizei, GLsizei) = NULL;

void (CODEGEN_FUNCPTR *blocks_ptrc_glBindTexture)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCopyTexImage1D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCopyTexImage2D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCopyTexSubImage1D)(GLenum, GLint, GLint, GLint, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCopyTexSubImage2D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDeleteTextures)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawArrays)(GLenum, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawElements)(GLenum, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGenTextures)(GLsizei, GLuint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsTexture)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPolygonOffset)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;

void (CODEGEN_FUNCPTR *blocks_ptrc_glBlendColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBlendEquation)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCopyTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawRangeElements)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexImage3D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;

void (CODEGEN_FUNCPTR *blocks_ptrc_glActiveTexture)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCompressedTexImage1D)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCompressedTexImage2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCompressedTexImage3D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCompressedTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCompressedTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCompressedTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetCompressedTexImage)(GLenum, GLint, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glSampleCoverage)(GLfloat, GLboolean) = NULL;

void (CODEGEN_FUNCPTR *blocks_ptrc_glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glMultiDrawArrays)(GLenum, const GLint *, const GLsizei *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glMultiDrawElements)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPointParameterf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPointParameterfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPointParameteri)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPointParameteriv)(GLenum, const GLint *) = NULL;

void (CODEGEN_FUNCPTR *blocks_ptrc_glBeginQuery)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBindBuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDeleteBuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDeleteQueries)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glEndQuery)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGenBuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGenQueries)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetBufferParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetBufferPointerv)(GLenum, GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetBufferSubData)(GLenum, GLintptr, GLsizeiptr, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetQueryObjectiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetQueryObjectuiv)(GLuint, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetQueryiv)(GLenum, GLenum, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsBuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsQuery)(GLuint) = NULL;
void * (CODEGEN_FUNCPTR *blocks_ptrc_glMapBuffer)(GLenum, GLenum) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glUnmapBuffer)(GLenum) = NULL;

void (CODEGEN_FUNCPTR *blocks_ptrc_glAttachShader)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBindAttribLocation)(GLuint, GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBlendEquationSeparate)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glCompileShader)(GLuint) = NULL;
GLuint (CODEGEN_FUNCPTR *blocks_ptrc_glCreateProgram)() = NULL;
GLuint (CODEGEN_FUNCPTR *blocks_ptrc_glCreateShader)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDeleteProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDeleteShader)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDetachShader)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDisableVertexAttribArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawBuffers)(GLsizei, const GLenum *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glEnableVertexAttribArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetActiveAttrib)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetActiveUniform)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetAttachedShaders)(GLuint, GLsizei, GLsizei *, GLuint *) = NULL;
GLint (CODEGEN_FUNCPTR *blocks_ptrc_glGetAttribLocation)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetProgramiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetShaderSource)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetShaderiv)(GLuint, GLenum, GLint *) = NULL;
GLint (CODEGEN_FUNCPTR *blocks_ptrc_glGetUniformLocation)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetUniformfv)(GLuint, GLint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetUniformiv)(GLuint, GLint, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetVertexAttribPointerv)(GLuint, GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetVertexAttribdv)(GLuint, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetVertexAttribfv)(GLuint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetVertexAttribiv)(GLuint, GLenum, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsProgram)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsShader)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glLinkProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glShaderSource)(GLuint, GLsizei, const GLchar *const*, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glStencilFuncSeparate)(GLenum, GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glStencilMaskSeparate)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glStencilOpSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform1f)(GLint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform1fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform1i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform1iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform2f)(GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform2fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform2i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform2iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform3f)(GLint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform3fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform3i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform3iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform4fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform4i)(GLint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform4iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformMatrix2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformMatrix3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUseProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glValidateProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib1d)(GLuint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib1dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib1f)(GLuint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib1fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib1s)(GLuint, GLshort) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib1sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib2d)(GLuint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib2dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib2f)(GLuint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib2fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib2s)(GLuint, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib2sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib3d)(GLuint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib3dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib3f)(GLuint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib3fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib3s)(GLuint, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib3sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4Nbv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4Niv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4Nsv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4Nub)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4Nubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4Nuiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4Nusv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4bv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4d)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4f)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4s)(GLuint, GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4ubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttrib4usv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *) = NULL;

void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformMatrix2x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformMatrix2x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformMatrix3x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformMatrix3x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformMatrix4x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformMatrix4x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;

void (CODEGEN_FUNCPTR *blocks_ptrc_glBeginConditionalRender)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBeginTransformFeedback)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBindBufferBase)(GLenum, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBindBufferRange)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBindFragDataLocation)(GLuint, GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBindFramebuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBindRenderbuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBindVertexArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glBlitFramebuffer)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum) = NULL;
GLenum (CODEGEN_FUNCPTR *blocks_ptrc_glCheckFramebufferStatus)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glClampColor)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glClearBufferfi)(GLenum, GLint, GLfloat, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glClearBufferfv)(GLenum, GLint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glClearBufferiv)(GLenum, GLint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glClearBufferuiv)(GLenum, GLint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glColorMaski)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDeleteFramebuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDeleteRenderbuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDeleteVertexArrays)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDisablei)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glEnablei)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glEndConditionalRender)() = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glEndTransformFeedback)() = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFlushMappedBufferRange)(GLenum, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFramebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFramebufferTexture1D)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFramebufferTexture3D)(GLenum, GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFramebufferTextureLayer)(GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGenFramebuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGenRenderbuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGenVertexArrays)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGenerateMipmap)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetBooleani_v)(GLenum, GLuint, GLboolean *) = NULL;
GLint (CODEGEN_FUNCPTR *blocks_ptrc_glGetFragDataLocation)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetFramebufferAttachmentParameteriv)(GLenum, GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetIntegeri_v)(GLenum, GLuint, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetRenderbufferParameteriv)(GLenum, GLenum, GLint *) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *blocks_ptrc_glGetStringi)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetTexParameterIiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetTexParameterIuiv)(GLenum, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetTransformFeedbackVarying)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetUniformuiv)(GLuint, GLint, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetVertexAttribIiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetVertexAttribIuiv)(GLuint, GLenum, GLuint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsEnabledi)(GLenum, GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsFramebuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsRenderbuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsVertexArray)(GLuint) = NULL;
void * (CODEGEN_FUNCPTR *blocks_ptrc_glMapBufferRange)(GLenum, GLintptr, GLsizeiptr, GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glRenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glRenderbufferStorageMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexParameterIiv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexParameterIuiv)(GLenum, GLenum, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTransformFeedbackVaryings)(GLuint, GLsizei, const GLchar *const*, GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform1ui)(GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform1uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform2ui)(GLint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform2uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform3ui)(GLint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform3uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform4ui)(GLint, GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniform4uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI1i)(GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI1iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI1ui)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI1uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI2i)(GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI2iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI2ui)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI2uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI3i)(GLuint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI3iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI3ui)(GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI3uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI4bv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI4i)(GLuint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI4iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI4sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI4ubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI4ui)(GLuint, GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI4uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribI4usv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid *) = NULL;

void (CODEGEN_FUNCPTR *blocks_ptrc_glCopyBufferSubData)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawArraysInstanced)(GLenum, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawElementsInstanced)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetActiveUniformBlockName)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetActiveUniformBlockiv)(GLuint, GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetActiveUniformName)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetActiveUniformsiv)(GLuint, GLsizei, const GLuint *, GLenum, GLint *) = NULL;
GLuint (CODEGEN_FUNCPTR *blocks_ptrc_glGetUniformBlockIndex)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetUniformIndices)(GLuint, GLsizei, const GLchar *const*, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glPrimitiveRestartIndex)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexBuffer)(GLenum, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glUniformBlockBinding)(GLuint, GLuint, GLuint) = NULL;

GLenum (CODEGEN_FUNCPTR *blocks_ptrc_glClientWaitSync)(GLsync, GLbitfield, GLuint64) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDeleteSync)(GLsync) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawElementsBaseVertex)(GLenum, GLsizei, GLenum, const GLvoid *, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawElementsInstancedBaseVertex)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glDrawRangeElementsBaseVertex)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *, GLint) = NULL;
GLsync (CODEGEN_FUNCPTR *blocks_ptrc_glFenceSync)(GLenum, GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glFramebufferTexture)(GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetBufferParameteri64v)(GLenum, GLenum, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetInteger64i_v)(GLenum, GLuint, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetInteger64v)(GLenum, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetMultisamplefv)(GLenum, GLuint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glGetSynciv)(GLsync, GLenum, GLsizei, GLsizei *, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *blocks_ptrc_glIsSync)(GLsync) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glMultiDrawElementsBaseVertex)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glProvokingVertex)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glSampleMaski)(GLuint, GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexImage2DMultisample)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glTexImage3DMultisample)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLsizei, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *blocks_ptrc_glWaitSync)(GLsync, GLbitfield, GLuint64) = NULL;

static int Load_Version_3_2()
{
	int numFailed = 0;
	blocks_ptrc_glBlendFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendFunc");
	if(!blocks_ptrc_glBlendFunc) numFailed++;
	blocks_ptrc_glClear = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glClear");
	if(!blocks_ptrc_glClear) numFailed++;
	blocks_ptrc_glClearColor = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glClearColor");
	if(!blocks_ptrc_glClearColor) numFailed++;
	blocks_ptrc_glClearDepth = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glClearDepth");
	if(!blocks_ptrc_glClearDepth) numFailed++;
	blocks_ptrc_glClearStencil = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glClearStencil");
	if(!blocks_ptrc_glClearStencil) numFailed++;
	blocks_ptrc_glColorMask = (void (CODEGEN_FUNCPTR *)(GLboolean, GLboolean, GLboolean, GLboolean))IntGetProcAddress("glColorMask");
	if(!blocks_ptrc_glColorMask) numFailed++;
	blocks_ptrc_glCullFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCullFace");
	if(!blocks_ptrc_glCullFace) numFailed++;
	blocks_ptrc_glDepthFunc = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDepthFunc");
	if(!blocks_ptrc_glDepthFunc) numFailed++;
	blocks_ptrc_glDepthMask = (void (CODEGEN_FUNCPTR *)(GLboolean))IntGetProcAddress("glDepthMask");
	if(!blocks_ptrc_glDepthMask) numFailed++;
	blocks_ptrc_glDepthRange = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glDepthRange");
	if(!blocks_ptrc_glDepthRange) numFailed++;
	blocks_ptrc_glDisable = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDisable");
	if(!blocks_ptrc_glDisable) numFailed++;
	blocks_ptrc_glDrawBuffer = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDrawBuffer");
	if(!blocks_ptrc_glDrawBuffer) numFailed++;
	blocks_ptrc_glEnable = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEnable");
	if(!blocks_ptrc_glEnable) numFailed++;
	blocks_ptrc_glFinish = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glFinish");
	if(!blocks_ptrc_glFinish) numFailed++;
	blocks_ptrc_glFlush = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glFlush");
	if(!blocks_ptrc_glFlush) numFailed++;
	blocks_ptrc_glFrontFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glFrontFace");
	if(!blocks_ptrc_glFrontFace) numFailed++;
	blocks_ptrc_glGetBooleanv = (void (CODEGEN_FUNCPTR *)(GLenum, GLboolean *))IntGetProcAddress("glGetBooleanv");
	if(!blocks_ptrc_glGetBooleanv) numFailed++;
	blocks_ptrc_glGetDoublev = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble *))IntGetProcAddress("glGetDoublev");
	if(!blocks_ptrc_glGetDoublev) numFailed++;
	blocks_ptrc_glGetError = (GLenum (CODEGEN_FUNCPTR *)())IntGetProcAddress("glGetError");
	if(!blocks_ptrc_glGetError) numFailed++;
	blocks_ptrc_glGetFloatv = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat *))IntGetProcAddress("glGetFloatv");
	if(!blocks_ptrc_glGetFloatv) numFailed++;
	blocks_ptrc_glGetIntegerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint *))IntGetProcAddress("glGetIntegerv");
	if(!blocks_ptrc_glGetIntegerv) numFailed++;
	blocks_ptrc_glGetString = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGetString");
	if(!blocks_ptrc_glGetString) numFailed++;
	blocks_ptrc_glGetTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLenum, GLvoid *))IntGetProcAddress("glGetTexImage");
	if(!blocks_ptrc_glGetTexImage) numFailed++;
	blocks_ptrc_glGetTexLevelParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLfloat *))IntGetProcAddress("glGetTexLevelParameterfv");
	if(!blocks_ptrc_glGetTexLevelParameterfv) numFailed++;
	blocks_ptrc_glGetTexLevelParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint *))IntGetProcAddress("glGetTexLevelParameteriv");
	if(!blocks_ptrc_glGetTexLevelParameteriv) numFailed++;
	blocks_ptrc_glGetTexParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetTexParameterfv");
	if(!blocks_ptrc_glGetTexParameterfv) numFailed++;
	blocks_ptrc_glGetTexParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexParameteriv");
	if(!blocks_ptrc_glGetTexParameteriv) numFailed++;
	blocks_ptrc_glHint = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glHint");
	if(!blocks_ptrc_glHint) numFailed++;
	blocks_ptrc_glIsEnabled = (GLboolean (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glIsEnabled");
	if(!blocks_ptrc_glIsEnabled) numFailed++;
	blocks_ptrc_glLineWidth = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glLineWidth");
	if(!blocks_ptrc_glLineWidth) numFailed++;
	blocks_ptrc_glLogicOp = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glLogicOp");
	if(!blocks_ptrc_glLogicOp) numFailed++;
	blocks_ptrc_glPixelStoref = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPixelStoref");
	if(!blocks_ptrc_glPixelStoref) numFailed++;
	blocks_ptrc_glPixelStorei = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPixelStorei");
	if(!blocks_ptrc_glPixelStorei) numFailed++;
	blocks_ptrc_glPointSize = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glPointSize");
	if(!blocks_ptrc_glPointSize) numFailed++;
	blocks_ptrc_glPolygonMode = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glPolygonMode");
	if(!blocks_ptrc_glPolygonMode) numFailed++;
	blocks_ptrc_glReadBuffer = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glReadBuffer");
	if(!blocks_ptrc_glReadBuffer) numFailed++;
	blocks_ptrc_glReadPixels = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *))IntGetProcAddress("glReadPixels");
	if(!blocks_ptrc_glReadPixels) numFailed++;
	blocks_ptrc_glScissor = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glScissor");
	if(!blocks_ptrc_glScissor) numFailed++;
	blocks_ptrc_glStencilFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLuint))IntGetProcAddress("glStencilFunc");
	if(!blocks_ptrc_glStencilFunc) numFailed++;
	blocks_ptrc_glStencilMask = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glStencilMask");
	if(!blocks_ptrc_glStencilMask) numFailed++;
	blocks_ptrc_glStencilOp = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum))IntGetProcAddress("glStencilOp");
	if(!blocks_ptrc_glStencilOp) numFailed++;
	blocks_ptrc_glTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexImage1D");
	if(!blocks_ptrc_glTexImage1D) numFailed++;
	blocks_ptrc_glTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexImage2D");
	if(!blocks_ptrc_glTexImage2D) numFailed++;
	blocks_ptrc_glTexParameterf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glTexParameterf");
	if(!blocks_ptrc_glTexParameterf) numFailed++;
	blocks_ptrc_glTexParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glTexParameterfv");
	if(!blocks_ptrc_glTexParameterfv) numFailed++;
	blocks_ptrc_glTexParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glTexParameteri");
	if(!blocks_ptrc_glTexParameteri) numFailed++;
	blocks_ptrc_glTexParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexParameteriv");
	if(!blocks_ptrc_glTexParameteriv) numFailed++;
	blocks_ptrc_glViewport = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glViewport");
	if(!blocks_ptrc_glViewport) numFailed++;
	blocks_ptrc_glBindTexture = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindTexture");
	if(!blocks_ptrc_glBindTexture) numFailed++;
	blocks_ptrc_glCopyTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint))IntGetProcAddress("glCopyTexImage1D");
	if(!blocks_ptrc_glCopyTexImage1D) numFailed++;
	blocks_ptrc_glCopyTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))IntGetProcAddress("glCopyTexImage2D");
	if(!blocks_ptrc_glCopyTexImage2D) numFailed++;
	blocks_ptrc_glCopyTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei))IntGetProcAddress("glCopyTexSubImage1D");
	if(!blocks_ptrc_glCopyTexSubImage1D) numFailed++;
	blocks_ptrc_glCopyTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTexSubImage2D");
	if(!blocks_ptrc_glCopyTexSubImage2D) numFailed++;
	blocks_ptrc_glDeleteTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteTextures");
	if(!blocks_ptrc_glDeleteTextures) numFailed++;
	blocks_ptrc_glDrawArrays = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei))IntGetProcAddress("glDrawArrays");
	if(!blocks_ptrc_glDrawArrays) numFailed++;
	blocks_ptrc_glDrawElements = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *))IntGetProcAddress("glDrawElements");
	if(!blocks_ptrc_glDrawElements) numFailed++;
	blocks_ptrc_glGenTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenTextures");
	if(!blocks_ptrc_glGenTextures) numFailed++;
	blocks_ptrc_glIsTexture = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsTexture");
	if(!blocks_ptrc_glIsTexture) numFailed++;
	blocks_ptrc_glPolygonOffset = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glPolygonOffset");
	if(!blocks_ptrc_glPolygonOffset) numFailed++;
	blocks_ptrc_glTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage1D");
	if(!blocks_ptrc_glTexSubImage1D) numFailed++;
	blocks_ptrc_glTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage2D");
	if(!blocks_ptrc_glTexSubImage2D) numFailed++;
	blocks_ptrc_glBlendColor = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glBlendColor");
	if(!blocks_ptrc_glBlendColor) numFailed++;
	blocks_ptrc_glBlendEquation = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBlendEquation");
	if(!blocks_ptrc_glBlendEquation) numFailed++;
	blocks_ptrc_glCopyTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTexSubImage3D");
	if(!blocks_ptrc_glCopyTexSubImage3D) numFailed++;
	blocks_ptrc_glDrawRangeElements = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *))IntGetProcAddress("glDrawRangeElements");
	if(!blocks_ptrc_glDrawRangeElements) numFailed++;
	blocks_ptrc_glTexImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexImage3D");
	if(!blocks_ptrc_glTexImage3D) numFailed++;
	blocks_ptrc_glTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage3D");
	if(!blocks_ptrc_glTexSubImage3D) numFailed++;
	blocks_ptrc_glActiveTexture = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glActiveTexture");
	if(!blocks_ptrc_glActiveTexture) numFailed++;
	blocks_ptrc_glCompressedTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexImage1D");
	if(!blocks_ptrc_glCompressedTexImage1D) numFailed++;
	blocks_ptrc_glCompressedTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexImage2D");
	if(!blocks_ptrc_glCompressedTexImage2D) numFailed++;
	blocks_ptrc_glCompressedTexImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexImage3D");
	if(!blocks_ptrc_glCompressedTexImage3D) numFailed++;
	blocks_ptrc_glCompressedTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexSubImage1D");
	if(!blocks_ptrc_glCompressedTexSubImage1D) numFailed++;
	blocks_ptrc_glCompressedTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexSubImage2D");
	if(!blocks_ptrc_glCompressedTexSubImage2D) numFailed++;
	blocks_ptrc_glCompressedTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexSubImage3D");
	if(!blocks_ptrc_glCompressedTexSubImage3D) numFailed++;
	blocks_ptrc_glGetCompressedTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLvoid *))IntGetProcAddress("glGetCompressedTexImage");
	if(!blocks_ptrc_glGetCompressedTexImage) numFailed++;
	blocks_ptrc_glSampleCoverage = (void (CODEGEN_FUNCPTR *)(GLfloat, GLboolean))IntGetProcAddress("glSampleCoverage");
	if(!blocks_ptrc_glSampleCoverage) numFailed++;
	blocks_ptrc_glBlendFuncSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glBlendFuncSeparate");
	if(!blocks_ptrc_glBlendFuncSeparate) numFailed++;
	blocks_ptrc_glMultiDrawArrays = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *, const GLsizei *, GLsizei))IntGetProcAddress("glMultiDrawArrays");
	if(!blocks_ptrc_glMultiDrawArrays) numFailed++;
	blocks_ptrc_glMultiDrawElements = (void (CODEGEN_FUNCPTR *)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei))IntGetProcAddress("glMultiDrawElements");
	if(!blocks_ptrc_glMultiDrawElements) numFailed++;
	blocks_ptrc_glPointParameterf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPointParameterf");
	if(!blocks_ptrc_glPointParameterf) numFailed++;
	blocks_ptrc_glPointParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glPointParameterfv");
	if(!blocks_ptrc_glPointParameterfv) numFailed++;
	blocks_ptrc_glPointParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPointParameteri");
	if(!blocks_ptrc_glPointParameteri) numFailed++;
	blocks_ptrc_glPointParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glPointParameteriv");
	if(!blocks_ptrc_glPointParameteriv) numFailed++;
	blocks_ptrc_glBeginQuery = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBeginQuery");
	if(!blocks_ptrc_glBeginQuery) numFailed++;
	blocks_ptrc_glBindBuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindBuffer");
	if(!blocks_ptrc_glBindBuffer) numFailed++;
	blocks_ptrc_glBufferData = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizeiptr, const GLvoid *, GLenum))IntGetProcAddress("glBufferData");
	if(!blocks_ptrc_glBufferData) numFailed++;
	blocks_ptrc_glBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, const GLvoid *))IntGetProcAddress("glBufferSubData");
	if(!blocks_ptrc_glBufferSubData) numFailed++;
	blocks_ptrc_glDeleteBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteBuffers");
	if(!blocks_ptrc_glDeleteBuffers) numFailed++;
	blocks_ptrc_glDeleteQueries = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteQueries");
	if(!blocks_ptrc_glDeleteQueries) numFailed++;
	blocks_ptrc_glEndQuery = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEndQuery");
	if(!blocks_ptrc_glEndQuery) numFailed++;
	blocks_ptrc_glGenBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenBuffers");
	if(!blocks_ptrc_glGenBuffers) numFailed++;
	blocks_ptrc_glGenQueries = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenQueries");
	if(!blocks_ptrc_glGenQueries) numFailed++;
	blocks_ptrc_glGetBufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetBufferParameteriv");
	if(!blocks_ptrc_glGetBufferParameteriv) numFailed++;
	blocks_ptrc_glGetBufferPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLvoid **))IntGetProcAddress("glGetBufferPointerv");
	if(!blocks_ptrc_glGetBufferPointerv) numFailed++;
	blocks_ptrc_glGetBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, GLvoid *))IntGetProcAddress("glGetBufferSubData");
	if(!blocks_ptrc_glGetBufferSubData) numFailed++;
	blocks_ptrc_glGetQueryObjectiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetQueryObjectiv");
	if(!blocks_ptrc_glGetQueryObjectiv) numFailed++;
	blocks_ptrc_glGetQueryObjectuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetQueryObjectuiv");
	if(!blocks_ptrc_glGetQueryObjectuiv) numFailed++;
	blocks_ptrc_glGetQueryiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetQueryiv");
	if(!blocks_ptrc_glGetQueryiv) numFailed++;
	blocks_ptrc_glIsBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsBuffer");
	if(!blocks_ptrc_glIsBuffer) numFailed++;
	blocks_ptrc_glIsQuery = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsQuery");
	if(!blocks_ptrc_glIsQuery) numFailed++;
	blocks_ptrc_glMapBuffer = (void * (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glMapBuffer");
	if(!blocks_ptrc_glMapBuffer) numFailed++;
	blocks_ptrc_glUnmapBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glUnmapBuffer");
	if(!blocks_ptrc_glUnmapBuffer) numFailed++;
	blocks_ptrc_glAttachShader = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glAttachShader");
	if(!blocks_ptrc_glAttachShader) numFailed++;
	blocks_ptrc_glBindAttribLocation = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, const GLchar *))IntGetProcAddress("glBindAttribLocation");
	if(!blocks_ptrc_glBindAttribLocation) numFailed++;
	blocks_ptrc_glBlendEquationSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendEquationSeparate");
	if(!blocks_ptrc_glBlendEquationSeparate) numFailed++;
	blocks_ptrc_glCompileShader = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glCompileShader");
	if(!blocks_ptrc_glCompileShader) numFailed++;
	blocks_ptrc_glCreateProgram = (GLuint (CODEGEN_FUNCPTR *)())IntGetProcAddress("glCreateProgram");
	if(!blocks_ptrc_glCreateProgram) numFailed++;
	blocks_ptrc_glCreateShader = (GLuint (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCreateShader");
	if(!blocks_ptrc_glCreateShader) numFailed++;
	blocks_ptrc_glDeleteProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDeleteProgram");
	if(!blocks_ptrc_glDeleteProgram) numFailed++;
	blocks_ptrc_glDeleteShader = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDeleteShader");
	if(!blocks_ptrc_glDeleteShader) numFailed++;
	blocks_ptrc_glDetachShader = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glDetachShader");
	if(!blocks_ptrc_glDetachShader) numFailed++;
	blocks_ptrc_glDisableVertexAttribArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDisableVertexAttribArray");
	if(!blocks_ptrc_glDisableVertexAttribArray) numFailed++;
	blocks_ptrc_glDrawBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLenum *))IntGetProcAddress("glDrawBuffers");
	if(!blocks_ptrc_glDrawBuffers) numFailed++;
	blocks_ptrc_glEnableVertexAttribArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glEnableVertexAttribArray");
	if(!blocks_ptrc_glEnableVertexAttribArray) numFailed++;
	blocks_ptrc_glGetActiveAttrib = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))IntGetProcAddress("glGetActiveAttrib");
	if(!blocks_ptrc_glGetActiveAttrib) numFailed++;
	blocks_ptrc_glGetActiveUniform = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))IntGetProcAddress("glGetActiveUniform");
	if(!blocks_ptrc_glGetActiveUniform) numFailed++;
	blocks_ptrc_glGetAttachedShaders = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLuint *))IntGetProcAddress("glGetAttachedShaders");
	if(!blocks_ptrc_glGetAttachedShaders) numFailed++;
	blocks_ptrc_glGetAttribLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetAttribLocation");
	if(!blocks_ptrc_glGetAttribLocation) numFailed++;
	blocks_ptrc_glGetProgramInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramInfoLog");
	if(!blocks_ptrc_glGetProgramInfoLog) numFailed++;
	blocks_ptrc_glGetProgramiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetProgramiv");
	if(!blocks_ptrc_glGetProgramiv) numFailed++;
	blocks_ptrc_glGetShaderInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetShaderInfoLog");
	if(!blocks_ptrc_glGetShaderInfoLog) numFailed++;
	blocks_ptrc_glGetShaderSource = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetShaderSource");
	if(!blocks_ptrc_glGetShaderSource) numFailed++;
	blocks_ptrc_glGetShaderiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetShaderiv");
	if(!blocks_ptrc_glGetShaderiv) numFailed++;
	blocks_ptrc_glGetUniformLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetUniformLocation");
	if(!blocks_ptrc_glGetUniformLocation) numFailed++;
	blocks_ptrc_glGetUniformfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat *))IntGetProcAddress("glGetUniformfv");
	if(!blocks_ptrc_glGetUniformfv) numFailed++;
	blocks_ptrc_glGetUniformiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint *))IntGetProcAddress("glGetUniformiv");
	if(!blocks_ptrc_glGetUniformiv) numFailed++;
	blocks_ptrc_glGetVertexAttribPointerv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLvoid **))IntGetProcAddress("glGetVertexAttribPointerv");
	if(!blocks_ptrc_glGetVertexAttribPointerv) numFailed++;
	blocks_ptrc_glGetVertexAttribdv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLdouble *))IntGetProcAddress("glGetVertexAttribdv");
	if(!blocks_ptrc_glGetVertexAttribdv) numFailed++;
	blocks_ptrc_glGetVertexAttribfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat *))IntGetProcAddress("glGetVertexAttribfv");
	if(!blocks_ptrc_glGetVertexAttribfv) numFailed++;
	blocks_ptrc_glGetVertexAttribiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexAttribiv");
	if(!blocks_ptrc_glGetVertexAttribiv) numFailed++;
	blocks_ptrc_glIsProgram = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsProgram");
	if(!blocks_ptrc_glIsProgram) numFailed++;
	blocks_ptrc_glIsShader = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsShader");
	if(!blocks_ptrc_glIsShader) numFailed++;
	blocks_ptrc_glLinkProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glLinkProgram");
	if(!blocks_ptrc_glLinkProgram) numFailed++;
	blocks_ptrc_glShaderSource = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, const GLint *))IntGetProcAddress("glShaderSource");
	if(!blocks_ptrc_glShaderSource) numFailed++;
	blocks_ptrc_glStencilFuncSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint, GLuint))IntGetProcAddress("glStencilFuncSeparate");
	if(!blocks_ptrc_glStencilFuncSeparate) numFailed++;
	blocks_ptrc_glStencilMaskSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glStencilMaskSeparate");
	if(!blocks_ptrc_glStencilMaskSeparate) numFailed++;
	blocks_ptrc_glStencilOpSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glStencilOpSeparate");
	if(!blocks_ptrc_glStencilOpSeparate) numFailed++;
	blocks_ptrc_glUniform1f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat))IntGetProcAddress("glUniform1f");
	if(!blocks_ptrc_glUniform1f) numFailed++;
	blocks_ptrc_glUniform1fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform1fv");
	if(!blocks_ptrc_glUniform1fv) numFailed++;
	blocks_ptrc_glUniform1i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glUniform1i");
	if(!blocks_ptrc_glUniform1i) numFailed++;
	blocks_ptrc_glUniform1iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform1iv");
	if(!blocks_ptrc_glUniform1iv) numFailed++;
	blocks_ptrc_glUniform2f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat))IntGetProcAddress("glUniform2f");
	if(!blocks_ptrc_glUniform2f) numFailed++;
	blocks_ptrc_glUniform2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform2fv");
	if(!blocks_ptrc_glUniform2fv) numFailed++;
	blocks_ptrc_glUniform2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glUniform2i");
	if(!blocks_ptrc_glUniform2i) numFailed++;
	blocks_ptrc_glUniform2iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform2iv");
	if(!blocks_ptrc_glUniform2iv) numFailed++;
	blocks_ptrc_glUniform3f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform3f");
	if(!blocks_ptrc_glUniform3f) numFailed++;
	blocks_ptrc_glUniform3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform3fv");
	if(!blocks_ptrc_glUniform3fv) numFailed++;
	blocks_ptrc_glUniform3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform3i");
	if(!blocks_ptrc_glUniform3i) numFailed++;
	blocks_ptrc_glUniform3iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform3iv");
	if(!blocks_ptrc_glUniform3iv) numFailed++;
	blocks_ptrc_glUniform4f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform4f");
	if(!blocks_ptrc_glUniform4f) numFailed++;
	blocks_ptrc_glUniform4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform4fv");
	if(!blocks_ptrc_glUniform4fv) numFailed++;
	blocks_ptrc_glUniform4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform4i");
	if(!blocks_ptrc_glUniform4i) numFailed++;
	blocks_ptrc_glUniform4iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform4iv");
	if(!blocks_ptrc_glUniform4iv) numFailed++;
	blocks_ptrc_glUniformMatrix2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2fv");
	if(!blocks_ptrc_glUniformMatrix2fv) numFailed++;
	blocks_ptrc_glUniformMatrix3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3fv");
	if(!blocks_ptrc_glUniformMatrix3fv) numFailed++;
	blocks_ptrc_glUniformMatrix4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4fv");
	if(!blocks_ptrc_glUniformMatrix4fv) numFailed++;
	blocks_ptrc_glUseProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glUseProgram");
	if(!blocks_ptrc_glUseProgram) numFailed++;
	blocks_ptrc_glValidateProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glValidateProgram");
	if(!blocks_ptrc_glValidateProgram) numFailed++;
	blocks_ptrc_glVertexAttrib1d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble))IntGetProcAddress("glVertexAttrib1d");
	if(!blocks_ptrc_glVertexAttrib1d) numFailed++;
	blocks_ptrc_glVertexAttrib1dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib1dv");
	if(!blocks_ptrc_glVertexAttrib1dv) numFailed++;
	blocks_ptrc_glVertexAttrib1f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat))IntGetProcAddress("glVertexAttrib1f");
	if(!blocks_ptrc_glVertexAttrib1f) numFailed++;
	blocks_ptrc_glVertexAttrib1fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib1fv");
	if(!blocks_ptrc_glVertexAttrib1fv) numFailed++;
	blocks_ptrc_glVertexAttrib1s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort))IntGetProcAddress("glVertexAttrib1s");
	if(!blocks_ptrc_glVertexAttrib1s) numFailed++;
	blocks_ptrc_glVertexAttrib1sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib1sv");
	if(!blocks_ptrc_glVertexAttrib1sv) numFailed++;
	blocks_ptrc_glVertexAttrib2d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib2d");
	if(!blocks_ptrc_glVertexAttrib2d) numFailed++;
	blocks_ptrc_glVertexAttrib2dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib2dv");
	if(!blocks_ptrc_glVertexAttrib2dv) numFailed++;
	blocks_ptrc_glVertexAttrib2f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib2f");
	if(!blocks_ptrc_glVertexAttrib2f) numFailed++;
	blocks_ptrc_glVertexAttrib2fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib2fv");
	if(!blocks_ptrc_glVertexAttrib2fv) numFailed++;
	blocks_ptrc_glVertexAttrib2s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort))IntGetProcAddress("glVertexAttrib2s");
	if(!blocks_ptrc_glVertexAttrib2s) numFailed++;
	blocks_ptrc_glVertexAttrib2sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib2sv");
	if(!blocks_ptrc_glVertexAttrib2sv) numFailed++;
	blocks_ptrc_glVertexAttrib3d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib3d");
	if(!blocks_ptrc_glVertexAttrib3d) numFailed++;
	blocks_ptrc_glVertexAttrib3dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib3dv");
	if(!blocks_ptrc_glVertexAttrib3dv) numFailed++;
	blocks_ptrc_glVertexAttrib3f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib3f");
	if(!blocks_ptrc_glVertexAttrib3f) numFailed++;
	blocks_ptrc_glVertexAttrib3fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib3fv");
	if(!blocks_ptrc_glVertexAttrib3fv) numFailed++;
	blocks_ptrc_glVertexAttrib3s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort, GLshort))IntGetProcAddress("glVertexAttrib3s");
	if(!blocks_ptrc_glVertexAttrib3s) numFailed++;
	blocks_ptrc_glVertexAttrib3sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib3sv");
	if(!blocks_ptrc_glVertexAttrib3sv) numFailed++;
	blocks_ptrc_glVertexAttrib4Nbv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttrib4Nbv");
	if(!blocks_ptrc_glVertexAttrib4Nbv) numFailed++;
	blocks_ptrc_glVertexAttrib4Niv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttrib4Niv");
	if(!blocks_ptrc_glVertexAttrib4Niv) numFailed++;
	blocks_ptrc_glVertexAttrib4Nsv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib4Nsv");
	if(!blocks_ptrc_glVertexAttrib4Nsv) numFailed++;
	blocks_ptrc_glVertexAttrib4Nub = (void (CODEGEN_FUNCPTR *)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte))IntGetProcAddress("glVertexAttrib4Nub");
	if(!blocks_ptrc_glVertexAttrib4Nub) numFailed++;
	blocks_ptrc_glVertexAttrib4Nubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttrib4Nubv");
	if(!blocks_ptrc_glVertexAttrib4Nubv) numFailed++;
	blocks_ptrc_glVertexAttrib4Nuiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttrib4Nuiv");
	if(!blocks_ptrc_glVertexAttrib4Nuiv) numFailed++;
	blocks_ptrc_glVertexAttrib4Nusv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttrib4Nusv");
	if(!blocks_ptrc_glVertexAttrib4Nusv) numFailed++;
	blocks_ptrc_glVertexAttrib4bv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttrib4bv");
	if(!blocks_ptrc_glVertexAttrib4bv) numFailed++;
	blocks_ptrc_glVertexAttrib4d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib4d");
	if(!blocks_ptrc_glVertexAttrib4d) numFailed++;
	blocks_ptrc_glVertexAttrib4dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib4dv");
	if(!blocks_ptrc_glVertexAttrib4dv) numFailed++;
	blocks_ptrc_glVertexAttrib4f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib4f");
	if(!blocks_ptrc_glVertexAttrib4f) numFailed++;
	blocks_ptrc_glVertexAttrib4fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib4fv");
	if(!blocks_ptrc_glVertexAttrib4fv) numFailed++;
	blocks_ptrc_glVertexAttrib4iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttrib4iv");
	if(!blocks_ptrc_glVertexAttrib4iv) numFailed++;
	blocks_ptrc_glVertexAttrib4s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glVertexAttrib4s");
	if(!blocks_ptrc_glVertexAttrib4s) numFailed++;
	blocks_ptrc_glVertexAttrib4sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib4sv");
	if(!blocks_ptrc_glVertexAttrib4sv) numFailed++;
	blocks_ptrc_glVertexAttrib4ubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttrib4ubv");
	if(!blocks_ptrc_glVertexAttrib4ubv) numFailed++;
	blocks_ptrc_glVertexAttrib4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttrib4uiv");
	if(!blocks_ptrc_glVertexAttrib4uiv) numFailed++;
	blocks_ptrc_glVertexAttrib4usv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttrib4usv");
	if(!blocks_ptrc_glVertexAttrib4usv) numFailed++;
	blocks_ptrc_glVertexAttribPointer = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *))IntGetProcAddress("glVertexAttribPointer");
	if(!blocks_ptrc_glVertexAttribPointer) numFailed++;
	blocks_ptrc_glUniformMatrix2x3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2x3fv");
	if(!blocks_ptrc_glUniformMatrix2x3fv) numFailed++;
	blocks_ptrc_glUniformMatrix2x4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2x4fv");
	if(!blocks_ptrc_glUniformMatrix2x4fv) numFailed++;
	blocks_ptrc_glUniformMatrix3x2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3x2fv");
	if(!blocks_ptrc_glUniformMatrix3x2fv) numFailed++;
	blocks_ptrc_glUniformMatrix3x4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3x4fv");
	if(!blocks_ptrc_glUniformMatrix3x4fv) numFailed++;
	blocks_ptrc_glUniformMatrix4x2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4x2fv");
	if(!blocks_ptrc_glUniformMatrix4x2fv) numFailed++;
	blocks_ptrc_glUniformMatrix4x3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4x3fv");
	if(!blocks_ptrc_glUniformMatrix4x3fv) numFailed++;
	blocks_ptrc_glBeginConditionalRender = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glBeginConditionalRender");
	if(!blocks_ptrc_glBeginConditionalRender) numFailed++;
	blocks_ptrc_glBeginTransformFeedback = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBeginTransformFeedback");
	if(!blocks_ptrc_glBeginTransformFeedback) numFailed++;
	blocks_ptrc_glBindBufferBase = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint))IntGetProcAddress("glBindBufferBase");
	if(!blocks_ptrc_glBindBufferBase) numFailed++;
	blocks_ptrc_glBindBufferRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glBindBufferRange");
	if(!blocks_ptrc_glBindBufferRange) numFailed++;
	blocks_ptrc_glBindFragDataLocation = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, const GLchar *))IntGetProcAddress("glBindFragDataLocation");
	if(!blocks_ptrc_glBindFragDataLocation) numFailed++;
	blocks_ptrc_glBindFramebuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindFramebuffer");
	if(!blocks_ptrc_glBindFramebuffer) numFailed++;
	blocks_ptrc_glBindRenderbuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindRenderbuffer");
	if(!blocks_ptrc_glBindRenderbuffer) numFailed++;
	blocks_ptrc_glBindVertexArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glBindVertexArray");
	if(!blocks_ptrc_glBindVertexArray) numFailed++;
	blocks_ptrc_glBlitFramebuffer = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum))IntGetProcAddress("glBlitFramebuffer");
	if(!blocks_ptrc_glBlitFramebuffer) numFailed++;
	blocks_ptrc_glCheckFramebufferStatus = (GLenum (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCheckFramebufferStatus");
	if(!blocks_ptrc_glCheckFramebufferStatus) numFailed++;
	blocks_ptrc_glClampColor = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glClampColor");
	if(!blocks_ptrc_glClampColor) numFailed++;
	blocks_ptrc_glClearBufferfi = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLfloat, GLint))IntGetProcAddress("glClearBufferfi");
	if(!blocks_ptrc_glClearBufferfi) numFailed++;
	blocks_ptrc_glClearBufferfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, const GLfloat *))IntGetProcAddress("glClearBufferfv");
	if(!blocks_ptrc_glClearBufferfv) numFailed++;
	blocks_ptrc_glClearBufferiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, const GLint *))IntGetProcAddress("glClearBufferiv");
	if(!blocks_ptrc_glClearBufferiv) numFailed++;
	blocks_ptrc_glClearBufferuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, const GLuint *))IntGetProcAddress("glClearBufferuiv");
	if(!blocks_ptrc_glClearBufferuiv) numFailed++;
	blocks_ptrc_glColorMaski = (void (CODEGEN_FUNCPTR *)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean))IntGetProcAddress("glColorMaski");
	if(!blocks_ptrc_glColorMaski) numFailed++;
	blocks_ptrc_glDeleteFramebuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteFramebuffers");
	if(!blocks_ptrc_glDeleteFramebuffers) numFailed++;
	blocks_ptrc_glDeleteRenderbuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteRenderbuffers");
	if(!blocks_ptrc_glDeleteRenderbuffers) numFailed++;
	blocks_ptrc_glDeleteVertexArrays = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteVertexArrays");
	if(!blocks_ptrc_glDeleteVertexArrays) numFailed++;
	blocks_ptrc_glDisablei = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glDisablei");
	if(!blocks_ptrc_glDisablei) numFailed++;
	blocks_ptrc_glEnablei = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glEnablei");
	if(!blocks_ptrc_glEnablei) numFailed++;
	blocks_ptrc_glEndConditionalRender = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glEndConditionalRender");
	if(!blocks_ptrc_glEndConditionalRender) numFailed++;
	blocks_ptrc_glEndTransformFeedback = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glEndTransformFeedback");
	if(!blocks_ptrc_glEndTransformFeedback) numFailed++;
	blocks_ptrc_glFlushMappedBufferRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr))IntGetProcAddress("glFlushMappedBufferRange");
	if(!blocks_ptrc_glFlushMappedBufferRange) numFailed++;
	blocks_ptrc_glFramebufferRenderbuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint))IntGetProcAddress("glFramebufferRenderbuffer");
	if(!blocks_ptrc_glFramebufferRenderbuffer) numFailed++;
	blocks_ptrc_glFramebufferTexture1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture1D");
	if(!blocks_ptrc_glFramebufferTexture1D) numFailed++;
	blocks_ptrc_glFramebufferTexture2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture2D");
	if(!blocks_ptrc_glFramebufferTexture2D) numFailed++;
	blocks_ptrc_glFramebufferTexture3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glFramebufferTexture3D");
	if(!blocks_ptrc_glFramebufferTexture3D) numFailed++;
	blocks_ptrc_glFramebufferTextureLayer = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glFramebufferTextureLayer");
	if(!blocks_ptrc_glFramebufferTextureLayer) numFailed++;
	blocks_ptrc_glGenFramebuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenFramebuffers");
	if(!blocks_ptrc_glGenFramebuffers) numFailed++;
	blocks_ptrc_glGenRenderbuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenRenderbuffers");
	if(!blocks_ptrc_glGenRenderbuffers) numFailed++;
	blocks_ptrc_glGenVertexArrays = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenVertexArrays");
	if(!blocks_ptrc_glGenVertexArrays) numFailed++;
	blocks_ptrc_glGenerateMipmap = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGenerateMipmap");
	if(!blocks_ptrc_glGenerateMipmap) numFailed++;
	blocks_ptrc_glGetBooleani_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLboolean *))IntGetProcAddress("glGetBooleani_v");
	if(!blocks_ptrc_glGetBooleani_v) numFailed++;
	blocks_ptrc_glGetFragDataLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetFragDataLocation");
	if(!blocks_ptrc_glGetFragDataLocation) numFailed++;
	blocks_ptrc_glGetFramebufferAttachmentParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLint *))IntGetProcAddress("glGetFramebufferAttachmentParameteriv");
	if(!blocks_ptrc_glGetFramebufferAttachmentParameteriv) numFailed++;
	blocks_ptrc_glGetIntegeri_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLint *))IntGetProcAddress("glGetIntegeri_v");
	if(!blocks_ptrc_glGetIntegeri_v) numFailed++;
	blocks_ptrc_glGetRenderbufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetRenderbufferParameteriv");
	if(!blocks_ptrc_glGetRenderbufferParameteriv) numFailed++;
	blocks_ptrc_glGetStringi = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glGetStringi");
	if(!blocks_ptrc_glGetStringi) numFailed++;
	blocks_ptrc_glGetTexParameterIiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexParameterIiv");
	if(!blocks_ptrc_glGetTexParameterIiv) numFailed++;
	blocks_ptrc_glGetTexParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint *))IntGetProcAddress("glGetTexParameterIuiv");
	if(!blocks_ptrc_glGetTexParameterIuiv) numFailed++;
	blocks_ptrc_glGetTransformFeedbackVarying = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *))IntGetProcAddress("glGetTransformFeedbackVarying");
	if(!blocks_ptrc_glGetTransformFeedbackVarying) numFailed++;
	blocks_ptrc_glGetUniformuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint *))IntGetProcAddress("glGetUniformuiv");
	if(!blocks_ptrc_glGetUniformuiv) numFailed++;
	blocks_ptrc_glGetVertexAttribIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexAttribIiv");
	if(!blocks_ptrc_glGetVertexAttribIiv) numFailed++;
	blocks_ptrc_glGetVertexAttribIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetVertexAttribIuiv");
	if(!blocks_ptrc_glGetVertexAttribIuiv) numFailed++;
	blocks_ptrc_glIsEnabledi = (GLboolean (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glIsEnabledi");
	if(!blocks_ptrc_glIsEnabledi) numFailed++;
	blocks_ptrc_glIsFramebuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsFramebuffer");
	if(!blocks_ptrc_glIsFramebuffer) numFailed++;
	blocks_ptrc_glIsRenderbuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsRenderbuffer");
	if(!blocks_ptrc_glIsRenderbuffer) numFailed++;
	blocks_ptrc_glIsVertexArray = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsVertexArray");
	if(!blocks_ptrc_glIsVertexArray) numFailed++;
	blocks_ptrc_glMapBufferRange = (void * (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, GLbitfield))IntGetProcAddress("glMapBufferRange");
	if(!blocks_ptrc_glMapBufferRange) numFailed++;
	blocks_ptrc_glRenderbufferStorage = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLsizei, GLsizei))IntGetProcAddress("glRenderbufferStorage");
	if(!blocks_ptrc_glRenderbufferStorage) numFailed++;
	blocks_ptrc_glRenderbufferStorageMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glRenderbufferStorageMultisample");
	if(!blocks_ptrc_glRenderbufferStorageMultisample) numFailed++;
	blocks_ptrc_glTexParameterIiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexParameterIiv");
	if(!blocks_ptrc_glTexParameterIiv) numFailed++;
	blocks_ptrc_glTexParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLuint *))IntGetProcAddress("glTexParameterIuiv");
	if(!blocks_ptrc_glTexParameterIuiv) numFailed++;
	blocks_ptrc_glTransformFeedbackVaryings = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, GLenum))IntGetProcAddress("glTransformFeedbackVaryings");
	if(!blocks_ptrc_glTransformFeedbackVaryings) numFailed++;
	blocks_ptrc_glUniform1ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint))IntGetProcAddress("glUniform1ui");
	if(!blocks_ptrc_glUniform1ui) numFailed++;
	blocks_ptrc_glUniform1uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform1uiv");
	if(!blocks_ptrc_glUniform1uiv) numFailed++;
	blocks_ptrc_glUniform2ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint, GLuint))IntGetProcAddress("glUniform2ui");
	if(!blocks_ptrc_glUniform2ui) numFailed++;
	blocks_ptrc_glUniform2uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform2uiv");
	if(!blocks_ptrc_glUniform2uiv) numFailed++;
	blocks_ptrc_glUniform3ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint, GLuint, GLuint))IntGetProcAddress("glUniform3ui");
	if(!blocks_ptrc_glUniform3ui) numFailed++;
	blocks_ptrc_glUniform3uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform3uiv");
	if(!blocks_ptrc_glUniform3uiv) numFailed++;
	blocks_ptrc_glUniform4ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glUniform4ui");
	if(!blocks_ptrc_glUniform4ui) numFailed++;
	blocks_ptrc_glUniform4uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform4uiv");
	if(!blocks_ptrc_glUniform4uiv) numFailed++;
	blocks_ptrc_glVertexAttribI1i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint))IntGetProcAddress("glVertexAttribI1i");
	if(!blocks_ptrc_glVertexAttribI1i) numFailed++;
	blocks_ptrc_glVertexAttribI1iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI1iv");
	if(!blocks_ptrc_glVertexAttribI1iv) numFailed++;
	blocks_ptrc_glVertexAttribI1ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexAttribI1ui");
	if(!blocks_ptrc_glVertexAttribI1ui) numFailed++;
	blocks_ptrc_glVertexAttribI1uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI1uiv");
	if(!blocks_ptrc_glVertexAttribI1uiv) numFailed++;
	blocks_ptrc_glVertexAttribI2i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint))IntGetProcAddress("glVertexAttribI2i");
	if(!blocks_ptrc_glVertexAttribI2i) numFailed++;
	blocks_ptrc_glVertexAttribI2iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI2iv");
	if(!blocks_ptrc_glVertexAttribI2iv) numFailed++;
	blocks_ptrc_glVertexAttribI2ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glVertexAttribI2ui");
	if(!blocks_ptrc_glVertexAttribI2ui) numFailed++;
	blocks_ptrc_glVertexAttribI2uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI2uiv");
	if(!blocks_ptrc_glVertexAttribI2uiv) numFailed++;
	blocks_ptrc_glVertexAttribI3i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint))IntGetProcAddress("glVertexAttribI3i");
	if(!blocks_ptrc_glVertexAttribI3i) numFailed++;
	blocks_ptrc_glVertexAttribI3iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI3iv");
	if(!blocks_ptrc_glVertexAttribI3iv) numFailed++;
	blocks_ptrc_glVertexAttribI3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glVertexAttribI3ui");
	if(!blocks_ptrc_glVertexAttribI3ui) numFailed++;
	blocks_ptrc_glVertexAttribI3uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI3uiv");
	if(!blocks_ptrc_glVertexAttribI3uiv) numFailed++;
	blocks_ptrc_glVertexAttribI4bv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttribI4bv");
	if(!blocks_ptrc_glVertexAttribI4bv) numFailed++;
	blocks_ptrc_glVertexAttribI4i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint))IntGetProcAddress("glVertexAttribI4i");
	if(!blocks_ptrc_glVertexAttribI4i) numFailed++;
	blocks_ptrc_glVertexAttribI4iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI4iv");
	if(!blocks_ptrc_glVertexAttribI4iv) numFailed++;
	blocks_ptrc_glVertexAttribI4sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttribI4sv");
	if(!blocks_ptrc_glVertexAttribI4sv) numFailed++;
	blocks_ptrc_glVertexAttribI4ubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttribI4ubv");
	if(!blocks_ptrc_glVertexAttribI4ubv) numFailed++;
	blocks_ptrc_glVertexAttribI4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glVertexAttribI4ui");
	if(!blocks_ptrc_glVertexAttribI4ui) numFailed++;
	blocks_ptrc_glVertexAttribI4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI4uiv");
	if(!blocks_ptrc_glVertexAttribI4uiv) numFailed++;
	blocks_ptrc_glVertexAttribI4usv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttribI4usv");
	if(!blocks_ptrc_glVertexAttribI4usv) numFailed++;
	blocks_ptrc_glVertexAttribIPointer = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glVertexAttribIPointer");
	if(!blocks_ptrc_glVertexAttribIPointer) numFailed++;
	blocks_ptrc_glCopyBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr))IntGetProcAddress("glCopyBufferSubData");
	if(!blocks_ptrc_glCopyBufferSubData) numFailed++;
	blocks_ptrc_glDrawArraysInstanced = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei, GLsizei))IntGetProcAddress("glDrawArraysInstanced");
	if(!blocks_ptrc_glDrawArraysInstanced) numFailed++;
	blocks_ptrc_glDrawElementsInstanced = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei))IntGetProcAddress("glDrawElementsInstanced");
	if(!blocks_ptrc_glDrawElementsInstanced) numFailed++;
	blocks_ptrc_glGetActiveUniformBlockName = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveUniformBlockName");
	if(!blocks_ptrc_glGetActiveUniformBlockName) numFailed++;
	blocks_ptrc_glGetActiveUniformBlockiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLint *))IntGetProcAddress("glGetActiveUniformBlockiv");
	if(!blocks_ptrc_glGetActiveUniformBlockiv) numFailed++;
	blocks_ptrc_glGetActiveUniformName = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveUniformName");
	if(!blocks_ptrc_glGetActiveUniformName) numFailed++;
	blocks_ptrc_glGetActiveUniformsiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *, GLenum, GLint *))IntGetProcAddress("glGetActiveUniformsiv");
	if(!blocks_ptrc_glGetActiveUniformsiv) numFailed++;
	blocks_ptrc_glGetUniformBlockIndex = (GLuint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetUniformBlockIndex");
	if(!blocks_ptrc_glGetUniformBlockIndex) numFailed++;
	blocks_ptrc_glGetUniformIndices = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, GLuint *))IntGetProcAddress("glGetUniformIndices");
	if(!blocks_ptrc_glGetUniformIndices) numFailed++;
	blocks_ptrc_glPrimitiveRestartIndex = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glPrimitiveRestartIndex");
	if(!blocks_ptrc_glPrimitiveRestartIndex) numFailed++;
	blocks_ptrc_glTexBuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint))IntGetProcAddress("glTexBuffer");
	if(!blocks_ptrc_glTexBuffer) numFailed++;
	blocks_ptrc_glUniformBlockBinding = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glUniformBlockBinding");
	if(!blocks_ptrc_glUniformBlockBinding) numFailed++;
	blocks_ptrc_glClientWaitSync = (GLenum (CODEGEN_FUNCPTR *)(GLsync, GLbitfield, GLuint64))IntGetProcAddress("glClientWaitSync");
	if(!blocks_ptrc_glClientWaitSync) numFailed++;
	blocks_ptrc_glDeleteSync = (void (CODEGEN_FUNCPTR *)(GLsync))IntGetProcAddress("glDeleteSync");
	if(!blocks_ptrc_glDeleteSync) numFailed++;
	blocks_ptrc_glDrawElementsBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *, GLint))IntGetProcAddress("glDrawElementsBaseVertex");
	if(!blocks_ptrc_glDrawElementsBaseVertex) numFailed++;
	blocks_ptrc_glDrawElementsInstancedBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei, GLint))IntGetProcAddress("glDrawElementsInstancedBaseVertex");
	if(!blocks_ptrc_glDrawElementsInstancedBaseVertex) numFailed++;
	blocks_ptrc_glDrawRangeElementsBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *, GLint))IntGetProcAddress("glDrawRangeElementsBaseVertex");
	if(!blocks_ptrc_glDrawRangeElementsBaseVertex) numFailed++;
	blocks_ptrc_glFenceSync = (GLsync (CODEGEN_FUNCPTR *)(GLenum, GLbitfield))IntGetProcAddress("glFenceSync");
	if(!blocks_ptrc_glFenceSync) numFailed++;
	blocks_ptrc_glFramebufferTexture = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture");
	if(!blocks_ptrc_glFramebufferTexture) numFailed++;
	blocks_ptrc_glGetBufferParameteri64v = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint64 *))IntGetProcAddress("glGetBufferParameteri64v");
	if(!blocks_ptrc_glGetBufferParameteri64v) numFailed++;
	blocks_ptrc_glGetInteger64i_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLint64 *))IntGetProcAddress("glGetInteger64i_v");
	if(!blocks_ptrc_glGetInteger64i_v) numFailed++;
	blocks_ptrc_glGetInteger64v = (void (CODEGEN_FUNCPTR *)(GLenum, GLint64 *))IntGetProcAddress("glGetInteger64v");
	if(!blocks_ptrc_glGetInteger64v) numFailed++;
	blocks_ptrc_glGetMultisamplefv = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLfloat *))IntGetProcAddress("glGetMultisamplefv");
	if(!blocks_ptrc_glGetMultisamplefv) numFailed++;
	blocks_ptrc_glGetSynciv = (void (CODEGEN_FUNCPTR *)(GLsync, GLenum, GLsizei, GLsizei *, GLint *))IntGetProcAddress("glGetSynciv");
	if(!blocks_ptrc_glGetSynciv) numFailed++;
	blocks_ptrc_glIsSync = (GLboolean (CODEGEN_FUNCPTR *)(GLsync))IntGetProcAddress("glIsSync");
	if(!blocks_ptrc_glIsSync) numFailed++;
	blocks_ptrc_glMultiDrawElementsBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei, const GLint *))IntGetProcAddress("glMultiDrawElementsBaseVertex");
	if(!blocks_ptrc_glMultiDrawElementsBaseVertex) numFailed++;
	blocks_ptrc_glProvokingVertex = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glProvokingVertex");
	if(!blocks_ptrc_glProvokingVertex) numFailed++;
	blocks_ptrc_glSampleMaski = (void (CODEGEN_FUNCPTR *)(GLuint, GLbitfield))IntGetProcAddress("glSampleMaski");
	if(!blocks_ptrc_glSampleMaski) numFailed++;
	blocks_ptrc_glTexImage2DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexImage2DMultisample");
	if(!blocks_ptrc_glTexImage2DMultisample) numFailed++;
	blocks_ptrc_glTexImage3DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexImage3DMultisample");
	if(!blocks_ptrc_glTexImage3DMultisample) numFailed++;
	blocks_ptrc_glWaitSync = (void (CODEGEN_FUNCPTR *)(GLsync, GLbitfield, GLuint64))IntGetProcAddress("glWaitSync");
	if(!blocks_ptrc_glWaitSync) numFailed++;
	return numFailed;
}

typedef int (*PFN_LOADFUNCPOINTERS)();
typedef struct blocksogl_StrToExtMap_s
{
	char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} blocksogl_StrToExtMap;

static blocksogl_StrToExtMap ExtensionMap[1] = {
	{"", NULL, NULL},
};

static int g_extensionMapSize = 0;

static blocksogl_StrToExtMap *FindExtEntry(const char *extensionName)
{
	int loop;
	blocksogl_StrToExtMap *currLoc = ExtensionMap;
	for(loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
	{
		if(strcmp(extensionName, currLoc->extensionName) == 0)
			return currLoc;
	}
	
	return NULL;
}

static void ClearExtensionVars()
{
}


static void LoadExtByName(const char *extensionName)
{
	blocksogl_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if(entry)
	{
		if(entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if(numFailed == 0)
			{
				*(entry->extensionVariable) = blocksogl_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = blocksogl_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = blocksogl_LOAD_SUCCEEDED;
		}
	}
}


static void ProcExtsFromExtList()
{
	GLint iLoop;
	GLint iNumExtensions = 0;
	blocks_ptrc_glGetIntegerv(GL_NUM_EXTENSIONS, &iNumExtensions);

	for(iLoop = 0; iLoop < iNumExtensions; iLoop++)
	{
		const char *strExtensionName = (const char *)blocks_ptrc_glGetStringi(GL_EXTENSIONS, iLoop);
		LoadExtByName(strExtensionName);
	}
}

int blocksogl_LoadFunctions()
{
	int numFailed = 0;
	ClearExtensionVars();
	
	blocks_ptrc_glGetIntegerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint *))IntGetProcAddress("glGetIntegerv");
	if(!blocks_ptrc_glGetIntegerv) return blocksogl_LOAD_FAILED;
	blocks_ptrc_glGetStringi = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glGetStringi");
	if(!blocks_ptrc_glGetStringi) return blocksogl_LOAD_FAILED;
	
	ProcExtsFromExtList();
	numFailed = Load_Version_3_2();
	
	if(numFailed == 0)
		return blocksogl_LOAD_SUCCEEDED;
	else
		return blocksogl_LOAD_SUCCEEDED + numFailed;
}

static int g_major_version = 0;
static int g_minor_version = 0;

static void GetGLVersion()
{
	glGetIntegerv(GL_MAJOR_VERSION, &g_major_version);
	glGetIntegerv(GL_MINOR_VERSION, &g_minor_version);
}

int blocksogl_GetMajorVersion()
{
	if(g_major_version == 0)
		GetGLVersion();
	return g_major_version;
}

int blocksogl_GetMinorVersion()
{
	if(g_major_version == 0) //Yes, check the major version to get the minor one.
		GetGLVersion();
	return g_minor_version;
}

int blocksogl_IsVersionGEQ(int majorVersion, int minorVersion)
{
	if(g_major_version == 0)
		GetGLVersion();
		
	if(majorVersion > g_major_version) return 1;
	if(majorVersion < g_major_version) return 0;
	if(minorVersion >= g_minor_version) return 1;
	return 0;
}

