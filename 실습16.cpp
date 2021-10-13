#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

using namespace std;

void initbuffer();
void make_vertexShaders();
void make_fragmentShaders();
void timer(int value);
void resize(int width, int height);
void specialkeycall(int key, int x, int y);
char* filetobuf(const string name);
GLuint make_shaderProgram();
GLvoid reshape(int w, int h);
GLvoid drawscene(GLvoid);
GLvoid keyboard(unsigned char key, int x, int y);

bool check = false;
bool model_change = true;
bool rotate_action = false;

int windowWidth, windowHeight;

int rotate_count = 0;
int hexa_rotate_x = 0;
int hexa_rotate_X = 0;
int hexa_rotate_y = 0;
int hexa_rotate_Y = 0;

int quad_rotate_x = 0;
int quad_rotate_X = 0;
int quad_rotate_y = 0;
int quad_rotate_Y = 0;

int axes_rotate_y = 0;
int axes_rotate_Y = 0;

float hexa_trans_x = -0.5f;
float hexa_trans_y = 0.25f;
float hexa_trans_z = 0.0f;
float quad_trans_x = 0.5f;
float quad_trans_y = 0.25f;
float quad_trans_z = 0.0f;

float hexa_scale_x = 1.0f;
float hexa_scale_y = 1.0f;
float hexa_scale_z = 1.0f;
float quad_scale_x = 1.0f;
float quad_scale_y = 1.0f;
float quad_scale_z = 1.0f;

float hexa_angle_x = 0.0f;
float hexa_angle_y = 0.0f;
float quad_angle_x = 0.0f;
float quad_angle_y = 0.0f;

float axes_scale_x = 1.0f;
float axes_scale_y = 1.0f;
float axes_scale_z = 1.0f;

float axes_angle_x = 30.0f;
float axes_angle_y = -30.0f;

float hexa_axes_angle_y = -30.0f;
float quad_axes_angle_y = -30.0f;


const GLchar* vertexShaderSource;
const GLchar* fragmentShaderSource;
GLUquadricObj* qobj1;
GLUquadricObj* qobj2;
GLuint VAO[3], VBO[6];
GLuint shaderID;
GLuint vertexshader;
GLuint fragmentshader;
GLuint triangleshaderProgram;

float make_line[] = {
	0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,

	-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,

	0.0f, 0.0f, 1.0f, 0.5f, 0.0f, 0.5f,
	0.0f, 0.0f, -1.0f, 0.5f, 0.0f, 0.5f
};

