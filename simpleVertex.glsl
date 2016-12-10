
# version 330 core
in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
out vec3 Position;
out vec3 Normal;
out vec4 Color;
uniform mat3 NormalMatrix;
uniform mat4 ModelView;
uniform mat4 MVP;

void main() {
	gl_Position = MVP * vPosition;
	Position = (ModelView * vPosition).xyz;
	Normal = normalize(NormalMatrix * vNormal);
	Color = vColor;
}