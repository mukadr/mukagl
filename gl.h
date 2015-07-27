#ifndef MUKAGL_GL_H
#define MUKAGL_GL_H

// Data types
typedef void			GLvoid;
typedef unsigned int		GLenum;
typedef unsigned int		GLuint;
typedef unsigned char		GLubyte;
typedef int			GLint;
typedef int			GLsizei;
typedef float			GLfloat;


// Data types constants
#define GL_UNSIGNED_BYTE	1


// Matrix Mode
#define GL_MODELVIEW		0
#define GL_PROJECTION		1


// Primitives
#define GL_POINTS		0
#define GL_TRIANGLES		1


// glPush/PopAttrib bits
#define GL_COLOR_BUFFER_BIT	1
#define GL_DEPTH_BUFFER_BIT	2


// Capabilities
#define GL_DEPTH_TEST		1


// Texture mapping
#define GL_TEXTURE_2D		100
#define GL_TEXTURE_MIN_FILTER	101
#define GL_TEXTURE_MAG_FILTER	102
#define GL_NEAREST		103
#define GL_TEXTURE_ENV		104
#define GL_TEXTURE_ENV_MODE	105
#define GL_DECAL		106


// Pixel drawing
#define GL_RGB			1
#define GL_RGBA			2


// Pixel mode
#define GL_UNPACK_ALIGNMENT	1


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
void	glEnable(int cap);
void	glDisable(int cap);
void	glClear(int opts);
void	glClearColor(float r, float g, float b, float a);
void	glColor3f(float r, float g, float b);


// Textures
void	glGenTextures(GLsizei n, GLuint *texture);
void	glBindTexture(GLenum target, GLuint texture);
void	glTexParameteri(GLenum target, GLenum pname, GLint param);
void	glPixelStorei(GLenum pname, GLint param);
void	glTexImage2D(GLenum target, GLint level, GLint internal_fmt,
		     GLsizei width, GLsizei height, GLint border, GLenum format,
		     GLenum type, const GLvoid *pixels);
void	glTexEnvf(GLenum target, GLenum pname, GLfloat param);
void	glTexCoord2f(GLfloat s, GLfloat t);


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
