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
float UPDATE_TIMER = 10;
drawOBJ el, font_draw_obj, square, squarea, squareb, squarec, squared, squaree, squaref;

glm::mat4 pro = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);

float xm = 0.5; 
float ym = 0.5;

bool updateEL = true, updateDisplay = true;

std::vector<drawOBJ*> blocks;
std::vector<Shader*> shaders;
std::vector<float> flatvec(24);

std::map<GLchar, Character> Characters;
std::string::const_iterator c;

unsigned int texture;
FT_Face face;
FT_Library ft;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

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
	
	el.create(flatvec);
	
	float x = -1.0,  y = -1.0,  h = 0.1666666666,  w = 0.1666666666;
	for (int i = 0; i <6; i++) {
		blocks.push_back( new drawOBJ() );
		blocks[i]->create(flatvec);
		blocks[i]->setData(
			{
				x,     y,		0,0,
				x,     y + h,	0,0,
				x + w, y,		0,0,
				x + w, y + h,	0,0,
			}
		);
		x = x + w;
 		blocks[i]->setIndices({ 0,1,2, 1,2,3 });
	}

	square.create(flatvec);
	square.setData(
		{
			0.16, 0.16,	0,0,
			0.16, 0.32,	0,0,
			0.32, 0.16,	0,0,
			0.32, 0.32,	0,0,
		}
	);
	square.setIndices({ 0,1,2, 1,2,3 });


	shaders.push_back(new Shader("./shaders/triangle.vec", "./shaders/triangle.frag"));
	shaders.push_back(new Shader("./shaders/ttf.vec", "./shaders/ttf.frag"));
}
void draw() {
	if (updateDisplay) {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		for (float i = -1; i <= 1; i += 0.5) {
			for (float r = -0.9; r <= 1; r += 0.5) {
				std::string strx = std::to_string(i);
				strx.resize(4);
				std::string stry = std::to_string(r);
				stry.resize(4);

				float random_integer, b, c;
				float lowest = 0.1, highest = 1.0;
				float range = (highest - lowest) + 1;
				random_integer = lowest + float(range * rand() / (RAND_MAX + 1.0));
				b = lowest + float(range * rand() / (RAND_MAX + 1.0));
				c = lowest + float(range * rand() / (RAND_MAX + 1.0));
				//std::cout << random_integer << "-" << b << "-" << c << "\n";
				renderText("x:" + strx + " y:" + stry, i, r, 0.009f, glm::vec3(b, random_integer, c));

				renderText("x:" + strx + " y:" + stry, i, r, 0.002f, glm::vec3(b, random_integer, c));
			}
		}


		shaders[0]->use();
		
		
		for (int i = 0; i < 6; i++) {

			float random_integer, b, c;
			float lowest = 0.1, highest = 1.0;
			float range = (highest - lowest) + 1;
			random_integer = lowest + float(range * rand() / (RAND_MAX + 1.0));
			b = lowest + float(range * rand() / (RAND_MAX + 1.0));
			c = lowest + float(range * rand() / (RAND_MAX + 1.0));

			shaders[0]->setVec3("colorIN", glm::vec3(b, random_integer, c));
			blocks[i]->bindVao();
			blocks[i]->draw();
		}

		shaders[0]->setVec3("colorIN", glm::vec3(0.212f, 1.0f, 0.353f));
		el.bindVao();
		el.draw();


		updateDisplay = false;
		glutSwapBuffers();
	}
}
void update(int) {
	glutPostRedisplay();
	if (updateEL) {
		std::vector<float> tmp = {
			(xm * -1), (ym * -1), 0.0f, 0.0f, // left  
			 xm, (ym * -1), 0.0f, 0.0f, // right 
			 xm - xm,  ym, 0.0f, 0.0f // top  
		}; 
		el.setData(tmp);
	}
	updateEL = false;
	updateDisplay = true;
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
		el.setData(flatvec);
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