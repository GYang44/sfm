#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <string>

#include "object3D.hpp"

#ifndef GL_VIS_h
#define GL_VIS_h


    extern object3D camera;

    extern float xRot;
    extern float yRot;
    extern float zRot;


    //Draw strings in GL window
    void drawString(const std::string & inString);

    //Draw reference coordinate in the GL window
    void DrawWorldCoornidates();

    //The function that move objects in the window
    //Currently can only draw the camera
    void RenderScene();

    //Change view point
    void SetupRC();

    //Change size of GL window
    void ChangeSize( GLsizei w, GLsizei h);

    //Keyboard interface
    void viewControl(unsigned char key, int x, int y);


#endif
