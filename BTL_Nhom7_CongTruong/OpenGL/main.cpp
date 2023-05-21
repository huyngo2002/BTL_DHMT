/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/
#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/
#include <cmath>
// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
typedef vec4 point4;
typedef vec4 color4;
using namespace std;
// Số các đỉnh của các tam giác
const int NumPoints = 36;
point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/
point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/
GLuint program;
mat4 xoayHinh, xoayCuaPhai, xoayCuaTrai, nghiengDai, nghiengRong, dichuyenngang;
GLfloat q = 0, e = 0, q1 = 0, q2 = 0, qb = 0, k = 0.0f;
GLfloat theta[3] = { 0, 0, 0 };
GLfloat dr = 5;
mat4 model;
GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;
mat4 dichuyen;
// Các tham sô cho projection
GLfloat l = -2.0, r = 2.0;
GLfloat bottom = -2.0, top = 2.0;
GLfloat zNear = 2.8, zFar = 16.0;

//
/* Khởi tạo các tham số chiếu sáng - tô bóng*/
color4 light_ambient(0.0, 0.0, 0.0, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1, 1, 1, 1);
point4 light_position(1.0, 1.0, 1.0, 0.0);


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
void doimau(GLfloat m1, GLfloat m2, GLfloat m3) {
	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(m1, m2, m3, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}
void matsan() {
	//mat4 instance = Scale(3.0f, 3.0f, 3.0f) * Scale(1.5f, 0.01f, 1.3f);
	mat4 instance = Scale(4.5f, 0.03f, 3.9f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void matsantruoc(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.4f, 0.01f, 0.3f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void matsanphai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.2f, 0.01f, 0.2f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void hangraodai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(1.5f, 0.15f, 0.01f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void hangraodaitrai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.6f, 0.15f, 0.01f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void hangraodaiphai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.5f, 0.15f, 0.01f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void hangraongan(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.01f, 0.15f, 1.3f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void hangraongantrai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.01f, 0.15f, 0.7f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void hangraonganphai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.01f, 0.15f, 0.4f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void thanhdohangrao(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.006f, 0.12f, 0.006f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * nghiengDai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void truccongtruoc(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.4f, 0.01f, 0.01f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * dichuyenngang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void trucdoccongtruoc(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.005f, 0.14f, 0.01f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * dichuyenngang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void betruccongtruoc(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.4f, 0.005f, 0.03f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * dichuyenngang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void trucbanhxecong(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.009f, 0.007f, 0.06f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * dichuyenngang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
//phần bánh xe cổng
void banhxecong(GLfloat x, GLfloat y, GLfloat z) {
	mat4 vitri = Translate(x * 3, y * 3, z * 3) * RotateZ(qb);
	GLfloat goc = 0;
	for (int i = 0; i < 360; i++) {
		mat4 instance = RotateZ(goc) * Scale(0.08f, 0.08f, 0.03f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * dichuyenngang * vitri * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
		goc += 0.2f;
	}
}
void loibanhxecong(GLfloat x, GLfloat y, GLfloat z) {
	mat4 vitri = Translate(x * 3, y * 3, z * 3) * RotateZ(qb);

	GLfloat goc = 0;
	for (int i = 0; i < 360; i++) {
		mat4 instance = RotateZ(goc) * Scale(0.05f, 0.05f, 0.033f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * dichuyenngang * vitri * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
		goc += 0.2f;
	}
}
void congcongtruong() {
	doimau(1, 0, 0);
	dichuyenngang = Translate(k, 0.0f, 0.0f);
	truccongtruoc(0.45f, 0.03f, -0.69f);
	truccongtruoc(0.45f, 0.17f, -0.69f);
	trucdoccongtruoc(0.25f, 0.1f, -0.69f);
	trucdoccongtruoc(0.27f, 0.1f, -0.69f);
	trucdoccongtruoc(0.29f, 0.1f, -0.69f);
	trucdoccongtruoc(0.31f, 0.1f, -0.69f);
	trucdoccongtruoc(0.33f, 0.1f, -0.69f);
	trucdoccongtruoc(0.35f, 0.1f, -0.69f);
	trucdoccongtruoc(0.37f, 0.1f, -0.69f);
	trucdoccongtruoc(0.39f, 0.1f, -0.69f);
	trucdoccongtruoc(0.41f, 0.1f, -0.69f);
	trucdoccongtruoc(0.43f, 0.1f, -0.69f);
	trucdoccongtruoc(0.45f, 0.1f, -0.69f);
	trucdoccongtruoc(0.47f, 0.1f, -0.69f);
	trucdoccongtruoc(0.49f, 0.1f, -0.69f);
	trucdoccongtruoc(0.51f, 0.1f, -0.69f);
	trucdoccongtruoc(0.53f, 0.1f, -0.69f);
	trucdoccongtruoc(0.55f, 0.1f, -0.69f);
	trucdoccongtruoc(0.57f, 0.1f, -0.69f);
	trucdoccongtruoc(0.59f, 0.1f, -0.69f);
	trucdoccongtruoc(0.61f, 0.1f, -0.69f);
	trucdoccongtruoc(0.63f, 0.1f, -0.69f);
	trucdoccongtruoc(0.65f, 0.1f, -0.69f);
	betruccongtruoc(0.45f, 0.02f, -0.69f);
	betruccongtruoc(0.45f, 0.09f, -0.69f);
	trucbanhxecong(0.45f, 0.02f, -0.69f);
	trucbanhxecong(0.65f, 0.02f, -0.69f);
	trucbanhxecong(0.25f, 0.02f, -0.69f);
	doimau(0.0f, 0.0f, 0.0f);
	banhxecong(0.45f, 0.02f, -0.71f);
	banhxecong(0.65f, 0.02f, -0.71f);
	banhxecong(0.25f, 0.02f, -0.71f);
	banhxecong(0.45f, 0.02f, -0.67f);
	banhxecong(0.65f, 0.02f, -0.67f);
	banhxecong(0.25f, 0.02f, -0.67f);
	doimau(255.0f, 255.0f, 255.0f);
	loibanhxecong(0.45f, 0.02f, -0.71f);
	loibanhxecong(0.65f, 0.02f, -0.71f);
	loibanhxecong(0.25f, 0.02f, -0.71f);
	loibanhxecong(0.45f, 0.02f, -0.67f);
	loibanhxecong(0.65f, 0.02f, -0.67f);
	loibanhxecong(0.25f, 0.02f, -0.67f);






}
void khungcuaY(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.03f, 0.25f, 0.03f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void khungcua() {
	khungcuaY(0.75f, 0.13f, 0.05f);
	khungcuaY(0.75f, 0.13f, 0.25f);

}
void canhcuaphai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.01f, 0.2f, 0.085f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * xoayCuaPhai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhcuatrai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.01f, 0.2f, 0.085f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * xoayCuaTrai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void taynamcuaphai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.02f, 0.04f, 0.005f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * xoayCuaPhai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void taynamcuatrai(GLfloat x, GLfloat y, GLfloat z) {
	mat4 instance = Translate(x * 3, y * 3, z * 3) * Scale(3.0f, 3.0f, 3.0f) * Scale(0.02f, 0.04f, 0.005f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * xoayCuaTrai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void bocuanho() {
	khungcua();
	doimau(1, 0, 0);
	canhcuaphai(0.75f, 0.11f, 0.2f);
	doimau(0, 0, 0);
	taynamcuaphai(0.75f, 0.11f, 0.17f);
	xoayCuaPhai = Translate(0.75f * 3, -0.11f * 3, 0.242f * 3) * RotateY(q1) * Translate(-0.75f * 3, 0.11f * 3, -0.25f * 3);
	doimau(1, 0, 0);
	canhcuatrai(0.75f, 0.11f, 0.1f);
	doimau(0, 0, 0);
	taynamcuatrai(0.75f, 0.11f, 0.13f);
	xoayCuaTrai = Translate(0.75f * 3, -0.11f * 3, 0.058f * 3) * RotateY(q2) * Translate(-0.75f * 3, 0.11f * 3, -0.05f * 3);
}
void congtruong() {
	doimau(1, 0.6, 0);
	matsan();
	matsantruoc(0.05f, 0.0f, -0.8f);
	matsanphai(0.85f, 0.0f, 0.15f);
	congcongtruong();
	doimau(0, 0.9, 0);
	hangraodai(0.0f, 0.07f, 0.65f);
	hangraodaitrai(-0.45f, 0.07f, -0.65f);
	hangraodaiphai(0.5f, 0.07f, -0.65f);
	hangraongan(-0.75f, 0.07f, 0.0f);
	hangraongantrai(0.75f, 0.07f, -0.3f);
	hangraonganphai(0.75f, 0.07f, 0.45f);

	nghiengDai = RotateX(30);
	doimau(0.5, 0.8, 1);
	thanhdohangrao(0.0f, 0.35f, 0.51f);
	thanhdohangrao(0.2f, 0.35f, 0.51f);
	thanhdohangrao(0.4f, 0.35f, 0.51f);
	thanhdohangrao(0.6f, 0.35f, 0.51f);
	thanhdohangrao(-0.2f, 0.35f, 0.51f);
	thanhdohangrao(-0.4f, 0.35f, 0.51f);
	thanhdohangrao(-0.6f, 0.35f, 0.51f);
	nghiengDai = RotateX(-30);
	thanhdohangrao(0.4f, 0.35f, -0.48f);
	thanhdohangrao(0.6f, 0.35f, -0.48f);
	thanhdohangrao(-0.4f, 0.35f, -0.48f);
	thanhdohangrao(-0.6f, 0.35f, -0.48f);
	nghiengDai = RotateZ(30);
	thanhdohangrao(-0.6f, 0.4f, 0.0f);
	thanhdohangrao(-0.6f, 0.4f, 0.2f);
	thanhdohangrao(-0.6f, 0.4f, 0.4f);
	thanhdohangrao(-0.6f, 0.4f, -0.2f);
	thanhdohangrao(-0.6f, 0.4f, -0.4f);
	nghiengDai = RotateZ(-30);
	thanhdohangrao(0.6f, 0.4f, -0.1f);
	thanhdohangrao(0.6f, 0.4f, 0.3f);
	thanhdohangrao(0.6f, 0.4f, 0.5f);
	thanhdohangrao(0.6f, 0.4f, -0.3f);
	thanhdohangrao(0.6f, 0.4f, -0.5f);


	bocuanho();
	doimau(1, 0.6, 0);

}
// TÒA NHÀ
void sannha(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 insatnce = Translate(x * 3, y * 3, z * 3) * Scale(1.5f, 0.06f, 0.6f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * insatnce);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}

void tru(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 insatnce = Translate(x * 3, y * 3, z * 3) * Scale(0.03f, 1.5f, 0.03f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * insatnce);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void toanha()
{
	doimau(1.0, 1.0, 0.0);
	sannha(0, 0, 0.4f);
	sannha(0, 0.1f, 0.4f);
	sannha(0, 0.2f, 0.4f);
	sannha(0, 0.3f, 0.4f);
	sannha(0, 0.4f, 0.4f);
	tru(-0.22f, 0.25f, 0.4f);
	tru(-0.22f, 0.25f, 0.33f);
	tru(-0.22f, 0.25f, 0.47f);
	tru(0.22f, 0.25f, 0.4f);
	tru(0.22f, 0.25f, 0.33f);
	tru(0.22f, 0.25f, 0.47f);
	tru(0.07f, 0.25f, 0.33f);
	tru(-0.07f, 0.25f, 0.33f); //0.15+0.15+0.14+0.06
	tru(0.07f, 0.25f, 0.47f);
	tru(-0.07f, 0.25f, 0.47f);
}

mat4 model_can;

//CẨU THÁP
void trucdung()
{
	mat4 instance = Scale(0.03f, 2.1f, 0.03f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * dichuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void trucngang()
{
	mat4 instance = Translate(-0.9f, 0.75f, 0) * Scale(2.4f, 0.03f, 0.03f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * dichuyen * model_can * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void day()
{
	mat4 instance = Translate(-1.8f, 0.6f, 0) * Scale(0.03f, 0.3f, 0.03f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * dichuyen * model_can * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
GLfloat yd = 1.0f, d = 0, yt = 0, xd = 0, aa = 0;


void cancau()
{
	doimau(0.5, 0, 0);
	trucdung();
	model_can = Translate(0, yt * 3, 0) * RotateY(aa);//chuyển tâm phải để y và z đã về đúng vịtri
	trucngang();
	model_can = model_can * Translate(xd * 3, 0, 0) * Translate(0, 0.75f, 0) * Scale(1, yd, 1) * Translate(0, -0.75f, 0);
	day();
}

// <Ngọ: Máy xúc>
mat4 mat_xuc, instance_mayxuc, instance_mayxuc_tay, instance_gau, dichXuong;

void createElement(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 insatnce = Scale(x / 2.0, y / 2.0, z / 2.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * dichXuong * xoayHinh * mat_xuc * instance_mayxuc * insatnce);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void can2(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 insatnce = Scale(x / 2.0, y / 2.0, z / 2.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * dichXuong * xoayHinh * mat_xuc * instance_mayxuc * instance_mayxuc_tay * insatnce);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void gaumuc(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 insatnce;
	for (int i = -90; i < 0; i += 3) {

		insatnce = Translate(0, -y / 4, 0) * RotateX(i) * Translate(0, y / 4, 0) * Scale(x / 2.0, y / 2.0, z / 2.0);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * dichXuong * xoayHinh * mat_xuc * instance_mayxuc * instance_mayxuc_tay * instance_gau * insatnce);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
	}
}
float xoaycan = 30, xoaycan2 = -20, xoaycan3 = 0;
void mayxuc() {
	doimau(1, 1, 0);
	instance_mayxuc = Translate(0, 0, 0);
	createElement(0.6, 0.1, 0.9);
	doimau(0.0, 0.0, 0.0);
	for (int i = 0; i < 360; i += 10) {
		instance_mayxuc = Translate(0.35 / 2, 0, 0.3 / 2) * RotateX(i);
		createElement(0.1, 0.22, 0.02);
	}
	for (int i = 0; i < 360; i += 10) {
		instance_mayxuc = Translate(-0.35 / 2, 0, 0.3 / 2) * RotateX(i);
		createElement(0.1, 0.22, 0.02);
	}
	for (int i = 0; i < 360; i += 10) {
		instance_mayxuc = Translate(-0.35 / 2, 0, -0.3 / 2) * RotateX(i);
		createElement(0.1, 0.22, 0.02);
	}
	for (int i = 0; i < 360; i += 10) {
		instance_mayxuc = Translate(0.35 / 2, 0, -0.3 / 2) * RotateX(i);
		createElement(0.1, 0.22, 0.02);
	}
	//truc
	doimau(1, 0.0, 0.0);
	for (int i = 0; i < 360; i += 10) {
		instance_mayxuc = Translate(0, 0.075 / 2, 0) * RotateY(i);
		createElement(0.4, 0.15, 0.05);
	}
	//buong may
	doimau(1, 1, 0.0);
	instance_mayxuc = Translate(0, 0.25 / 2, 0);
	createElement(0.6, 0.2, 0.9);
	instance_mayxuc = Translate(0.125 / 2, 0.5 / 2, -0.325 / 2);
	createElement(0.25, 0.4, 0.25);
	doimau(1, 1, 1);
	instance_mayxuc = Translate(0.125 / 2, 0.5 / 2, -0.326 / 2);
	createElement(0.22, 0.35, 0.25);
	//can cau
	doimau(1, 1, 0);
	instance_mayxuc = Translate(0, 0.25 / 2, -0.45 / 2) * RotateX(xoaycan) * Translate(0, 0, -0.4 / 2);
	createElement(0.15, 0.15, 0.8);
	//canh tay thu 2
	doimau(1, 1, 0);
	instance_mayxuc_tay = Translate(0, 0, -0.45 / 2) * RotateX(xoaycan2) * Translate(0, 0, -0.25 / 2);
	can2(0.1, 0.1, 0.6);
	//gauu
	doimau(1, 1, 1);
	instance_gau = Translate(0, 0.05 / 2, -0.3 / 2) * RotateX(xoaycan3) * Translate(0, -0.2 / 2, -0.0);
	gaumuc(0.2, 0.2, 0.1);

}
float dmm = -0.5, dn = 1.5, xoay_may = 0, tinh_tien_x = 0, tinh_tien_y = 0, tinh_tien_z = 0;
void buil_xuc() {
	mat_xuc = Translate(dn + tinh_tien_x, 0.25 + tinh_tien_y, dmm + tinh_tien_z) * RotateY(xoay_may);
	mayxuc();

}
// </Ngọ: Máy xúc>



// Điều : Xe tải 
//xe tai
//GLdouble x_thung = 0.8, y_thung = 0.01, z_thung = 0.3, x_dau = 0.4, y_dau = 0.5,
//z_dau = 0.3, banh_xe = 0.05, y_khung = 0.03, x_khung = 0.1, zz = 0, qq = 0;

mat4 model_tang;
GLfloat theta1[] = { 0, 0, 0, 0 , 0, 0 };

GLdouble x_thung = 0.8 / 2, y_thung = 0.01 / 2, z_thung = 0.3 / 2, x_dau = 0.4 / 2, y_dau = 0.5 / 2,
z_dau = 0.3 / 2, banh_xe = 0.05 / 2, y_khung = 0.03 / 2, x_khung = 0.1 / 2, zz = 0, qq = 0;


void dauxe()
{
	mat4 instance = Translate(0, -0.05, 0) * Translate(x_thung / 2 + x_dau / 2, y_dau / 2 - y_thung / 2, 0.0) * Scale(x_dau, y_dau - 0.1, z_dau);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
mat4 mtbanhxe;

void thanhtru(GLfloat x) {
	mat4 mtTru = RotateY(x) * Translate(0.25, 0, 0) * Scale(0.05, 0.05, 0.05);
	doimau(0, 0, 0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * mtbanhxe * mtTru);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanhtru2(GLfloat x) {
	mat4 mtTru2 = RotateY(x) * Translate(0.1, 0, 0) * Scale(0.1, 0.05, 0.05);
	doimau(1, 1, 1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * mtbanhxe * mtTru2);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void truxe(GLfloat x, GLfloat y, GLfloat z) {

	mtbanhxe = Translate(x, y, z) * RotateX(90) * Scale(0.3 / 2, 0.5 / 2, 0.3 / 2);
	for (int i = 0; i < 360; i += 10)
	{
		thanhtru(i);
		thanhtru2(i);
	}
}
void khung(GLfloat a, GLfloat b, GLfloat c)
{
	mat4 instance = Translate(a, b, c) * Scale(x_thung, y_khung, z_thung);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thung1()
{

	mat4 instance = Scale(x_thung, y_thung, z_thung);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints); //thung day

	instance = Translate(0, y_dau / 2 - y_dau / 5, z_thung / 2) * RotateX(90) * instance;
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints); //thung mat trai

	instance = Translate(0, y_dau - z_thung / 1.5, 0) * RotateX(180) * instance;
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints); // thung mat phai

	instance = Scale(x_thung * 1.5, 1, z_thung * 1.3) * Translate(x_thung / 1.65, 0, 0) * RotateY(90) * instance;
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints); // thung mat truoc


}
void cuaxetrai()
{
	mat4 instance = Translate(0, -0.05, 0) * Translate(x_thung / 1.4, y_dau / 2, z_dau / 2) * Scale(0.1, 0.1, 0.005);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cuaxephai()
{
	mat4 instance = Translate(0, -0.05, 0) * RotateY(zz) * Translate(x_thung / 1.4, y_dau / 2, -z_dau / 2) * Scale(0.1, 0.1, 0.005);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void truc_nang()
{
	mat4 instance = Scale(0.02, qq, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_tang * model * xoayHinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void xed()
{
	doimau(1.0, 1.0, 0.0);
	dauxe();
	doimau(1, 0, 0);
	cuaxephai();
	cuaxetrai();
}
void xeb()
{
	khung(0.1, -(y_thung) / 2, 0);
	truxe(-(x_thung / 3) + banh_xe, -(y_thung + banh_xe) / 2, -z_thung / 2); //banh phai sau
	truxe(-(x_thung / 3) + banh_xe, -(y_thung + banh_xe) / 2, z_thung / 2); //banh trai sau
	truxe(-(x_thung / 3) + x_thung / 2 + banh_xe, -(y_thung + banh_xe) / 2, -z_thung / 2); //banh phai giua
	truxe(-(x_thung / 3) + x_thung / 2 + banh_xe, -(y_thung + banh_xe) / 2, z_thung / 2); //banh trai giua

	truxe(-(x_thung / 3) + x_thung + banh_xe, -(y_thung + banh_xe) / 2, -z_thung / 2); //banh phai truoc
	truxe(-(x_thung / 3) + x_thung + banh_xe, -(y_thung + banh_xe) / 2, z_thung / 2); //banh trai truoc
}
GLfloat i = 0;
// Dương
mat4 quay, controlXe, instance, Model;
mat4 mt, mtbanhxe2;
GLfloat a, b, c = 0;
GLfloat x;
GLfloat len_x = 0;
GLfloat x_x, x_y, x_z = 0;
mat4 viTri = Translate(-0.5, 0.4, 0);
mat4 xoayXeLu;
void changeColorCube(point4 input) {
	for (int i = 0; i < NumPoints; i++) {
		colors[i] = input;
	}
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
}

void vienLop1(GLfloat x) {
	mat4 mtTru = RotateY(x) * Translate(0.25, 0, 0) * Scale(0.3, 0.3, 0.3) * Scale(0.5, 0.5, 0.5);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * mt * quay * viTri * controlXe * xoayXeLu * mtbanhxe2 * mtTru);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void vienLop2(GLfloat x) {
	mat4 mtTru2 = RotateY(x) * Translate(0.1, 0, 0) * Scale(0.6, 0.2, 0.2) * Scale(0.5, 0.5, 0.5);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * mt * quay * viTri * controlXe * xoayXeLu * mtbanhxe2 * mtTru2);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void lopXe(GLfloat x, GLfloat y, GLfloat z, GLfloat m) {
	mtbanhxe2 = RotateY(m) * Translate(x / 2, y / 2, z / 2) * RotateX(90) * Scale(0.3, 0.5, 0.3) * Scale(0.5, 0.5, 0.5);
	for (int i = 0; i < 360; i++) {

		vienLop1(i);


	}
}
void loiBanh(GLfloat x, GLfloat y, GLfloat z, GLfloat m) {
	mtbanhxe2 = RotateY(m) * Translate(x / 2, y / 2, z / 2) * RotateX(90) * Scale(0.3, 0.5, 0.3) * Scale(0.5, 0.5, 0.5);
	for (int i = 0; i < 360; i++) {

		vienLop2(i);


	}
}
void banhlu(GLfloat x, GLfloat y, GLfloat z, GLfloat m) {
	mtbanhxe2 = RotateY(m) * Translate(x / 2, y / 2, z / 2) * RotateX(90) * Scale(0.6, 2.8, 0.5) * Scale(1.0, 1.0, 1.0);
	for (int i = 0; i < 360; i++) {
		vienLop1(i);
		vienLop2(i);
	}
}
void than(GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(x / 2, y / 2, z / 2) * Scale(0.5, 0.3, 0.6) * Scale(0.5, 0.5, 0.5);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * quay * viTri * controlXe * xoayXeLu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void cabin(GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(x / 2, y / 2, z / 2) * Scale(0.5, 0.3, 0.2) * Scale(0.5, 0.5, 0.5);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * quay * viTri * controlXe * xoayXeLu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void thanhDo(GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(x / 2, y / 2, z / 2) * Scale(0.01, 0.1, 0.4) * Scale(0.5, 0.5, 0.5);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * quay * viTri * controlXe * xoayXeLu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void thanhDo2(GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(x / 2, y / 2, z / 2) * Scale(0.2, 0.1, 0.01) * Scale(0.5, 0.5, 0.5);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * quay * viTri * controlXe * xoayXeLu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void cua(GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(x / 2, y / 2, z / 2) * Scale(0.05, 0.2, 0.15) * Scale(0.5, 0.5, 0.5);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * quay * viTri * controlXe * xoayXeLu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void cuaChinh(GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(x / 2, y / 2, z / 2) * Scale(0.4, 0.25, 0.05) * Scale(0.5, 0.5, 0.5);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * xoayHinh * quay * viTri * controlXe * xoayXeLu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Xe() {
	doimau(0.0, 0.7, 1.0);
	cua(0.23, 0.3, 0);
	cua(-0.23, 0.3, 0);
	cuaChinh(0, 0.3, -0.08);
	doimau(1.0, 1.0, 0.0);
	cabin(0, 0.3, 0);
	doimau(1.0, 0.7, 0.0);
	than(0, 0, 0);
	doimau(1.0, 1.0, 0.0);
	thanhDo(0.25, -0.05, -0.4);
	thanhDo(-0.25, -0.05, -0.4);
	thanhDo2(0.35, -0.05, -0.6);
	thanhDo2(-0.35, -0.05, -0.6);
	thanhDo(0.45, -0.05, -0.8);
	thanhDo(-0.45, -0.05, -0.8);
	doimau(0, 0, 0);
	lopXe(0, -0.15, 0.25, -90);
	lopXe(0, -0.15, -0.25, -90);
	banhlu(-0.85, 0, 0, -90);
	doimau(1.0, 1.0, 0);
	loiBanh(0, -0.15, 0.25, -90);
	loiBanh(0, -0.15, -0.25, -90);
}
float camera[] = { 0.0, 4.5, 3.0 };
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model = RotateX(-40) * RotateY(210);
	xoayHinh = RotateY(q) * RotateX(e);
	dichuyen = Translate(1.05f, 1.05f, 1.2f) * RotateY(-30);
	congtruong();
	toanha();
	controlXe = Translate(0, 0, len_x);
	xoayXeLu = RotateX(x_x) * RotateY(x_y) * RotateZ(x_z);
	Xe();
	cancau();
	dichXuong = Translate(0.0f, -0.15f, 0.0f);
	buil_xuc(); // Máy xúc của Ngọ
	// Xe của Điều
	//xe tai
	model = model * Translate(0, 0.3, -1.2);
	model = model * RotateY(theta1[0]);
	model = model * RotateX(theta1[2]);
	model = model * RotateZ(theta1[3]);
	model = model * Translate(theta1[4], 0, 0);
	xed();
	xeb();
	model_tang = Translate(0, qq, 0);
	truc_nang();
	doimau(1.0, 1.0, 0.0);
	model = model * Translate(-i / 2, 2 * i, 0) * RotateZ(theta1[1]);
	thung1();
	vec4 eye = vec4(camera[0], camera[1], camera[2], 1);
	vec4 at = vec4(0.0, 0.0, 0.0, 1);
	vec4 up = vec4(0, 1, 0, 1);
	mat4 View = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, View);
	mat4 Projection = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, Projection);
	const vec3 viewer_pos(0.0, 4.5, 3.0);
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	vec4 eye(0, 0, 2, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(-1, 1, -1, 1, 1, 4);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glViewport(0, 0, width, height);
}
bool isFW = false, isBK = false;
GLfloat speedFW_Cong = 0.0f;
GLfloat speedBK_Cong = 0.0f;
void update(int x) {
	if (isFW && speedFW_Cong < 4.0f)
		speedFW_Cong += 0.2f;

	if (isFW && speedFW_Cong < 2.0f)
		speedFW_Cong += 0.1f;

	if (isFW && speedFW_Cong < 1.0f)
		speedFW_Cong += 0.05f;

	if (isBK && speedBK_Cong < 4.0f)
		speedBK_Cong += 0.2f;
	if (isBK && speedBK_Cong < 2.0f)
		speedBK_Cong += 0.1f;

	if (isBK && speedBK_Cong < 1.0f)
		speedBK_Cong += 0.05f;

	if (!isFW)
		speedFW_Cong = 0.0f;

	if (!isBK)
		speedBK_Cong = 0.0f;
	glutTimerFunc(300, update, 0);

}
void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case '1':
		camera[0] += 0.1;
		glutPostRedisplay();
		break;
	case '2':
		camera[0] -= 0.1;
		glutPostRedisplay();
		break;
	case '3':
		camera[1] += 0.1;
		glutPostRedisplay();
		break;
	case '4':
		camera[1] -= 0.1;
		glutPostRedisplay();
		break;
	case '5':
		camera[2] += 0.1;
		glutPostRedisplay();
		break;
	case '6':
		camera[2] -= 0.1;
		glutPostRedisplay();
		break;
	case 'p': // Ngo
		xoay_may -= 10;
		glutPostRedisplay();
		break;
	case 'P': // Ngo
		xoay_may += 10;
		glutPostRedisplay();
		break;
	case 'a': // Ngo
		if ((int)abs(xoay_may) % 360 == 0)
		{
			//tinh_tien_x = 0;
			tinh_tien_z += 0.03;
		}
		else if ((int)abs(xoay_may) % 360 == 180)
		{
			//tinh_tien_x = 0;
			tinh_tien_z -= 0.03;
		}
		else if ((int)abs(xoay_may) % 360 == 90)
		{
			tinh_tien_x -= 0.03;
			//tinh_tien_z = 0;
		}
		else if ((int)abs(xoay_may) % 360 == 270)
		{
			tinh_tien_x += 0.03;
			//tinh_tien_z = 0;
		}
		else {
			if (xoay_may > 0)
			{
				tinh_tien_x += 0.03;
				tinh_tien_z += 0.03 * tan((xoay_may) * 3.14 / 180);
			}
			else {
				tinh_tien_x -= 0.03;
				tinh_tien_z -= 0.03 * tan((xoay_may) * 3.14 / 180);
			}
		}
		glutPostRedisplay();
		break;
	case 'A': // Ngo
		if ((int)abs(xoay_may) % 360 == 0)
		{
			//tinh_tien_x = 0;
			tinh_tien_z -= 0.03;
		}
		else if ((int)abs(xoay_may) % 360 == 180)
		{
			//tinh_tien_x = 0;
			tinh_tien_z += 0.03;
		}
		else if ((int)abs(xoay_may) % 360 == 90)
		{
			tinh_tien_x += 0.03;
			//tinh_tien_z = 0;
		}
		else if ((int)abs(xoay_may) % 360 == 270)
		{
			tinh_tien_x -= 0.03;
			//tinh_tien_z = 0;
		}
		else {
			if (xoay_may > 0)
			{
				tinh_tien_x -= 0.03;
				tinh_tien_z -= 0.03 * tan((xoay_may) * 3.14 / 180);
			}
			else {
				tinh_tien_x += 0.03;
				tinh_tien_z += 0.03 * tan((xoay_may) * 3.14 / 180);
			}
		}
		glutPostRedisplay();
		break;
	case 's': // Ngo
		if (xoaycan3 < 20)
			xoaycan3 += 5;
		glutPostRedisplay();
		break;
	case 'S': // Ngo
		if (xoaycan3 > -40)
			xoaycan3 -= 5;
		glutPostRedisplay();
		break;
	case 'd': // Ngo
		if (xoaycan2 < 0)
			xoaycan2 += 5;
		glutPostRedisplay();
		break;
	case 'D': // Ngo
		if (xoaycan2 > -75)
			xoaycan2 -= 5;
		glutPostRedisplay();
		break;
	case 'f': // Ngo
		if (xoaycan < 55) {
			xoaycan += 5;
		}
		glutPostRedisplay();
		break;
	case 'F': // Ngo
		if (xoaycan >= 20) {
			xoaycan -= 5;
		}
		glutPostRedisplay();
		break;
	case 'q': // QA
		q += 10;
		glutPostRedisplay();
		break;
	case 'Q': // QA
		q -= 10;
		glutPostRedisplay();
		break;
	case 'W': // QA
		e += 10;
		glutPostRedisplay();
		break;
	case 'w': // QA
		e -= 10;
		glutPostRedisplay();
		break;
	case 'e': // QA
		q1 += 10;
		if (q1 > 90) {
			q1 = 90;
		}
		glutPostRedisplay();
		break;
	case 'E': // QA
		q1 -= 10;
		if (q1 < -90) {
			q1 = -90;
		}
		glutPostRedisplay();
		break;
	case 'r': // QA
		q2 -= 10;
		if (q2 < -90) {
			q2 = -90;
		}
		glutPostRedisplay();
		break;
	case 'R': // QA
		q2 += 10;
		if (q2 > 90) {
			q2 = 90;
		}
		glutPostRedisplay();
		break;
	case 't': // QA
		isFW = true;
		k -= 0.06f * speedFW_Cong;
		qb -= 5;
		if (k < -0.4f * 3) {
			k = -0.4f * 3;
		}
		glutPostRedisplay();
		break;
	case 'T': // QA
		isBK = true;
		k += 0.06f * speedBK_Cong;
		qb += 5;
		if (k > 0.0f) {
			k = 0.0f;
		}
		glutPostRedisplay();
		break;

	case 'y': // Thao
		aa += 5;
		if (aa > 90)
			aa = 90;
		glutPostRedisplay();
		break;
	case 'Y': // Thao
		aa -= 5;
		if (aa < -90)
			aa = -90;
		glutPostRedisplay();
		break;
	case 'u': // Thao
		yt -= 0.05;
		if (yt < -0.4f)
			yt = -0.4f;
		glutPostRedisplay();
		break;
	case 'U': // Thao
		yt += 0.05;
		if (yt > 0)
			yt = 0;
		glutPostRedisplay();
		break;
	case 'i': // Thao
		yd += 0.1f;
		if (yd > 5.5)
			yd = 5.5;
		glutPostRedisplay();
		break;
	case 'I': // Thao
		yd -= 0.1f;
		if (yd < 0.5f)
			yd = 0.5f;
		glutPostRedisplay();
		break;
	case 'o': // Thao
		xd += 0.05f;
		if (xd > 0.5f)
			xd = 0.5f;
		glutPostRedisplay();
		break;
	case 'O': // Thao
		xd -= 0.05f;
		if (xd < 0)
			xd = 0;
		glutPostRedisplay();
		break;
		// Điều 
	case 'G':
		theta1[0] += 5;
		if (theta1[0] > 360) theta1[0] -= 360;
		glutPostRedisplay();
		break;

	case 'g':
		// một số lệnh 
		theta1[0] -= 5;
		if (theta1[0] > 360) theta1[0] -= 360;
		glutPostRedisplay();
		break;
		//xoay xe theo ox
	case 'H':
		theta1[2] += 5;
		if (theta1[0] > 360) theta1[0] -= 360;
		glutPostRedisplay();
		break;
	case 'h':
		// một số lệnh 
		theta1[2] -= 5;
		if (theta1[0] > 360) theta1[0] -= 360;
		glutPostRedisplay();
		break;
		//xoay xe theo oz
	case 'J':
		theta1[3] += 5;
		if (theta1[0] > 360) theta1[0] -= 360;
		glutPostRedisplay();
		break;
	case 'j':
		// một số lệnh 
		theta1[3] -= 5;
		if (theta1[0] > 360) theta1[0] -= 360;
		glutPostRedisplay();
		break;
		//tiến xe
	case 'K':
		theta1[4] += 0.05;
		theta1[0] += 5;
		glutPostRedisplay();
		break;
		//lùi xe
	case 'k':
		// một số lệnh 
		theta1[4] -= 0.05;

		glutPostRedisplay();
		break;

		//nâng trục
	case 'L':
		// một số lệnh 
		if (theta1[1] < 30)
		{
			theta1[1] += 5;
			i = i + 0.01;
			qq = qq + 0.022;

		}
		else {}
		glutPostRedisplay();
		break;
		//hạ trục
	case 'l':
		// một số lệnh 

		if (theta1[1] > 0)
		{
			theta1[1] -= 5;
			i = i - 0.01;
			qq = qq - 0.022;
		}
		glutPostRedisplay();
		break;
		//tiến sang trái
	case 'Z':
		theta1[4] += 0.1;
		theta1[0] += 5;
		if (theta1[0] > 360) theta1[0] -= 360;
		glutPostRedisplay();
		break;
		//tiến sang phải
	case 'z':
		theta1[4] += 0.1;
		theta1[0] -= 5;
		if (theta1[0] <= -360) theta1[0] -= 360;
		glutPostRedisplay();
		break;
	case 'x':
		len_x -= 0.1;
		glutPostRedisplay();
		break;
	case 'X':
		len_x += 0.1;
		glutPostRedisplay();
		break;
	case 'c':
		x_x += 10;
		glutPostRedisplay();
		break;
	case 'C':
		x_x -= 10;
		glutPostRedisplay();
		break;
	case 'v':
		x_y += 10;
		glutPostRedisplay();
		break;
	case 'V':
		x_y -= 10;
		glutPostRedisplay();
		break;
	case 'b':
		x_z += 10;
		glutPostRedisplay();
		break;
	case 'B':
		x_z += 10;
		glutPostRedisplay();
		break;
	}
}
void KeyUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 't':
		isFW = false;
		break;
	case 'T':
		isBK = false;
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("A Cube is rotated by keyboard and shaded");



	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(KeyUp);  //
	glutTimerFunc(200, update, 0);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}