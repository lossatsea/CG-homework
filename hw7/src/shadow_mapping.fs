#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
	vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int specN;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 norm)
{
	//转换到[-1, 1]
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//转换到[0,1],来和深度比较
	projCoords = projCoords * 0.5 + 0.5;
	//得到最近深度
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	//片元的当前深度
	float currentDepth = projCoords.z;
	//贴图偏移
	float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	//片元是否在阴影中
	//float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
	if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;
}

void main()
{
    //FragColor = vec4(1.0); // set alle 4 vector values to 1.0
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	vec3 norm = normalize(fs_in.Normal);

	vec3 ambient = ambientStrength * lightColor;
	
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor * diffuseStrength;

	vec3 viewDir = normalize(viewPos - fs_in.FragPos); 
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(norm, halfwayDir), 0.0), specN);
	vec3 specular = specularStrength * spec * lightColor;  

	//阴影
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace, lightDir, norm);
	vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	
	FragColor = vec4(result, 1.0);
}