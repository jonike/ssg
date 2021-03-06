//
// shader.cpp
// uses shader supplied on command line
//

#include "ssg.h"
using namespace glm;

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;
using namespace ssg;

FrameBufferObject *fbo = NULL;
Texture *fboTexture = NULL;

string shaderName;  // the base name of the .vert/.frag shader pair used for rendering the graph
string fragShaderName; // the base name of the .frag shader for the effects pass

Ptr<Instance> root;

Camera camera;
int width, height;

float getNow() {
  return static_cast<double>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0 ;
}

vec3 lerp (vec3 start, vec3 end, float u )
{
  return vec3 ( start*(1-u) + end*u );
}

void display ()
{
  //  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  static float lastFrame = getNow();
  float now = getNow();

  root->update(now-lastFrame);

  //  camera.draw(root);

  // draw the scene to the fbo
  fbo->bind();
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  //  glClear ( GL_DEPTH_BUFFER_BIT );
  camera.draw(root);
  fbo->unbind();

  // draw a fullscreen quad with the fbo texture
  fboTexture->bind(0);
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  //glClear (  GL_DEPTH_BUFFER_BIT );
  fboTexture->renderFullscreenQuad();
  fboTexture->unbind(0);

  glutSwapBuffers();
  lastFrame = now;

}

void timer ( int delay )
{
  glutTimerFunc( delay, timer, delay );
  glutPostRedisplay();
}

void reshape (int w, int h)
{
  width = w;
  height = h;
  // init fbo for offscreen rendering
  if (fboTexture)
    delete fboTexture;
  fboTexture = new Texture ( width, height, /*fp*/true, /*mipmaps*/true, 0/* */ );

  if (fbo)
    delete fbo;
  fbo = new FrameBufferObject ( fboTexture );

  // init for postprocess render from fbo
  string fragshader = "shaders120/";
  fragshader += fragShaderName + ".frag";
  fboTexture->setupRenderFullscreenQuad("shaders120/fxchain.vert", fragshader.c_str());

  //    fboTexture->setupRenderFullscreenQuad("shaders120/blur2.vert", "shaders120/zoom1.frag");
    //  fboTexture->setupRenderFullscreenQuad("shaders120/blur2.vert", "shaders120/blur2.frag");
  //    fboTexture->setupRenderFullscreenQuad("shaders120/blur2.vert", "shaders120/threshold.frag");
  //  fboTexture->setupRenderFullscreenQuad("shaders120/blur2.vert", "shaders120/edge.frag");

  //  glUseProgram(fboTexture->drawTextureShader);
  GLuint widthUniformLocation = glGetUniformLocation(fboTexture->drawTextureShader,"width");
  glUniform1f(widthUniformLocation, width);
  GLuint heightUniformLocation = glGetUniformLocation(fboTexture->drawTextureShader,"height");
  glUniform1f(heightUniformLocation, height);
  //  glUseProgram(0);

  camera.setupPerspective ( w, h );
}


void keyboard (unsigned char key, int x, int y)
{
  switch (key) {

  case 27: /* ESC */
    exit(0);
    break;
  default:
    break;
  }
}

void init (int argc, char **argv)
{
  

  //  create a primitive.  if supplied on command line, read a .obj wavefront file
  Ptr<Primitive> prim;
  if ( argc == 4 ) {
    prim = Ptr<Primitive> (new ObjFilePrimitive ( argv[1] ) );
    shaderName = argv[2];
    fragShaderName = argv[3];
  } else {
    cout << "usage: " << argv[0] << " objfilename 3DshaderName FXshaderName\n";
      //    exit(1);
    prim = Ptr<Primitive> (new ObjFilePrimitive ( "objfiles/cube.obj" ) );
    shaderName = "PhongShading";
    fragShaderName = "edge";
  }

  // create a root Instance to contain this primitive
  Ptr<Instance> instance ( new Instance() );
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );

  // enable camera trackball
  camera.enableTrackball (true);

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 4,10,5,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );

  // create a material to use
  Material *mat = new Material;
  mat->ambient = vec4 ( 0.1, 0.1, 0.2, 1.0 );
  mat->diffuse = vec4 ( 0.5, 0.7, 0.5, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;
  mat->program = mat->loadShaders ( shaderName.c_str() );

  // attach the material to the instance
  instance->setMaterial ( mat );

  // set the instance as the scene root
  root = instance;

  // misc OpenGL state
  glClearColor (0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);

}


void 
mouse ( int button, int state, int x, int y )
{
  if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
    camera.startMouse ( x, height-y );
  }
}

void
motion ( int x, int y ) 
{
  camera.dragMouse(x,height-y);
}


int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  glutInitWindowSize (300, 300); 
  glutInitWindowPosition (100, 100);
  glutCreateWindow (argv[0]);
  
#ifndef __APPLE__
  glewInit();
#endif
  init(argc,argv);
  
  glutDisplayFunc ( display );
  glutReshapeFunc ( reshape );
  glutKeyboardFunc ( keyboard );
  glutMouseFunc ( mouse );
  glutMotionFunc ( motion );
  glutTimerFunc( 33,timer,33 ); 
  glutMainLoop();
  return 0;
}


