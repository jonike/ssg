//
// example10.cpp
// another articulated figure
//

#include "ssg.h"
#include "Camera.h"
using namespace glm;

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;
using namespace ssg;

ModelNode *root;
Primitive *prim;
Camera camera;
int width, height;
vector<vec3> controlPoints;
Instance *body, *ulArm, *llArm, *urArm, *lrArm, *ulLeg, *urLeg, *llLeg, *lrLeg, *head;

float baseFrequency = 4.0; //Hz

vec3 upperLeftArmJointOffset (1,1,0);
vec3 lowerLeftArmJointOffset (1,0,0);
vec3 upperRightArmJointOffset (-1,1,0);
vec3 lowerRightArmJointOffset (-1,0,0);
vec3 upperLeftLegJointOffset (0.5,-1,0);
vec3 upperRightLegJointOffset (-0.5,-1,0);
vec3 lowerLeftLegJointOffset ( 0,-1,0);
vec3 lowerRightLegJointOffset ( 0,-1,0);
vec3 headJointOffset ( 0,0.9,0 );

float getNow() {
  return static_cast<double>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0 ;
}

///////////////////////////////////////////////////////////////

mat4
upperLeftArmMatrix ( float angle1, float angle2 )
{
  mat4 t = translate ( mat4(), vec3 ( upperLeftArmJointOffset ) );
  mat4 r = rotate ( mat4(), angle1, vec3(0,0,1) );
  mat4 r2 = rotate ( mat4(), angle2, vec3(0,1,0) );
  return t * r * r2;
}

mat4
lowerLeftArmMatrix ( float angle )
{
  mat4 t = translate ( mat4(), vec3 ( lowerLeftArmJointOffset ) );
  mat4 r = rotate ( mat4(), angle, vec3(0,0,1) );
  return t * r;
}
mat4
upperRightArmMatrix ( float angle )
{
  mat4 t = translate ( mat4(), vec3 ( upperRightArmJointOffset ) );
  mat4 r = rotate ( mat4(), angle, vec3(0,0,1) );
  return t * r;
}

mat4
lowerRightArmMatrix ( float angle )
{
  mat4 t = translate ( mat4(), vec3 ( lowerRightArmJointOffset ) );
  mat4 r = rotate ( mat4(), angle, vec3(0,0,1) );
  return t * r;
}

mat4 
upperLeftLegMatrix ( float angle )
{
  mat4 t = translate ( mat4(), upperLeftLegJointOffset );
  mat4 r = rotate ( mat4(), angle, vec3(1,0,0) );
  return t * r;
}

mat4
upperRightLegMatrix ( float angle )
{
  mat4 t = translate ( mat4(), upperRightLegJointOffset );
  mat4 r = rotate ( mat4(), angle, vec3(1,0,0) );
  return t * r;
}

mat4 
lowerLeftLegMatrix ( float angle )
{
  mat4 t = translate ( mat4(), lowerLeftLegJointOffset );
  mat4 r = rotate ( mat4(), angle, vec3(1,0,0) );
  return t * r;
}

mat4
lowerRightLegMatrix ( float angle )
{
  mat4 t = translate ( mat4(), lowerRightLegJointOffset );
  mat4 r = rotate ( mat4(), angle, vec3(1,0,0) );
  return t * r;
}

mat4
headMatrix ( float angle )
{
  mat4 t = translate ( mat4(), headJointOffset );
  mat4 r = rotate ( mat4(), angle, vec3(0,0,1) );
  return t * r;
}