float make_hexahedron[] = {
	-0.25f, 0.25f, 0.25f, 1.0f, 0.0f, 0.0f,
	0.25f, 0.25f, 0.25f, 0.0f, 1.0f, 0.0f,
	-0.25f, 0.25f, -0.25f, 0.5f, 0.5f, 0.5f,

	0.25f, 0.25f, 0.25f, 0.0f, 1.0f, 0.0f,
	0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 1.0f,
	-0.25f, 0.25f, -0.25f, 0.5f, 0.5f, 0.5f,

	-0.25f, -0.25f, 0.25f, 0.0f, 0.0f, 0.0f,
	-0.25f, -0.25f, -0.25f, 0.3f, 0.0f, 0.7f,
	0.25f, -0.25f, -0.25f, 0.5f, 0.3f, 0.7f,

	-0.25f, -0.25f, 0.25f, 0.0f, 0.0f, 0.0f,
	0.25f, -0.25f, -0.25f, 0.5f, 0.3f, 0.7f,
	0.25f, -0.25f, 0.25f, 0.0f, 1.0f, 1.0f,

	-0.25f, -0.25f, 0.25f, 0.0f, 0.0f, 0.0f,
	0.25f, -0.25f, 0.25f, 0.0f, 1.0f, 1.0f,
	-0.25f, 0.25f, 0.25f, 1.0f, 0.0f, 0.0f,

	0.25f, -0.25f, 0.25f, 0.0f, 1.0f, 1.0f,
	0.25f, 0.25f, 0.25f, 0.0f, 1.0f, 0.0f,
	-0.25f, 0.25f, 0.25f, 1.0f, 0.0f, 0.0f,

	0.25f, -0.25f, -0.25f, 0.5f, 0.3f, 0.7f,
	-0.25f, -0.25f, -0.25f, 0.3f, 0.0f, 0.7f,
	0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 1.0f,

	-0.25f, -0.25f, -0.25f, 0.3f, 0.0f, 0.7f,
	-0.25f, 0.25f, -0.25f, 0.5f, 0.5f, 0.5f,
	0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 1.0f,

	-0.25f, -0.25f, -0.25f, 0.3f, 0.0f, 0.7f,
	-0.25f, 0.25f, 0.25f, 1.0f, 0.0f, 0.0f,
	-0.25f, 0.25f, -0.25f, 0.5f, 0.5f, 0.5f,

	-0.25f, -0.25f, -0.25f, 0.3f, 0.0f, 0.7f,
	-0.25f, -0.25f, 0.25f, 0.0f, 0.0f, 0.0f,
	-0.25f, 0.25f, 0.25f, 1.0f, 0.0f, 0.0f,

	0.25f, -0.25f, 0.25f, 0.0f, 1.0f, 1.0f,
	0.25f, -0.25f, -0.25f, 0.5f, 0.3f, 0.7f,
	0.25f, 0.25f, 0.25f, 0.0f, 1.0f, 0.0f,

	0.25f, -0.25f, -0.25f, 0.5f, 0.3f, 0.7f,
	0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 1.0f,
	0.25f, 0.25f, 0.25f, 0.0f, 1.0f, 0.0f
};

float make_quadrant[] = {
	-0.3f, -0.3f, -0.3f, 0.5f, 0.5f, 0.5f,  //1
	0.0f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,  //0
	0.3f, -0.3f, -0.3f, 1.0f, 0.0f, 0.3f,  //2

	0.3f, -0.3f, -0.3f, 1.0f, 0.0f, 0.3f,  //2
	0.0f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,  //0
	0.3f, -0.3f, 0.3f, 0.0f, 0.7f, 1.0f,  //3

	0.3f, -0.3f, 0.3f, 0.0f, 0.7f, 1.0f,  //3
	0.0f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,  //0
	-0.3f, -0.3f, 0.3f, 1.0f, 1.0f, 0.12f,  //4

	-0.3f, -0.3f, -0.3f, 0.5f, 0.5f, 0.5f,  //1
	-0.3f, -0.3f, 0.3f, 1.0f, 1.0f, 0.12f,  //4
	0.0f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,  //0

	-0.3f, -0.3f, 0.3f, 1.0f, 1.0f, 0.12f,  //4
	-0.3f, -0.3f, -0.3f, 0.5f, 0.5f, 0.5f,  //1
	0.3f, -0.3f, -0.3f, 1.0f, 0.0f, 0.3f,  //2

	-0.3f, -0.3f, 0.3f, 1.0f, 1.0f, 0.12f,  //4
	0.3f, -0.3f, -0.3f, 1.0f, 0.0f, 0.3f,  //2
	0.3f, -0.3f, 0.3f, 0.0f, 0.7f, 1.0f  //3
};

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 600);
	glutCreateWindow("example1");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else {
		cout << "GLEW initialized" << endl;
	}
	make_vertexShaders();
	make_fragmentShaders();
	shaderID = make_shaderProgram();
	initbuffer();
	glutDisplayFunc(drawscene);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(1, timer, 1);
	glutReshapeFunc(resize);
	glutMainLoop();
}
void make_vertexShaders() {
	GLchar* vertexsource;

	vertexsource = filetobuf("vertexshader.glsl");

	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, &vertexsource, NULL);
	glCompileShader(vertexshader);

	GLint result;
	GLchar errorlog[512];
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vertexshader, 512, NULL, errorlog);
		cerr << "Error:vertexshader 컴파일 실패" << errorlog << endl;
	}
}
void make_fragmentShaders() {
	GLchar* fragmentsource;
	fragmentsource = filetobuf("fragmentshader.glsl");

	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentshader);

	GLint result;
	GLchar errorlog[512];
	glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fragmentshader, 512, NULL, errorlog);
		cerr << "Error:fragmentshader 컴파일 실패" << errorlog << endl;
	}
}
char* filetobuf(const string name)
{
	vector<char> tempFile;
	ifstream in(name, ios::binary);
	char temp;
	while (true) {
		noskipws(in);
		in >> temp;
		if (in.eof()) {
			tempFile.push_back(0);
			break;
		}
		else
			tempFile.push_back(temp);
	}
	char* addr = new char[tempFile.size()];
	for (int i = 0; i < tempFile.size(); i++) {
		addr[i] = tempFile[i];
	}
	return addr;
}
GLuint make_shaderProgram() {

	GLuint shaderprogramID = glCreateProgram();

	glAttachShader(shaderprogramID, vertexshader);
	glAttachShader(shaderprogramID, fragmentshader);

	glLinkProgram(shaderprogramID);

	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);

	GLint result;
	GLchar errorlog[512];

	glGetProgramiv(shaderprogramID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(triangleshaderProgram, 512, NULL, errorlog);
		cerr << "Error: shader program 연결 실패" << errorlog << endl;
	}
	glUseProgram(shaderprogramID);
	return shaderprogramID;
}

