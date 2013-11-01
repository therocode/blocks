class Player : Entity
{
    Player(EntityCore@ core, uint id)
    {
        super(core, id);
    }

    void onFrame(int frameNumber)
    {
        if(isOnGround())
        {
        }
        //consolePrint("entity id is " + mId);
    }

    void onGround(bool landed)
    {
        if(landed)
            consolePrint("lalalaaaa i am on the ground");
        else
            consolePrint("lalalaaaa i am on the air");
    }
}
