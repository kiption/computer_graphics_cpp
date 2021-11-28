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
void PractalPyramid(int PractalCount, int Practal, float Px, float Py, float Pz, float Pr, float Pg, float Pb);
void RandomlocationSnow();
void CleanLoactionSnow();
void RandomlocationBuilding();

int buildingCount = 8;
int windowWidth, windowHeight;
int MakePractalCount = 0;
int ConPractalCount = 0;
int SnowCount = 50;

//공전&자전 회전값
float CameraTheta = 0.0f;
float ObjectThetaX = 0.0f;
float ObjectThetaY = 0.0f;
float LightRaidian = 0.0f;
float SquareRotate = 0.0f;
float SquareRevolute = 0.0f;

//프랙탈 피라미드 초기값
float PrimalCorx = 0.0f;
float PrimalCory = 0.0f;
float PrimalCorz = 0.0f;
float PrimalCorr = 0.5f;
float PrimalCorg = 0.5f;
float PrimalCorb = 0.5f;

//스위치 불변수
bool CameraRotationB = false;
bool CameraRotationA = false;
bool SquareRotateA = false;
bool SquareRotateB = false;
bool SquareRevoluteA = false;
bool SquareRevoluteB = false;
bool FirstPersonView = false;
bool MakeSnow = false;

struct Snow {
    float x;
    float y;
    float z;
};
struct Building {
    float x;
    float y;
    float z;
};

//조명 초기위치
float ScaleZ = 5.0f;
float ScaleY = 1.0f;

unsigned int cubetexture[2];

CameraViewPoint PrimeCameraPoint;

Building BuildingLocate[8];
Snow Snowlocate[50];
Snow Snowvector[50];
Snow SaveSnow[50];

GLuint VAO[4], VBO[12];
GLuint shaderID;
GLuint qobjshader;
GLuint vertexshader;
GLuint fragmentshader;
GLuint triangleshaderProgram;

