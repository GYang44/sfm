#include "glVisualizer.hpp"

GLfloat lScale(5.0f), wScale(40.0f);

void drawString(const std::string & inString)
{
	for (uint i(0); i < inString.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, inString[i]);
	}
	return;
}

void drawWorldCoornidates()
{
	glColor3f( 0.34f, 0.81f, 1.0f );//blue
	glBegin( GL_LINES );
	{
		// x-axis
		glVertex3f( -wScale, 0.0f, 0.0f);
		glVertex3f(  wScale, 0.0f, 0.0f);

		// x direction indicator
		glVertex3f( wScale, 0.0f, 0.0f);
		glVertex3f( wScale - 3.0f, 3.0f, 0.0f);
		glVertex3f( wScale, 0.0f, 0.0f);
		glVertex3f( wScale - 3.0f, -3.0f, 0.0f);
	}
	glEnd();
	glRasterPos3f(wScale,0,0);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'X');

	glColor3f( 0.0f, 1.0f, 0.0f );//green
	glBegin( GL_LINES );
	{
		// y-axis
		glVertex3f( 0.0f, -wScale, 0.0f);
		glVertex3f( 0.0f, wScale, 0.0f);

		// y direction indicator
		glVertex3f( 0.0f, wScale, 0.0f);
		glVertex3f( 3.0f, wScale - 3.0f, 0.0f);
		glVertex3f( 0.0f, wScale, 0.0f);
		glVertex3f( -3.0f, wScale - 3.0f, 0.0f);
	}
	glEnd();
	glRasterPos3f(0,wScale,0);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'Y');

	glColor3f( 1.0f, 0.0f, 0.0f );//red
	glBegin( GL_LINES );
	{
		// z-axis 
		glVertex3f( 0.0f, 0.0f, -wScale );
		glVertex3f( 0.0f, 0.0f, wScale );

		// z direction indicator
		glVertex3f( 0.0f, 0.0f, wScale );
		glVertex3f( 0.0f, 3.0f, wScale - 3.0f );
		glVertex3f( 0.0f, 0.0f, wScale );
		glVertex3f( 0.0f, -3.0f, wScale - 3.0f );
	}
	glEnd();
	glRasterPos3f(0,0,wScale);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'Z');

}

void drawPolyLine(const std::vector<arma::Mat<double>> & inLine)
{
	assert(inLine.size() > 1); //require 2 points for a line
	glColor3f(0.34f, 0.81f, 1.0f);
	std::vector<arma::Mat<double>>::const_iterator it = inLine.begin();
	glBegin( GL_LINE_STRIP);
	{
		while(it != inLine.end())
		{
			glVertex3f((*it)(0), (*it)(1), (*it)(2));
			it++;
		}
	}
	glEnd();
	return;
}

