Entity@ createEntity(const string type, float x, float y, float z)
{
    return cast<Entity@>(createIEntity(type, x, y, z));
}

shared class Entity : IEntity
{
    Entity(EntityCore@ core)
    {
        @mCore = core;
    }

    EntityCore @mCore;

    void setPosition(float x, float y, float z)
    {
        consolePrint("script trying to set entity to " + x + " " + y + " " + z);
        mCore.setPosition(x, y, z);
    }
}

class Elephant : Entity
{
    Elephant(EntityCore@ core)
    {
        super(core);
        consolePrint("hej jag skapades och luktar elefant");
    }

    ~Elephant()
    {
        consolePrint("en... elefant destruerades...");
    }
}
