#version 120
//
// PhongShadingShadowsFog.frag = compute fragment lighting with shadows
//
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;
//uniform sampler2DShadow ShadowMapTexture;
uniform sampler2D ShadowMapTexture;
uniform int  ShadowEnable;
uniform float FogDensity;
uniform vec4  FogColor;
uniform float FogStart;
uniform float FogEnd; // ignored for now

// these are all in eye coords
varying vec3 Light, View, Normal, EyeCoords;
varying vec4 ShadowMapCoord;

void main() 
{ 
  vec4 color;
  vec3 L = normalize(Light);
  vec3 V = normalize(View);
  vec3 N = normalize(Normal);
  vec3 H = normalize ( L + V );

  // Compute terms in the illumination equation
  vec4 ambient = AmbientProduct;
  
  float Kd = max( dot(L, N), 0.0 );
  vec4  diffuse = Kd * DiffuseProduct;
  
  float visibility = 1.0;
  vec4 smCoord = ShadowMapCoord / ShadowMapCoord.w;
  if ( ShadowEnable==1 && texture2D( ShadowMapTexture, smCoord.xy ).z < smCoord.z){
    visibility = 0.0;
  }
  

  float Ks =  pow( max(dot(N, H), 0.0), Shininess );

  vec4  specular = Ks * SpecularProduct;
  
  vec4 surfaceColor = clamp ( ambient + visibility* (diffuse + specular), 0.0, 1.0);

  float z = length ( EyeCoords );
  z = z * z;
  float fogDist = max ( 0.0, z - FogStart );
  float fogFactor = exp ( -FogDensity * fogDist );
  gl_FragColor = fogFactor * surfaceColor + (1-fogFactor) * FogColor;
} 
