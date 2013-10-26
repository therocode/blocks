Entity@ createEntity(const string type, float x, float y, float z)
{
    return cast<Entity@>(createIEntity(type, x, y, z));
}

Entity@ createEntity(const string type, Vec3 position)
{
    return cast<Entity@>(createIEntity(type, position.x, position.y, position.z));
}

shared class Entity : IEntity
{
    Entity(EntityCore@ core, uint id)
    {
        @mCore = core;
        mId = id;
    }

    void setId(uint id)
    {
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

    //void applyImpulse(const Vec3 force)
    //{
    //    applyImpulse(mId, force);
    //}
}
