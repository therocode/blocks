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
    }

    void onGround(bool landed)
    {
        if(landed)
        {
			consolePrint(toString(getPosition()), VERBOSE);
            consolePrint("lalalaaaa i am on the ground");
            Vec3 originalPos = getPosition();
            Vec3 position = originalPos;
            position.y -= 1.0f;
            position.x = originalPos.x + 0.25;
            position.z = originalPos.z + 0.25;
            uint16 type = getVoxelType(position);

            //consolePrint("stepped on a " + type);
            bool bounce = false;
            if(type == 12)
            {
                setVoxelType(position, 13);
                bounce = true;
                //applyImpulse(0.0f, 5.0f, 0.0f);
            }

            position.x = originalPos.x - 0.25;
            position.z = originalPos.z + 0.25;
            type = getVoxelType(position);
            if(type == 12)
            {
                setVoxelType(position, 13);
              //  applyImpulse(0.0f, 5.0f, 0.0f);
              bounce = true;
            }

            position.x = originalPos.x - 0.25;
            position.z = originalPos.z - 0.25;
            type = getVoxelType(position);
            if(type == 12)
            {
                setVoxelType(position, 13);
              //  applyImpulse(0.0f, 5.0f, 0.0f);
               bounce = true;
            }

            position.x = originalPos.x + 0.25;
            position.z = originalPos.z - 0.25;
            type = getVoxelType(position);
            if(type == 12)
            {
                setVoxelType(position, 13);
               bounce = true;
            }
            if(bounce){
                applyImpulse(0.0f, 5.0f, 0.0f);
            }

        }
        else
            consolePrint("lalalaaaa i am on the air");
    }
}
