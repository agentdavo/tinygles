#include "x11.hpp"
#include "gl.h"
#include "api.hpp"
#include "text.hpp"
#include "debug.hpp"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <array>

namespace fp
{

    static tGLfixed gTime = 0.0f;
    static GLuint limit = 0;
    static GLuint count = 1;

    static tGLfixed black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    static tGLfixed white[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    // Use std::array to hold colors for cubes (4 elements for RGBA)
    static std::vector<std::array<tGLfixed, 4>> cubes;

    // Function to generate random colors
    void generate_random_color(std::array<tGLfixed, 4> &color)
    {
        color[0] = (tGLfixed)(rand() % 100) / 100.0f;
        color[1] = (tGLfixed)(rand() % 100) / 100.0f;
        color[2] = (tGLfixed)(rand() % 100) / 100.0f;
        color[3] = 1.0f; // Fully opaque
    }

    // Function to generate random cubes
    void generate_cubes(int numCubes)
    {
        for (int i = 0; i < numCubes; i++)
        {
            std::array<tGLfixed, 4> cubeColor;
            generate_random_color(cubeColor);
            cubes.push_back(cubeColor);
        }
    }

    // Function to draw a cube using triangles
    void draw_cube(const std::array<tGLfixed, 4> &color)
    {
        static tGLfixed size = 0.1f;

        glBegin(GL_TRIANGLES); // Use triangles instead of quads

        // Set the cube's color
        glColor3xv(color.data());

        // Front face (two triangles)
        glVertex3x(-size, -size, size);
        glVertex3x(size, -size, size);
        glVertex3x(size, size, size);

        glVertex3x(size, size, size);
        glVertex3x(-size, size, size);
        glVertex3x(-size, -size, size);

        // Back face (two triangles)
        glVertex3x(-size, -size, -size);
        glVertex3x(-size, size, -size);
        glVertex3x(size, size, -size);

        glVertex3x(size, size, -size);
        glVertex3x(size, -size, -size);
        glVertex3x(-size, -size, -size);

        // Top face (two triangles)
        glVertex3x(-size, size, -size);
        glVertex3x(-size, size, size);
        glVertex3x(size, size, size);

        glVertex3x(size, size, size);
        glVertex3x(size, size, -size);
        glVertex3x(-size, size, -size);

        // Bottom face (two triangles)
        glVertex3x(-size, -size, -size);
        glVertex3x(size, -size, -size);
        glVertex3x(size, -size, size);

        glVertex3x(size, -size, size);
        glVertex3x(-size, -size, size);
        glVertex3x(-size, -size, -size);

        // Right face (two triangles)
        glVertex3x(size, -size, -size);
        glVertex3x(size, size, -size);
        glVertex3x(size, size, size);

        glVertex3x(size, size, size);
        glVertex3x(size, -size, size);
        glVertex3x(size, -size, -size);

        // Left face (two triangles)
        glVertex3x(-size, -size, -size);
        glVertex3x(-size, -size, size);
        glVertex3x(-size, size, size);

        glVertex3x(-size, size, size);
        glVertex3x(-size, size, -size);
        glVertex3x(-size, -size, -size);

        glEnd();
    }

    void draw_cubes(int count)
    {
        for (int i = 0; i < count; i++)
        {
            // Apply transformation per cube
            glPushMatrix(); // Use OpenGL glPushMatrix()

            // Random translation and rotation for each cube
            glTranslatex(-1.0f + (rand() % 200) / 100.0f, -1.0f + (rand() % 200) / 100.0f, -5.0f + (rand() % 100) / 50.0f);
            glRotatex(gTime + i * 0.05f, rand() % 2, rand() % 2, rand() % 2);

            draw_cube(cubes[i % cubes.size()]);
            glPopMatrix(); // Use OpenGL glPopMatrix()
        }
    }

    void setup_lighting()
    {
        tGLfixed light_position[] = {10.0f, 10.0f, 10.0f, 1.0f};
        tGLfixed light_diffuse[] = {0.8f, 0.8f, 0.8f, 0.8f};
        tGLfixed light_specular[] = {0.8f, 0.8f, 0.8f, 0.8f};
        tGLfixed light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};

        glEnable(GL_LIGHTING); // Standard OpenGL call
        glEnable(GL_LIGHT0);   // Standard OpenGL call

        // Set light parameters
        glLightxv(GL_LIGHT0, GL_POSITION, light_position); // Use OpenGL function
        glLightxv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightxv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glLightxv(GL_LIGHT0, GL_AMBIENT, light_ambient);

        glEnable(GL_COLOR_MATERIAL); // Standard OpenGL call
    }

