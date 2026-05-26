#pragma once

#include <string>
#include <vector>

unsigned int loadTexture(const std::string& path);
unsigned int loadCubemap(const std::vector<std::string>& faces);

