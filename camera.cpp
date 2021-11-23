#include "camera.h"

void cameraFun(bool FirstPersonView, float ObjectThetaX, float ObjectThetaY,float CameraTheta, CameraViewPoint CameraPoint, GLuint shaderID,int windowWidth,int windowHeight) {
	
	
	
	if (FirstPersonView) {
		glm::vec3 PlayerCameraPos = glm::vec3(0.0f,0.0f,0.0f);        //카메라 위치 추후에 조정 
		//PlayerCameraPos += glm::vec3(AnchorPointX + 0.35f, 0.75f, AnchorPointZ + 0.25f);           카메라는 객체보다 살짝 앞에 있을것. 그 수치는 차후에 수정
		glm::vec3 ModelSpace = PlayerCameraPos;
		ModelSpace.z += 1.0f;
		glm::vec3 PlayerCameraDir = PlayerCameraPos - ModelSpace;
		glm::vec4 PlayerModelCen = glm::vec4(-PlayerCameraDir, 1);

		glm::mat4 tempPlayerCameraPos = glm::mat4(1.0f);
		tempPlayerCameraPos = glm::rotate(tempPlayerCameraPos, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));    //1인칭 경우 카메라 각도 여기서 설정, 기본 0도로 설정해놓음. 변수 지정 필요
		PlayerModelCen = tempPlayerCameraPos * PlayerModelCen;

		glm::vec3 ModelPosition = glm::vec3(
			PlayerModelCen.x + PlayerCameraPos.x, PlayerModelCen.y + PlayerCameraPos.y, PlayerModelCen.z + PlayerCameraPos.z);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec3 PlayerCameraRight = glm::cross(up, glm::normalize(PlayerCameraDir));
		glm::vec3 PlayerCameraUp = glm::cross(glm::normalize(PlayerCameraDir), PlayerCameraRight);
		glm::mat4 PlayerCameraView = glm::mat4(1.0f);
		PlayerCameraView = glm::lookAt(PlayerCameraPos, ModelPosition, PlayerCameraUp);
		unsigned int ModelViewLocation = glGetUniformLocation(shaderID, "ViewTransform");
		glUniformMatrix4fv(ModelViewLocation, 1, GL_FALSE, glm::value_ptr(PlayerCameraView));

		glm::vec3 ViewP;
		ViewP = glm::vec3(PlayerCameraPos);
		unsigned int ViewPositionLocation = glGetUniformLocation(shaderID, "ViewPosTransform");
		glUniform3fv(ViewPositionLocation, 1, glm::value_ptr(ViewP));

	}
	else {
		glm::vec3 ModelCameraPos = glm::vec3(CameraPoint.ViewX, CameraPoint.ViewY, CameraPoint.ViewZ); // EYE
		glm::mat4 CameraPosDistance = glm::mat4(1.0f);
		CameraPosDistance = glm::rotate(CameraPosDistance, glm::radians(ObjectThetaY), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 CameraPosDis = glm::vec4(ModelCameraPos, 1);
		CameraPosDis = CameraPosDistance * CameraPosDis;
		glm::vec3 CameraPostionDirection = glm::vec3(CameraPosDis.x, CameraPosDis.y, CameraPosDis.z);  //EYE 변환

		glm::vec3 ModelCameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 ModelCameraDirection = CameraPostionDirection - ModelCameraTarget;

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 CameraDirectionDistance = glm::mat4(1.0f);
		CameraDirectionDistance = glm::rotate(CameraDirectionDistance, glm::radians(CameraTheta), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 CameraDirectionDis = CameraDirectionDistance * glm::vec4(ModelCameraDirection, 1);
		glm::vec3 CameraDirection;
		CameraDirection.x = CameraDirectionDis.x;
		CameraDirection.y = CameraDirectionDis.y;
		CameraDirection.z = CameraDirectionDis.z;
		CameraDirection = glm::normalize(CameraDirection);

		glm::mat4 CenterDirectionDistance = glm::mat4(1.0f);
		CenterDirectionDistance = glm::rotate(CenterDirectionDistance, glm::radians(CameraTheta), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 CenterDirectionDis = glm::vec3(ModelCameraTarget - CameraPostionDirection);
		glm::vec4 CenterDirection = glm::vec4(CenterDirectionDis, 1);
		CenterDirection = CenterDirectionDistance * CenterDirection;
		CenterDirectionDis = glm::vec3(CenterDirection.x + CameraPostionDirection.x,
			CenterDirection.y + CameraPostionDirection.y, CenterDirection.z + CameraPostionDirection.z); //n

		glm::vec3 ModelCameraRight = glm::normalize(glm::cross(up, CameraDirection)); //v

		glm::vec3 cameraUp = glm::cross(CameraDirection, ModelCameraRight); //u

		glm::mat4 ModelView = glm::mat4(1.0f);
		ModelView = glm::lookAt(CameraPostionDirection, CenterDirectionDis, cameraUp);
		unsigned int ModelViewLocation = glGetUniformLocation(shaderID, "ViewTransform");
		glUniformMatrix4fv(ModelViewLocation, 1, GL_FALSE, glm::value_ptr(ModelView));

		//조명 카메라 위치
		glm::vec3 ViewP;
		ViewP = glm::vec3(CameraPostionDirection);
		unsigned int ViewPositionLocation = glGetUniformLocation(shaderID, "ViewPosTransform");
		glUniform3fv(ViewPositionLocation, 1, glm::value_ptr(ViewP));

    }
	//원근
	glm::mat4 ModelProj = glm::mat4(1.0f);
	ModelProj = glm::perspective(glm::radians(60.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);
	unsigned int ModelProjLocation = glGetUniformLocation(shaderID, "ProjectionTransform");
	glUniformMatrix4fv(ModelProjLocation, 1, GL_FALSE, &ModelProj[0][0]);
}