vector<glm::vec4>ModelsValue[4];
vector<glm::vec4>normalValue[4];
vector<glm::vec2>vtValue[4];

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
    ReadObj("plane3.obj", ModelsValue[0], normalValue[0], vtValue[0]);
    ReadObj("cube3_.obj", ModelsValue[1], normalValue[1], vtValue[1]);
    ReadObj("pyramid.obj", ModelsValue[2], normalValue[2], vtValue[2]);
    ReadObj("Sphere2.obj", ModelsValue[3], normalValue[3], vtValue[3]);

    RandomlocationBuilding();
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
    fragmentsource = filetobuf("fragmentshader2.glsl");

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
void RandomlocationSnow() {
    for (int i = 0; i < SnowCount; i++) {
        random_device rd;
        default_random_engine dre(rd());
        uniform_real_distribution<>locationY(2.5, 4.0);
        uniform_real_distribution<>locationXZ(-2.5, 2.5);
        uniform_real_distribution<>speedY(0.02, 0.05);
        float Primex = locationXZ(dre);
        float Primez = locationXZ(dre);
        float Primey = locationY(dre);
        float Speed = speedY(dre);

        Snowlocate[i].x = Primex;
        Snowlocate[i].y = Primey;
        Snowlocate[i].z = Primez;

        SaveSnow[i].x = Primex;
        SaveSnow[i].y = Primey;
        SaveSnow[i].z = Primez;

        Snowvector[i].y = Speed;
    }
}
void CleanLoactionSnow() {
    for (int i = 0; i < SnowCount; i++) {
        memset(&Snowlocate[i], 0.0, sizeof(Snowlocate[i]));
        memset(&SaveSnow[i], 0.0, sizeof(SaveSnow[i]));
        memset(&Snowvector[i], 0.0, sizeof(Snowvector[i]));
    }
}
void RandomlocationBuilding() {
    float x[8] = { -2.0, 2.0, -2.0, 2.0, -2.0, 2.0, -2.0, 2.0 };
    float z[8] = { -2.0, -2.0, -1.0, -1.0, 1.0, 1.0, 2.0, 2.0 };

    for (int i = 0; i < buildingCount; i++) {
        
        BuildingLocate[i].x = x[i];
        BuildingLocate[i].y = 0.5f;
        BuildingLocate[i].z = z[i];
    }
    
}
void drawmodels() {
    //바닥
	glBindVertexArray(VAO[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubetexture[0]);
	glUniform1i(glGetUniformLocation(shaderID, "texture1"), 0);
    
    unsigned int BoolPlaneCheck = glGetUniformLocation(shaderID, "Check");
    glUniform1i(BoolPlaneCheck, 0);
	
    glm::mat4 PlaneModel = glm::mat4(1.0f);
    //PlaneModel = glm::rotate(PlaneModel, glm::radians(SquareRevolute), glm::vec3(1.0f, 0.0f, 0.0f));
    PlaneModel = glm::scale(PlaneModel, glm::vec3(6.0f, 1.0f, 6.0f));
	
    unsigned int ModelUpLocation = glGetUniformLocation(shaderID, "ModelTransform");
	glUniformMatrix4fv(ModelUpLocation, 1, GL_FALSE, glm::value_ptr(PlaneModel));

	glm::mat4 PlaneNormalModel = glm::mat4(1.0f);
    //PlaneNormalModel = glm::rotate(PlaneNormalModel, glm::radians(SquareRevolute), glm::vec3(1.0f, 0.0f, 0.0f));
    //PlaneNormalModel = glm::rotate(PlaneNormalModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int PlaneNormalLocation = glGetUniformLocation(shaderID, "NormalTransform");
	glUniformMatrix4fv(PlaneNormalLocation, 1, GL_FALSE, glm::value_ptr(PlaneNormalModel));
   
	glDrawArrays(GL_TRIANGLES, 0, ModelsValue[0].size());
    
    PractalPyramid(MakePractalCount, ConPractalCount, PrimalCorx, PrimalCory, PrimalCorz, PrimalCorr, PrimalCorg, PrimalCorb);
    
    if (MakeSnow) {
        for (int i = 0; i < SnowCount; i++) {
            glBindVertexArray(VAO[3]);
            glm::mat4 Snowmodel = glm::mat4(1.0f);
            Snowmodel = glm::rotate(Snowmodel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
            Snowmodel = glm::translate(Snowmodel, glm::vec3(Snowlocate[i].x, Snowlocate[i].y, Snowlocate[i].z));
            Snowmodel = glm::scale(Snowmodel, glm::vec3(0.05f, 0.05f, 0.05f));
            unsigned int SnowmodelLocation = glGetUniformLocation(shaderID, "ModelTransform");
            glUniformMatrix4fv(SnowmodelLocation, 1, GL_FALSE, glm::value_ptr(Snowmodel));
            glm::mat4 SnowNormalmodel = glm::mat4(1.0f);
            SnowNormalmodel = glm::translate(SnowNormalmodel, glm::vec3(Snowlocate[i].x, Snowlocate[i].y, Snowlocate[i].z));
            SnowNormalmodel = glm::rotate(SnowNormalmodel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
            unsigned int SnowNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
            glUniformMatrix4fv(SnowNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(SnowNormalmodel));
            unsigned int SnowobjColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
            glUniform3f(SnowobjColorLocation, 1.0f, 1.0f, 1.0f);
            unsigned int BoolCheck = glGetUniformLocation(shaderID, "Check");
            glUniform1i(BoolCheck, 2);
            glDrawArrays(GL_TRIANGLES, 0, ModelsValue[3].size());
        }
    }

    glEnable(GL_BLEND);
    for (int i = 0; i < buildingCount; i++) {
        glBindVertexArray(VAO[1]);
        glm::mat4 BuildingModel = glm::mat4(1.0f);
        BuildingModel = glm::translate(BuildingModel, glm::vec3(BuildingLocate[i].x, BuildingLocate[i].y, BuildingLocate[i].z));
        BuildingModel = glm::scale(BuildingModel, glm::vec3(0.25f, 1.0f, 0.25f));

        unsigned int BuildingLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(BuildingLocation, 1, GL_FALSE, glm::value_ptr(BuildingModel));

        glm::mat4 BuildingNormalModel = glm::mat4(1.0f);
        BuildingNormalModel = glm::translate(BuildingNormalModel, glm::vec3(BuildingLocate[i].x, BuildingLocate[i].y, BuildingLocate[i].z));
        unsigned int BuildingNormalLocation = glGetUniformLocation(shaderID, "NormalTransform");
        glUniformMatrix4fv(BuildingNormalLocation, 1, GL_FALSE, glm::value_ptr(BuildingNormalModel));
        unsigned int BoolBuildingCheck = glGetUniformLocation(shaderID, "Check");
        glUniform1i(BoolBuildingCheck, 1);
        unsigned int BuildingobjColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(BuildingobjColorLocation, 0.69f, 0.87f, 0.92f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, ModelsValue[1].size());
    }
    glDisable(GL_BLEND);


}
void PractalPyramid(int PractalCount, int Practal, float Px, float Py, float Pz, float Pr, float Pg, float Pb) {

    if (PractalCount == 0) {
        glBindVertexArray(VAO[2]);
        glm::mat4 pyramodel = glm::mat4(1.0f);
        pyramodel = glm::rotate(pyramodel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        pyramodel = glm::translate(pyramodel, glm::vec3(Px, Py, Pz));
        pyramodel = glm::scale(pyramodel, glm::vec3(powf(0.5, Practal), powf(0.5, Practal), powf(0.5, Practal)));
        unsigned int pyramodelLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(pyramodelLocation, 1, GL_FALSE, glm::value_ptr(pyramodel));
        glm::mat4 pyraNormalmodel = glm::mat4(1.0f);
        pyraNormalmodel = glm::rotate(pyraNormalmodel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        pyramodel = glm::translate(pyramodel, glm::vec3(Px, Py, Pz));
        unsigned int pyraNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
        glUniformMatrix4fv(pyraNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(pyraNormalmodel));
        unsigned int pyraCheck = glGetUniformLocation(shaderID, "Check");
        glUniform1i(pyraCheck, 2);
        unsigned int pyraobjColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(pyraobjColorLocation, Pr, Pg, Pb);

        glDrawArrays(GL_TRIANGLES, 0, ModelsValue[2].size());
    }
    else {
        PractalPyramid(PractalCount - 1, Practal, Px + powf(0.5, (Practal - PractalCount + 2)), Py, Pz + powf(0.5, (Practal - PractalCount + 2)),
            Pr - powf(0.5, Practal - PractalCount + 2), Pg + powf(0.25, Practal - PractalCount + 2), Pb);

        PractalPyramid(PractalCount - 1, Practal, Px + powf(0.5, (Practal - PractalCount + 2)), Py, Pz - (powf(0.5, (Practal - PractalCount + 2))),
            Pr - powf(0.5, Practal - PractalCount + 2), Pg + powf(0.25, Practal - PractalCount + 2), Pb);

        PractalPyramid(PractalCount - 1, Practal, Px - (powf(0.5, (Practal - PractalCount + 2))), Py, Pz - (powf(0.5, (Practal - PractalCount + 2))),
            Pr - powf(0.5, Practal - PractalCount + 2), Pg + powf(0.25, Practal - PractalCount + 2), Pb);

        PractalPyramid(PractalCount - 1, Practal, Px - (powf(0.5, (Practal - PractalCount + 2))), Py, Pz + powf(0.5, (Practal - PractalCount + 2)),
            Pr - powf(0.5, Practal - PractalCount + 2), Pg + powf(0.25, Practal - PractalCount + 2), Pb);

        PractalPyramid(PractalCount - 1, Practal, Px, Py + powf(0.5, (Practal - PractalCount + 1)), Pz,
            Pr, Pg + powf(0.25, Practal - PractalCount + 2), Pb);
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

    glGenVertexArrays(4, VAO);

    for (int i = 0; i < 4; i++) {
        glBindVertexArray(VAO[i]);
        glGenBuffers(3, &VBO[3 * i]);
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
    glGenTextures(1, &cubetexture[0]);
    glBindTexture(GL_TEXTURE_2D, cubetexture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int cubewidthImage, cubeheightImage, cubenumberOfChannel;
    unsigned char* cubedata = stbi_load("planeUV.bmp", &cubewidthImage, &cubeheightImage, &cubenumberOfChannel, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage, cubeheightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(cubedata);
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
        ScaleY = 1.0f;
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
    case's':
        MakeSnow = !MakeSnow;
        if (MakeSnow) {
            RandomlocationSnow();
        }
        else {
            CleanLoactionSnow();
        }
        break;
    case '1':
        MakePractalCount = 0;
        ConPractalCount = 0;
        break;
    case '2':
        MakePractalCount = 1;
        ConPractalCount = 1;
        break;
    case '3':
        MakePractalCount = 2;
        ConPractalCount = 2;
        break;
    case '4':
        MakePractalCount = 3;
        ConPractalCount = 3;
        break;
    case '5':
        MakePractalCount = 4;
        ConPractalCount = 4;
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
    if (MakeSnow) {
        for (int i = 0; i < SnowCount; i++) {
            Snowlocate[i].y -= Snowvector[i].y;

            if (Snowlocate[i].y <= 0.0f) {
                Snowlocate[i].y = SaveSnow[i].y;
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(1, timer, 1);
}