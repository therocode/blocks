#pragma once
#include "package.h"
#include "../messages.h"

using RebuildScriptsRequestedPackage = Package<RebuildScriptsRequested_tag, char>;

using ChunkLoadedPackage = Package<ChunkCreated_tag, ChunkCoordinate, VoxelTypeArray>;
using GfxEntityAddedPackage = Package<AddGfxEntity_tag, size_t, glm::vec3>;
using GfxEntityMovedPackage = Package<MoveGfxEntity_tag, size_t, glm::vec3>;
using GfxEntityRemovedPackage = Package<RemoveGfxEntity_tag, size_t>;
