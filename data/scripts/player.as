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
		//if(frameNumber == 1)
			//setPosition(800, 60, 800);
			//setPosition(10, 60, 0);
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
