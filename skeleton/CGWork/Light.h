#pragma once

typedef enum 
{
    LIGHT_ID_AMBIENT=-1,
    LIGHT_ID_1=0,
    LIGHT_ID_2,
    LIGHT_ID_3,
    LIGHT_ID_4,
    LIGHT_ID_5,
    LIGHT_ID_6,
    LIGHT_ID_7,
    LIGHT_ID_8,
    MAX_LIGHT
} LightID;

typedef  enum 
{
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_SPOT,
    LIGHT_TYPE_SPOTLIGHT
} LightType;

typedef  enum 
{
    LIGHT_SPACE_VIEW,
    LIGHT_SPACE_LOCAL
} LightSpace;


class LightParams
{
public:

    //light enabled
    bool enabled;
    //type directional,point,spot
    LightType type;
    //local or view space
    LightSpace space;

    //color 0-255 RGB
    int colorR;
    int colorG;
    int colorB;

    //position
    double posX;
    double posY;
    double posZ;

    //direction
    double dirX;
    double dirY;
    double dirZ;

    double spotlightCone;
    double spotlightFactor;
    
    LightParams() :
        enabled(false), type(LIGHT_TYPE_DIRECTIONAL), space(LIGHT_SPACE_VIEW),
        colorR(110), colorG(110), colorB(110), posX(500), posY(1000), posZ(0),
        dirX(0), dirY(0), dirZ(-1), spotlightCone(60),spotlightFactor(1)
    {}

    
protected:
private:
};