GLvoid drawscene() {
	glClearColor(0.5, 0.5, 0.5, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	initbuffer();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(shaderID);
	glm::mat4 linemodel = glm::mat4(1.0f);
	linemodel = glm::rotate(linemodel, glm::radians(axes_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
	linemodel = glm::rotate(linemodel, glm::radians(axes_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
	linemodel = glm::scale(linemodel, glm::vec3(axes_scale_x, axes_scale_y, axes_scale_z));
	unsigned int linemodelLocation = glGetUniformLocation(shaderID, "modeltransform");
	glUniformMatrix4fv(linemodelLocation, 1, GL_FALSE, glm::value_ptr(linemodel));
	glBindVertexArray(VAO[0]);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, 6);
	if (model_change) {
		glm::mat4 hexahedronmodel = glm::mat4(1.0f);
		hexahedronmodel = glm::scale(hexahedronmodel, glm::vec3(axes_scale_x, axes_scale_y, axes_scale_z));
		if (rotate_action) {
			hexahedronmodel = glm::rotate(hexahedronmodel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			hexahedronmodel = glm::rotate(hexahedronmodel, glm::radians(hexa_axes_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			hexahedronmodel = glm::rotate(hexahedronmodel, glm::radians(axes_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
			hexahedronmodel = glm::rotate(hexahedronmodel, glm::radians(axes_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		hexahedronmodel = glm::translate(hexahedronmodel, glm::vec3(hexa_trans_x, hexa_trans_y, hexa_trans_z));
		hexahedronmodel = glm::rotate(hexahedronmodel, glm::radians(hexa_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
		hexahedronmodel = glm::rotate(hexahedronmodel, glm::radians(hexa_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		hexahedronmodel = glm::scale(hexahedronmodel, glm::vec3(hexa_scale_x, hexa_scale_y, hexa_scale_z));
		unsigned int hexahedronmodelLocation = glGetUniformLocation(shaderID, "modeltransform");
		glUniformMatrix4fv(hexahedronmodelLocation, 1, GL_FALSE, glm::value_ptr(hexahedronmodel));
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//glm::mat4 quadrantmodel = glm::mat4(1.0f);
		//quadrantmodel = glm::scale(quadrantmodel, glm::vec3(axes_scale_x, axes_scale_y, axes_scale_z));
		//if (rotate_action) {
		//	quadrantmodel = glm::rotate(quadrantmodel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//	quadrantmodel = glm::rotate(quadrantmodel, glm::radians(quad_axes_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		//}
		//else {
		//	quadrantmodel = glm::rotate(quadrantmodel, glm::radians(axes_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
		//	quadrantmodel = glm::rotate(quadrantmodel, glm::radians(axes_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		//}
		//quadrantmodel = glm::translate(quadrantmodel, glm::vec3(quad_trans_x, quad_trans_y, quad_trans_z));
		//quadrantmodel = glm::rotate(quadrantmodel, glm::radians(quad_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
		//quadrantmodel = glm::rotate(quadrantmodel, glm::radians(quad_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		//quadrantmodel = glm::scale(quadrantmodel, glm::vec3(quad_scale_x, quad_scale_y, quad_scale_z));
		//unsigned int quadrantmodelLocation = glGetUniformLocation(shaderID, "modeltransform");
		//glUniformMatrix4fv(quadrantmodelLocation, 1, GL_FALSE, glm::value_ptr(quadrantmodel));
		//glBindVertexArray(VAO[2]);
		//glDrawArrays(GL_TRIANGLES, 0, 18);
	}
	else {
		qobj1 = gluNewQuadric();
		gluQuadricDrawStyle(qobj1, GLU_LINE);

		glm::mat4 spheremodel = glm::mat4(1.0f);
		spheremodel = glm::rotate(spheremodel, glm::radians(axes_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
		spheremodel = glm::rotate(spheremodel, glm::radians(axes_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		spheremodel = glm::translate(spheremodel, glm::vec3(-0.5f, 0.25f, 0.0f));
		spheremodel = glm::rotate(spheremodel, glm::radians(hexa_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
		spheremodel = glm::rotate(spheremodel, glm::radians(hexa_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		spheremodel = glm::scale(spheremodel,  glm::vec3(0.0f, 1.0f, 0.0f));
		unsigned int spheremodellLocation = glGetUniformLocation(shaderID, "modeltransform");
		glUniformMatrix4fv(spheremodellLocation, 1, GL_FALSE, glm::value_ptr(spheremodel));
		gluSphere(qobj1, 0.25, 20, 20);

		qobj2 = gluNewQuadric();
		gluQuadricDrawStyle(qobj2, GLU_LINE);
		glm::mat4 cylindermodel = glm::mat4(1.0f);
		cylindermodel = glm::rotate(cylindermodel, glm::radians(axes_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
		cylindermodel = glm::rotate(cylindermodel, glm::radians(axes_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		cylindermodel = glm::translate(cylindermodel, glm::vec3(0.5f, 0.3f, 0.0f));
		cylindermodel = glm::rotate(cylindermodel, glm::radians(quad_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
		cylindermodel = glm::rotate(cylindermodel, glm::radians(quad_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
		unsigned int cylindermodelLocation = glGetUniformLocation(shaderID, "modeltransform");
		glUniformMatrix4fv(cylindermodelLocation, 1, GL_FALSE, glm::value_ptr(cylindermodel));
		gluCylinder(qobj2, 0.25, 0.25, 0.5, 20, 8);
	}
	glutSwapBuffers();
}

GLvoid reshape(int w, int h) {
	glViewport(0, 0, 800, 600);
}

void resize(int width, int height) {
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, width, height);
}

void initbuffer() {

	glGenVertexArrays(3, VAO);

	glBindVertexArray(VAO[0]);
	glGenBuffers(2, &VBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(make_line), make_line, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(make_line), make_line, GL_STREAM_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(VAO[1]);
	glGenBuffers(2, &VBO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(make_hexahedron), make_hexahedron, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(make_hexahedron), make_hexahedron, GL_STREAM_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO[2]);
	glGenBuffers(2, &VBO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(make_quadrant), make_quadrant, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(make_quadrant), make_quadrant, GL_STREAM_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

GLvoid keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'c':
		check = false;
		rotate_action = false;
		model_change = !model_change;
		hexa_rotate_x = 0;
		hexa_rotate_X = 0;
		hexa_rotate_y = 0;
		hexa_rotate_Y = 0;
		quad_rotate_x = 0;
		quad_rotate_X = 0;
		quad_rotate_y = 0;
		quad_rotate_Y = 0;
		axes_rotate_y = 0;
		axes_rotate_Y = 0;
		break;
	case 'x':
		hexa_rotate_x++;
		hexa_rotate_X = 0;
		check = true;
		break;
	case 'X':
		hexa_rotate_X++;
		hexa_rotate_x = 0;
		check = true;
		break;
	case 'y':
		hexa_rotate_y++;
		hexa_rotate_Y = 0;
		check = true;
		break;
	case 'Y':
		hexa_rotate_Y++;
		hexa_rotate_y = 0;
		check = true;
		break;
	case 'a':
		quad_rotate_x++;
		quad_rotate_X = 0;
		check = true;
		break;
	case 'A':
		quad_rotate_X++;
		quad_rotate_x = 0;
		check = true;
		break;
	case 'b':
		quad_rotate_y++;
		quad_rotate_Y = 0;
		check = true;
		break;
	case 'B':
		quad_rotate_Y++;
		quad_rotate_y = 0;
		check = true;
		break;
	case 'r':
		axes_rotate_y++;
		axes_rotate_Y = 0;
		check = true;
		break;
	case 'R':
		axes_rotate_Y++;
		axes_rotate_y = 0;
		check = true;
		break;
	case '1':
		hexa_scale_x += 0.02f;
		hexa_scale_y += 0.02f;
		hexa_scale_z += 0.02f;
		break;
	case '2':
		hexa_scale_x -= 0.02f;
		hexa_scale_y -= 0.02f;
		hexa_scale_z -= 0.02f;
		break;
	case '3':
		quad_scale_x += 0.02f;
		quad_scale_y += 0.02f;
		quad_scale_z += 0.02f;
		break;
	case '4':
		quad_scale_x -= 0.02f;
		quad_scale_y -= 0.02f;
		quad_scale_z -= 0.02f;
		break;
	case '5':
		axes_scale_x += 0.02f;
		axes_scale_y += 0.02f;
		axes_scale_z += 0.02f;
		break;
	case '6':
		axes_scale_x -= 0.02f;
		axes_scale_y -= 0.02f;
		axes_scale_z -= 0.02f;
		break;
	case 'v':
		hexa_trans_x += 0.02f;
		quad_trans_x += 0.02f;
		break;
	case 'V':
		hexa_trans_x -= 0.02f;
		quad_trans_x -= 0.02f;
		break;	
	case 'n':
		hexa_trans_y += 0.02f;
		quad_trans_y += 0.02f;
		break;
	case 'N':
		hexa_trans_y -= 0.02f;
		quad_trans_y -= 0.02f;
		break;
	case 'm':
		hexa_trans_z += 0.02f;
		quad_trans_z += 0.02f;
		break;
	case 'M':
		hexa_trans_z -= 0.02f;
		quad_trans_z -= 0.02f;
		break;
	case 't':
		check = true;
		rotate_action = true;
		break;

	case 's':
		check = false;
		rotate_action = false;
		hexa_angle_x = 0.0f;
		hexa_angle_y = 0.0f;
		quad_angle_x = 0.0f;
		quad_angle_y = 0.0f;
		axes_angle_x = 30.0f;
		axes_angle_y = -30.0f;
		hexa_rotate_x = 0;
		hexa_rotate_X = 0;
		hexa_rotate_y = 0;
		hexa_rotate_Y = 0;
		quad_rotate_x = 0;
		quad_rotate_X = 0;
		quad_rotate_y = 0;
		quad_rotate_Y = 0;
		axes_rotate_y = 0;
		axes_rotate_Y = 0;
		break;
	case 'q':
		exit(0);
		cout << "exit the program" << endl;
		break;
	default:
		break;
	}
	glutPostRedisplay();

}

void timer(int value) {
	if (check) {
		if (rotate_action) {
			if (rotate_count == 0) {
				hexa_trans_x = -0.5f;
				hexa_trans_y = 0.25f;
				hexa_trans_z = 0.0f;
				
				quad_trans_x = 0.5f;
				quad_trans_y = 0.25f;
				quad_trans_z = 0.0f;

				hexa_angle_x = 0.0f;
				hexa_angle_y = 0.0f;
				
				quad_angle_x = 0.0f;
				quad_angle_y = 0.0f;
				rotate_count = 1;
			}
			else {
				if ((((int)hexa_axes_angle_y % 180) == 0) || (((int)quad_axes_angle_y%180)==0)) {
					rotate_count++;
					
				}
				if (rotate_count == 9) {
					rotate_count = 0;
				}
				hexa_trans_x += 0.001f;
				hexa_trans_z += 0.001f;

				


				
				hexa_axes_angle_y++;
				quad_axes_angle_y++;
			}
		}
		else {
			//육면체
			if (hexa_rotate_x > 0) {
				if (hexa_rotate_x % 2 == 1) {
					hexa_angle_x++;
				}
				else if (hexa_rotate_x % 2 == 0) {
					hexa_rotate_x = 0;
				}
			}
			if (hexa_rotate_X > 0) {
				if (hexa_rotate_X % 2 == 1) {
					hexa_angle_x--;
				}
				else if (hexa_rotate_X % 2 == 0) {
					hexa_rotate_X = 0;
				}
			}
			if (hexa_rotate_y > 0) {
				if (hexa_rotate_y % 2 == 1) {
					hexa_angle_y++;
				}
				else if (hexa_rotate_y % 2 == 0) {
					hexa_rotate_y = 0;
				}
			}
			if (hexa_rotate_Y > 0) {
				if (hexa_rotate_Y % 2 == 1) {
					hexa_angle_y--;
				}
				else if (hexa_rotate_Y % 2 == 0) {
					hexa_rotate_y = 0;
				}
			}
			//사각뿔
			if (quad_rotate_x > 0) {
				if (quad_rotate_x % 2 == 1) {
					quad_angle_x++;
				}
				else if (quad_rotate_x % 2 == 0) {

					quad_rotate_x = 0;
				}
			}
			if (quad_rotate_X > 0) {
				if (quad_rotate_X % 2 == 1) {
					quad_angle_x--;
				}
				else if (quad_rotate_X % 2 == 0) {
					quad_rotate_X = 0;
				}
			}
			if (quad_rotate_y > 0) {
				if (quad_rotate_y % 2 == 1) {
					quad_angle_y++;
				}
				else if (quad_rotate_y % 2 == 0) {
					quad_rotate_y = 0;
				}
			}
			if (quad_rotate_Y > 0) {
				if (quad_rotate_Y % 2 == 1) {
					quad_angle_y--;
				}
				else if (quad_rotate_Y % 2 == 0) {
					quad_rotate_y = 0;
				}
			}
			//공전
			if (axes_rotate_y > 0) {
				if (axes_rotate_y % 2 == 1) {
					axes_angle_y++;
				}
				else if (axes_rotate_y % 2 == 0) {
					axes_rotate_y = 0;
				}
			}
			if (axes_rotate_Y > 0) {
				if (axes_rotate_Y % 2 == 1) {
					axes_angle_y--;
				}
				else if (axes_rotate_Y % 2 == 0) {
					axes_rotate_y = 0;
				}
			}
		}
	}
	glutPostRedisplay();
	glutTimerFunc(60, timer, 1);
}