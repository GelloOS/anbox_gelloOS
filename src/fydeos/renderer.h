#ifndef ANBOX_FYDEOS_RENDERER_H_
#define ANBOX_FYDEOS_RENDERER_H_

#include <GL/gl.h>
#include <EGL/eglext.h>

namespace anbox{
namespace fydeos{

struct Buffer_Ext{
  EGLImageKHR image;
  GLuint texture;
  GLuint fbo;
};

class WaylandRenderer{
public:
  virtual ~WaylandRenderer(){}

  virtual Buffer_Ext* bind() = 0;
  virtual void unbind(Buffer_Ext *pExt) = 0;
};

class Renderer{
public:
  virtual ~Renderer(){}
  
  virtual void createFydeBuffer(Buffer_Ext *pExt, EGLint *pAttr) = 0;
  virtual void deleteFydeBuffer(Buffer_Ext *pExt) = 0;
  // virtual void setWaylandRenderer(WaylandRenderer *pWaylandRenderer) = 0;
};

// class FydePlatform{
// public:
//   virtual ~FydePlatform(){}

//   virtual void set_fyde_renderer(const std::shared_ptr<Renderer> &renderer) = 0;
// };

}  
}

#endif