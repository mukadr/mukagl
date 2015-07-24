#include <unistd.h>

#include <SDL_image.h>

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

GLuint wtex;

SDL_Surface *load_image(const char *filename)
{
	SDL_Surface *img;

	img = IMG_Load(filename);
	if (!img) {
		fprintf(stderr, "Unable to load image %s: %s\n", filename, IMG_GetError());
		exit(1);
	}
	return img;
}

static void draw_wall(void)
{
	glBindTexture(GL_TEXTURE_2D, wtex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLES);

	// left
	glColor3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f,  1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f( 1.0f,  1.0f, 0.0f);

	// right
	glColor3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f( 1.0f,  1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f( 1.0f, -1.0f, 0.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void display(void)
{
	static int angle = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// camera
	glTranslatef(-camx, -camy, -camz);

	glRotatef(angle, 0.0f, 0.0f, 1.0f);

	draw_wall();

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
	}
}

int main(int argc, char *argv[])
{
	SDL_Surface *img;
	int flags;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(300, 200);
	glutCreateWindow("Texture mapping sample" TITLE_SUFFIX);

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	glEnable(GL_DEPTH_TEST);

	// init sdl_image
	flags = IMG_INIT_PNG;
	if (!(IMG_Init(flags) & flags)) {
		fprintf(stderr, "SDL_image error: %s\n", IMG_GetError());
		return 1;
	}

	img = load_image("tex1.jpg");

	glGenTextures(1, &wtex);
	glBindTexture(GL_TEXTURE_2D, wtex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

	glutMainLoop();

	return 0;
}

