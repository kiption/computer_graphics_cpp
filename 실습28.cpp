#define STB_IMAGE_IMPLEMENTATION
#include "readObj.h"
#include "camera.h"
#include "Light.h"
#include "stb_image.h"
#include <vector>
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <Windows.h>
#include <fstream>
#include <random>
#include <string>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <time.h>

using namespace std;

//vertex shader programs
void initbuffer();
void initTexture();
void make_vertexShaders();
void make_fragmentShaders();
char* filetobuf(const string name);
GLuint make_shaderProgram();

//main loop funtions
void timer(int value);
void resize(int width, int height);
GLvoid drawscene(GLvoid);
GLvoid reshape(int w, int h);
GLvoid keyboard(unsigned char key, int x, int y);
void drawmodels();

int windowWidth, windowHeight;

float CameraTheta = 0.0f;
float ObjectThetaX = 0.0f;
float ObjectThetaY = 0.0f;
float LightRaidian = 0.0f;
float SquareRotate = 0.0f;
float SquareRevolute = 0.0f;
//스위치 불변수

bool CameraRotationB = false;
bool CameraRotationA = false;
bool SquareRotateA = false;
bool SquareRotateB = false;
bool SquareRevoluteA = false;
bool SquareRevoluteB = false;

bool FirstPersonView = false;
bool ChangeModel = false;

//조명 초기위치
float ScaleZ = 5.0f;
float ScaleY = 1.0f;

unsigned int cubetexture[2];

CameraViewPoint PrimeCameraPoint;

GLuint VAO[2], VBO[6];
GLuint shaderID;
GLuint qobjshader;
GLuint vertexshader;
GLuint fragmentshader;

vector<glm::vec4>ModelsValue[2];
vector<glm::vec4>normalValue[2];
vector<glm::vec2>vtValue[2];

glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600);
    glutCreateWindow("example3");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else {
        cout << "GLEW initialized" << endl;
    }

    ReadObj("cube3_.obj", ModelsValue[0],normalValue[0], vtValue[0]);
    ReadObj("pyramid.obj", ModelsValue[1],normalValue[1], vtValue[1]);
    
    make_vertexShaders();
    make_fragmentShaders();
    make_shaderProgram();
    initbuffer();
    initTexture();

    glutDisplayFunc(drawscene);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(1, timer, 1);
    glutReshapeFunc(resize);
    glutMainLoop();
}

void make_vertexShaders() {
    GLchar* vertexsource2;
    vertexsource2 = filetobuf("qobjvertexshader.glsl");

    qobjshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(qobjshader, 1, &vertexsource2, NULL);
    glCompileShader(qobjshader);

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

    shaderID = glCreateProgram();

    glAttachShader(shaderID, qobjshader);
    glAttachShader(shaderID, fragmentshader);

    glLinkProgram(shaderID);

    glDeleteShader(qobjshader);
    glDeleteShader(fragmentshader);

    GLint result;
    GLchar errorlog[512];

    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorlog);
        cerr << "Error: shader program 연결 실패" << errorlog << endl;
    }
    glUseProgram(shaderID);

    return shaderID;
}


