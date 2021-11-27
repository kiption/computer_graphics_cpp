#include "readObj2.h"

void ReadObj2(const std::string objfilename, std::vector<glm::vec4>& vertex, std::vector<glm::vec4>& normalVertex)
{
	int lineCount = 0;
	std::string line;
	std::string check[6];
	int vertexNum = 0;
	int normalNum = 0;
	std::ifstream inFile(objfilename);
	std::vector<glm::vec4>face;
	std::vector<glm::vec4>Noramlface;

	while (std::getline(inFile, line)) {
		if (line[0] == 'v' && line[1] == ' ') {
			vertexNum++;
			normalNum++;
		}
		std::cout << line << std::endl;
	}
	glm::vec4* vertexData = new glm::vec4[vertexNum];
	glm::vec4* normalData = new glm::vec4[normalNum];

	inFile.clear();
	inFile.seekg(0, std::ios::beg);
	vertexNum = 0;
	normalNum = 0;
	char head[2];
	int faceNum[4];
	int vnNum[4];
	std::string nt;
	char n;
	int t;
	char s;
	while (inFile >> std::noskipws >> head[0]) {
		if (head[0] == 'v') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				inFile >> std::skipws >> vertexData[vertexNum].x >> vertexData[vertexNum].y >> vertexData[vertexNum].z;
				vertexNum++;
			}
			else if (head[1] == 'n') {
				inFile >> std::skipws >> normalData[normalNum].x >> normalData[normalNum].y >> normalData[normalNum].z;
				normalNum++;
			}

			head[1] = '\0';
		}
		if (head[0] == 'f') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				inFile >> std::skipws >> faceNum[0];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> t;
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[0];


				inFile >> std::skipws >> faceNum[1];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> t;
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[1];


				inFile >> std::skipws >> faceNum[2];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> t;
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[2];

				inFile >> std::skipws >> faceNum[3];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> t;
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[3];

				glm::vec4 temp = glm::vec4(faceNum[0], faceNum[1], faceNum[2], faceNum[3]); //1
				glm::vec4 vntemp = glm::vec4(vnNum[0], vnNum[1], vnNum[2], vnNum[3]);// 1
				face.push_back(temp);
				Noramlface.push_back(vntemp);
			}
			head[1] = '\0';
		}
	}
	for (auto iter = face.begin(); iter < face.end(); iter++) {
		vertex.push_back(vertexData[(int)(iter->x) - 1]);
		vertex.push_back(vertexData[(int)(iter->y) - 1]);
		vertex.push_back(vertexData[(int)(iter->z) - 1]);
		vertex.push_back(vertexData[(int)(iter->w) - 1]);
	}
	for (auto iter = Noramlface.begin(); iter < Noramlface.end(); iter++) {
		normalVertex.push_back(normalData[(int)(iter->x) - 1]);
		normalVertex.push_back(normalData[(int)(iter->y) - 1]);
		normalVertex.push_back(normalData[(int)(iter->z) - 1]);
		normalVertex.push_back(normalData[(int)(iter->w) - 1]);
	}
	delete[] vertexData;
	delete[] normalData;
	inFile.close();
}
