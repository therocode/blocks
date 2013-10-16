void onFrame(int frameNumber)
{
    print("scripts are running:!\n");

    if(frameNumber > 1200)
        setGravity(0.001f);
}
