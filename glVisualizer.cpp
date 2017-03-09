#include "glVisualizer.hpp"

//float xRot, yRot, zRot;
//object3D camera;

void drawString(const std::string & inString)
{
	for (uint i(0); i < inString.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, inString[i]);
	}
	return;
}

void DrawWorldCoornidates()
{
	//draw axis
	{
		glColor3f( 0.34f, 0.81f, 1.0f );//blue
		glBegin( GL_LINES );
		{
			// x-axis
			glVertex3f( -200.0f, 0.0f, 0.0f);
			glVertex3f( 200.0f, 0.0f, 0.0f);
			glVertex3f( 200.0f, 0.0f, 0.0f);
			glVertex3f( 193.0f, 3.0f, 0.0f);
			glVertex3f( 200.0f, 0.0f, 0.0f);
			glVertex3f( 193.0f,-3.0f, 0.0f);
		}
		glEnd();
		glRasterPos3f(200,0,0);
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'X');

		glColor3f( 0.0f, 1.0f, 0.0f );//green
		glBegin( GL_LINES );
		{
			// y-axis
			glVertex3f( 0.0f, -200.0f, 0.0f);
			glVertex3f( 0.0f, 200.0f, 0.0f);
			glVertex3f( 0.0f, 200.0f, 0.0f);
			glVertex3f( 3.0f, 193.0f, 0.0f);
			glVertex3f( 0.0f, 200.0f, 0.0f);
			glVertex3f( -3.0f, 193.0f, 0.0f);
		}
		glEnd();
		glRasterPos3f(0,200,0);
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'Y');

		glColor3f( 1.0f, 0.0f, 0.0f );//red
		glBegin( GL_LINES );
		{
			// z-axis (drawn in oppsite direction)
			glVertex3f( 0.0f, 0.0f, -200.0f );
			glVertex3f( 0.0f, 0.0f, 200.0f );
			glVertex3f( 0.0f, 0.0f, 200.0f );
			glVertex3f( 0.0f, 3.0f, 193.0f );
			glVertex3f( 0.0f, 0.0f, 200.0f );
			glVertex3f( 0.0f, -3.0f, 193.0f);
		}
		glEnd();
		glRasterPos3f(0,0,200);
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'Z');
	}

}

