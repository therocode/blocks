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

    void setPosition(int x, int y)
    {
        mCore.setPosition(x,y);
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
