#include <GL/freeglut.h>
#include <iostream>
#include <sstream>
#include <string>
#include <assert.h>
#include <armadillo>

#include "object3D.hpp"

#ifndef GL_VIS_h
#define GL_VIS_h

    //Global object defining camera gesture 3D object
    extern CameraObj camera;

    //Global variables defining roation of the scene
    extern float xRot;
    extern float yRot;
    extern float zRot;
    extern GLfloat lScale;

    //Draw strings in GL window
    void drawString(const std::string & inString);

    //Draw reference coordinate in the GL window
    void drawWorldCoornidates();

    //The function that move objects in the window
    //Currently can only draw the camera
    void renderScene();

    //Change view point
    void setupRC();

    //Change size of GL window
    void changeSize( GLsizei w, GLsizei h);

    //Keyboard interface
    void viewControl(unsigned char key, int x, int y);

    void draw3D_Object(const Object3D & object);

    void drawPolyLine(const std::vector<arma::Mat<double>> & inLine);
#endif
