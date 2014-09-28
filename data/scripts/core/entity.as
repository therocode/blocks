Entity@ createEntity(const string type, uint32 worldId, Vec3 position)
{
    return cast<Entity@>(createIEntity(type, worldId, position));
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

    void setPosition(const Vec3 vec)
    {
        mCore.setPosition(vec);
    }

    Vec3 getPosition()
    {
        return mCore.getPosition();
    }

    bool isOnGround()
    {
        return mCore.isOnGround();
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
