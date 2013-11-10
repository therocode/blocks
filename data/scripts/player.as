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
		if(frameNumber == 1)
			setPosition(0.75, 60, -1.75);
			//setPosition(10, 60, 0);
            //setPosition(-11.548175f, 16.806120f, -8.115272f);
        //consolePrint("entity id is " + mId);
    }

    void onGround(bool landed)
    {
        if(landed)
        {
			consolePrint(toString(getPosition()), VERBOSE);
            consolePrint("lalalaaaa i am on the ground");
        }
        else
            consolePrint("lalalaaaa i am on the air");
    }
}
