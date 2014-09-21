﻿#include <iostream>
#include "GLWidget3D.h"
#include "MainWindow.h"
#include <GL/GLU.h>
#include "OBJLoader.h"

/**
 * This event handler is called when the mouse press events occur.
 */
void GLWidget3D::mousePressEvent(QMouseEvent *e)
{
	camera.mouseDown(e->x(), e->y());
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e)
{
	camera.mouseUp();

	updateGL();
}

/**
 * This event handler is called when the mouse move events occur.
 */
void GLWidget3D::mouseMoveEvent(QMouseEvent *e)
{
	if (e->buttons() & Qt::LeftButton) {
		camera.rotate(e->x(), e->y());
	} else if (e->buttons() & Qt::RightButton) {
		camera.zoom(e->x(), e->y());
	}

	updateGL();
}

/**
 * This function is called once before the first call to paintGL() or resizeGL().
 */
void GLWidget3D::initializeGL()
{
    // Initialize glew library
    glewInit();

	glEnable(GL_DEPTH_TEST);

    // Create the gpgpu object
    sim = new WaterSimulator(512, 512);
	time = 0;

	timer.start(16, this);

	loadOBJ("models/cube.obj");
}

void GLWidget3D::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    updateGL();
}

/**
 * This function is called whenever the widget has been resized.
 */
void GLWidget3D::resizeGL(int width, int height)
{
	height = height?height:1;
	
	delete sim;
	sim = new WaterSimulator(width, height);

	camera.setWindowSize(width, height);

	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)width/(GLfloat)height, 0.1f, 10000);
	glMatrixMode(GL_MODELVIEW);
}

/**
 * This function is called whenever the widget needs to be painted.
 */
void GLWidget3D::paintGL()
{
	// Update the cells' states
	sim->update((float)time * 0.01);
	time++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -camera.z);
	glMultMatrixd(camera.rt);

	glBindTexture(GL_TEXTURE_2D, sim->textureId());
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < vertices.size(); i += 3) {
		glBegin(GL_TRIANGLES);
		for (int j = 0; j < 3; ++j) {
			glTexCoord2f(vertices[i + j].texCoord[0], vertices[i + j].texCoord[1]);
			//glNormal3f(vertices[i + j].normal[0], vertices[i + j].normal[1], vertices[i + j].normal[2]);
			glVertex3f(vertices[i + j].position[0], vertices[i + j].position[1], vertices[i + j].position[2]);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

/**
 * Load an OBJ file and create the corresponding VAO.
 */
void GLWidget3D::loadOBJ(const QString& filename)
{
	OBJLoader::load(filename, vertices);
}