void RenderScene()
{
	//scale of the length of the aruco
	double lScale(100.0f);

	// reset matrix state
	glLoadIdentity();

	// set rotation of the matrix
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);

	//visual render opengl and opencv
	glClear( GL_COLOR_BUFFER_BIT );
	DrawWorldCoornidates();


	//Draw camera in world coordinate
	if (camera.isInitialed)
	{
		glColor3f( 0.34f, 0.81f, 1.0f );//blue
		glBegin( GL_LINES );
		{
			// x-axis
			glVertex3f( camera.p(0,0),
						camera.p(1,0),
						camera.p(2,0));
			glVertex3f( camera.p(0,0) + lScale * camera.wr(0,0),
						camera.p(1,0) + lScale * camera.wr(1,0),
						camera.p(2,0) + lScale * camera.wr(2,0));
		}
		glEnd();

		glColor3f( 0.0f, 1.0f, 0.0f );//green
		glBegin( GL_LINES );
		{
		// y-axis
			glVertex3f( camera.p(0,0),
						camera.p(1,0),
						camera.p(2,0));
			glVertex3f( camera.p(0,0) + lScale * camera.wr(0,1),
						camera.p(1,0) + lScale * camera.wr(1,1),
						camera.p(2,0) + lScale * camera.wr(2,1));

		}
		glEnd();

		glColor3f( 1.0f, 0.0f, 0.0f );//red
		glBegin( GL_LINES );
		{
			// z-axis
			glVertex3f( camera.p(0,0),
						camera.p(1,0),
						camera.p(2,0));
			glVertex3f( camera.p(0,0) + lScale * camera.wr(0,2),
						camera.p(1,0) + lScale * camera.wr(1,2),
						camera.p(2,0) + lScale * camera.wr(2,2));
		}
		glEnd();

		//draw feild of view of camera
		glColor3f( 1.0f, 1.0f, 0.0f );//yellow
		glBegin( GL_LINES );
		{
			glVertex3f( camera.p(0,0),
						camera.p(1,0),
						camera.p(2,0));
			glVertex3f( camera.p(0,0) + 2 * lScale * camera.wr(0,0) + lScale * camera.wr(0,1) + lScale * camera.wr(0,2),
						camera.p(1,0) + 2 * lScale * camera.wr(1,0) + lScale * camera.wr(1,1) + lScale * camera.wr(1,2),
						camera.p(2,0) + 2 * lScale * camera.wr(2,0) + lScale * camera.wr(2,1) + lScale * camera.wr(2,2));
			glVertex3f( camera.p(0,0),
						camera.p(1,0),
						camera.p(2,0));
			glVertex3f( camera.p(0,0) - 2 * lScale * camera.wr(0,0) + lScale * camera.wr(0,1) + lScale * camera.wr(0,2),
						camera.p(1,0) - 2 * lScale * camera.wr(1,0) + lScale * camera.wr(1,1) + lScale * camera.wr(1,2),
						camera.p(2,0) - 2 * lScale * camera.wr(2,0) + lScale * camera.wr(2,1) + lScale * camera.wr(2,2));
			glVertex3f( camera.p(0,0),
						camera.p(1,0),
						camera.p(2,0));
			glVertex3f( camera.p(0,0) - 2 * lScale * camera.wr(0,0) - lScale * camera.wr(0,1) + lScale * camera.wr(0,2),
						camera.p(1,0) - 2 * lScale * camera.wr(1,0) - lScale * camera.wr(1,1) + lScale * camera.wr(1,2),
						camera.p(2,0) - 2 * lScale * camera.wr(2,0) - lScale * camera.wr(2,1) + lScale * camera.wr(2,2));
			glVertex3f( camera.p(0,0),
						camera.p(1,0),
						camera.p(2,0));
			glVertex3f( camera.p(0,0) + 2 * lScale * camera.wr(0,0) - lScale * camera.wr(0,1) + lScale * camera.wr(0,2),
						camera.p(1,0) + 2 * lScale * camera.wr(1,0) - lScale * camera.wr(1,1) + lScale * camera.wr(1,2),
						camera.p(2,0) + 2 * lScale * camera.wr(2,0) - lScale * camera.wr(2,1) + lScale * camera.wr(2,2));
		}
		glEnd();
		glBegin( GL_LINE_STRIP );
		{
			glVertex3f( camera.p(0,0) + 2 * lScale * camera.wr(0,0) + lScale * camera.wr(0,1) + lScale * camera.wr(0,2),
						camera.p(1,0) + 2 * lScale * camera.wr(1,0) + lScale * camera.wr(1,1) + lScale * camera.wr(1,2),
						camera.p(2,0) + 2 * lScale * camera.wr(2,0) + lScale * camera.wr(2,1) + lScale * camera.wr(2,2));
			glVertex3f( camera.p(0,0) - 2 * lScale * camera.wr(0,0) + lScale * camera.wr(0,1) + lScale * camera.wr(0,2),
						camera.p(1,0) - 2 * lScale * camera.wr(1,0) + lScale * camera.wr(1,1) + lScale * camera.wr(1,2),
						camera.p(2,0) - 2 * lScale * camera.wr(2,0) + lScale * camera.wr(2,1) + lScale * camera.wr(2,2));
			glVertex3f( camera.p(0,0) - 2 * lScale * camera.wr(0,0) - lScale * camera.wr(0,1) + lScale * camera.wr(0,2),
						camera.p(1,0) - 2 * lScale * camera.wr(1,0) - lScale * camera.wr(1,1) + lScale * camera.wr(1,2),
						camera.p(2,0) - 2 * lScale * camera.wr(2,0) - lScale * camera.wr(2,1) + lScale * camera.wr(2,2));
			glVertex3f( camera.p(0,0) + 2 * lScale * camera.wr(0,0) - lScale * camera.wr(0,1) + lScale * camera.wr(0,2),
						camera.p(1,0) + 2 * lScale * camera.wr(1,0) - lScale * camera.wr(1,1) + lScale * camera.wr(1,2),
						camera.p(2,0) + 2 * lScale * camera.wr(2,0) - lScale * camera.wr(2,1) + lScale * camera.wr(2,2));
			glVertex3f( camera.p(0,0) + 2 * lScale * camera.wr(0,0) + lScale * camera.wr(0,1) + lScale * camera.wr(0,2),
						camera.p(1,0) + 2 * lScale * camera.wr(1,0) + lScale * camera.wr(1,1) + lScale * camera.wr(1,2),
						camera.p(2,0) + 2 * lScale * camera.wr(2,0) + lScale * camera.wr(2,1) + lScale * camera.wr(2,2));
		}
		glEnd();

		std::ostringstream tmp;
		tmp << camera.p(0,0) << ' ' << camera.p(1,0) << ' ' << camera.p(2,0);
		std::string position = tmp.str();
		glRasterPos3f( camera.p(0,0) + lScale * camera.wr(0,0),
					camera.p(1,0) + lScale * camera.wr(1,0),
					camera.p(2,0) + lScale * camera.wr(2,0));
		drawString(position);
	}

	// Restore transformations
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
	return;
}

void SetupRC()
{
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glColor3f( 1.0f, 0.0f, 0.0f );
}

void ChangeSize( GLsizei w, GLsizei h )
{
	GLfloat nRange = 2000.0f;
    // Prevent a divide by zero
    if(h == 0)
        h = 1;

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
