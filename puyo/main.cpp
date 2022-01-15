//c++ libs
#include <cstdlib>
#include <iostream>
#include <string>
#include <iterator>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>
#include <map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

#include <stdlib.h>
#include <time.h>

//my libs
#include "drawobj.h"
#include "shader.h"
#include "structs.h"

#include "grid.h"
#include "slime.h"


float WINDOW_WIDTH = 800;
float WINDOW_HEIGHT = 600;
float UPDATE_TIMER = 60;
bool REDRAW = true;

std::string DEV_OUTPUT = "";

//std::vector<drawOBJ*> drawObjs;
drawOBJ slimes[6];

std::vector<Shader*> shaders;

drawOBJ textObj;
drawOBJ puyoBlock;
std::map<std::string, unsigned int> textures;
std::map<GLchar, Character> Characters;
std::string::const_iterator c;
unsigned int texture;

//FONT LIB
FT_Face face;
FT_Library ft;

//DEV CRAP
float x = 400.0;
float y = 300.0;
float z = -30.0f;
int iSecret = rand() % 5 + 1;

grid gameBoard;
slime first;
vector<slime>sls;
bool lines = true;
void setupFont() {
	if (FT_Init_FreeType(&ft)) {
		outputLog("ERROR::FREETYPE: Could not init FreeType Library");
	}
	if (FT_New_Face(ft, "./fonts/Lato-Bold.ttf", 0, &face)) {
		outputLog("ERROR::FREETYPE: Failed to load font");
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		outputLog("ERROR::FREETYTPE: Failed to load Glyph");
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
void renderText(drawOBJ* ftd, std::string text, float x, float y, float size, glm::vec3 color = glm::vec3(0.0f, 1.0f, 1.0f)) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::mat4 projection = glm::ortho(0.0f, WINDOW_WIDTH, 0.0f, WINDOW_HEIGHT);
	
	shaders[1]->use();
	shaders[1]->setVec3("textColor", color);
	shaders[1]->setMat4("projection", projection);

	glActiveTexture(GL_TEXTURE0);

	
	
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * size;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * size;

		float w = ch.Size.x * size;
		float h = (ch.Size.y * size);
		// update VBO for each character
		float v1[] = {
			xpos, ypos + h, 			0.0f, 0.0f,
			xpos, ypos, 				0.0f, 1.0f,
			xpos + w, ypos, 			1.0f, 1.0f,
			xpos + w, ypos + h, 		1.0f, 0.0f,
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		ftd->setData(v1);
		
		ftd->draw();

		x += (ch.Advance >> 6) * size; // bitshift by 6 to get value in pixels (2^6 = 64)
	}

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
	
}

void genTexture(std::string name, std::string path) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

	if (data) {
		printf("Texture %s is loaded With %i channels  \n", name.c_str(), nrChannels);

		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}else{ 
		printf("Failed to load texture\n");
	}

	textures.insert(std::pair<std::string, unsigned int>(name, texture));
	printf("%s is mapped to ID %u \n\n", name.c_str(), textures[name]);
}

