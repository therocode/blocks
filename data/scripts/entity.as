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
        consolePrint("script trying to set entity to " + x + " " + y + " " + z);
        mCore.setPosition(x, y, z);
    }
}

class Elephant : Entity
{
    Elephant(EntityCore@ core, uint id)
    {
        super(core, id);
        consolePrint("hej jag skapades och luktar elefant");
    }

    void onFrame(int frameNumber)
    {
        consolePrint("I am an onframe elephant and my id is " + mId);
    }

    ~Elephant()
    {
        consolePrint("en... elefant destruerades...");
    }
}
