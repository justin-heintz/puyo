//c++ libs
#include <cstdlib>
#include <iostream>
#include <string>
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

//my libs
#include "drawobj.h"
#include "shader.h"

bool logAll = true;
#include "structs.h"

float WINDOW_WIDTH = 800;
float WINDOW_HEIGHT = 600;
float UPDATE_TIMER = 60;
bool REDRAW = true;

std::string DEV_OUTPUT = "";

int ii = 0;

//std::vector<drawOBJ*> drawObjs;
drawOBJ slimes[4];

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
float x = 1.0;
float y = 0.0;
float z = -24.0f;

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
void renderText(drawOBJ* ftd, std::string text, float x, float y, float size, glm::vec3 color = glm::vec3(0.0f, 1.0f, 1.0f)) {
	glm::mat4 projection = glm::ortho(0.0f, WINDOW_WIDTH, 0.0f, WINDOW_HEIGHT);
	
	shaders[1]->use();
	shaders[1]->setVec3("textColor", color);
	shaders[1]->setMat4("projection", projection);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * size;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * size;

		float w = ch.Size.x * size;
		float h = (ch.Size.y * size);
		// update VBO for each character
		float v1[] = {
			xpos, ypos + h, 0.0f,			0.0f, 0.0f,
			xpos, ypos, 0.0f,				0.0f, 1.0f,
			xpos + w, ypos, 0.0f,			1.0f, 1.0f,
			xpos + w, ypos + h, 0.0f,		1.0f, 0.0f,
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
	
	textObj.create();
	puyoBlock.create();

 
	for (int i = 0; i < std::size(slimes); i++) {
		slimes[i].create();
	}
	
	outputLog("init");
}
void draw() {
	//float x = 1.0;
	//float y = 0.0;

	if (REDRAW) {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		outputLog("draw", false);

		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

		//glm::mat4 view = glm::mat4(1.0f);
		//view = glm::translate(view, glm::vec3(x-100, 0.0, 0.0f));

		glm::mat4 projection;
		projection = projection = glm::perspective(45.0f, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);//glm::perspective(fov, aspect, near, far);

		shaders[0]->use();
		shaders[0]->setMat4("model", model);
		shaders[0]->setMat4("projection", projection);
		glDisable(GL_BLEND);//disable this or  everything is transparent
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures["puyo"]);

		for (int i = 0; i < std::size(slimes); i++) {

			glm::mat4 view = glm::mat4(1.0f);
			view = glm::translate(view, glm::vec3((x - (WINDOW_WIDTH/2)) * 0.15 , (y - (WINDOW_HEIGHT/2)) * - 0.15, z));
			shaders[0]->setMat4("view", view);
			slimes[i].draw();
		}

		renderText(&textObj, "abcdefghijklmnopqrstuvwxyz", 480.0f, 550.0f, 0.5, glm::vec3(1.0, 1.0, 1.0));
		renderText(&textObj, "Eat Fresh Bro", 480.0f, 500.0f, 1, glm::vec3(1.0, 1.0, 1.0));
		renderText(&textObj, "--" + std::to_string(ii) + std::to_string(true) + "--", 0, 500.0f, 1, glm::vec3(1.0, 1.0, 1.0));
		renderText(&textObj, DEV_OUTPUT, 0, 0, 1, glm::vec3(1.0, 1.0, 1.0));
		
		REDRAW = false;
		glutSwapBuffers();
	}
}
void update(int) {
	glutPostRedisplay();
	outputLog("update",false);
	ii++;
	REDRAW = true;
	glutTimerFunc(1000.0 / UPDATE_TIMER, update, 0);
}

void normalKeysFunc(unsigned char key, int x, int y) {
	DEV_OUTPUT = key;
	if (key == 'w') { z = z + 0.1; }
	if (key == 's') { z = z - 0.1; }
	if (key == 'a') {}
	if (key == 'd') {}
}
void mouseFunc(int button, int state, int mx, int my) {
	std::cout << button << " | " << state << " | " << x << " | " << y << " | \n";
	DEV_OUTPUT = button + " | " + std::to_string(state) + " | " + std::to_string(mx) + " | " + std::to_string(my) + " | " + std::to_string(z);

}
void mouseMoveFunc(int mx, int my) {
	std::cout << x << " | " << y << " | \n";
	DEV_OUTPUT = std::to_string(x) + " | " + std::to_string(y) + " | " + std::to_string(z);
	x =  mx;
	y =  my;
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("DEV");

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	glEnable(GL_MULTISAMPLE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction "WHAT DOES This Mean" ?
	glEnable(GL_TEXTURE_2D);
	

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
	//glutTimerFunc(100, cleartext, 0);

	//start looop
	glutMainLoop();
}
