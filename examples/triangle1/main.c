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
float camz = 4.0f;

void display(void)
{
	static int angle = 0;

	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	// camera
	glTranslatef(-camx, -camy, -camz);

	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f( 1.0f, -1.0f, 0.0f);
	glVertex3f( 0.0f,  1.0f, 0.0f);
	glEnd();

	if (++angle == 360)
		angle = 0;

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
	gluPerspective(60.0f, (float)w/h, 0.1f, 10.0f);

	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(300, 200);
	glutCreateWindow("GL_POINTS example1" TITLE_SUFFIX);

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}
