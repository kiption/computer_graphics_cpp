#pragma once
#include<string>
#include<glm/glm/glm.hpp>
#include<fstream>
#include<vector>
#include<string>
#include<glm/glm/glm.hpp>
#include<iostream>
void ReadObj2(const std::string objfilename, std::vector<glm::vec4>& vertex, std::vector<glm::vec4>& normalVertex, std::vector<glm::vec4>& vtVertex);
