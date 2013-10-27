class Player : Entity
{
    Player(EntityCore@ core, uint id)
    {
        super(core, id);
    }

    void onFrame(int frameNumber)
    {
        //consolePrint("entity id is " + mId);
    }

    void onGround(bool landed)
    {
        if(landed)
        {
            consolePrint("player has landed", WARNING);
        }
        else
        {
            consolePrint("player has taken off", WARNING);
        }
    }
}
