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

float ObjectThetaX = 0.0f;
float ObjectThetaY = 0.0f;

float CameraTheta = 0.0f;
int windowWidth, windowHeight;
int ObitalRotate = 0;

void drawmodels();
void drawObital();
void RandomColorLight();

bool CameraRotationA = false;
bool CameraRotationB = false;
bool CrainBodyRotateA = false;
bool CrainBodyRotateB = false;
bool CrainArmRotateAB = false;
bool FirstPersonView = false;

float SquareRotate = 0.0f;
float LightRaidian = 0.0f;
bool SquareRotateA = false;
bool SquareRotateB = false;
bool LightRotateA = false;
bool LightRotateB = false;
bool ChangeModel = true;
int LightCount = 0;
int ArmRotateVector = 1;

CameraViewPoint PrimeCameraPoint;

struct LightObital {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
}LightRotateOrbital;
struct CrainRotate {
    float bodyRotateX = 0.0f;
    float bodyRotateY = 0.0f;
    float bodyRotateZ = 0.0f;

    float armLRotateX = 0.0f;
    float armLRotateY = 0.0f;
    float armLRotateZ = 0.0f;

    float armRRotateX = 0.0f;
    float armRRotateY = 0.0f;
    float armRRotateZ = 0.0f;

    float cameraAngle = 0.0f;
}CrainRotateSize;

struct CrainTrans {
    float AllTransX = 0.0f;
    float AllTransY = 0.0f;
    float AllTransZ = 0.0f;
}CrainTransSize;
float ScaleX = 2.0f;
float ScaleY = 1.0f;
float ScaleZ = -1.0f;

GLuint VAO[6], VBO[12];
GLuint shaderID;
GLuint qobjshader;
GLuint vertexshader;
GLuint fragmentshader;
GLuint triangleshaderProgram;

vector<glm::vec4>ModelsValue[6];
vector<glm::vec4>normalValue[6];
vector<float>ObitalValue;
vector<float>LightOrbitalValue;

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
    ReadObj2("plane.obj", ModelsValue[0], normalValue[0]);
    ReadObj2("crainCube.obj", ModelsValue[1], normalValue[1]);
    ReadObj2("crainCube2.obj", ModelsValue[2], normalValue[2]);
    ReadObj2("crainArm.obj", ModelsValue[3], normalValue[3]);
    ReadObj2("crainArm.obj", ModelsValue[4], normalValue[4]);
    ReadObj2("crainCube.obj", ModelsValue[5], normalValue[5]);

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


