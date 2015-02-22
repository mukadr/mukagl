#ifndef MUKAGL_GL_H
#define MUKAGL_GL_H

// Matrix Mode
#define GL_MODELVIEW	0
#define GL_PROJECTION	1


// Primitives
#define GL_POINTS	0


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


#endif // MUKAGL_GL_H
