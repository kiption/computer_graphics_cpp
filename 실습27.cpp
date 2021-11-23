#include "readObj.h"
#include "readObj2.h"
#include "camera.h"
#include "Light.h"
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
void drawObital();
void RandomColorLight();
void RandomlocationSnow();
void CleanLoactionSnow();
void PractalPyramid(int PractalCount, int Practal, float Px, float Py, float Pz, float Pr, float Pg, float Pb);

int windowWidth, windowHeight;
int ObitalRotate = 0;
int SnowCount = 50;
int LightCount = 0;
int MakePractalCount = 0;
int ConPractalCount = 0;
int LightPowerCount = 8;

//프랙탈 피라미드 초기값
float PrimalCorx = 0.0f;
float PrimalCory = 0.0f;
float PrimalCorz = 0.0f;
float PrimalCorr = 0.5f;
float PrimalCorg = 0.5f;
float PrimalCorb = 0.5f;

//공전&자전 회전값
float CameraTheta = 0.0f;
float ObjectTheta = 0.0f;
float LightRaidian = 0.0f;
float SquareRotate = 0.0f;
float SunRotate = 0.0f;
float EarthRotate = 0.0f;
float MoonRotate = 0.0f;
float ObjectThetaX = 0.0f;
float ObjectThetaY = 0.0f;


//스위치 불변수
bool MakeOrbital = true;
bool MakeSnow = false;
bool PlanetOrbital = false;
bool CameraRotationB = false;
bool CameraRotationA = false;
bool SquareRotateA = false;
bool SquareRotateB = false;
bool LightRotateA = false;
bool LightRotateB = false;
bool FirstPersonView = false;

struct Snow {
    float x;
    float y;
    float z;
};

//조명 초기위치
float ScaleZ = 4.0f;
float ScaleY = 1.0f;

CameraViewPoint PrimeCameraPoint;

GLuint VAO[10], VBO[20];
GLuint shaderID;
GLuint qobjshader;
GLuint vertexshader;
GLuint fragmentshader;
GLuint triangleshaderProgram;

