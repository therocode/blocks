class Player : Entity
{
    Player(EntityCore@ core, uint id)
    {
        super(core, id);
    }

    void onFrame(int frameNumber)
    {
        consolePrint("entity id is " + mId);
    }
}
