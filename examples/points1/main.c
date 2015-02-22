#include <unistd.h>

#ifdef __MUKAGL__
#include "../../gl.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <stdio.h>

#define WIDTH 500
#define HEIGHT 500

float camx = 0.0f;
float camy = 1.0f;
float camz = 4.0f;

void display(void)
{
	static int angle = 0;
	int i, j;

	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	// camera
	glTranslatef(-camx, -camy, -camz);

	for (j = 0; j < 20; j++) {
		for (i = 0; i < 50; i += 5) {
			int a = (angle + i) % 360;

			glPushMatrix();
			glRotatef(a, 0.0f, 1.0f, 0.0f);

			glBegin(GL_POINTS);
			glVertex3f(1.0f, 0.04f * j, 0.0f);
			glEnd();

			glPopMatrix();
		}
	}

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
	gluPerspective(60.0f, (float)WIDTH/HEIGHT, 0.1f, 10.0f);

	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(300, 200);
	glutCreateWindow("Hello World!");

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}

