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


int windowWidth, windowHeight;

int ObitalRotate = 0;
float ObjectTheta = 0.0f;
float CameraTheta = 0.0f;
float ObjectThetaX = 0.0f;
float ObjectThetaY = 0.0f;
float LightRaidian = 0.0f;
float SquareRotate = 0.0f;

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
bool ChangeModel = false;

struct LightObital {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

}LightRotateOrbital;

//조명 초기위치
float ScaleZ = 3.0f;
float ScaleY = 0.0f;

CameraViewPoint PrimeCameraPoint;

GLuint VAO[11], VBO[33];
GLuint shaderID;
GLuint qobjshader;
GLuint vertexshader;
GLuint fragmentshader;
GLuint triangleshaderProgram;

vector<glm::vec4>ModelsValue[11];
vector<glm::vec4>normalValue[11];
vector<glm::vec4>vtValue[11];

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

    ReadObj2("square1.obj", ModelsValue[0],normalValue[0], vtValue[0]);
    ReadObj2("square2.obj", ModelsValue[1],normalValue[1], vtValue[1]);
    ReadObj2("square3.obj", ModelsValue[2],normalValue[2], vtValue[2]);
    ReadObj2("square1.obj", ModelsValue[3],normalValue[3], vtValue[3]);
    ReadObj2("square2.obj", ModelsValue[4],normalValue[4], vtValue[4]);
    ReadObj2("square3.obj", ModelsValue[5],normalValue[5], vtValue[5]);

    ReadObj2("square1.obj", ModelsValue[6], normalValue[6], vtValue[6]);
    ReadObj2("triangle1.obj", ModelsValue[7], normalValue[7], vtValue[7]);
    ReadObj2("triangle2.obj", ModelsValue[8], normalValue[8], vtValue[8]);
    ReadObj2("triangle3.obj", ModelsValue[9], normalValue[9], vtValue[9]);
    ReadObj2("triangle4.obj", ModelsValue[10], normalValue[10], vtValue[10]);
    
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

    //고정 물체 사각형
    if (ChangeModel) {
        glBindVertexArray(VAO[0]);
        glm::mat4 HexaUpModel = glm::mat4(1.0f);
        //HexaUpModel = glm::rotate(HexaUpModel, glm::radians(AxisAnglex), glm::vec3(1.0f, 0.0f, 0.0f));
        HexaUpModel = glm::rotate(HexaUpModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        HexaUpModel = glm::translate(HexaUpModel, glm::vec3(0.0f, 0.51f, 0.0f));
        //HexaUpModel = glm::rotate(HexaUpModel, glm::radians(ModelAngleSize.upAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        unsigned int ModelUpLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(ModelUpLocation, 1, GL_FALSE, glm::value_ptr(HexaUpModel));
        unsigned int ModelUpFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(ModelUpFragLocation, 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_QUADS, 0, ModelsValue[0].size());

        glBindVertexArray(VAO[1]);
        glm::mat4 HexaFrontModel = glm::mat4(1.0f);
        //HexaFrontModel = glm::rotate(HexaFrontModel, glm::radians(AxisAnglex), glm::vec3(1.0f, 0.0f, 0.0f));
        HexaFrontModel = glm::rotate(HexaFrontModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        //HexaFrontModel = glm::translate(HexaFrontModel, glm::vec3(0.0f, 0.0f, ModelTransSize.transZ));
        //HexaFrontModel = glm::rotate(HexaFrontModel, glm::radians(ModelAngleSize.frontAngel), glm::vec3(1.0f, 0.0f, 0.0f));
        HexaFrontModel = glm::translate(HexaFrontModel, glm::vec3(0.0f, 0.0f, 0.51f));
        HexaFrontModel = glm::rotate(HexaFrontModel, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        unsigned int ModelFrontLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(ModelFrontLocation, 1, GL_FALSE, glm::value_ptr(HexaFrontModel));
        unsigned int ModelFrontFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(ModelFrontFragLocation, 1.0f, 0.4f, 0.0f);
        glDrawArrays(GL_QUADS, 0, ModelsValue[1].size());

        glBindVertexArray(VAO[2]);
        glm::mat4 HexaLeftModel = glm::mat4(1.0f);
        //HexaLeftModel = glm::rotate(HexaLeftModel, glm::radians(AxisAnglex), glm::vec3(1.0f, 0.0f, 0.0f));
        HexaLeftModel = glm::rotate(HexaLeftModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        HexaLeftModel = glm::translate(HexaLeftModel, glm::vec3(-0.51, 0.0f, 0.0f));
        //HexaLeftModel = glm::translate(HexaLeftModel, glm::vec3(0.0f, ModelTransSize.transY, 0.0f));
        //HexaLeftModel = glm::rotate(HexaLeftModel, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //HexaLeftModel = glm::rotate(HexaLeftModel, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int ModelLeftLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(ModelLeftLocation, 1, GL_FALSE, glm::value_ptr(HexaLeftModel));
        unsigned int ModelLeftFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(ModelLeftFragLocation, 0.54f, 0.0f, 1.0f);
        glDrawArrays(GL_QUADS, 0, ModelsValue[2].size());

        glBindVertexArray(VAO[3]);
        glm::mat4 HexaDownModel = glm::mat4(1.0f);
        //HexaDownModel = glm::rotate(HexaDownModel, glm::radians(AxisAnglex), glm::vec3(1.0f, 0.0f, 0.0f));
        HexaDownModel = glm::rotate(HexaDownModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        HexaDownModel = glm::translate(HexaDownModel, glm::vec3(0.0f, -0.51f, 0.0f));
        //HexaDownModel = glm::rotate(HexaDownModel, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //HexaDownModel = glm::rotate(HexaDownModel, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int ModelDownLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(ModelDownLocation, 1, GL_FALSE, glm::value_ptr(HexaDownModel));
        unsigned int ModelDownFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(ModelDownFragLocation, 0.86f, 0.625f, 0.86f);
        glDrawArrays(GL_QUADS, 0, ModelsValue[3].size());

        glBindVertexArray(VAO[4]);
        glm::mat4 HexaBackModel = glm::mat4(1.0f);
        //HexaBackModel = glm::rotate(HexaBackModel, glm::radians(AxisAnglex), glm::vec3(1.0f, 0.0f, 0.0f));
        HexaBackModel = glm::rotate(HexaBackModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        HexaBackModel = glm::translate(HexaBackModel, glm::vec3(0.0f, 0.0f, -0.51f));
        //HexaBackModel = glm::rotate(HexaBackModel, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //HexaBackModel = glm::rotate(HexaBackModel, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int ModelBackLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(ModelBackLocation, 1, GL_FALSE, glm::value_ptr(HexaBackModel));
        unsigned int ModelBackFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(ModelBackFragLocation, 1.0f, 1.0f, 0.0f);
        glDrawArrays(GL_QUADS, 0, ModelsValue[4].size());

       

        glBindVertexArray(VAO[5]);
        glm::mat4 HexaRightModel = glm::mat4(1.0f);
        //HexaRightModel = glm::rotate(HexaRightModel, glm::radians(AxisAnglex), glm::vec3(1.0f, 0.0f, 0.0f));
        HexaRightModel = glm::rotate(HexaRightModel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        HexaRightModel = glm::translate(HexaRightModel, glm::vec3(0.51f, 0.0f, 0.0f));
        //HexaRightModel = glm::translate(HexaRightModel, glm::vec3(0.0f, ModelTransSize.transY, 0.0f));
        //HexaRightModel = glm::rotate(HexaRightModel, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //HexaRightModel = glm::rotate(HexaRightModel, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int ModelRightLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(ModelRightLocation, 1, GL_FALSE, glm::value_ptr(HexaRightModel));
        unsigned int ModelRightFragLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(ModelRightFragLocation, 0.54f, 0.0f, 1.0f);
        glDrawArrays(GL_QUADS, 0, ModelsValue[5].size());
    }
    else {
    //    고정 물체 피라미드
    //    glBindVertexArray(VAO[3]);
      /*  glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        unsigned int modelLocation = glGetUniformLocation(shaderID, "ModelTransform");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glm::mat4 Normalmodel = glm::mat4(1.0f);
        Normalmodel = glm::rotate(Normalmodel, glm::radians(SquareRotate), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int NormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
        glUniformMatrix4fv(NormalmodelLocation, 1, GL_FALSE, glm::value_ptr(Normalmodel));
        unsigned int objColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
        glUniform3f(objColorLocation, 0.6f, 0.5f, 0.3f);
        glDrawArrays(GL_TRIANGLES, 0, ModelsValue[2].size());*/
    }

    //궤도
    glBindVertexArray(VAO[1]);
    glm::mat4 OrbitalModel = glm::mat4(1.0f);
    OrbitalModel = glm::scale(OrbitalModel, glm::vec3(ScaleZ, 0.0f, ScaleZ));
    unsigned int OrbitalModelModelLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(OrbitalModelModelLocation, 1, GL_FALSE, glm::value_ptr(OrbitalModel));
    unsigned int OrbitalModelLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(OrbitalModelLocation, 0.2f, 0.5f, 0.8f);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_STRIP, 0, ObitalValue.size() / 6);

    //조명 박스
    glBindVertexArray(VAO[2]);
    glm::mat4 LightBox = glm::mat4(1.0f);

    LightBox = glm::rotate(LightBox, glm::radians(LightRaidian), glm::vec3(0.0f, 1.0f, 0.0f));
    LightBox = glm::translate(LightBox, glm::vec3(0.0f, 0.0f, ScaleZ));
    LightBox = glm::scale(LightBox, glm::vec3(1.0f, 1.0f, 1.0f));
    unsigned int LightBoxLocation = glGetUniformLocation(shaderID, "ModelTransform");
    glUniformMatrix4fv(LightBoxLocation, 1, GL_FALSE, glm::value_ptr(LightBox));
    glm::mat4 LightBoxNormalmodel = glm::mat4(1.0f);
    LightBoxNormalmodel = glm::rotate(LightBoxNormalmodel, glm::radians(LightRaidian), glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int LightBoxNormalmodelLocation = glGetUniformLocation(shaderID, "NormalTransform");
    glUniformMatrix4fv(LightBoxNormalmodelLocation, 1, GL_FALSE, glm::value_ptr(LightBoxNormalmodel));
    unsigned int LightBoxColorLocation = glGetUniformLocation(shaderID, "ObjectColor");
    glUniform3f(LightBoxColorLocation, 0.2f, 0.2f, 0.2f);
    glDrawArrays(GL_TRIANGLES, 0, ModelsValue[1].size());

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
   
    glGenVertexArrays(11, VAO);

    for (int i = 0; i < 11; i++) {
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vtValue[i].size(), &vtValue[i][0], GL_STREAM_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(2);
    }
}

GLvoid keyboard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'y':
        CameraRotationA = !CameraRotationA;
        CameraRotationB = false;
        break;
    case 'Y':
        CameraRotationB = !CameraRotationB;
        CameraRotationA = false;
        break;
    case 'z':
        ScaleZ += 0.2f;
        MakeOrbital = true;
        break;
    case 'Z':
        ScaleZ -= 0.2f;
        MakeOrbital = true;
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
        LightRotateA = !LightRotateA;
        LightRotateB = false;
        break;
    case'R':
        LightRotateB = !LightRotateB;
        LightRotateA = false;
        break;
    case 'c':
      
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
        LightRotateOrbital.x = LightOrbitalValue[6 * ObitalRotate];
        LightRotateOrbital.y = LightOrbitalValue[6 * ObitalRotate + 1];
        LightRotateOrbital.z = LightOrbitalValue[6 * ObitalRotate + 2];
        ObitalRotate++;
        LightRaidian -= 1.0f;
        if (ObitalRotate == 360) {
            ObitalRotate = 0;
        }
    }
    if (SquareRotateA) {
        SquareRotate += 1.0f;
    }
    if (SquareRotateB) {
        SquareRotate -= 1.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(1, timer, 1);
}