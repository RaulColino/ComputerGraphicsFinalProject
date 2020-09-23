#version 330 core

struct Light {
    vec3  position;
    vec3  direction;
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float innerCutOff;
    float outerCutOff;
    float c0;
    float c1;
    float c2;
};

struct Material {
    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    vec4  emissive;
    float shininess;
};

struct Textures {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    sampler2D normal;
    float     shininess;
};

#define NLD 1
#define NLP 4
#define NLF 1

uniform vec3     ucpos;
uniform Light    uLightGlobal;
uniform Light    uLightDirectional[NLD];
uniform Light    uLightPositional[NLP];
uniform Light    uLightFocal[NLF];
uniform Material uMaterial;
uniform Textures uTextures;
uniform Textures uTexturesFlat;
uniform Textures uTexturesSteep;
uniform bool     uNormalMapFlat;
uniform bool     uNormalMapSteep;
uniform float    uSteepness;
uniform vec3     uSnowColor;
uniform bool     uWithMaterials;
uniform bool     uNormalMap;
uniform bool     uWireframeMode;
uniform bool     uTerrainShading;

in  vec3  vnor;
in  vec3  vpos;
in  vec3  vtcolor;
in  float vtslope;
in  float vtheight;
in  vec2  vtex;
out vec4  outColor;


vec3 funDirectional(Light light, Material material, vec3 N, vec3 V);
vec3 funPositional (Light light, Material material, vec3 N, vec3 V);
vec3 funFocal      (Light light, Material material, vec3 N, vec3 V);

void main() {

    if(uWireframeMode){

        outColor = vec4(1.0, 1.0, 1.0, 1.0);

    }else{

        vec3 N = normalize(vnor);
        vec3 V = normalize(ucpos - vpos);

        Material material;
        if(uTerrainShading){   
            
            bool  vtflat     = vtslope > uSteepness;
            bool  vtlow      = vtheight < 0.5;
            float alpha      = 0.6;
            vec4  skyColor   = vec4(0.3804, 0.5331, 0.7101, 1.0);
            vec4  lightColor = vec4(0.80, 0.69, 0.63, 1.0);
            if(vtflat && vtlow){                
                material.ambient   = mix(texture(uTexturesFlat.diffuse ,vtex), skyColor, 0.9)*0.3;
                material.diffuse   = mix(texture(uTexturesFlat.diffuse ,vtex), lightColor, 0.5);
                material.specular  = texture(uTexturesFlat.specular,vtex);
                material.emissive  = texture(uTexturesFlat.emissive,vtex);
                material.shininess = uTexturesFlat.shininess;
                if(uNormalMapFlat) N = normalize(N + normalize(2.0*texture(uTexturesFlat.normal,vtex).rgb - 1.0).xyz);
            }else{
                material.ambient   = mix(texture(uTexturesSteep.diffuse ,vtex), skyColor, 0.9)*0.3;
                material.diffuse   = mix(texture(uTexturesSteep.diffuse ,vtex), lightColor, 0.5);
                material.specular  = texture(uTexturesSteep.specular,vtex);
                material.emissive  = texture(uTexturesSteep.emissive,vtex);
                material.shininess = uTexturesSteep.shininess;
                if(uNormalMapSteep) N = normalize(N + normalize(2.0*texture(uTexturesSteep.normal,vtex).rgb - 1.0).xyz);
            }  
            vec3 baseColor = material.emissive.rgb + uLightGlobal.ambient * material.ambient.rgb;
            vec3 blendedColor = uSnowColor*0.7;
            if(vtcolor != uSnowColor)blendedColor = mix(vtcolor, baseColor, alpha); 
            for(int i=0; i<NLD; i++) blendedColor += funDirectional(uLightDirectional[i],material,N,V); 
             
            outColor = vec4(blendedColor, material.diffuse.a);
            
        }else {

            if(uWithMaterials){
                material.ambient   = uMaterial.ambient;
                material.diffuse   = uMaterial.diffuse;
                material.specular  = uMaterial.specular;
                material.emissive  = uMaterial.emissive;
                material.shininess = uMaterial.shininess;
            }else{
                material.ambient   = texture(uTextures.diffuse ,vtex)*0.5;
                material.diffuse   = texture(uTextures.diffuse ,vtex);
                material.specular  = texture(uTextures.specular,vtex);
                material.emissive  = texture(uTextures.emissive,vtex);
                material.shininess = uTextures.shininess;
                if(uNormalMap) N = normalize(N + normalize(2.0*texture(uTextures.normal,vtex).rgb - 1.0).xyz);
            }
            vec3 color = material.emissive.rgb + uLightGlobal.ambient * material.ambient.rgb;
            for(int i=0; i<NLD; i++) color += funDirectional(uLightDirectional[i],material,N,V);
            for(int i=0; i<NLP; i++) color += funPositional(uLightPositional[i],material,N,V);
            for(int i=0; i<NLF; i++) color += funFocal(uLightFocal[i],material,N,V);  

            outColor = vec4(color, material.diffuse.a);

        }
    }

}

