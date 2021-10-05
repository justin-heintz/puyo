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
drawOBJ el;
drawOBJ font_draw_obj;


glm::mat4 pro = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);

std::vector<Shader*> shaders;
float xm = 0.5; 
float ym = 0.5;
bool updateEL = true;
std::vector<float> flatvec{
0.0f, 0.0f, 0.0f, 0.0f,
0.0f, 0.0f, 0.0f, 0.0f, 
0.0f, 0.0f, 0.0f, 0.0f,
0.0f, 0.0f, 0.0f, 0.0f,
0.0f, 0.0f, 0.0f, 0.0f, 
0.0f, 0.0f, 0.0f, 0.0f
};
std::string text = "Hello World";
unsigned int texture;
std::map<GLchar, Character> Characters;
FT_Face face;
FT_Library ft;
float scale = 0.009f;
 
std::string::const_iterator c;
void setupFont() {
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	//FT_Face face;
	if (FT_New_Face(ft, "./fonts/Lato-Bold.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, 10);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
	}

	for (unsigned char cc = 0; cc < 128; cc++) {
		// load character glyph 
		if (FT_Load_Char(face, cc, FT_LOAD_RENDER)) {
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
		Characters.insert(std::pair<char, Character>(cc, character));
	}//end of the for

	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}
void init() {
	setupFont();
 
	font_draw_obj.createT(flatvec);
	el.createT(flatvec);
	
	shaders.push_back(new Shader("./shaders/triangle.vec", "./shaders/triangle.frag"));
	shaders.push_back(new Shader("./shaders/ttf.vec", "./shaders/ttf.frag"));
}
void draw() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	shaders[1]->use();
	shaders[1]->setVec3("textColor", glm::vec3(1.0f, 1.0f, 1.0f));

	// iterate through all characters
	font_draw_obj.bindVao();
	float xt = 0.0f + xm;
	float yt = 0.0f + ym;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		float xpos = xt + ch.Bearing.x * scale;
		float ypos = yt - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		std::vector<float> vertices{
			 xpos,     ypos + h,   0.0f, 0.0f ,
			 xpos,     ypos,       0.0f, 1.0f ,
			 xpos + w, ypos,       1.0f, 1.0f ,

			 xpos,     ypos + h,   0.0f, 0.0f ,
			 xpos + w, ypos,       1.0f, 1.0f ,
			 xpos + w, ypos + h,   1.0f, 0.0f 
		};
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		font_draw_obj.updateData(vertices);
	
		font_draw_obj.bindT();
		
		xt += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	//glBindTexture(GL_TEXTURE_2D, 0);


	shaders[0]->use();
	el.bindVao();
	el.bindT();

	glDisable(GL_BLEND);
    glutSwapBuffers();
}
void update(int) {
	glutPostRedisplay();
	if (updateEL) {
		std::vector<float> tmp = {
			(xm * -1), (ym * -1), 0.0f, 0.0f, // left  
			 xm, (ym * -1), 0.0f, 0.0f, // right 
			 xm - xm,  ym, 0.0f, 0.0f // top  
		}; 
		std::cout << xm << " - " << ym << "\n";
		el.updateData(tmp);
	}
	updateEL = false;
	glutTimerFunc(1000.0 / UPDATE_TIMER, update, 0);
}
void normalKeysFunc(unsigned char key,int x,int y) {
	std::cout << key << "\n";
	if (key == '0') {
		std::vector<float> flatvec{
			-0.6f, -0.6f, 0.0f, 0.0f, // left  
			 0.6f, -0.6f, 0.0f, 0.0f, // right 
			 0.0f,  0.5f, 0.0f, 0.0f // top  
		};
		el.updateData(flatvec);
	}
	if (key == '1') {}
	if (key == '2') {}
	if (key == '5') {}
	if (key == '6') {}
	if (key == '3') {}

	if (key == 'w') {
		ym = ym+ 0.1;
		updateEL = true;
	}
	if (key == 's') {
		ym = ym - 0.1;
		updateEL = true;
	}
	if (key == 'a') {
		xm = xm- 0.1;
		updateEL = true;
	}
	if (key == 'd') {
		xm = xm + 0.1;
		updateEL = true;
	}
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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
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