void draw3D_Object(const Object3D & object)
{
	glColor3f( 0.34f, 0.81f, 1.0f );//blue
	glBegin( GL_LINES );
	{
		// x-axis
		glVertex3f( object.p(0,0),
					object.p(1,0),
					object.p(2,0));
		glVertex3f( object.p(0,0) + lScale * object.wr(0,0),
					object.p(1,0) + lScale * object.wr(1,0),
					object.p(2,0) + lScale * object.wr(2,0));
	}
	glEnd();

	glColor3f( 0.0f, 1.0f, 0.0f );//green
	glBegin( GL_LINES );
	{
		// y-axis
		glVertex3f( object.p(0,0),
					object.p(1,0),
					object.p(2,0));
		glVertex3f( object.p(0,0) + lScale * object.wr(0,1),
					object.p(1,0) + lScale * object.wr(1,1),
					object.p(2,0) + lScale * object.wr(2,1));

	}
	glEnd();

	glColor3f( 1.0f, 0.0f, 0.0f );//red
	glBegin( GL_LINES );
	{
		// z-axis
		glVertex3f( object.p(0,0),
					object.p(1,0),
					object.p(2,0));
		glVertex3f( object.p(0,0) + lScale * object.wr(0,2),
					object.p(1,0) + lScale * object.wr(1,2),
					object.p(2,0) + lScale * object.wr(2,2));
	}
	glEnd();

	//draw feild of view of object
	glColor3f( 1.0f, 1.0f, 0.0f );//yellow
	glBegin( GL_LINES );
	{
		glVertex3f( object.p(0,0),
					object.p(1,0),
					object.p(2,0));
		glVertex3f( object.p(0,0) + 2 * lScale * object.wr(0,0) + lScale * object.wr(0,1) + lScale * object.wr(0,2),
					object.p(1,0) + 2 * lScale * object.wr(1,0) + lScale * object.wr(1,1) + lScale * object.wr(1,2),
					object.p(2,0) + 2 * lScale * object.wr(2,0) + lScale * object.wr(2,1) + lScale * object.wr(2,2));
		glVertex3f( object.p(0,0),
					object.p(1,0),
					object.p(2,0));
		glVertex3f( object.p(0,0) - 2 * lScale * object.wr(0,0) + lScale * object.wr(0,1) + lScale * object.wr(0,2),
					object.p(1,0) - 2 * lScale * object.wr(1,0) + lScale * object.wr(1,1) + lScale * object.wr(1,2),
					object.p(2,0) - 2 * lScale * object.wr(2,0) + lScale * object.wr(2,1) + lScale * object.wr(2,2));
		glVertex3f( object.p(0,0),
					object.p(1,0),
					object.p(2,0));
		glVertex3f( object.p(0,0) - 2 * lScale * object.wr(0,0) - lScale * object.wr(0,1) + lScale * object.wr(0,2),
					object.p(1,0) - 2 * lScale * object.wr(1,0) - lScale * object.wr(1,1) + lScale * object.wr(1,2),
					object.p(2,0) - 2 * lScale * object.wr(2,0) - lScale * object.wr(2,1) + lScale * object.wr(2,2));
		glVertex3f( object.p(0,0),
					object.p(1,0),
					object.p(2,0));
		glVertex3f( object.p(0,0) + 2 * lScale * object.wr(0,0) - lScale * object.wr(0,1) + lScale * object.wr(0,2),
					object.p(1,0) + 2 * lScale * object.wr(1,0) - lScale * object.wr(1,1) + lScale * object.wr(1,2),
					object.p(2,0) + 2 * lScale * object.wr(2,0) - lScale * object.wr(2,1) + lScale * object.wr(2,2));
	}
	glEnd();
	glBegin( GL_LINE_STRIP );
	{
		glVertex3f( object.p(0,0) + 2 * lScale * object.wr(0,0) + lScale * object.wr(0,1) + lScale * object.wr(0,2),
					object.p(1,0) + 2 * lScale * object.wr(1,0) + lScale * object.wr(1,1) + lScale * object.wr(1,2),
					object.p(2,0) + 2 * lScale * object.wr(2,0) + lScale * object.wr(2,1) + lScale * object.wr(2,2));
		glVertex3f( object.p(0,0) - 2 * lScale * object.wr(0,0) + lScale * object.wr(0,1) + lScale * object.wr(0,2),
					object.p(1,0) - 2 * lScale * object.wr(1,0) + lScale * object.wr(1,1) + lScale * object.wr(1,2),
					object.p(2,0) - 2 * lScale * object.wr(2,0) + lScale * object.wr(2,1) + lScale * object.wr(2,2));
		glVertex3f( object.p(0,0) - 2 * lScale * object.wr(0,0) - lScale * object.wr(0,1) + lScale * object.wr(0,2),
					object.p(1,0) - 2 * lScale * object.wr(1,0) - lScale * object.wr(1,1) + lScale * object.wr(1,2),
					object.p(2,0) - 2 * lScale * object.wr(2,0) - lScale * object.wr(2,1) + lScale * object.wr(2,2));
		glVertex3f( object.p(0,0) + 2 * lScale * object.wr(0,0) - lScale * object.wr(0,1) + lScale * object.wr(0,2),
					object.p(1,0) + 2 * lScale * object.wr(1,0) - lScale * object.wr(1,1) + lScale * object.wr(1,2),
					object.p(2,0) + 2 * lScale * object.wr(2,0) - lScale * object.wr(2,1) + lScale * object.wr(2,2));
		glVertex3f( object.p(0,0) + 2 * lScale * object.wr(0,0) + lScale * object.wr(0,1) + lScale * object.wr(0,2),
					object.p(1,0) + 2 * lScale * object.wr(1,0) + lScale * object.wr(1,1) + lScale * object.wr(1,2),
					object.p(2,0) + 2 * lScale * object.wr(2,0) + lScale * object.wr(2,1) + lScale * object.wr(2,2));
	}
	glEnd();

	std::ostringstream tmp;
	tmp << object.p(0,0) << ' ' << object.p(1,0) << ' ' << object.p(2,0);
	std::string position = tmp.str();
	glRasterPos3f( object.p(0,0) + lScale * object.wr(0,0),
				object.p(1,0) + lScale * object.wr(1,0),
				object.p(2,0) + lScale * object.wr(2,0));
	drawString(position);
}