    void setup2D(tGLfixed width, tGLfixed height)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        fp::glOrthox(
            0,             // Left
            600,  // Right
            600, // Bottom
            0,             // Top
            -1,            // Near
            1              // Far
        );

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
    }

    void restore3D()
    {
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    void drawRectangle(GLint window_width, GLint window_height)
    {
        // Define the rectangle dimensions
        GLint rectWidth = 50;
        GLint rectHeight = 50;

        // Coordinates for the rectangle at the top-left corner
        GLint x = 0;
        GLint y = 0;

        // Convert coordinates to fixed-point
        GLfixed vertices[] = {
            x << 16, y << 16,                             // Bottom-left
            (x + rectWidth) << 16, y << 16,               // Bottom-right
            x << 16, (y + rectHeight) << 16,              // Top-left
            (x + rectWidth) << 16, (y + rectHeight) << 16 // Top-right
        };

        // Enable vertex arrays
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FIXED, 0, vertices);

        // Set the color (e.g., Red)
        glColor4ub(255, 0, 0, 255);

        // Draw the rectangle using a triangle strip
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Disable vertex arrays
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    void draw(void)
    {
        // Clear the screen with the background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Use OpenGL glClear()

        // Draw 1000 random cubes
        draw_cubes(10);

        // Setup 2D orthographic projection for text rendering
        setup2D(600, 600); // Replace with your window dimensions

        // Draw "Hello World" text at the top-left corner with 10 pixel padding
        const char *textBuffer = "Hello World";
        fp::glDrawText(textBuffer, 10, 10, 0xFFFFFFFF); // White color

        // Restore the previous projection and modelview matrices
        restore3D();

        fp::tkSwapBuffers(); // Assuming this is a custom function to swap buffers

        count++;
        if (count == limit)
        {
            exit(0);
        }
    }

    void idle(void)
    {
        gTime += 0.5f;
        draw();
    }

    GLenum key(int k, GLenum mask)
    {
        // Process key input
        return GL_FALSE;
    }

    void reshape(int width, int height)
    {
        std::cout << "reshape() called with width = " << width << ", height = " << height << std::endl;
        glViewport(0, 0, (GLint)width, (GLint)height); // Use OpenGL glViewport
        glMatrixMode(GL_PROJECTION);                   // Use OpenGL glMatrixMode
        glLoadIdentity();
        // Adjust the Frustum to ensure everything is within the view
        glFrustumx(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 10.0f); // Use OpenGL glFrustumx
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void init(void)
    {
        std::cout << "init() called" << std::endl;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Standard OpenGL function
        glEnable(GL_DEPTH_TEST);                   // Enable depth testing
        generate_cubes(10);                        // Generate 1000 cubes with random colors
        setup_lighting();                          // Setup global lighting
        initFontTexture();
        GLContext *c = gl_get_context();
        printGLContext(c);
    }

} // namespace fp

int main(int argc, char **argv)
{
    // Check if a frame limit is provided as an argument
    if (argc > 1)
    {
        fp::limit = std::atoi(argv[1]) + 1; // Correctly update limit within the fp namespace
    }
    else
    {
        fp::limit = 0; // Default to 0 if no argument is provided
    }

    // Start the main application loop
    return fp::ui_loop(argc, argv, "Cube Rendering");
}
