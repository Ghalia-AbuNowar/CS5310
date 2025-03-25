#version 330

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    vec3 spotDirection;   
    float spotAngle;     
};


in vec3 fNormal;
in vec4 fPosition;

const int MAXLIGHTS = 10;

uniform MaterialProperties material;
uniform LightProperties light[MAXLIGHTS];
uniform int numLights;

out vec4 fColor;

void main()
{
    vec3 lightVec,viewVec,reflectVec;
    vec3 normalView;
    vec3 ambient,diffuse,specular;
    float nDotL,rDotV;


    fColor = vec4(0,0,0,1);

    for (int i=0;i<numLights;i++)
    {
        if (light[i].position.w!=0)
            lightVec = normalize(light[i].position.xyz - fPosition.xyz);
        else
            lightVec = normalize(-light[i].position.xyz);

        bool isSpot = light[i].spotAngle > 0.0;

        bool insideCone = true;
        if (isSpot){
            float theta = dot(normalize(-lightVec), normalize(light[i].spotDirection));
            insideCone = theta > cos(radians(light[i].spotAngle)); // SHOULD THIS BE IN RADIANS
        }


        vec3 tNormal = fNormal;
        normalView = normalize(tNormal.xyz);
        nDotL = dot(normalView,lightVec);
        nDotL = max(nDotL, 0.0);

        float shade;
        if (nDotL > 0.8) shade = 1.0;       
        else if (nDotL > 0.5) shade = 0.6;  
        else if (nDotL > 0.2) shade = 0.3; 
        else shade = 0.1;                
        

        ambient = material.ambient * light[i].ambient * shade;
        diffuse = material.diffuse * light[i].diffuse * shade;

        if (isSpot) {
            if (insideCone){
                fColor = fColor + vec4(material.ambient*shade, 1.0);
            } else {
                fColor = fColor + vec4(material.ambient*shade, 1.0);
          }
        } 
        
        if (!isSpot) {
           fColor = fColor + vec4(material.ambient*shade, 1.0);
       }

    }
}