void renderScene()
{

	// reset matrix state
	glLoadIdentity();

	// set rotation of the matrix
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);

	//initial color
	glClear( GL_COLOR_BUFFER_BIT );

	//visual render opengl and opencv
	drawWorldCoornidates();

	//Draw camera in world coordinate

	if (camera.isInitialed)
	{
		draw3D_Object(camera);
		if(camera.trajactory.size() > 1)
			drawPolyLine(camera.trajactory);
	}

	// Restore transformations
	//glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
	return;
}

void setupRC()
{
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glColor3f( 1.0f, 0.0f, 0.0f );
}

void changeSize( GLsizei w, GLsizei h )
{
	GLfloat nRange = 5 * 20.0f;
	// Prevent a divide by zero
	if(h == 0) h = 1;

  // Set Viewport to window dimensions
  glViewport(0, 0, w, h);

  // Reset projection matrix stack
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Establish clipping volume (left, right, bottom, top, near, far)
  if (w <= h)
		glOrtho (-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange, nRange);
  else
    glOrtho (-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange, nRange);

  // Reset Model view matrix stack
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
	glutPostRedisplay();
}

void viewControl(unsigned char key, int x, int y)
{
	//float xRot, yRot, zRot;
	switch (key)
	{
		case 'a':
			zRot += 10.0;
			if (zRot > 180)
			{
				zRot -= 360;
			}
			else if (zRot <= -180)
			{
				zRot += 360;
			}
			break;
		case 'd':
			zRot -= 10.0;
			if (zRot > 180)
			{
				zRot -= 360;
			}
			else if (zRot <= -180)
			{
				zRot += 360;
			}
			break;
		case 's':
			xRot -= 10.0;
			if (xRot > 180)
			{
				xRot -= 360;
			}
			else if (xRot <= -180)
			{
				xRot += 360;
			}
			break;
		case 'w':
			xRot += 10.0;
			if (xRot > 180)
			{
				xRot -= 360;
			}
			else if (xRot <= -180)
			{
				xRot += 360;
			}
			break;
		case 'q':
			yRot += 10.0;
			if (yRot > 180)
			{
				yRot -= 360;
			}
			else if (yRot <= -180)
			{
				yRot += 360;
			}
			break;
		case 'e':
			yRot -= 10.0;
			if (yRot > 180)
			{
				yRot -= 360;
			}
			else if (yRot <= -180)
			{
				yRot += 360;
			}
			break;
	}
	glutPostRedisplay();
}
