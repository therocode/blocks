class Entity
{
    void onFrame() 
    {
    }
}

class Elephant : Entity
{
    Elephant()
    {
        //consolePrint("hej jag skapades och luktar elefant\n");
    }

    ~Elephant()
    {
        //consolePrint("en... elefant destruerades...\n");
    }
}