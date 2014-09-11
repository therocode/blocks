#pragma once
#include "blockstd.hpp"

class AABox
{
    public:
        glm::vec3 corner;
        float x,y,z;

        AABox(const glm::vec3& corner, float x, float y, float z);
        AABox();

        void setBox(const glm::vec3& corner, float x, float y, float z);

        // for use in frustum computations
        glm::vec3 getVertexP(const glm::vec3& normal);
        glm::vec3 getVertexN(const glm::vec3& normal);
};

class Plane  
{
    public:
        glm::vec3 normal,point;
        float d;

        //Plane(const glm::vec3& v1,  const glm::vec3& v2,  const glm::vec3& v3);

        void set3Points(const glm::vec3& v1,  const glm::vec3& v2,  const glm::vec3& v3);
        void setNormalAndPoint(const glm::vec3& normal, const glm::vec3& point);
        void setCoefficients(float a, float b, float c, float d);
        float distance(const glm::vec3& p);

        void print();
};



class Frustum
{
    public:
        enum {OUTSIDE, INTERSECT, INSIDE};
        Plane mPl[6];
        glm::vec3 mNtl,mNtr,mNbl,mNbr,mFtl,mFtr,mFbl,mFbr;
        float mNearD, mFarD, mRatio, mAngle,mTang;
        float mNw,mNh,mFw,mFh;

        void setCamInternals(float angle, float ratio, float nearD, float farD);
        void setCamDef(const glm::vec3& p, const glm::vec3& l, const glm::vec3& u);
        int pointInFrustum(const glm::vec3& p);
        int sphereInFrustum(const glm::vec3& p, float raio);
        int boxInFrustum(AABox &b);
    private:

        enum {
            TOP = 0, BOTTOM, LEFT,
            RIGHT, NEARP, FARP
        };

};
