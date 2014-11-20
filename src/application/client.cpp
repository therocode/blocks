#include "client.hpp"
#include <iostream>
#include <fea/ui/sdl2windowbackend.hpp>
#include <fea/ui/sdl2inputbackend.hpp>
#include "../lognames.hpp"
#include "../application/applicationmessages.hpp"
#include "../rendering/renderingsystem.hpp"
#include "../input/inputadaptor.hpp"
#include "../world/raycaster.hpp"


Client::Client(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mFrameNumber(0),
    mBus(bus),
    mLogger(mBus, LogLevel::VERB),
	mWindow(new fea::SDL2WindowBackend()),
	mInputAdaptor(std::unique_ptr<InputAdaptor>(new InputAdaptor(mBus))),
	mQuit(false)
{
    subscribe(mBus, *this);

	mWindow.create(fea::VideoMode(800, 600, 32), "Blocky", fea::Style::Default, fea::ContextSettings(32, 32, 0, 3, 3));
    mLockedMouse = true;
	mWindow.lockCursor(true);
	mWindow.setVSyncEnabled(false);
	mRenderingSystem = std::unique_ptr<RenderingSystem>(std::unique_ptr<RenderingSystem>(new RenderingSystem(mBus)));
	mWindow.setFramerateLimit(60);

	mBus.send(WindowResizeMessage{800, 600});

	std::vector<unsigned char> icon;
	loadTexture("data/textures/icon16x16.png", 16, 16, icon);
	mWindow.setIcon(16, 16, icon.data());
    mFPSCounter.setMaxFPS(0);
    mFPSCounter.setSampleTime(0.5f);
	//if there's an error, display it

    mClientNetworkingSystem = std::unique_ptr<ClientNetworkingSystem>(new ClientNetworkingSystem(bus, parameters));
}

Client::~Client()
{
	mWindow.close();
	mBus.send(LogMessage{"Shutting down client", clientName, LogLevel::INFO});
}

bool Client::loadTexture(const std::string& path, uint32_t width, uint32_t height, std::vector<unsigned char>& result)
{

	//decode
	unsigned error = lodepng::decode(result, width, height, path);

	//if there's an error, display it
	if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	return true;
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
}

void Client::update()
{
    mBus.send(FrameMessage{mFrameNumber});
	mInputAdaptor->update();
    mFrameNumber++;
}

void Client::render()
{
    mFPSCounter.frameBegin();
	mRenderingSystem->render();
	mWindow.swapBuffers();
    mFPSCounter.frameEnd();
    if(mFrame++ % 60 == 0)
    mWindow.setTitle("Blocks | FPS:" + std::to_string((int)mFPSCounter.getAverageFPS()));
}

void Client::handleMessage(const ClientActionMessage& received)
{
	if(received.action == InputAction::QUIT)
	{
		mQuit = true;
	}
}

void Client::handleMessage(const ClientChunksDeliveredMessage& received)
{
    for(uint32_t i = 0; i < received.coordinates.size(); i++)
    {
        const ChunkCoord& coordinate = received.coordinates[i];
        if(mHighlightedChunks.chunkIsHighlighted(coordinate))
        {
            Chunk chunk(received.rleIndices[i], received.rleSegments[i]);

            mLocalChunks[coordinate] = chunk;

            updateChunk(coordinate);

            if(mLocalChunks.find(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1));
            }
        }
    }
}

void Client::handleMessage(const VoxelUpdatedMessage& received)
{
    ChunkCoord chunkCoord = VoxelToChunk::convert(received.voxel);
    ChunkVoxelCoord chunkVoxelCoord = VoxelToChunkVoxel::convert(received.voxel);

    auto chunk = mLocalChunks.find(chunkCoord);

    if(chunk != mLocalChunks.end())
    {
        chunk->second.setVoxelType(chunkVoxelCoord, received.type);
        updateChunk(chunkCoord);
    }

    ChunkCoord leftNeighbour = ChunkCoord(chunkCoord.x - 1, chunkCoord.y, chunkCoord.z);
    if(chunkVoxelCoord.x == 0 && mLocalChunks.find(leftNeighbour) != mLocalChunks.end())
    {
        updateChunk(leftNeighbour);
    }

    ChunkCoord rightNeighbour = ChunkCoord(chunkCoord.x + 1, chunkCoord.y, chunkCoord.z);
    if(chunkVoxelCoord.x == chunkWidth - 1 && mLocalChunks.find(rightNeighbour) != mLocalChunks.end())
    {
        updateChunk(rightNeighbour);
    }

    ChunkCoord topNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y + 1, chunkCoord.z);
    if(chunkVoxelCoord.y == chunkWidth - 1 && mLocalChunks.find(topNeighbour) != mLocalChunks.end())
    {
        updateChunk(topNeighbour);
    }

    ChunkCoord bottomNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y - 1, chunkCoord.z);
    if(chunkVoxelCoord.y == 0 && mLocalChunks.find(bottomNeighbour) != mLocalChunks.end())
    {
        updateChunk(bottomNeighbour);
    }

    ChunkCoord frontNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y, chunkCoord.z + 1);
    if(chunkVoxelCoord.z == chunkWidth - 1 && mLocalChunks.find(frontNeighbour) != mLocalChunks.end())
    {
        updateChunk(frontNeighbour);
    }

    ChunkCoord backNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y, chunkCoord.z - 1);
    if(chunkVoxelCoord.z == 0 && mLocalChunks.find(backNeighbour) != mLocalChunks.end())
    {
        updateChunk(backNeighbour);
    }
}