vec3 funDirectional(Light light, Material material, vec3 N, vec3 V) {

    vec3  L = normalize(-light.direction);
    vec3  R = normalize(reflect(-L,N));

    float dotLN = dot(L,N); //L y N tienen que estar normalizados para equivaler a cosθ en la ec. de Phong
    float dotRV = 0.0;
    if(dotLN<0.0) dotLN = 0.0;
    else          dotRV = max(dot(R,V), 0.0);

    vec3  ambient  = light.ambient  * material.ambient.rgb;
    vec3  diffuse  = light.diffuse  * material.diffuse.rgb  * dotLN;
    vec3  specular = light.specular * material.specular.rgb * pow(dotRV,material.shininess);

    vec3 color = ambient + diffuse + specular;

    return color;
}

vec3 funPositional(Light light, Material material, vec3 N, vec3 V) {

    vec3  L = normalize(light.position - vpos);
    vec3  R = normalize(reflect(-L,N));

    float dotLN = dot(L,N);
    float dotRV = 0.0;
    if(dotLN<0.0) dotLN = 0.0;
    else          dotRV = max(dot(R,V), 0.0);
    
    vec3  ambient  = light.ambient  * material.ambient.rgb;
    vec3  diffuse  = light.diffuse  * material.diffuse.rgb  * dotLN;
    vec3  specular = light.specular * material.specular.rgb * pow(dotRV,material.shininess);

    float distance    = length(light.position - vpos);
    float attenuation = 1.0/(light.c0 + light.c1*distance + light.c2*pow(distance,2));

    vec3 color = attenuation*(ambient + diffuse + specular);

    return color;
}

vec3 funFocal(Light light, Material material, vec3 N, vec3 V) {

    vec3  L = normalize(light.position - vpos);
    vec3  R = normalize(reflect(-L,N));

    float dotLN = dot(L,N);
    float dotRV = 0.0;
    if(dotLN<0.0) dotLN = 0.0;
    else          dotRV = max(dot(R,V), 0.0);
    
    vec3  ambient  = light.ambient  * material.ambient.rgb;
    vec3  diffuse  = light.diffuse  * material.diffuse.rgb  * dotLN;
    vec3  specular = light.specular * material.specular.rgb * pow(dotRV,material.shininess);

    float distance    = length(light.position - vpos);
    float attenuation = 1.0/(light.c0 + light.c1*distance + light.c2*pow(distance,2));

    vec3  direction   = normalize(-light.direction); 
    float intensity   = clamp((light.outerCutOff - dot(L,direction))/(light.outerCutOff - light.innerCutOff), 0.0, 1.0);

    vec3 color = attenuation*(ambient + intensity*(diffuse + specular));

    return color;
}