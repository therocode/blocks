#pragma once
#include "blockstd.hpp"

using EntityCreator = std::function<fea::WeakEntityPtr(const std::string&, std::function<void(fea::EntityPtr)>)>;
