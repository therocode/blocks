Entity@ createEntity(const string type, float x, float y, float z)
{
    return cast<Entity@>(createIEntity(type, x, y, z));
}

Entity@ createEntity(const string type, Vec3 position)
{
    return cast<Entity@>(createIEntity(type, position.x, position.y, position.z));
}

void removeEntity(Entity@ entity)
{
    removeEntity(entity.getId());
}

shared class Entity : IEntity
{
    Entity(EntityCore@ core, uint id)
    {
        @mCore = core;
        mId = id;
    }

    EntityCore @mCore;
    uint mId;

    void setPosition(float x, float y, float z)
    {
        mCore.setPosition(x, y, z);
    }

    void setPosition(const Vec3 vec)
    {
        mCore.setPosition(vec.x, vec.y, vec.z);
    }

    Vec3 getPosition()
    {
        return mCore.getPosition();
    }

    bool isOnGround()
    {
        return mCore.isOnGround();
    }

    void applyImpulse(float x, float y, float z)
    {
        applyImpulseOnEntity(mId, Vec3(x, y, z));
    }

    void applyImpulse(const Vec3 force)
    {
        applyImpulseOnEntity(mId, force);
    }

    uint32 getId()
    {
        return mId;
    }
}
