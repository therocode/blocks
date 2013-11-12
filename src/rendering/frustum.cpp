#include "frustum.h"

#define ANG2RAD 3.14159265358979323846/180.0

AABox::AABox(const glm::vec3& corner,  float x, float y, float z)
{
    setBox(corner,x,y,z);
}



AABox::AABox()
{
    corner.x = 0; corner.y = 0; corner.z = 0;

    x = 1.0f;
    y = 1.0f;
    z = 1.0f;
}   

void AABox::setBox(const glm::vec3& corner,  float x, float y, float z) {


    this->corner = corner;

    if (x < 0.0) {
        x = -x;
        this->corner.x -= x;
    }
    if (y < 0.0) {
        y = -y;
        this->corner.y -= y;
    }
    if (z < 0.0) {
        z = -z;
        this->corner.z -= z;
    }
    this->x = x;
    this->y = y;
    this->z = z;


}



glm::vec3 AABox::getVertexP(const glm::vec3& normal) {

    glm::vec3 res = corner;

    if (normal.x > 0)
        res.x += x;

    if (normal.y > 0)
        res.y += y;

    if (normal.z > 0)
        res.z += z;

    return(res);
}

glm::vec3 AABox::getVertexN(const glm::vec3& normal) {

    glm::vec3 res = corner;

    if (normal.x < 0)
        res.x += x;

    if (normal.y < 0)
        res.y += y;

    if (normal.z < 0)
        res.z += z;

    return(res);
}

//Plane::Plane( const glm::vec3&v1,  const glm::vec3&v2,  const glm::vec3&v3) 
//{
//    set3Points(v1,v2,v3);
//}

void Plane::set3Points( const glm::vec3&v1,  const glm::vec3&v2,  const glm::vec3&v3)
{
    glm::vec3 aux1, aux2;

    aux1 = v1 - v2;
    aux2 = v3 - v2;

    normal = aux2 * aux1;

    normal = glm::normalize(normal); 
    point = v2;
    d = -(glm::dot(normal, point));
}
    
void Plane::setNormalAndPoint(const glm::vec3&normal, const glm::vec3&point)
{
    this->normal = normal;
    this->normal = glm::normalize(this->normal);
    d = -(glm::dot(this->normal, point));
}

void Plane::setCoefficients(float a, float b, float c, float d)
{
    // set the normal vector
    normal = glm::vec3(a,b,c);
    //compute the lenght of the vector
    float l = glm::length(normal.length());
    // normalize the vector
    normal = glm::normalize(normal);
    // and divide d by th length as well
    this->d = d/l;
}




float Plane::distance(const glm::vec3&p)
{
    return (d + glm::dot(normal, p));
}



void Frustum::setCamInternals(float angle, float ratio, float nearD, float farD)
{

    // store the information
    mRatio = ratio;
    mAngle = angle;
    mNearD = nearD;
    mFarD = farD;

    // compute width and height of the near and far plane sections
    mTang = (float)tan(ANG2RAD * angle * 0.5) ;
    mNh = nearD * mTang;
    mNw = mNh * ratio;
    mFh = mFarD  * mTang;
    mFw = mFh * ratio;
}

void Frustum::setCamDef(const glm::vec3& p, const glm::vec3& l, const glm::vec3& u)
{

    glm::vec3 dir,nc,fc,X,Y,Z;

    // compute the Z axis of camera
    // this axis points in the opposite direction from
    // the looking direction
    Z = p - l;
    Z = glm::normalize(Z);

    // X axis of camera with given "up" vector and Z axis
    X = u * Z;
    X = glm::normalize(X);

    // the real "up" vector is the cross product of Z and X
    Y = Z * X;

    // compute the centers of the near and far planes
    nc = p - Z * mNearD;
    fc = p - Z * mFarD;

    // compute the 4 corners of the frustum on the near plane
    mNtl = nc + Y * mNh - X * mNw;
    mNtr = nc + Y * mNh + X * mNw;
    mNbl = nc - Y * mNh - X * mNw;
    mNbr = nc - Y * mNh + X * mNw;

    // compute the 4 corners of the frustum on the far plane
    mFtl = fc + Y * mFh - X * mFw;
    mFtr = fc + Y * mFh + X * mFw;
    mFbl = fc - Y * mFh - X * mFw;
    mFbr = fc - Y * mFh + X * mFw;

    // compute the six planes
    // the function set3Points assumes that the points
    // are given in counter clockwise order
    mPl[TOP].set3Points(mNtr,mNtl,mFtl);
    mPl[BOTTOM].set3Points(mNbl,mNbr,mFbr);
    mPl[LEFT].set3Points(mNtl,mNbl,mFbl);
    mPl[RIGHT].set3Points(mNbr,mNtr,mFbr);
    mPl[NEARP].set3Points(mNtl,mNtr,mNbr);
    mPl[FARP].set3Points(mFtr,mFtl,mFbl);
}

int Frustum::pointInFrustum(const glm::vec3& p)
{
    int result = INSIDE;
    for(int i=0; i < 6; i++)
    {
        if (mPl[i].distance(p) < 0)
            return OUTSIDE;
    }
    return result;
}


int Frustum::sphereInFrustum(const glm::vec3& p, float raio)
{
    int result = INSIDE;
    float distance;

    for(int i=0; i < 6; i++)
    {
        distance = mPl[i].distance(p);
        if (distance < -raio)
            return OUTSIDE;
        else if (distance < raio)
            result =  INTERSECT;
    }

    return(result);
}


int Frustum::boxInFrustum(AABox &b)
{
    int result = INSIDE;
    for(int i=0; i < 6; i++)
    {
        if(mPl[i].distance(b.getVertexP(mPl[i].normal)) < 0)
            return OUTSIDE;
        else if(mPl[i].distance(b.getVertexN(mPl[i].normal)) < 0)
            result = INTERSECT;
    }
    return(result);
}

