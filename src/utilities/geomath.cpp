#include "geomath.hpp"

float AABB::min(const int& axis)const{
    if(axis == 0){
        return x;
    }
    if(axis == 1){
        return y;
    }
    if(axis == 2){
        return z;
    }
    return x;
}

float AABB::max(const int& axis)const{
    if(axis == 0){
        return x + width;
    }
    if(axis == 1){
        return y + height;
    }
    if(axis == 2){
        return z + depth;
    }
    return x;
}

bool testAABBAABB(const AABB& a, const AABB& b) 
{
    if(a.x + a.width  <= b.x || a.x >= b.x + b.width)  return false;
    if(a.y + a.height <= b.y || a.y >= b.y + b.height) return false;
    if(a.z + a.depth  <= b.z || a.z >= b.z + b.depth)  return false;
    return true;
}

bool testAABBAABB(const AABB& a, const AABB& b, glm::vec3& smallestPenetration)
{
    if(testAABBAABB(a, b)){
        glm::vec3 aSize = glm::vec3(a.width, a.height, a.depth);
        glm::vec3 bSize = glm::vec3(b.width, b.height, b.depth);
        glm::vec3 d = glm::vec3(a.x, a.y, a.z) - glm::vec3(b.x, b.y, b.z);
        for(int i = 0; i < 3; i ++){
            if(d[i] < 0){
                d[i] += aSize[i];
            }else{
                d[i] -= bSize[i];
            }
        }
        float rn = glm::abs(d.x);
        int h = 0;
        if(glm::abs(d.y) < rn){
            rn = glm::abs(d.y);
            h = 1;
        }
        if(glm::abs(d.z < rn)){
            rn = glm::abs(d.z);
            h = 2;
        }
        smallestPenetration = glm::vec3(0.f);
        smallestPenetration[h] = d[h];
        if(d[h] < 0)
            smallestPenetration[h] -= EPSILON;
        else
            smallestPenetration[h] += EPSILON;
        return true;
    }
    return false;
}

float sweepAABB(const AABB _a, const AABB _b, const glm::vec3 va, const glm::vec3 vb, glm::ivec3& n)
{
    glm::vec3 v = va - vb;
    AABB newAABB = _a;
    newAABB.x = v.x;
    newAABB.y = v.y;
    newAABB.z = v.z;
    AABB a = _a;
    AABB b = _b;
    b.x -= a.x;
    b.y -= a.y;
    b.z -= a.z;
    a.x = a.y = a.z = 0.0f;


    n.x = n.y = n.z = 0;
    if(!testAABBAABB(newAABB, b))
    {
        return 1.0;
    }
    if(false && testAABBAABB(a, b)){
        glm::vec3 d = glm::vec3(a.x, a.y, a.z) - glm::vec3(b.x, b.y, b.z);
        glm::vec3 r = d / v;
        float rn = glm::abs(r.x);
        int h = 0;
        if(glm::abs(r.y) < rn){
            rn = glm::abs(r.y);
            h = 1;
        }
        if(glm::abs(r.z < rn)){
            rn = glm::abs(r.z);
            h = 2;
        }
        n[h] = (v[h] < 0)?1: -1;
        return 1.0;
    }



    float epsilon = 0.01f;
    float xEntry, yEntry, zEntry;
    float xExit,  yExit,  zExit;
    if(v.x > 0.0f)
    {
        xEntry = b.x - (a.x + a.width);
        xExit  = (b.x + b.width) - a.x;
    }else
    {
        xEntry = (b.x + b.width) - a.x;
        xExit  = b.x - (a.x + a.width);
    }

    if(v.y > 0.0f)
    {
        yEntry = b.y - (a.y + a.height);
        yExit  = (b.y + b.height) - a.y;
    }else
    {
        yEntry = (b.y + b.height) - a.y;
        yExit  = b.y - (a.y + a.height);
    }

    if(v.z > 0.0f)
    {
        zEntry = b.z - (a.z + a.depth);
        zExit  = (b.z + b.depth) - a.z;
    }else
    {
        zEntry = (b.z + b.depth) - a.z;
        zExit  = b.z - (a.z + a.depth);
    }
    //  printf("entry: %f, %f,%f\n", xEntry, yEntry, zEntry);
    float xs, ys, zs;
    float xe, ye, ze;
    float infinity = std::numeric_limits<float>::infinity();

    if(v.x == 0)
    {
        xs = -infinity;
        xe =  infinity;
    }else
    {
        xs = xEntry / v.x;
        xe = xExit  / v.x;
    }

    if(v.y == 0)
    {
        ys = -infinity;
        ye =  infinity;
    }else
    {
        ys = yEntry / v.y;
        ye = yExit  / v.y;
    }
    if(v.z  == 0)
    {
        zs = -infinity;
        ze =  infinity;
    }else
    {
        zs = zEntry / v.z;
        ze = zExit  / v.z;
    }

    float entry = xs;
    float exit  = xe;
    if(ys > entry) entry = ys;
    if(zs > entry) entry = zs;

    if(ye < exit)  exit = ye;
    if(ze < exit)  exit = ze;

    if(entry >= exit)
    {
        n.x = n.y = n.z = 0.0f;
        //      printf("what\n");
        return 1.0f;
    }else if(xs < 0.0f && ys < 0.0f && zs < 0.0f)
    {
        n.x = n.y = n.z = 0.0f;
        //      printf("inside\n");
        return 1.0f;
    }else if(xs > 1.0f && ys > 1.0f && zs > 1.0f)
    {
        n.x = n.y = n.z = 0;
        //      printf("longer\n");
        return 1.0f;
    }
    else
    {
        // printf("entries: %f, %f, %f\n", b.x + b.width - a.x,b.y + b.height - a.y, b.z + b.depth - a.z);
        int axis = 0;
        float maxL = xs;
        if(ys > maxL)
        {
            maxL = ys;
            axis = 1;
        }
        if(zs > maxL)
        {
            maxL = zs;
            axis = 2;
        }

        if(axis == 0)
        {
            if(xEntry < 0.0f)
            {
                n.x = 1;
            }else
            {
                n.x = -1;
            }
        }else if(axis == 1)
        {
            if(yEntry < 0.0f)
            {
                n.y = 1;
            }else
            {
                n.y = -1;
            }
        }else if(axis == 2)
        {
            if(zEntry < 0.0f)
            {
                n.z = 1;
            }else
            {
                n.z = -1;
            }
        }
        //printf("coollllllll %f, %f, %f, v: %f, %f, %f. n:%f, %f, %f\n", b.x - a.x, b.y - a.y, b.z - a.z, v.x, v.y, v.z, n.x, n.y, n.z);
        if(maxL > 1.0f)
        {
            n.x = n.y = n.z = 0;
            return 1.0f;
        }
    }
//Renderer::sDebugRenderer.drawBox(b.x + b.width*0.5f, b.y + b.height*0.5f, b.z + b.depth*0.5f, b.width  + 0.001f, b.height + 0.001f, b.depth + 0.001f, DebugRenderer::GREEN);
    return entry;
}
