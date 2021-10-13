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

float BLOCK_WIDTH = 0.1666666666;
float BLOCK_HEIGHT = 0.1666666666;

float UPDATE_TIMER = 10;

drawOBJ font_draw_obj;

bool updateEL = true, updateDisplay = true;

std::vector<drawOBJ*> blocks;
std::vector<Shader*> shaders;
std::vector<float> flatvec(24);

std::map<GLchar, Character> Characters;
std::string::const_iterator c;

unsigned int texture;
FT_Face face;
FT_Library ft;

void setupFont() {
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	//FT_Face face;
	if (FT_New_Face(ft, "./fonts/Lato-Bold.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

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
void renderText(std::string text, float x, float y, float size, glm::vec3 color = glm::vec3(0.0f, 1.0f, 1.0f )) {
	shaders[1]->use();
	shaders[1]->setVec3("textColor", color);
	font_draw_obj.bindVao();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * size;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * size;

		float w = ch.Size.x * size;
		float h = ch.Size.y * size;
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
		font_draw_obj.setData(vertices);
		font_draw_obj.draw();
		x += (ch.Advance >> 6) * size; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void init() {
	setupFont();
 	font_draw_obj.create(flatvec);
	font_draw_obj.setIndices({0,1,2,0,5,2});
 
	float x = -1.0,  y = -1.0,  h = BLOCK_HEIGHT,  w = BLOCK_WIDTH;
	for (int i = 0; i <6; i++) {
		blocks.push_back( new drawOBJ() );
		blocks[i]->create({
				x,     y,		0,0,
				x,     y + h,	0,0,
				x + w, y,		0,0,
				x + w, y + h,	0,0,
			});
		x = x + w;
 		blocks[i]->setIndices({ 0,1,2, 1,2,3 });
	}

	//load shaders
	shaders.push_back(new Shader("./shaders/triangle.vec", "./shaders/triangle.frag"));
	shaders.push_back(new Shader("./shaders/ttf.vec", "./shaders/ttf.frag"));
}
void draw() {
	if (updateDisplay) {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		shaders[0]->use();
		
		for (int i = 0; i < 6; i++) {
			shaders[0]->setVec3("colorIN", glm::vec3(1.0,0.1,0.5));
			blocks[i]->bindVao();
			blocks[i]->draw();
		}

		updateDisplay = false;
		glutSwapBuffers();
	}
}
void update(int) {
	glutPostRedisplay();
	updateDisplay = true;
	std::cout << "\n";
	glutTimerFunc(1000.0 / UPDATE_TIMER, update, 0);
}
void normalKeysFunc(unsigned char key,int x,int y) {
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