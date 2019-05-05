#version 330 core
out vec4 FragColor;
  
in vec3 Normal;
in vec3 FragPos;
in vec3 LightingColor;

uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 objectColor;
uniform bool isPhong;
uniform int specN;

void main()
{
	if(isPhong){
		vec3 ambient = ambientStrength * lightColor;

		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor * diffuseStrength;

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), specN);
		vec3 specular = specularStrength * spec * lightColor;  

		vec3 result = (ambient + diffuse + specular) * objectColor;
		FragColor = vec4(result, 1.0);
	}else{
		FragColor = vec4(LightingColor * objectColor, 1.0);
	}
}