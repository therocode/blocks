bool flip = false;

void onFrame(int frameNumber)
{
    //consolePrint("scripts are running:!\n");

    if(randomChance(0.5f / 60.0f))
    {
        if(!flip)
        {
            setGravity(0.001f);
            flip = true;
            consolePrint("flipped gravity!\n");
        }
        else
        {
            Entity@ entity = createEntity("Elephant");
            setGravity(-0.003f);
            flip = false;
            consolePrint("flipped gravity to " + toString(-0.003f) + " !\n");
        }

    }

    if(frameNumber > 2000)
        setGravity(-0.001f);
}
