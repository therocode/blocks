#include "billboard.h"

Billboard::Billboard(const VBO& vbo, const glm::vec3& position, float pitch, float yaw) : mVbo(vbo), mPosition(position), mPitch(pitch), mYaw(yaw)
{
}
