//c++ libs
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <map>

//opengl libs
#include <GL/glew.h>
#include <GL/glut.h>

//math libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//my libs
#include "drawobj.h"
#include "shader.h"

int WINDOW_WIDTH = 100;
int WINDOW_HEIGHT = 100;
float UPDATE_TIMER = 60;

void draw() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glutSwapBuffers();
}
void update(int) {
	glutPostRedisplay();
	glutTimerFunc(1000.0 / UPDATE_TIMER, update, 0);
}
void normalKeysFunc(unsigned char key, int x, int y) {
	std::cout << key << "\n";
	if (key == '0') {}
	if (key == '1') {}
	if (key == '2') {}
	if (key == '5') {}
	if (key == '6') {}
	if (key == '3') {}
	if (key == 'w') {}
	if (key == 's') {}
	if (key == 'a') {}
	if (key == 'd') {}
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Puyo Puyo");

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_2D);
	glewInit();

	//draw
	glutDisplayFunc(draw);
	glutIdleFunc(draw);

	glutKeyboardFunc(normalKeysFunc);

	//update
	glutTimerFunc(1000.0 / UPDATE_TIMER, update, 0);

	//start looop
	glutMainLoop();
}