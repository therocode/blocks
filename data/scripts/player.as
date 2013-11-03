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
		if(frameNumber == 10)
			//setPosition(80000, 60, 80000);
			setPosition(0, -8100, 0);
        //consolePrint("entity id is " + mId);
    }

    void onGround(bool landed)
    {
        if(landed){
			consolePrint(toString(getPosition()), VERBOSE);
            consolePrint("lalalaaaa i am on the ground");
        }else
            consolePrint("lalalaaaa i am on the air");
    }
}
