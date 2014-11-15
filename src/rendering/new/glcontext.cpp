#include "glcontext.hpp"
#include "../opengl.hpp"
#include <fea/assert.hpp>

GLContext::GLContext()
{
    auto glStatus = ogl_LoadFunctions();
    FEA_ASSERT(glStatus != ogl_LOAD_FAILED, "Could not initialize gl! Make sure there is a valid OpenGL context!");
}
