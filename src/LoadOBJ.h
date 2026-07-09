#pragma once

#include <string>

#include "Components.h"

const int VERTICES_PER_TRIANGLE = 3;


Model loadObjFile(const std::string& path);
