bool flip = false;

void onFrame(int frameNumber)
{
    //consolePrint("scripts are running:!\n");

    if(randomIntRange(0, 60) == 0)
    {
        if(!flip)
        {
            setGravity(0.003f);
            flip = true;
            consolePrint("flipped gravity!\n");
        }
        else
        {
            setGravity(-0.003f);
            flip = false;
            consolePrint("flipped gravity!\n");
        }

    }

    if(frameNumber > 2000)
        setGravity(0.001f);
}