void drawmodels() {
    
    //바닥
    glBindVertexArray(VAO[0]);
    glm::mat4 PlaneModel = glm::mat4(1.0f);
    PlaneModel = glm::scale(PlaneModel, glm::vec3(2.5f, 0, 2.5f));
    unsigned int PlaneModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(PlaneModelLocation, 1, GL_FALSE, glm::value_ptr(PlaneModel));
    glm::mat4 PlaneNormalmodel = glm::mat4(1.0f);
    PlaneNormalmodel = glm::rotate(PlaneNormalmodel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int PlaneNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(PlaneNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(PlaneNormalmodel));
    unsigned int PlaneModelFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(PlaneModelFragLocation, 0.5f, 0.75f, 0.27f);
    glDrawArrays(GL_QUADS, 0, ModelsValue[0].size());

    //탱크 다리부분
    glBindVertexArray(VAO[1]);
    glm::mat4 RailModel = glm::mat4(1.0f);
    RailModel = glm::translate(RailModel, glm::vec3(CrainTransSize.AllTransX, CrainTransSize.AllTransY, CrainTransSize.AllTransZ));
    //RailModel = glm::scale(RailModel, glm::vec3(0.5f, 0.5f, 0.5f));
    unsigned int RailModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(RailModelLocation, 1, GL_FALSE, glm::value_ptr(RailModel));
    glm::mat4 RailNormalmodel = glm::mat4(1.0f);
    RailNormalmodel = glm::translate(RailNormalmodel, glm::vec3(CrainTransSize.AllTransX, CrainTransSize.AllTransY, CrainTransSize.AllTransZ));
    unsigned int RailNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(RailNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(RailNormalmodel));
    unsigned int RailModelFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(RailModelFragLocation, 0.5f, 0.5f, 0.0f);
    glDrawArrays(GL_QUADS, 0, ModelsValue[1].size());


    //탱크 몸통 부분
    glBindVertexArray(VAO[2]);
    glm::mat4 BodyModel = glm::mat4(1.0f);
    BodyModel = glm::translate(BodyModel, glm::vec3(CrainTransSize.AllTransX, CrainTransSize.AllTransY, CrainTransSize.AllTransZ));
    BodyModel = glm::rotate(BodyModel, glm::radians(CrainRotateSize.bodyRotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    //BodyModel = glm::scale(BodyModel, glm::vec3(0.5f, 0.5f, 0.5f));
    unsigned int BodyModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(BodyModelLocation, 1, GL_FALSE, glm::value_ptr(BodyModel));
    glm::mat4 BodyNormalmodel = glm::mat4(1.0f);
    BodyNormalmodel = glm::translate(BodyNormalmodel, glm::vec3(CrainTransSize.AllTransX, CrainTransSize.AllTransY, CrainTransSize.AllTransZ));
    BodyNormalmodel = glm::rotate(BodyNormalmodel, glm::radians(CrainRotateSize.bodyRotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int BodyNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(BodyNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(BodyNormalmodel));
    unsigned int BodyModelFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(BodyModelFragLocation, 0.56f, 0.47f, 0.29f);
    glDrawArrays(GL_QUADS, 0, ModelsValue[2].size());

    //탱크 왼쪽 팔 부분
    glBindVertexArray(VAO[3]);
    glm::mat4 LeftArmModel = glm::mat4(1.0f);
    LeftArmModel = glm::translate(LeftArmModel, glm::vec3(CrainTransSize.AllTransX, CrainTransSize.AllTransY, CrainTransSize.AllTransZ));
    LeftArmModel = glm::rotate(LeftArmModel, glm::radians(CrainRotateSize.bodyRotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    LeftArmModel = glm::translate(LeftArmModel, glm::vec3(-0.2f, 0.0f, 0.0f));
    LeftArmModel = glm::translate(LeftArmModel, glm::vec3(0.0f, 0.4f, 0.0f));
    LeftArmModel = glm::rotate(LeftArmModel, glm::radians(CrainRotateSize.armLRotateX), glm::vec3(1.0f, 0.0f, 0.0f));
    LeftArmModel = glm::translate(LeftArmModel, glm::vec3(0.0f, -0.4f, 0.0f));
    //LeftArmModel = glm::scale(LeftArmModel, glm::vec3(0.5f, 0.5f, 0.5f));
    unsigned int LeftArmModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(LeftArmModelLocation, 1, GL_FALSE, glm::value_ptr(LeftArmModel));
    glm::mat4 LeftNormalmodel = glm::mat4(1.0f);
    LeftNormalmodel = glm::translate(LeftNormalmodel, glm::vec3(CrainTransSize.AllTransX, CrainTransSize.AllTransY, CrainTransSize.AllTransZ));
    LeftNormalmodel = glm::rotate(LeftNormalmodel, glm::radians(CrainRotateSize.bodyRotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    LeftNormalmodel = glm::translate(LeftNormalmodel, glm::vec3(-0.2f, 0.0f, 0.0f));
    LeftNormalmodel = glm::translate(LeftNormalmodel, glm::vec3(0.0f, 0.4f, 0.0f));
    LeftNormalmodel = glm::rotate(LeftNormalmodel, glm::radians(CrainRotateSize.armLRotateX), glm::vec3(1.0f, 0.0f, 0.0f));
    LeftNormalmodel = glm::translate(LeftNormalmodel, glm::vec3(0.0f, -0.4f, 0.0f));
    unsigned int LeftNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(LeftNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(LeftNormalmodel));
    unsigned int LeftArmModelFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(LeftArmModelFragLocation, 0.75f, 0.75f, 0.75f);
    glDrawArrays(GL_QUADS, 0, ModelsValue[3].size());

    //탱크 오른쪽 팔 부분
    glBindVertexArray(VAO[4]);
    glm::mat4 RightArmModel = glm::mat4(1.0f);
    //RightArmModel = glm::rotate(RightArmModel, glm::radians(AxisAnglex), glm::vec3(1.0f, 0.0f, 0.0f));
    //RightArmModel = glm::rotate(RightArmModel, glm::radians(AxisAngley), glm::vec3(0.0f, 1.0f, 0.0f));
    RightArmModel = glm::translate(RightArmModel, glm::vec3(CrainTransSize.AllTransX, CrainTransSize.AllTransY, CrainTransSize.AllTransZ));
    RightArmModel = glm::rotate(RightArmModel, glm::radians(CrainRotateSize.bodyRotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    RightArmModel = glm::translate(RightArmModel, glm::vec3(0.2f, 0.0f, 0.0f));
    RightArmModel = glm::translate(RightArmModel, glm::vec3(0.0f, 0.4f, 0.0f));
    RightArmModel = glm::rotate(RightArmModel, glm::radians(CrainRotateSize.armRRotateX), glm::vec3(1.0f, 0.0f, 0.0f));
    RightArmModel = glm::translate(RightArmModel, glm::vec3(0.0f, -0.4f, 0.0f));
    //RightArmModel = glm::scale(RightArmModel, glm::vec3(0.5f, 0.5f, 0.5f));
    unsigned int RightArmModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(RightArmModelLocation, 1, GL_FALSE, glm::value_ptr(RightArmModel));
    glm::mat4 RightNormalmodel = glm::mat4(1.0f);
    RightNormalmodel = glm::translate(RightNormalmodel, glm::vec3(CrainTransSize.AllTransX, CrainTransSize.AllTransY, CrainTransSize.AllTransZ));
    RightNormalmodel = glm::rotate(RightNormalmodel, glm::radians(CrainRotateSize.bodyRotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    RightNormalmodel = glm::translate(RightNormalmodel, glm::vec3(0.2f, 0.0f, 0.0f));
    RightNormalmodel = glm::translate(RightNormalmodel, glm::vec3(0.0f, 0.4f, 0.0f));
    RightNormalmodel = glm::rotate(RightNormalmodel, glm::radians(CrainRotateSize.armRRotateX), glm::vec3(1.0f, 0.0f, 0.0f));
    RightNormalmodel = glm::translate(RightNormalmodel, glm::vec3(0.0f, -0.4f, 0.0f));
    unsigned int RightNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(RightNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(RightNormalmodel));
    unsigned int RightArmModelFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(RightArmModelFragLocation, 0.22f, 0.18f, 0.19f);
    glDrawArrays(GL_QUADS, 0, ModelsValue[4].size());
    
    //조명 박스
    glBindVertexArray(VAO[5]);
    glm::mat4 LightBox = glm::mat4(1.0f);
    LightBox = glm::rotate(LightBox, glm::radians(LightRaidian), glm::vec3(0.0f, 1.0f, 0.0f));
    LightBox = glm::translate(LightBox, glm::vec3(ScaleX + 0.2f, ScaleY, ScaleZ - 0.2f));
    LightBox = glm::scale(LightBox, glm::vec3(0.3f, 0.5f, 0.3f));
    unsigned int LightBoxLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(LightBoxLocation, 1, GL_FALSE, glm::value_ptr(LightBox));
    glm::mat4 LightBoxNormalmodel = glm::mat4(1.0f);
    LightBoxNormalmodel = glm::rotate(LightBoxNormalmodel, glm::radians(LightRaidian), glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int LightBoxNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(LightBoxNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(LightBoxNormalmodel));
    unsigned int LightBoxColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(LightBoxColorLocation, 0.2f, 0.2f, 0.2f);
    glDrawArrays(GL_QUADS, 0, ModelsValue[5].size());
}

GLvoid drawscene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(shaderID);

    cameraFun(FirstPersonView, ObjectThetaX, ObjectThetaY, CameraTheta, PrimeCameraPoint,  shaderID, windowWidth,  windowHeight);


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
    glGenVertexArrays(6, VAO);

    for (int i = 0; i < 6; i++) {
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
        break;
    case 'M':
        ScaleY = 0.0f;
        LightColor.x = 1.0f;
        LightColor.y = 1.0f;
        LightColor.z = 1.0f;
        break;
    case 'r':
        LightRotateA = !LightRotateA;
        LightRotateB = false;
        break;
    case 'R':
        LightRotateB = !LightRotateB;
        LightRotateA = false;
        break;
    case's':
        LightRotateB = false;
        LightRotateA = false;
        break;
    case 'b':
        CrainTransSize.AllTransZ += 0.2f;
        break;
    case 'B':
        CrainTransSize.AllTransZ -= 0.2f;
        break;
    case 'k':
        CrainBodyRotateA = !CrainBodyRotateA;
        CrainBodyRotateB = false;
        break;
    case 'K':
        CrainBodyRotateB = !CrainBodyRotateB;
        CrainBodyRotateA = false;
        break;
    case 'o':
        CrainArmRotateAB = !CrainArmRotateAB;
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
    if (CrainBodyRotateA) {
        CrainRotateSize.bodyRotateY++;
    }
    if (CrainBodyRotateB) {
        CrainRotateSize.bodyRotateY--;
    }
    if (CrainArmRotateAB) {
        if (CrainRotateSize.armLRotateX >= 90) {
            ArmRotateVector = -1;
        }
        else if (CrainRotateSize.armLRotateX <= -90) {
            ArmRotateVector = 1;
        }

        if (ArmRotateVector == 1) {
            CrainRotateSize.armLRotateX++;
            CrainRotateSize.armRRotateX--;
        }
        else if (ArmRotateVector == -1) {
            CrainRotateSize.armLRotateX--;
            CrainRotateSize.armRRotateX++;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(1, timer, 1);
}