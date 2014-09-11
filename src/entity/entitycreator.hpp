#pragma once
#include "blockstd.hpp"

using EntityCreator = std::function<fea::WeakEntityPtr(const std::string&, const glm::vec3&)>;