Snow Snowlocate[50];
Snow Snowvector[50];
Snow SaveSnow[50];
vector<glm::vec4>ModelsValue[7];
vector<glm::vec4>normalValue[7];
vector<float>ObitalValue[3];

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
    ReadObj2("plane.obj", ModelsValue[0], normalValue[0]);  //바닥
    ReadObj("Sphere2.obj", ModelsValue[1], normalValue[1]); //태양,지구,달
    ReadObj("Sphere2.obj", ModelsValue[2], normalValue[2]);
    ReadObj("Sphere2.obj", ModelsValue[3], normalValue[3]);
    ReadObj("cube.obj", ModelsValue[4], normalValue[4]); //조명박스
    ReadObj("Sphere2.obj", ModelsValue[5], normalValue[5]); //눈
    ReadObj("pyramid.obj", ModelsValue[6], normalValue[6]); //피라미드

    make_vertexShaders();
    make_fragmentShaders();
    make_shaderProgram();
    initbuffer();

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
void drawObital() {
    float CircleX, CircleZ, CircleY;
    for (int i = 0; i < 360; i++) {
        float r = 1.0;
        CircleX = r * cos(3.14 * i / 180);
        CircleY = (-1) * r * sin(3.14 * i / 180);
        CircleZ = r * sin(3.14 * i / 180);

        ObitalValue[0].push_back(CircleX);
        ObitalValue[0].push_back(CircleY);
        ObitalValue[0].push_back(CircleZ);

        ObitalValue[0].push_back(0.0f);
        ObitalValue[0].push_back(1.0f);
        ObitalValue[0].push_back(0.0f);
    }
    for (int i = 0; i < 360; i++) {
        float r = 2.0;
        CircleX = r * cos(3.14 * i / 180);
        CircleZ = r * sin(3.14 * i / 180);

        ObitalValue[1].push_back(CircleX);
        ObitalValue[1].push_back(2.0f);
        ObitalValue[1].push_back(CircleZ);
                    
        ObitalValue[1].push_back(0.0f);
        ObitalValue[1].push_back(1.0f);
        ObitalValue[1].push_back(0.0f);
    }
    for (int i = 0; i < 360; i++) {
        float r = 3.0;
        CircleX = r * cos(3.14 * i / 180);
        CircleY = r * sin(3.14 * i / 180);
        CircleZ = r * sin(3.14 * i / 180);

        ObitalValue[2].push_back(CircleX);
        ObitalValue[2].push_back(CircleY);
        ObitalValue[2].push_back(CircleZ);
                    
        ObitalValue[2].push_back(0.0f);
        ObitalValue[2].push_back(1.0f);
        ObitalValue[2].push_back(0.0f);
    }
}
void drawmodels() {
    //바닥
    glBindVertexArray(VAO[0]);
    glm::mat4 PlaneModel = glm::mat4(1.0f);
    PlaneModel = glm::scale(PlaneModel, glm::vec3(7.0f, 1.0f, 7.0f));
    unsigned int PlaneModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(PlaneModelLocation, 1, GL_FALSE, glm::value_ptr(PlaneModel));
    glm::mat4 PlaneNormalmodel = glm::mat4(1.0f);
    PlaneNormalmodel = glm::rotate(PlaneNormalmodel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int PlaneNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(PlaneNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(PlaneNormalmodel));
    unsigned int PlaneModelFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(PlaneModelFragLocation, 0.9f, 0.9f, 0.98f);
    glDrawArrays(GL_QUADS, 0, ModelsValue[0].size());
    
    //태양
    glBindVertexArray(VAO[1]);
    glm::mat4 Sunmodel = glm::mat4(1.0f);
    Sunmodel = glm::rotate(Sunmodel, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Sunmodel = glm::translate(Sunmodel, glm::vec3(0.5f, -0.15f, 0.0f));
    Sunmodel = glm::rotate(Sunmodel, glm::radians(SunRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    Sunmodel = glm::translate(Sunmodel, glm::vec3(1.0f, 1.0f, 0.0f));
    Sunmodel = glm::scale(Sunmodel, glm::vec3(0.15f, 0.15f, 0.15f));
    unsigned int SunmodelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(SunmodelLocation, 1, GL_FALSE, glm::value_ptr(Sunmodel));
    glm::mat4 SunNormalmodel = glm::mat4(1.0f);
    SunNormalmodel = glm::rotate(SunNormalmodel, glm::radians(SunRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int SunNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(SunNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(SunNormalmodel));
    unsigned int SunobjColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(SunobjColorLocation, 0.58f, 0.0f, 0.82f);
    glDrawArrays(GL_TRIANGLES, 0, ModelsValue[1].size());

    //지구
    glBindVertexArray(VAO[2]);
    glm::mat4 Earthmodel = glm::mat4(1.0f);
    Earthmodel = glm::rotate(Earthmodel, glm::radians(EarthRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    Earthmodel = glm::translate(Earthmodel, glm::vec3(2.0f, 1.0f, 0.0f));
    Earthmodel = glm::scale(Earthmodel, glm::vec3(0.15f, 0.15f, 0.15f));
    unsigned int EarthmodelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(EarthmodelLocation, 1, GL_FALSE, glm::value_ptr(Earthmodel));
    glm::mat4 EarthNormalmodel = glm::mat4(1.0f);
    EarthNormalmodel = glm::rotate(EarthNormalmodel, glm::radians(EarthRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int EarthNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(EarthNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(EarthNormalmodel));
    unsigned int EarthobjColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(EarthobjColorLocation, 0.56f, 0.94f, 0.56f);
    glDrawArrays(GL_TRIANGLES, 0, ModelsValue[2].size());

    //달
    glBindVertexArray(VAO[3]);
    glm::mat4 Moonmodel = glm::mat4(1.0f);
    Moonmodel = glm::rotate(Moonmodel, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Moonmodel = glm::translate(Moonmodel, glm::vec3(-0.15f, 0.0f, 0.0f));
    Moonmodel = glm::rotate(Moonmodel, glm::radians(MoonRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    Moonmodel = glm::translate(Moonmodel, glm::vec3(-3.0f, 1.0f, 0.0f));
    Moonmodel = glm::scale(Moonmodel, glm::vec3(0.15f, 0.15f, 0.15f));
    unsigned int MoonmodelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(MoonmodelLocation, 1, GL_FALSE, glm::value_ptr(Moonmodel));
    glm::mat4 MoonNormalmodel = glm::mat4(1.0f);
    MoonNormalmodel = glm::rotate(MoonNormalmodel, glm::radians(MoonRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int MoonNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(MoonNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(MoonNormalmodel));
    unsigned int MoonobjColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(MoonobjColorLocation, 0.69f, 0.87f, 0.92f);
    glDrawArrays(GL_TRIANGLES, 0, ModelsValue[3].size());

    //조명 박스
    glBindVertexArray(VAO[4]);
    glm::mat4 LightBox = glm::mat4(1.0f);
    LightBox = glm::rotate(LightBox, glm::radians(LightRaidian), glm::vec3(0.0f, 1.0f, 0.0f));
    LightBox = glm::translate(LightBox, glm::vec3(0.0f, ScaleY, ScaleZ));
    LightBox = glm::scale(LightBox, glm::vec3(0.3f, 0.3f, 0.3f));
    unsigned int LightBoxLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(LightBoxLocation, 1, GL_FALSE, glm::value_ptr(LightBox));
    glm::mat4 LightBoxNormalmodel = glm::mat4(1.0f);
    LightBoxNormalmodel = glm::rotate(LightBoxNormalmodel, glm::radians(LightRaidian), glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int LightBoxNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(LightBoxNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(LightBoxNormalmodel));
    unsigned int LightBoxColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(LightBoxColorLocation, 0.2f, 0.2f, 0.2f);
    glDrawArrays(GL_TRIANGLES, 0, ModelsValue[4].size());

    //눈
    if (MakeSnow) {
        for (int i = 0; i < SnowCount; i++) {
            glBindVertexArray(VAO[5]);
            glm::mat4 Snowmodel = glm::mat4(1.0f);
            Snowmodel = glm::rotate(Snowmodel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
            Snowmodel = glm::translate(Snowmodel, glm::vec3(Snowlocate[i].x , Snowlocate[i].y, Snowlocate[i].z));
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
            glDrawArrays(GL_TRIANGLES, 0, ModelsValue[5].size());
        }
    }
    //피라미드

    PractalPyramid(MakePractalCount, ConPractalCount, PrimalCorx, PrimalCory, PrimalCorz, PrimalCorr, PrimalCorg, PrimalCorb);

	glBindVertexArray(VAO[7]);
	glm::mat4 Orbital1Model = glm::mat4(1.0f);
    Orbital1Model = glm::translate(Orbital1Model, glm::vec3(0.0f, 1.0f, 0.0f));
    Orbital1Model = glm::rotate(Orbital1Model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Orbital1Model = glm::scale(Orbital1Model, glm::vec3(1.0f, 0.0f, 1.0f));
	unsigned int OrbitalModel1ModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
	glUniformMatrix4fv(OrbitalModel1ModelLocation, 1, GL_FALSE, glm::value_ptr(Orbital1Model));
	glm::mat4 Orbital1NormalModel = glm::mat4(1.0f);
	unsigned int Orbital1NormalModelModelLocation = glGetUniformLocation(shaderID, "NormalTransform");
	glUniformMatrix4fv(Orbital1NormalModelModelLocation, 1, GL_FALSE, glm::value_ptr(Orbital1NormalModel));
	unsigned int Orbital1ModelLocation = glGetUniformLocation(shaderID, "ObjectColor");
	glUniform3f(Orbital1ModelLocation, 1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, ObitalValue[0].size() / 6);

	glBindVertexArray(VAO[8]);
	glm::mat4 Orbital2Model = glm::mat4(1.0f);
    Orbital2Model = glm::translate(Orbital2Model, glm::vec3(0.0f, 1.0f, 0.0f));
    Orbital2Model = glm::scale(Orbital2Model, glm::vec3(1.0f, 0.0f, 1.0f));
	unsigned int Orbital2ModelModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
	glUniformMatrix4fv(Orbital2ModelModelLocation, 1, GL_FALSE, glm::value_ptr(Orbital2Model));
	glm::mat4 Orbital2NormalModel = glm::mat4(1.0f);
	unsigned int Orbital2NormalModelModelLocation = glGetUniformLocation(shaderID, "NormalTransform");
	glUniformMatrix4fv(Orbital2NormalModelModelLocation, 1, GL_FALSE, glm::value_ptr(Orbital2NormalModel));
	unsigned int Orbital2ModelLocation = glGetUniformLocation(shaderID, "ObjectColor");
	glUniform3f(Orbital2ModelLocation, 1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, ObitalValue[1].size() / 6);


	glBindVertexArray(VAO[9]);
	glm::mat4 Orbital3Model = glm::mat4(1.0f);
    Orbital3Model = glm::translate(Orbital3Model, glm::vec3(0.0f, 1.0f, 0.0f));
    Orbital3Model = glm::rotate(Orbital3Model, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Orbital3Model = glm::scale(Orbital3Model, glm::vec3(1.0f, 0.0f, 1.0f));
	unsigned int Orbital3ModelModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
	glUniformMatrix4fv(Orbital3ModelModelLocation, 1, GL_FALSE, glm::value_ptr(Orbital3Model));
	glm::mat4 Orbital3NormalModel = glm::mat4(1.0f);
	unsigned int Orbital3NormalModelModelLocation = glGetUniformLocation(shaderID, "NormalTransform");
	glUniformMatrix4fv(Orbital3NormalModelModelLocation, 1, GL_FALSE, glm::value_ptr(Orbital3NormalModel));
	unsigned int Orbital3ModelLocation = glGetUniformLocation(shaderID, "ObjectColor");
	glUniform3f(Orbital3ModelLocation, 1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, ObitalValue[2].size() / 6);
    

}

void PractalPyramid(int PractalCount, int Practal, float Px, float Py, float Pz, float Pr, float Pg, float Pb) {

    if (PractalCount == 0) {
        glBindVertexArray(VAO[6]);
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
        unsigned int pyraobjColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(pyraobjColorLocation, Pr, Pg, Pb);
        glDrawArrays(GL_TRIANGLES, 0, ModelsValue[6].size());
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
    if (MakeOrbital) {
        drawObital();
        MakeOrbital = false;
    }

    glGenVertexArrays(10, VAO);

    for (int i = 0; i < 7; i++) {
        glBindVertexArray(VAO[i]);
        glGenBuffers(2, &VBO[2 * i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2 * i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * ModelsValue[i].size(), &ModelsValue[i][0], GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0); //--- 위치 속성
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2 * i + 1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * normalValue[i].size(), &normalValue[i][0], GL_STREAM_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(1);
    }
    for (int j = 0; j < 3; j++) {
        glBindVertexArray(VAO[j + 7]);
        glGenBuffers(2, &VBO[2 * j + 14]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2 * j +14]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ObitalValue[j].size(), &ObitalValue[j][0], GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- 위치 속성
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2 * j + 15]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ObitalValue[j].size(), &ObitalValue[j][0], GL_STREAM_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }


}
void RandomColorLight() {
    if (LightCount % 3 == 1) {
        LightColor.x = 1.0f;
        LightColor.y = 0.0f;
        LightColor.z = 0.0f;
    }
    else if (LightCount % 3 == 2) {
        LightColor.x = 0.0f;
        LightColor.y = 1.0f;
        LightColor.z = 0.0f;
    }
    else if (LightCount % 3 == 0) {
        LightColor.x = 0.0f;
        LightColor.y = 0.0f;
        LightColor.z = 1.0f;
    }
}
GLvoid keyboard(unsigned char key, int x, int y) {
    switch (key)
    {
    case'x':
        PrimeCameraPoint.ViewX -= 0.5f;
        break;
    case'X':
        PrimeCameraPoint.ViewX += 0.5f;
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
        PrimeCameraPoint.ViewZ -= 0.5f;
        break;
    case 'Z':
        PrimeCameraPoint.ViewZ += 0.5f;
        break;
    case 'm':
        ScaleY = 20.0f;
        LightColor.x = 0.2f;
        LightColor.y = 0.2f;
        LightColor.z = 0.2f;
        LightPowerCount = 0;
        break;
    case 'M':
        ScaleY = 1.0f;
        LightColor.x = 1.0f;
        LightColor.y = 1.0f;
        LightColor.z = 1.0f;
        LightPowerCount = 8;
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
        LightRotateA = !LightRotateA;
        LightRotateB = false;
        break;
    case 'R':
        LightRotateB = !LightRotateB;
        LightRotateA = false;
        break;
    case 'c':
        RandomColorLight();
        LightCount++;
        break;
    case 'C':
        LightColor.x = 1.0f;
        LightColor.y = 1.0f;
        LightColor.z = 1.0f;
        LightCount = 0;
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
    case '6':
        MakePractalCount = 5;
        ConPractalCount = 5;
        break;
    case '7':
        MakePractalCount = 6;
        ConPractalCount = 6;
        break;
    case 'i':
        ScaleZ += 0.2f;
        break;
    case 'o':
        ScaleZ -= 0.2f;
        break;
    case '+':
        if (LightPowerCount != 8) {
            LightColor.x += 0.1f;
            LightColor.y += 0.1f;
            LightColor.z += 0.1f;
            LightPowerCount++;
        }
        break;
    case '-':
        if (LightPowerCount != 0) {
            LightColor.x -= 0.1f;
            LightColor.y -= 0.1f;
            LightColor.z -= 0.1f;
            LightPowerCount--;
        }

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
        ObjectTheta++;
    }
    if (CameraRotationB) {
        ObjectTheta--;
    }
    if (LightRotateA) {
        LightRaidian -= 1.0f;
    }
    if (LightRotateB) {
        LightRaidian += 1.0f;
    }
    if (SquareRotateA) {
        SquareRotate += 1.0f;  
    }
    if (SquareRotateB) {
        SquareRotate -= 1.0f;       
    }
    if (MakeSnow) {
        for (int i = 0; i < SnowCount; i++) {
            Snowlocate[i].y -= Snowvector[i].y;

            if (Snowlocate[i].y <= 0.0f) {
                Snowlocate[i].y = SaveSnow[i].y;
            }
        }
    }
    SunRotate += 1.5f;
    EarthRotate += 2.0f;
    MoonRotate += 3.0f;
    glutPostRedisplay();
    glutTimerFunc(1, timer, 1);
}