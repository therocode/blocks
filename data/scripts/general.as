bool flip = false;

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
    
    if(frameNumber % 2 == 0)
    {
        Entity@ entity = createEntity("Elephant", randomFloatRange(-30.0f, 30.0f), 70.0f, randomFloatRange(-30.0f, 30.0f));
    }

    setGravity(-0.005f);

    if(frameNumber > 2000)
        setGravity(-0.001f);
}
