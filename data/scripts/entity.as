Entity@ createEntity(const string type, float x, float y, float z)
{
    return cast<Entity@>(createIEntity(type, x, y, z));
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
            setPosition(randomFloatRange(-30.0f, 30.0f), 70.0f, randomFloatRange(-30.0f, 30.0f));
            Vec3 vector;
            //consolePrint(vector.x);
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

    //void onFrame(int frameNumber)
    //{
    //    createEntity("Elephant", 
    //}
}
