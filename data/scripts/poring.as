class poring : Entity
{
    bool teleporter;

    poring(EntityCore@ core, uint id)
    {
        super(core, id);
        teleporter = randomChance(0.1f);
    }

    void onFrame(int frameNumber)
    {
        if(teleporter)
        {
            //setPosition(Vec3(randomFloatRange(-30.0f, 30.0f), 70.0f, randomFloatRange(-30.0f, 30.0f)));
            Vec3 vector(2.0f, 0.0f, 0.0f);
            Vec3 vector2(0.0f, 100.0f, 0.0f);
            //consolePrint("vector is " + toString(normalise(cross(vector, vector2))));

            float hej = 1.0f;
            hej = 0.0f;
            //consolePrint(toString(5.0f/hej));
        }
    }

    ~poring()
    {
    }
}
