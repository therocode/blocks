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
			setPosition(81920, 30, 0);
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
