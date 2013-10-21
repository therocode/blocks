bool flip = false;

Entity@ created;

void onFrame(int frameNumber)
{
    //consolePrint("scripts are running:!\n");

    if(randomChance(0.0f))
    {
        if(!flip)
        {
            setGravity(0.001f);
            flip = true;
            consolePrint("flipped gravity!\n");
        }
        else
        {
            setGravity(-0.003f);
            flip = false;
            consolePrint("flipped gravity to " + toString(-0.003f) + " !\n");
        }

    }
    
    if(frameNumber % 10 == 0)
    {
        @created = createEntity("Elephant", randomFloatRange(-30.0f, 30.0f), 70.0f, randomFloatRange(-30.0f, 30.0f));

        //created.setPosition(0.0f, 50.0f, 0.0f);

    }
    else if(frameNumber % 10 == 1)
    {
        //removeEntity(created);
    }

    setGravity(-0.005f);

    if(frameNumber > 2000)
        setGravity(-0.001f);
}
