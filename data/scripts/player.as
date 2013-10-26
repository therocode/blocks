class Player : Entity
{
    Player(EntityCore@ core, uint id)
    {
        super(core, id);
    }

    void onFrame(int frameNumber)
    {
        applyImpulse(mId, Vec3(randomFloatRange(-1.0f, 1.0f), randomFloatRange(-1.0f, 1.0f), randomFloatRange(-1.0f, 1.0f)));
    }
}