void init() {
	setupFont();
	
	//load shaders
	shaders.push_back(new Shader("./shaders/triangle.vec", "./shaders/triangle.frag"));
	shaders.push_back(new Shader("./shaders/ttf.vec", "./shaders/ttf.frag"));

	genTexture("puyo","textures/puyo.png");
	genTexture("test","textures/test.png");
	genTexture("ani","textures/animation_test.png");
		
	sls.push_back(slime());
	sls[0].create();
	sls[0].updateCords(glm::vec2(400,300));
	textObj.create();
 
	for (int i = 0; i < std::size(slimes); i++) {
		slimes[i].create();
	}
}
void draw() {
	if (REDRAW) {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (!lines) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}else{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		//rotate: translate: scale

		// 570d1d RED vec3(0.341,0.051,0.114)
		// 0a421e GREEN vec3(0.039,0.259,0.118)
		// 2545a0 BLUE vec3(0.145,0.271,0.627)
		// 604000 YELLOW vec3(0.376,0.251,0.)
		// 4d17b7 PURPLE vec3(0.302,0.09,0.718)

		glm::mat4 projection;
		projection = projection = glm::perspective(45.0f, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);//glm::perspective(fov, aspect, near, far);

		shaders[0]->use();

		switch (rand() % 5 + 1) {
		case 1:
			shaders[0]->setVec4("colorIn", glm::vec4(1.0, 1.0, 0.0, 1.0));
		break;
		case 2:
			shaders[0]->setVec4("colorIn", glm::vec4(0.039, 0.259, 0.118, 1.0));
		break;
		case 3:
			shaders[0]->setVec4("colorIn", glm::vec4(0.145, 0.271, 0.627, 1.0));
		break;
		case 4:
			shaders[0]->setVec4("colorIn", glm::vec4(0.302, 0.09, 0.718, 1.0));
		break;
		case 5:
			shaders[0]->setVec4("colorIn", glm::vec4(0.376, 0.251, 0.0, 1.0));
		break;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures["ani"]);

		for (int i = 0; i < std::size(slimes); i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(x + (2 * i ), 1.0f, 1.0f));
			shaders[0]->setMat4("model", model);
			
			glm::mat4 view = glm::mat4(1.0f);
			view = glm::translate(view, glm::vec3((x - (WINDOW_WIDTH / 2)) * 0.15, (y - (WINDOW_HEIGHT / 2)) * -0.15, z));
			shaders[0]->setMat4("view", view);
			
			shaders[0]->setMat4("projection", projection);
			slimes[i].draw();
		}

		shaders[0]->use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
		shaders[0]->setMat4("model", model);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3((sls[0].cords.x - (WINDOW_WIDTH / 2)) , (sls[0].cords.y - (WINDOW_HEIGHT / 2)) , z));
		shaders[0]->setMat4("view", view);

		shaders[0]->setMat4("projection", projection);
		sls[0].image.draw();

		renderText(&textObj, "abcdefghijklmnopqrstuvwxyz", 480.0f, 550.0f, 0.5, glm::vec3(0.341, 0.051, 0.114));
		renderText(&textObj, "Eat Fresh Bro", 480.0f, 500.0f, 1, glm::vec3(1.0, 1.0, 1.0));
		

		REDRAW = false;
		glutSwapBuffers();
	}
}

void updateFrames(int) {
	for (int i = 0; i < std::size(slimes); i++) {
		slimes[i].updateTexturePos();
	}
	sls[0].image.updateTexturePos();
	glutTimerFunc(1000.0 /3, updateFrames, 0);
}
void update(int) {
	/*
	float xpos = 0, ypos = -20, w = 20, h=20;
	float xpos = 0, ypos = 0, w = 0, h = 0;
	float v1[] =
	{
		xpos, ypos + h,		  0.0f, 1.0f,
		xpos, ypos,			  0.0f, 0.0f,
		xpos + w, ypos,		  1.0f, 0.0f,
		xpos + w, ypos + h,   1.0f, 1.0f
	};

	sls[0].image.setData(v1);
	*/
	first.cords.x += 0.05;
	first.updateCords(glm::vec2(0.1,1.1));
	x = first.cords.x;
	y = first.cords.y;
	glutPostRedisplay();
	
	// initialize random seed:
	iSecret = rand() % 5 + 1;
	srand(time(NULL));
	//printf("x:%f, y:%f \n", x, y);
	REDRAW = true;
	glutTimerFunc(1000.0 / UPDATE_TIMER, update, 0);
}

void normalKeysFunc(unsigned char key, int x, int y) {
	DEV_OUTPUT = key;
	if (key == 'w') { z = z + 0.9; }
	if (key == 's') { z = z - 0.9; }
	if (key == 'a') { lines = false; printf("%d \n",lines); }
	if (key == 'd') { lines = true; printf("%d \n", lines);	}
}

void mouseFunc(int button, int state, int mx, int my) {
	DEV_OUTPUT = button + " | " + std::to_string(state) + " | " + std::to_string(mx) + " | " + std::to_string(my) + " | " + std::to_string(z);
}
void mouseMoveFunc(int mx, int my) {
	DEV_OUTPUT = std::to_string(x) + " | " + std::to_string(y) + " | " + std::to_string(z);
	//x =  mx;
	//y =  my;
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(200, 200);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("DEV");

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction "WHAT DOES This Mean" ?
	
	
	glewInit();
	init();
	
	//IO
	glutKeyboardFunc(normalKeysFunc);
	glutMouseFunc(mouseFunc);
	glutPassiveMotionFunc(mouseMoveFunc);

	//draw
	glutDisplayFunc(draw);
	glutIdleFunc(draw);

	//update
	glutTimerFunc(1000.0 / UPDATE_TIMER, update, 0);
	glutTimerFunc(1000.0 / 3, updateFrames, 0);

	//start looop
	glutMainLoop();
}