void drawmodels() {
    //고정 물체 사각형
    if (ChangeModel) {
        glUseProgram(shaderID);
        glBindVertexArray(VAO[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubetexture[0]);        
        glUniform1i(glGetUniformLocation(shaderID, "texture1"), 0);
            
        glm::mat4 HexaUpModel = glm::mat4(1.0f);
        HexaUpModel = glm::rotate(HexaUpModel, glm::radians(SquareRevolute), glm::vec3(1.0f, 0.0f, 0.0f));
        HexaUpModel = glm::rotate(HexaUpModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int ModelUpLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(ModelUpLocation, 1, GL_FALSE, glm::value_ptr(HexaUpModel));
        
        glm::mat4 CubeNormalModel = glm::mat4(1.0f);
        CubeNormalModel = glm::rotate(CubeNormalModel, glm::radians(SquareRevolute), glm::vec3(1.0f, 0.0f, 0.0f));
        CubeNormalModel = glm::rotate(CubeNormalModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int ModelNormalLocation = glGetUniformLocation(shaderID, "NormalTransform");
        glUniformMatrix4fv(ModelNormalLocation, 1, GL_FALSE, glm::value_ptr(CubeNormalModel));
    
        glDrawArrays(GL_TRIANGLES, 0, ModelsValue[0].size());              

    }
    else {
        glUseProgram(shaderID);
        glBindVertexArray(VAO[1]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubetexture[1]);
        glUniform1i(glGetUniformLocation(shaderID, "texture1"), 0);

        glm::mat4 HexaUpModel = glm::mat4(1.0f);
        HexaUpModel = glm::rotate(HexaUpModel, glm::radians(SquareRevolute), glm::vec3(1.0f, 0.0f, 0.0f));
        HexaUpModel = glm::rotate(HexaUpModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int ModelUpLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(ModelUpLocation, 1, GL_FALSE, glm::value_ptr(HexaUpModel));

        glm::mat4 CubeNormalModel = glm::mat4(1.0f);
        CubeNormalModel = glm::rotate(CubeNormalModel, glm::radians(SquareRevolute), glm::vec3(1.0f, 0.0f, 0.0f));
        CubeNormalModel = glm::rotate(CubeNormalModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int ModelNormalLocation = glGetUniformLocation(shaderID, "NormalTransform");
        glUniformMatrix4fv(ModelNormalLocation, 1, GL_FALSE, glm::value_ptr(CubeNormalModel));
        glDrawArrays(GL_TRIANGLES, 0, ModelsValue[1].size());
    }
}

GLvoid drawscene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(shaderID);

    cameraFun(FirstPersonView, ObjectThetaX, ObjectThetaY, CameraTheta, PrimeCameraPoint, shaderID, windowWidth, windowHeight);

    LightFun(LightColor, LightRaidian, ScaleY, shaderID, ScaleZ);

    drawmodels();

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
   
    glGenVertexArrays(2, VAO);

    for (int i = 0; i < 2; i++) {
        glBindVertexArray(VAO[i]);
        glGenBuffers(3, &VBO[3*i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[3 * i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * ModelsValue[i].size(), &ModelsValue[i][0], GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0); //--- 위치 속성
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[3 * i + 1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * normalValue[i].size(), &normalValue[i][0], GL_STREAM_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[3 * i + 2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vtValue[i].size(), &vtValue[i][0], GL_STREAM_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        glEnableVertexAttribArray(2);
    }
}

void initTexture() {
    glGenTextures(2, &cubetexture[0]);
    glBindTexture(GL_TEXTURE_2D, cubetexture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int cubewidthImage, cubeheightImage, cubenumberOfChannel;
    unsigned char* cubedata = stbi_load("cubeUV4_.bmp", &cubewidthImage, &cubeheightImage, &cubenumberOfChannel, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage, cubeheightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(cubedata);

    glBindTexture(GL_TEXTURE_2D, cubetexture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    int pyrawidthImage, pyraheightImage, pyranumberOfChannel;
    unsigned char* pyradata = stbi_load("pyramidUV.bmp", &pyrawidthImage, &pyraheightImage, &pyranumberOfChannel, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pyrawidthImage, pyraheightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, pyradata);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(pyradata);

}

GLvoid keyboard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'x':
        PrimeCameraPoint.ViewX += 0.2f;
        break;
    case 'X':
        PrimeCameraPoint.ViewX -= 0.2f;
        break;
    case 'y':
        CameraRotationA = !CameraRotationA;
        CameraRotationB = false;
        break;
    case 'Y':
        CameraRotationB = !CameraRotationB;
        CameraRotationA = false;
        break;
    case 'z':
        PrimeCameraPoint.ViewZ += 0.2f;
        break;
    case 'Z':
        PrimeCameraPoint.ViewZ -= 0.2f; 
        break;
    case 'm':
        ScaleY = 20.0f;
        LightColor.x = 0.2f;
        LightColor.y = 0.2f;
        LightColor.z = 0.2f;
        break;
    case 'M':
        ScaleY = 0.0f;
        LightColor.x = 1.0f;
        LightColor.y = 1.0f;
        LightColor.z = 1.0f;
        break;
    case 't':
        SquareRotateA = !SquareRotateA;
        SquareRotateB = false;
        break;
    case 'T':
        SquareRotateB = !SquareRotateB;
        SquareRotateA = false;
        break;
    case 'r':
        SquareRevoluteA = !SquareRevoluteA;
        SquareRevoluteB = false;
        break;
    case'R':
        SquareRevoluteB = !SquareRevoluteB;
        SquareRevoluteA = false;
        break;
    case 'c':
        ChangeModel = !ChangeModel;
        break;
    case 's':
        CameraRotationB = false;
        CameraRotationA = false;
        SquareRotateA = false;
        SquareRotateB = false;
        SquareRevoluteA = false;
        SquareRevoluteB = false;
        SquareRotate = 0.0f;
        SquareRevolute = 0.0f;
        ObjectThetaY = 0.0f;
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
    if (CameraRotationA) {
        ObjectThetaY++;
    }
    if (CameraRotationB) {
        ObjectThetaY--;
    }
    if (SquareRotateA) {
        SquareRotate += 1.0f;
    }
    if (SquareRotateB) {
        SquareRotate -= 1.0f;
    }
    if (SquareRevoluteA) {
        SquareRevolute += 1.0f;
    }
    if (SquareRevoluteB) {
        SquareRevolute -= 1.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(1, timer, 1);
}