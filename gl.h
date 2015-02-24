#ifndef MUKAGL_GL_H
#define MUKAGL_GL_H

// Matrix Mode
#define GL_MODELVIEW		0
#define GL_PROJECTION		1


// Primitives
#define GL_POINTS		0
#define GL_TRIANGLES		1


// glPush/PopAttrib bits
#define GL_COLOR_BUFFER_BIT	1


// Transformation
void	glMatrixMode(int mode);

void	glPushMatrix(void);
void	glPopMatrix(void);

void	glLoadIdentity(void);

void	glTranslatef(float x, float y, float z);
void	glRotatef(float angle, float x, float y, float z);
void	glScalef(float sx, float sy, float sz);

void	gluPerspective(float fovy, float aspect, float near, float far);


// Drawing functions
void	glBegin(int mode);
void	glEnd(void);

void	glVertex3f(float x, float y, float z);


// Miscellaneous
void	glClear(int opts);
void	glClearColor(float r, float g, float b, float a);


// GLUT
#define GLUT_RGB	1
#define GLUT_DEPTH	2
#define GLUT_DOUBLE	4

void	glutInit(int *pargc, char **argv);
void	glutInitWindowSize(int width, int height);
void	glutInitWindowPosition(int x, int y);
void	glutInitDisplayMode(int mode);
void	glutCreateWindow(const char *title);

void	glutDisplayFunc(void (*func)(void));
void	glutIdleFunc(void (*func)(void));
void	glutKeyboardFunc(void (*func)(unsigned char key, int x, int y));
void	glutReshapeFunc(void (*func)(int width, int height));

void	glutSwapBuffers();
void	glutPostRedisplay();

void	glutMainLoop(void);
void	glutLeaveMainLoop(void);


#endif // MUKAGL_GL_H
