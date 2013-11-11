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
			setPosition(0.75, 30, -1.75);
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
            Vec3 position = getPosition();
            position.y -= 1.0f;
            uint16 type = getVoxelType(position);
            consolePrint("stepped on a " + type);

            if(type == 7)
            {
                setVoxelType(position, 0);
                applyImpulse(0.0f, 5.0f, 0.0f);
            }
        }
        else
            consolePrint("lalalaaaa i am on the air");
    }
}
