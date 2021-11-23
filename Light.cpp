#include "Light.h"

void LightFun(glm::vec3 &LightColor, float LightRaidian, float ScaleY, GLuint shaderID, float ScaleZ) {

    glm::mat4 LightPosition = glm::mat4(1.0f);
    LightPosition = glm::rotate(LightPosition, glm::radians(LightRaidian), glm::vec3(0.0f, 1.0f, 0.0f));
    LightPosition = glm::translate(LightPosition, glm::vec3(0.0f, ScaleY, ScaleZ - 0.5f));
    unsigned int lightPosLocation = glGetUniformLocation(shaderID, "LightPos");
    glUniform3fv(lightPosLocation, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
    unsigned int lightColorLocation = glGetUniformLocation(shaderID, "LightColor");
    glUniform3fv(lightColorLocation, 1, glm::value_ptr(LightColor));
    unsigned int LightTransformLocation = glGetUniformLocation(shaderID, "LightTransform");
    glUniformMatrix4fv(LightTransformLocation, 1, GL_FALSE, glm::value_ptr(LightPosition));


}