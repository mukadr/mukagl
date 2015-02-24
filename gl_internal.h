#ifndef MUKAGL_GL_INTERNAL_H
#define MUKAGL_GL_INTERNAL_H

// Viewport
int	gl_view_width(void);
int	gl_view_height(void);


// Rasterization
void	gl_raster_point(int x, int y);
void	gl_clear_color_buffer(void);
void	gl_clear_color(float r, float g, float b, float a);
void	gl_color(float r, float g, float b, float a);


#endif // MUKAGL_GL_INTERNAL_H
