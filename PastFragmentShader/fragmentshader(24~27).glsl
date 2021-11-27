#version 330 core
in vec3 FragPos;
in vec3 Normal;

uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 ObjectColor;
uniform vec3 ViewPosTransform;
uniform mat4 LightTransform;

out vec4 FragColor;

void main()
{
   float AmbientLight = 0.3f;
   vec3 Ambient = AmbientLight * LightColor;

   vec3 MyLightpos = vec3(LightTransform * vec4(LightPos, 1.0f));
   vec3 NormalDir = normalize(Normal);
   vec3 LightDir = normalize(MyLightpos - FragPos);
   float DiffuseLight = max(dot(NormalDir, LightDir), 0.0);
   vec3 Diffuse = DiffuseLight * LightColor;

   int Shine = 128;
   vec3 ViewDir = normalize(ViewPosTransform - FragPos);
   vec3 ReflectDir = reflect(-LightDir, NormalDir);
   float SpecularColor = max(dot(ViewDir, ReflectDir), 0.0);
   SpecularColor = pow(SpecularColor, Shine);
   vec3 Specular = SpecularColor * LightColor;

   vec3 SubResult = Ambient + Diffuse + Specular;
   vec3 Result = vec3(SubResult.x * ObjectColor.x, SubResult.y * ObjectColor.y, SubResult.z * ObjectColor.z);
   FragColor = vec4(Result, 1.0);

}