void Client::handleMessage(const ClientChunkDeletedMessage& received)
{
    if(mLocalChunks.erase(received.coordinate) > 0)
    {
        mBus.send(ChunkVBODeletedMessage{received.coordinate});
    }
}

void Client::handleMessage(const CursorLockedMessage& received)
{
    mWindow.lockCursor(received.locked);
}

void Client::handleMessage(const GameStartMessage& received)
{
}

void Client::handleMessage(const ClientAttachedToEntityMessage& received)
{
    mCurrentWorld = received.world;
    mCurrentEntity = received.entityId;
    mHighlightRadius = received.highlightRange;
    auto highlighted = mHighlightedChunks.addHighlightEntity(0, WorldToChunk::convert(received.position), mHighlightRadius);
    mLastChunk = WorldToChunk::convert(received.position);

    if(highlighted.size() > 0)
        mBus.send(ClientRequestedChunksMessage{mCurrentWorld, highlighted});
}

void Client::handleMessage(const ClientEnteredWorldMessage& received)
{
    mCurrentWorld = received.world;

    auto dehighlighted = mHighlightedChunks.removeHighlightEntity(0);

    auto highlighted = mHighlightedChunks.addHighlightEntity(0, mLastChunk, mHighlightRadius);

    if(highlighted.size() > 0)
        mBus.send(ClientRequestedChunksMessage{mCurrentWorld, highlighted});

    for(const auto& chunk : dehighlighted)
        mBus.send(ClientChunkDeletedMessage{chunk});
}

void Client::handleMessage(const ClientPositionMessage& received)
{
    const auto& highlighted = mHighlightedChunks.moveHighlightEntity(0, WorldToChunk::convert(received.position));
    mLastChunk = WorldToChunk::convert(received.position);

    if(highlighted.first.size() > 0)
        mBus.send(ClientRequestedChunksMessage{mCurrentWorld, highlighted.first});

    for(const auto& chunk : highlighted.second)
        mBus.send(ClientChunkDeletedMessage{chunk});
}

bool Client::requestedQuit()
{
	return mQuit;
}

void Client::updateChunk(const ChunkCoord& coordinate)
{
    Chunk* mainChunk = &mLocalChunks.at(coordinate);
    Chunk* topChunk = nullptr;
    Chunk* bottomChunk = nullptr;
    Chunk* frontChunk = nullptr;
    Chunk* backChunk = nullptr;
    Chunk* leftChunk = nullptr;
    Chunk* rightChunk = nullptr;

    auto top    = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z));
    auto bottom = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z));
    auto front  = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1));
    auto back   = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1));
    auto left   = mLocalChunks.find(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z));
    auto right  = mLocalChunks.find(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z));

    if(top != mLocalChunks.end())
        topChunk = &top->second;
    if(bottom != mLocalChunks.end())
        bottomChunk = &bottom->second;
    if(front != mLocalChunks.end())
        frontChunk = &front->second;
    if(back != mLocalChunks.end())
        backChunk = &back->second;
    if(left != mLocalChunks.end())
        leftChunk = &left->second;
    if(right != mLocalChunks.end())
        rightChunk = &right->second;

    mBus.send(UpdateChunkVboMessage{coordinate, mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk});
}

void Client::updateVoxelLookAt()
{
	glm::vec3 direction = glm::vec3(glm::cos(mPitch) * glm::sin(mYaw), glm::sin(mPitch), glm::cos(mPitch) * glm::cos(mYaw));

	VoxelCoord block;
	uint32_t face;
	bool f = RayCaster::getVoxelAtRay(mLocalChunks, mPosition + glm::vec3(0, 0.6f, 0), direction, 200.f, face, block);

    mBus.send(FacingBlockMessage{block});
}

void Client::handleMessage(const MoveGfxEntityMessage& received)
{
    if(received.id == mCurrentEntity)
    {
        mPosition = received.position;
        updateVoxelLookAt();
    }
}

void Client::handleMessage(const RotateGfxEntityMessage& received)
{
    if(received.id == mCurrentEntity)
    {
        mPitch = received.pitch;
        mYaw = received.yaw;
    }
}
