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
            //consolePrint("lalalaaaa i am on the ground");
        }
        //consolePrint("entity id is " + mId);
    }

    void onGround(bool landed)
    {
        if(landed)
        {
            consolePrint("player has landed", WARNING);

            for(int i = 0; i < 50; i++)
            {
                Entity@ elephant = createEntity("Elephant", getPosition());
                elephant.applyImpulse(randomFloatRange(-0.5f, 0.5f), randomFloatRange(0.1f, 1.0f), randomFloatRange(-0.5f, 0.5f));
            }
        }
        else
        {
            consolePrint("player has taken off", WARNING);
        }
    }
}
