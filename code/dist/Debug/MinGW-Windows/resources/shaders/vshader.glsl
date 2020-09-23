#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexture;

uniform mat4  uN;
uniform mat4  uM;
uniform mat4  uPVM;
uniform bool  uTerrainShading;
uniform vec3  uGrassColor;
uniform vec3  uRockColor;
uniform vec3  uSnowColor;
uniform float uSteepness;

out vec3  vnor;
out vec3  vpos;
out float vtslope;
out float vtheight;
out vec3  vtcolor; 
out vec2  vtex;


void main() {

    vec4 inPositionH = vec4(inPosition,1);
    vec4 inNormalH   = vec4(inNormal  ,0);

    vnor  = (uN*inNormalH).xyz;
    vpos  = (uM*inPositionH).xyz;
    vtex  = inTexture;

    gl_Position = uPVM * inPositionH;

    if(uTerrainShading){
        
        vtheight = inPosition.y;
        vtslope  = normalize(inNormal).y;
        
        if(vtheight < 0.5){  
          
            if(vtslope > uSteepness)
                vtcolor = uGrassColor;
            else
                vtcolor = uRockColor;
 
        }else{   
         
            if(vtslope > uSteepness) 
               vtcolor = uSnowColor;
            else
               vtcolor = uRockColor; 
 
        }
    }
 
}


