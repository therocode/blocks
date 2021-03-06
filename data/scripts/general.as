bool flip = false;

Entity@ created;

void gameStarted()
{
    //consolePrint("Verbose message from the script", VERBOSE);
    //consolePrint("Info message from the script", INFO);
    //consolePrint("Warning message from the script", WARNING);
    //consolePrint("Error message from the script", ERROR);
    consolePrint("----------------Game started!---------------------");
    consolePrint("<- tag test", "Giraffe");
    consolePrint("<- tag+level test " + toString(Vec3(0.0f, 1.0f, -1.0f)), "Giraffe", VERBOSE);

    createEntity("pallkars", 0, Vec3(5.0f * 1 + 0.5f, 100.0f + randomFloatRange(0.f, 80.f), 5.0f * 0 + 0.5f));
    createEntity("pallkars", 0, Vec3(5.0f * 0 + 0.5f, 100.0f + randomFloatRange(0.f, 80.f), 5.0f * 1 + 0.5f));
    createEntity("pallkars", 0, Vec3(5.0f * 1 + 0.5f, 100.0f + randomFloatRange(0.f, 80.f), 5.0f * 2 + 0.5f));
    createEntity("poring", 0, Vec3(5.0f * 0 + 0.5f, 100.0f + randomFloatRange(0.f, 80.f), 5.0f * 0 + 0.5f));
    createEntity("poring", 0, Vec3(5.0f * 1 + 0.5f, 100.0f + randomFloatRange(0.f, 80.f), 5.0f * 1 + 0.5f));
    createEntity("poring", 0, Vec3(5.0f * 0 + 0.5f, 100.0f + randomFloatRange(0.f, 80.f), 5.0f * 2 + 0.5f));
}

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
            setGravity(-0.001f);
            flip = false;
            consolePrint("flipped gravity to " + toString(-0.003f) + " !\n");
        }

    }
    
    if(frameNumber % 1 == 0)
    {
       //@created = createEntity("Elephant", randomFloatRange(-30.0f, 30.0f), 70.0f, randomFloatRange(-30.0f, 30.0f));
       setVoxelType(0, Vec3(randomFloatRange(-30.0f, 30.0f), 40.0f, randomFloatRange(-30.0f, 30.0f)), 12);
    }
    else if(frameNumber % 10 == 1)
    {
        //removeEntity(created);
    }

    //setGravity(-0.005f);

    //if(frameNumber > 2000)
    //   setGravity(0.001f);
}
