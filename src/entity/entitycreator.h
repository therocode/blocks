#pragma once
#include "blockstd.h"

using EntityCreator = std::function<fea::WeakEntityPtr(const std::string&, const glm::vec3&)>;
