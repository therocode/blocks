Entity@ createEntity(const string type, float x, float y, float z)
{
    return cast<Entity@>(createIEntity(type, x, y, z));
}

shared class Entity : IEntity
{
    //EntityCore @mCore;

    void setPosition(int x, int y)
    {
        //mCore.setPosition(x,y);
    }
}

class Elephant : Entity
{
    Elephant()
    {
        consolePrint("hej jag skapades och luktar elefant");
    }

    ~Elephant()
    {
        consolePrint("en... elefant destruerades...");
    }
}
