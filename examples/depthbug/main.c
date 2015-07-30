// Verifying problems caused by the abscence of clipping
#include <unistd.h>

#ifdef __MUKAGL__
#include "../../gl.h"
#define TITLE_SUFFIX " (mukaGL)"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#define TITLE_SUFFIX " (OpenGL)"
#endif

#include <stdio.h>

float camx = 0.0f;
float camy = 0.0f;
float camz = 5.0f;
float cam_angle = 0.0f;
float scale_z = 1.0f;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// camera
	glRotatef(cam_angle, 0.0f, -1.0f, 0.0f);
	glTranslatef(-camx, -camy, -camz);

	// red
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f( 0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f,-1.0f, 0.0f);
	glVertex3f( 1.0f,-1.0f, 0.0f);
	glEnd();

	// green
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f( 0.5f, 1.0f,  0.0f);
	glVertex3f( 0.5f,-1.0f, -scale_z);
	glVertex3f( 0.5f,-1.0f,  scale_z);
	glEnd();

	glutSwapBuffers();
}

void idle(void)
{
	glutPostRedisplay();
	usleep(10000);
}

void resize(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)w/h, 1.0f, 10.0f);

	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
	case 'W':
		camz -= 0.1f;
		break;
	case 's':
	case 'S':
		camz += 0.1f;
		break;
	case 'a':
		cam_angle++;
		break;
	case 'd':
		cam_angle--;
		break;
	case 'e':
		scale_z++;
		break;
	case 'q':
		scale_z--;
		break;
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(300, 200);
	glutCreateWindow("Depth bug" TITLE_SUFFIX);

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	glEnable(GL_DEPTH_TEST);

#ifndef __MUKAGL__
	glDisable(GL_CULL_FACE);
#endif
	glutMainLoop();

	return 0;
}

