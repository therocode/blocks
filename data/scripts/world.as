bool entityEntersWorld(uint32 oldWorldId, uint32 newWorldId, uint32 entityId)
{
    Entity@ entity = getEntity(entityId);

    if(oldWorldId == 0)
    {
        Vec3 position = entity.getPosition();

        entity.setPosition(Vec3(position.x / 8.0f, position.y, position.z / 8.0f));
    }
    else if(oldWorldId == 1)
    {
        Vec3 position = entity.getPosition();

        entity.setPosition(Vec3(position.x * 8.0f, position.y, position.z * 8.0f));
    }
    
    return true;
}
