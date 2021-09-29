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

//font lib
#include <ft2build.h>
#include FT_FREETYPE_H 

//my libs
#include "drawobj.h"
#include "shader.h"
#include "structs.h"

float WINDOW_WIDTH = 300;
float WINDOW_HEIGHT = 300;
float UPDATE_TIMER = 60;

glm::mat4 pro = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
drawOBJ el;
std::vector<Shader*> shaders;
void init() {
/*
	unsigned int texture;
	std::map<GLchar, Character> Characters;
	FT_Face face;
	FT_Library ft;
	//FT_Library ft;
	if (FT_Init_FreeType(&ft)) { 
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl; 
	}
	//FT_Face face;
	if (FT_New_Face(ft, "./fonts/Lato-Bold.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl; 
	}
	FT_Set_Pixel_Sizes(face, 0, 12);
	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) { 
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl; 
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	
	for (unsigned char c = 0; c < 128; c++) {
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		}

		// generate texture
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}//end of the for
	glBindTexture(GL_TEXTURE_2D, 0);
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAOF);
	glGenBuffers(1, &VBOF);
	glBindVertexArray(VAOF);
	glBindBuffer(GL_ARRAY_BUFFER, VBOF);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	*/
	std::vector<float> vecs[] {
		{0.5f,  0.5f,		0.0f, 0.0f },
		{0.5f, -0.5f,       0.0f, 1.0f },
		{0.5f, -0.5f,       1.0f, 1.0f }
	};
	el.create(vecs);
	std::cout << "\n" << sizeof(vecs) << "\n" << sizeof(float)  << "\n" << typeid(vecs).name() << "\n" << typeid(vecs[0]).name() << "\n" << typeid(vecs[0][0]).name() << "\n";
	shaders.push_back(new Shader("./ttf.vec", "./ttf.frag"));

}
void draw() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	shaders[0]->use();
	glUniform3f(glGetUniformLocation(shaders[0]->ID, "textColor"), 1, 1, 1);
	shaders[0]->setVec3("textColor", 1, 0, 1);
	el.bind();
	el.draw();

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
	init();
	//draw
	glutDisplayFunc(draw);
	glutIdleFunc(draw);

	glutKeyboardFunc(normalKeysFunc);

	//update
	glutTimerFunc(1000.0 / UPDATE_TIMER, update, 0);

	//start looop
	glutMainLoop();
}