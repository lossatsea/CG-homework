#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightingColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool isPhong;

uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform int specN;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = aNormal;

	gl_Position = projection * view * model * vec4(aPos, 1.0);

	if(isPhong){
		LightingColor = vec3(1.0f);
	}else{
		vec3 Position = vec3(model * vec4(aPos, 1.0));
		vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

		vec3 ambient = ambientStrength * lightColor;
  	
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - Position);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor * diffuseStrength;
    
		vec3 viewDir = normalize(viewPos - Position);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = specularStrength * spec * lightColor;      

		LightingColor = ambient + diffuse + specular;
	}
	
}