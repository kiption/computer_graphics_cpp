#pragma once
#include<string>
#include<glm/glm/glm.hpp>
#include<fstream>
#include<vector>
#include<string>
#include<glm/glm/glm.hpp>
#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<glm/glm/glm.hpp>
#include<glm/glm/ext.hpp>
#include<glm/glm/gtc/matrix_transform.hpp>

struct CameraViewPoint {
    float ViewX = 0.0f;
    float ViewY = 2.0f;
    float ViewZ = 6.0f;
};

void cameraFun(bool FirstPersonView, float ObjectThetaX, float ObjectThetaY, float CameraTheta, CameraViewPoint CameraPoint, GLuint shaderID, int windowWidth, int windowHeight);

	