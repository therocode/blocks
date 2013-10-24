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
}

class Elephant : Entity
{
    bool teleporter;

    Elephant(EntityCore@ core, uint id)
    {
        super(core, id);
        teleporter = randomChance(0.1f);
    }

    void onFrame(int frameNumber)
    {
        if(teleporter)
        {
            setPosition(Vec3(randomFloatRange(-30.0f, 30.0f), 70.0f, randomFloatRange(-30.0f, 30.0f)));
            Vec3 vector(2.0f, 0.0f, 0.0f);
            Vec3 vector2(0.0f, 100.0f, 0.0f);
            //consolePrint("vector is " + toString(normalise(cross(vector, vector2))));

            float hej = 1.0f;
            hej = 0.0f;
            //consolePrint(toString(5.0f/hej));
        }
    }

    ~Elephant()
    {
    }
}

class Player : Entity
{
    Player(EntityCore@ core, uint id)
    {
        super(core, id);
    }

    void onFrame(int frameNumber)
    {
        //createEntity("Elephant", getPosition());
    }
}