void display ()
{
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  static float lastFrame = getNow();
  float now = getNow();

  root->update(now-lastFrame);

  ulArm->setMatrix ( upperLeftArmMatrix ( 60 * sin(baseFrequency * now), 45 * sin(baseFrequency * now) ) );
  llArm->setMatrix ( lowerLeftArmMatrix ( 45 * sin(baseFrequency * now) ) );

  urArm->setMatrix ( upperRightArmMatrix ( 45 * -sin(baseFrequency * now) ) );
  lrArm->setMatrix ( lowerRightArmMatrix ( 60 * -sin(baseFrequency * now) ) );

  ulLeg->setMatrix ( upperLeftLegMatrix ( 45 * sin ( baseFrequency * now ) ) );

  urLeg->setMatrix ( upperRightLegMatrix ( -45 * sin ( baseFrequency * now ) ) );

  head->setMatrix ( headMatrix ( 10 * sin(baseFrequency * now) ) );

  camera.draw(root);

  glutSwapBuffers();
  lastFrame = now;

  //  std::cout << now << std::endl;
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
  ObjFilePrimitive *bodyprim, *upperArm, *lowerArm, *headPrim;
  bodyprim = new ObjFilePrimitive ( "objfiles/body.obj" );
  upperArm = new ObjFilePrimitive ( "objfiles/upper_arm.obj" );
  lowerArm = new ObjFilePrimitive ( "objfiles/lower_arm.obj" );
  headPrim = new ObjFilePrimitive ( "objfiles/bill.obj" );

  // create the graph

  body = new Instance;
  body->addChild ( bodyprim );
  body->setMatrix ( mat4() );

  ulArm = new Instance;
  ulArm->addChild ( upperArm );
  ulArm->setMatrix ( upperLeftArmMatrix(0,0) );
  body->addChild ( ulArm );

  llArm = new Instance;
  llArm->addChild ( lowerArm );
  llArm->setMatrix ( lowerLeftArmMatrix(0) );
  ulArm->addChild ( llArm );

  urArm = new Instance;
  Instance *urArmLink = new Instance;
  urArmLink->addChild ( upperArm );
  urArmLink->setMatrix ( rotate(mat4(), 180.0f,vec3(0,0,1)) );
  urArm->addChild ( urArmLink );
  urArm->setMatrix ( upperRightArmMatrix(0) );
  body->addChild ( urArm );

  lrArm = new Instance;
  Instance *lrArmLink = new Instance;
  lrArmLink->addChild ( lowerArm );
  lrArmLink->setMatrix ( rotate ( mat4(), 180.0f, vec3(0,0,1) ) );
  lrArm->addChild ( lrArmLink );
  lrArm->setMatrix ( lowerRightArmMatrix(0) );
  urArm->addChild ( lrArm );

  ulLeg = new Instance;
  Instance* ulLegLink = new Instance;
  ulLegLink->addChild ( upperArm );
  ulLegLink->setMatrix ( rotate ( mat4(), -90.0f, vec3(0,0,1) ) );
  ulLeg->addChild ( ulLegLink );
  ulLeg->setMatrix ( upperLeftLegMatrix(0) );
  body->addChild ( ulLeg );

  urLeg = new Instance;
  urLeg->addChild ( ulLegLink ); // sharing is caring
  urLeg->setMatrix ( upperRightLegMatrix(0) );
  body->addChild ( urLeg );

  llLeg = new Instance;
  Instance *llLegLink = new Instance;
  llLegLink->addChild ( lowerArm );
  llLegLink->setMatrix ( rotate ( mat4(), -90.0f, vec3(0,0,1) ) );
  llLeg->addChild ( llLegLink );
  //  llLeg->setMatrix ( upperLeftLegMatrix(0) );
  llLeg->setMatrix ( mat4());
  ulLeg->addChild ( llLeg );

  lrLeg = new Instance;
  lrLeg->addChild ( llLegLink ); // share
  llLeg->setMatrix ( lowerLeftLegMatrix(0) );
  urLeg->addChild ( llLeg );

  head = new Instance;
  head->addChild ( headPrim );
  head->setMatrix ( headMatrix (0) );
  body->addChild(head);


  // set the instance as the scene root
  root = body;

  // enable camera trackball
  camera.enableTrackball (true);

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 4,10,5,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );

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


