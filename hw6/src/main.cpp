#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imGUI/imgui.h>
#include <imGUI/imgui_impl_glfw_gl3.h>
#include <stb_image.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include "Camera.h"
#include "Shader.h"
using namespace std;

//��Ӧ�������룬����ESCʱ�رմ��ڣ�����ASDW�ƶ������
void processInput(GLFWwindow* window);

//�ȴ�����ESC����ֹ��ʼ������ʱ�������̹ر�
void error_waiting_close(GLFWwindow* window);

//����ƶ��Ļص�����
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//�����ڴ�С�ı�ʱ����������ͼҲ�ᱻ����
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//��������ֵΪ10��
int extence(int number);

//�������Ϊ0~1�ĸ�����
float toFloat(int number);

//��һ���߶Σ�x1��y1������>��x2��y2��
void drawLine(int x1, int x2, int y1, int y2);

//��һ��Բ���뾶Ϊradius
void drawCircle(int radius);

//ʹ�ù�դ���㷨��������Σ�����ֵΪ��������
void fillTrangle(int x1, int y1, int x2, int y2, int x3, int y3);

//���ڴ�С
const unsigned int width = 600;
const unsigned int height = 600;
unsigned int shaderProgram;
unsigned int lightProgram;
//Ҫ���ĵ������
int length = 0;
//Ҫ���ĵ��xֵ
vector<int> arrayX;
//Ҫ���ĵ��yֵ
vector<int> arrayY;
//Camera����
Camera camera;
//�����ƶ�
float lastX = width / 2;
float lastY = height / 2;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
//�ƶ��ٶȺ�������
float speed = 1.0f;
float sensitivity = 0.05f;
glm::vec3 lightPos(-2.4f, 2.0f, 4.0f);
float deltaTime = 0.0f;

int main() {
	//��ʼ��GLFW����
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
	GLFWwindow* window = glfwCreateWindow(width, height, "homework6", NULL, NULL);
	//��ʼ��������
	if (window == NULL) {
		cout << "Failed to creste GLFW window." << endl;
		error_waiting_close(window);
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//��ʼ��glad������
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		error_waiting_close(window);
		return -1;
	}

	Shader cubeShader("cube.vs", "cube.fs");

	glEnable(GL_DEPTH_TEST);
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//����ImGUI����
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//�������ݣ��������ɫ
	float vertices[] = {
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
		
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	};

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	float ambientStrength = 0.1f;
	float specularStrength = 0.5f;
	float diffuseStrength = 1.0f;
	int specN = 32;
	bool isPhong = true;

	//ѭ����Ⱦ
	while (!glfwWindowShouldClose(window))
	{
		//������
		processInput(window);

		//��Ⱦ������ɫ
		int view_width, view_height;
		glfwGetFramebufferSize(window, &view_width, &view_height);
		glViewport(0, 0, view_width, view_height);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		//����ImGui����
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Shading");
		ImGui::SliderFloat("ambient", &ambientStrength, 0.01, 0.5);
		ImGui::SliderFloat("specular", &specularStrength, 0.1, 2);
		ImGui::SliderFloat("diffuse", &diffuseStrength, 0.1, 2);
		ImGui::SliderInt("specN", &specN, 1, 256);
		ImGui::Checkbox("isPhong", &isPhong);
		ImGui::End();

		//��������
		cubeShader.use();
		//glUseProgram(lightProgram);

		cubeShader.setBool("isPhong", isPhong);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.getView();
		glm::mat4 model = glm::mat4(1.0f);
		//��ֵ
		cubeShader.setMat4("model", model);
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("projection", projection);
		cubeShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		cubeShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		cubeShader.setVec3("lightPos", lightPos);
		cubeShader.setVec3("viewPos", camera.getPos());
		cubeShader.setFloat("ambientStrength", ambientStrength);
		cubeShader.setFloat("specularStrength", specularStrength);
		cubeShader.setFloat("diffuseStrength", diffuseStrength);
		cubeShader.setInt("specN", specN);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//��ȾImGui����
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		//˫����
		glfwSwapBuffers(window);
		//��ѯIO�¼�
		glfwPollEvents();
	}

	//ɾ���������Դ
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

/*------------------------------------------BASIC------------------------------*/

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveForward(speed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveBack(speed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveLeft(speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveRight(speed);
}

void error_waiting_close(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	//��Ӧ����ƶ�����һ��ʱ��ʼ����ʼ��
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float offsetX = xpos - lastX;
	float offsetY = lastY - ypos;//ע����Ļ������ԭ�������½�
	lastX = xpos;
	lastY = ypos;
	//���������ȣ�����ʼ��Ϊ0.05
	offsetX *= sensitivity;
	offsetY *= sensitivity;
	//�ۼƵ�ǰ�Ƕ�
	yaw += offsetX;
	pitch += offsetY;
	//���ø����ǵ�����
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	else if (pitch < -89.0f) {
		pitch = -89.0f;
	}
	//���븩���Ǻ�ƫ����
	camera.rotate(pitch, yaw);
}

/*--------------------------------------HOMMEWORK5----------------------------------------*/

//void Homework5(GLFWwindow* window) {
//	//�������ݣ��������ɫ
//	float vertices[] = {
//		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
//		 1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
//		 1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
//		 1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
//		-1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
//		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
//
//		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
//		 1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
//		 1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
//		 1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
//		-1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
//		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
//
//		-1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
//		-1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
//		-1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
//		-1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
//		-1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
//		-1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
//
//		 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
//		 1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
//		 1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
//		 1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
//		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
//		 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
//
//		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
//		 1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
//		 1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
//		 1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
//		-1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
//		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
//
//		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
//		 1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
//		 1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
//		 1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
//		-1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
//		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 1.0f
//	};
//
//	unsigned int VBO;
//	unsigned int VAO;
//	//����VBO, VAO����
//	glGenBuffers(1, &VBO);
//	glGenVertexArrays(1, &VAO);
//	//�󶨶�������
//	glBindVertexArray(VAO);
//	//���嶥������
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//	//���ö������ԣ�λ�ã�ָ��
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//
//	//model��view��projection�ĳ�ʼ��
//	glm::mat4 model_2 = glm::mat4(1.0f);
//	glm::mat4 model_1 = glm::mat4(1.0f);
//	model_1 = glm::translate(model_1, glm::vec3(-3.5f, 0.5f, -1.5f));
//
//	glm::mat4 view_1 = glm::mat4(1.0f);
//	view_1 = glm::translate(view_1, glm::vec3(0.0f, 0.0f, -20.0f));
//	glm::mat4 view_2 = glm::mat4(1.0f);
//
//	glm::mat4 projection = glm::mat4(1.0f);
//	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 50.0f);
//
//	glm::mat4 projection_or = glm::mat4(1.0f);
//	projection_or = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 50.0f);
//
//	//ImGUI����ʾ����
//	bool problem1 = true;
//	bool problem2 = false;
//	bool isOrthon = false;
//	bool bonus = false;
//
//	//����ͶӰ��Ĭ�ϲ���ֵ�Ͳ�����ʼ��
//	float left_initial = -10.0f;
//	float  bottom_initial = -10.0f;
//	float right_initial = 10.0f;
//	float top_initial = 10.0f;
//	float _near_initial = 0.1f;
//	float _far_initial = 50.0f;
//	float left = left_initial;
//	float right = right_initial;
//	float bottom = bottom_initial;
//	float top = top_initial;
//	float _near = _near_initial;
//	float _far = _far_initial;
//
//	//͸��ͶӰ��Ĭ�ϲ���ֵ�Ͳ�����ʼ��
//	float fov_initial = 45.0f;
//	float fratio_initial = (float)width / (float)height;
//	float fov = fov_initial;
//	float fratio = fratio_initial;
//	float _near2 = _near_initial;
//	float _far2 = _far_initial;
//
//	//���ư뾶�ĳ�ʼ��
//	float radius_initial = 10.0f;
//	float radius = radius_initial;
//
//	cout << "If You Want Enter Bonus Mode? [y/n]" << endl;
//	do {
//		char in[2];
//		cin >> in;
//		if (in[0] == 'y') {
//			//bonus��������꣬�������Ӧ
//			glfwSetCursorPosCallback(window, mouse_callback);
//			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//			bonus = true;
//			break;
//		}
//		else if (in[0] == 'n') {
//			break;
//		}
//		else {
//			cout << "Error enter!";
//		}
//	} while (true);
//
//
//
//	//ѭ����Ⱦ
//	while (!glfwWindowShouldClose(window))
//	{
//		//������
//		processInput(window, bonus);
//
//		//��Ⱦ������ɫ
//		int view_width, view_height;
//		glfwGetFramebufferSize(window, &view_width, &view_height);
//		glViewport(0, 0, view_width, view_height);
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//		glClear(GL_DEPTH_BUFFER_BIT);
//
//
//		//��ֵ
//		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
//		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
//		unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
//
//		//����ImGui����
//		ImGui_ImplGlfwGL3_NewFrame();
//		ImGui::Begin("Camera Setting");
//
//		//basic��ҵ
//		if (!bonus) {
//			//pro1��ͶӰ���⣻pro2����������
//			ImGui::Checkbox("Problem1", &problem1);
//			ImGui::Checkbox("Problem2", &problem2);
//
//			if (problem1) {
//				//�������������ʼ��
//				radius = radius_initial;
//
//				//ͶӰ
//				projection_or = glm::ortho(left, right, bottom, top, _near, _far);
//				projection = glm::perspective(glm::radians(fov), fratio, _near2, _far2);
//				ImGui::Checkbox("Orthon Mode", &isOrthon);
//				//����ͶӰ
//				if (isOrthon) {
//					//�󶨣�ע��right>left, top > bottom, far > near
//					ImGui::SliderFloat("Left", &left, -10.0f, 10.0f);
//					ImGui::SliderFloat("Right", &right, left, 10.0f);
//					ImGui::SliderFloat("Bottom", &bottom, -10.0f, 10.0f);
//					ImGui::SliderFloat("Top", &top, bottom, 10.0f);
//					ImGui::SliderFloat("Near", &_near, 0.0f, 30.0f);
//					ImGui::SliderFloat("Far", &_far, _near, 50.0f);
//
//					//͸��ͶӰ�����ĳ�ʼ��
//					fov = fov_initial;
//					fratio = fratio_initial;
//					_near2 = _near_initial;
//					_far2 = _far_initial;
//				}
//				//͸��ͶӰ
//				else {
//					//�󶨣�far > near 
//					ImGui::SliderFloat("FOV", &fov, 0.0f, 90.0f);
//					ImGui::SliderFloat("Fradio", &fratio, 0.5f, 1.5f);
//					ImGui::SliderFloat("Near", &_near2, 0.0f, 30.0f);
//					ImGui::SliderFloat("Far", &_far2, _near2, 50.0f);
//
//					//����ͶӰ�����ĳ�ʼ��
//					left = left_initial;
//					right = right_initial;
//					bottom = bottom_initial;
//					top = top_initial;
//					_near = _near_initial;
//					_far = _far_initial;
//				}
//			}
//
//			if (problem2) {
//				if (!problem1) {
//					//ͶӰ���������ʼ��
//					fov = fov_initial;
//					_near2 = _near_initial;
//					_far2 = _far_initial;
//					left = left_initial;
//					right = right_initial;
//					bottom = bottom_initial;
//					top = top_initial;
//					_near = _near_initial;
//					_far = _far_initial;
//
//					//ͶӰ
//					projection = glm::perspective(glm::radians(fov_initial), fratio_initial, _near_initial, _far_initial);
//					//�켣Բ
//					float camPosX = sin(clock() / 1000.0)*radius;
//					float camPosZ = cos(clock() / 1000.0)*radius;
//					view_2 = glm::lookAt(glm::vec3(camPosX, 5.0f, camPosZ),
//						glm::vec3(0.0f, 0.0f, 0.0f),
//						glm::vec3(0.0f, 1.0f, 0.0f));
//					ImGui::SliderFloat("Radius", &radius, 0.0f, 40.0f);
//				}
//			}
//
//			//�������ⲻͬ�ظ�ֵ
//			if (problem1) {
//				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model_1[0][0]);
//				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view_1[0][0]);
//			}
//			else {
//				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model_2[0][0]);
//				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view_2[0][0]);
//			}
//			if (problem1 && isOrthon) {
//				glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection_or[0][0]);
//			}
//			else {
//				glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
//			}
//		}
//		//bonus����
//		else {
//			ImGui::SliderFloat("Speed", &speed, -30.0f, 1.0f);
//			ImGui::SliderFloat("Sensitivity", &sensitivity, 0.01f, 0.1f);
//
//			//����Camera���view��Ա
//			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model_2[0][0]);
//			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &camera.getView()[0][0]);
//			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
//		}
//
//		ImGui::End();
//
//		//��������
//		glUseProgram(shaderProgram);
//
//		glBindVertexArray(VAO);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//
//		//��ȾImGui����
//		ImGui::Render();
//		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
//
//		//˫����
//		glfwSwapBuffers(window);
//		//��ѯIO�¼�
//		glfwPollEvents();
//	}
//
//	//ɾ���������Դ
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	ImGui_ImplGlfwGL3_Shutdown();
//	ImGui::DestroyContext();
//	glfwTerminate();
//}
/*--------------------------------------HOMEWOKR4------------------------------------------*/
//void Homework4(GLFWwindow* window) {
//	float vertices[] = {
//		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
//		 1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
//		 1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
//		 1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
//		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
//		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
//
//		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
//		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
//		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
//		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
//		-1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
//		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
//
//		-1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
//		-1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
//		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
//		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
//		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
//		-1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
//
//		 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
//		 1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
//		 1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
//		 1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
//		 1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
//		 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
//
//		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
//		 1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
//		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
//		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
//		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
//		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
//
//		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
//		 1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
//		 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
//		 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
//		-1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
//		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f
//	};
//
//	unsigned int VBO;
//	unsigned int VAO;
//	//����VBO, VAO����
//	glGenBuffers(1, &VBO);
//	glGenVertexArrays(1, &VAO);
//	//�󶨶�������
//	glBindVertexArray(VAO);
//	//���嶥������
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//	//���ö������ԣ�λ�ã�ָ��
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//
//	//����
//	unsigned int texture = 0;
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//	//���Ʒ�ʽ�������ظ�
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
//	//������ˣ���Сʱʹ������ڣ��Ŵ�ʱʹ�����Է�ʽ
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	//����ͼƬ
//	int pic_width, pic_height, nrChannels;
//	stbi_set_flip_vertically_on_load(true);
//	unsigned char* data = stbi_load("pic.jpg", &pic_width, &pic_height, &nrChannels, 0);
//	//��������
//	if (data) {
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pic_width, pic_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else {
//		cout << "Failed to load texture" << endl;
//	}
//	stbi_image_free(data);
//
//	glm::mat4 view = glm::mat4(1.0f);
//	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));
//
//	glm::mat4 projection = glm::mat4(1.0f);
//	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
//
//	bool isTranslateH = false;
//	bool isTranslateV = false;
//	bool isScale = false;
//	bool isTotate = false;
//	bool isDepth = true;
//	float angle = 0.001;
//	float size = 1;
//	float distenceX = 0.01;
//	float distenceY = 0.01;
//	int direction = 1;
//
//	//ѭ����Ⱦ
//	while (!glfwWindowShouldClose(window))
//	{
//		//����ESC��
//		processInput(window);
//
//		//��Ⱦ������ɫ
//		int view_width, view_height;
//		glfwGetFramebufferSize(window, &view_width, &view_height);
//		glViewport(0, 0, view_width, view_height);
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//		glClear(GL_DEPTH_BUFFER_BIT);
//		if (isDepth)
//			glEnable(GL_DEPTH_TEST);
//		else
//			glDisable(GL_DEPTH_TEST);
//
//		//�󶨲���������
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, texture);
//
//		//����ImGui����
//		ImGui_ImplGlfwGL3_NewFrame();
//		ImGui::Begin("Transform");
//		ImGui::Checkbox("Translate Horizontally", &isTranslateH);
//		ImGui::Checkbox("Translate Vertically", &isTranslateV);
//		ImGui::Checkbox("Totate", &isTotate);
//		ImGui::Checkbox("Scale", &isScale);
//		ImGui::Checkbox("Enable_Depth", &isDepth);
//		ImGui::End();
//
//		glm::mat4 model = glm::mat4(1.0f);
//		if (isTranslateH) {
//			model = glm::translate(model, glm::vec3(distenceX, 0.0, 0.0));
//			distenceX += 0.005f * direction;
//			if (distenceX >= 5) direction = -1;
//			else if (distenceX <= -5) direction = 1;
//		}
//		else if (isTranslateV) {
//			model = glm::translate(model, glm::vec3(0.0, distenceY, 0.0));
//			distenceY += 0.005f * direction;
//			if (distenceY >= 5) direction = -1;
//			else if (distenceY <= -5) direction = 1;
//		}
//		else if (isTotate) {
//			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 1.0f));
//			angle += 0.001f;
//		}
//		else if (isScale) {
//			model = glm::scale(model, glm::vec3(1.0, 1.0, 1.0)*size);
//			size += 0.001f * direction;
//			if (size >= 1.5) direction = -1;
//			else if (size <= 0.5) direction = 1;
//		}
//
//		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
//		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
//		unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
//
//		//cout << modelLoc << " " << viewLoc << " " << projectionLoc << endl;
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
//		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
//		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
//
//
//		//��������
//		glUseProgram(shaderProgram);
//
//		glBindVertexArray(VAO);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//
//		//��ȾImGui����
//		ImGui::Render();
//		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
//
//		//˫����
//		glfwSwapBuffers(window);
//		//��ѯIO�¼�
//		glfwPollEvents();
//	}
//}

/*--------------------------------------HOMEWORK3------------------------------------------*/

//void Homework3(GLFWwindow* window) {
//	//�Ƿ���ʾԲ
//	bool Circle = false;
//	//�Ƿ����������
//	bool isFill = false;
//
//	//���������������
//	int x1, x2, x3, y1, y2, y3, radius;
//	x1 = x2 = x3 = y1 = y2 = y3 = 0;
//	cout << "Please enter the first point's position (-50, 50). format: x y" << endl;
//	cin >> x1 >> y1;
//	cout << "Please enter the second point's position (-50, 50). format: x y" << endl;
//	cin >> x2 >> y2;
//	cout << "Please enter the third point's position (-50, 50). format: x y" << endl;
//	cin >> x3 >> y3;
//	//��������ֵ
//	x1 = extence(x1);
//	y1 = extence(y1);
//	x2 = extence(x2);
//	y2 = extence(y2);
//	x3 = extence(x3);
//	y3 = extence(y3);
//	//Ĭ�ϰ뾶Ϊ10
//	radius = 10;
//
//
//	//ѭ����Ⱦ
//	while (!glfwWindowShouldClose(window))
//	{
//		//����ESC��
//		processInput(window);
//		//��ѯIO�¼�
//		glfwPollEvents();
//
//		//ÿ��ѭ������������ֵ
//		arrayX.clear();
//		arrayY.clear();
//		length = 0;
//		//�ж���ʾԲ����������
//		if (Circle) {
//			drawCircle(extence(radius));
//		}
//		else {
//			//�ж��Ƿ����������
//			if (!isFill) {
//				drawLine(x1, x3, y1, y3);
//				drawLine(x1, x2, y1, y2);
//				drawLine(x2, x3, y2, y3);
//			}
//			else {
//				fillTrangle(x1, y1, x2, y2, x3, y3);
//			}
//		}
//
//		//���õ��ĵ������ֵѰ��vertices����
//		float* vertices = new float[length * 3];
//		for (int i = 0; i < length; i++) {
//			vertices[i * 3] = toFloat(arrayX[i]);
//			vertices[i * 3 + 1] = toFloat(arrayY[i]);
//			vertices[i * 3 + 2] = 0.0f;
//		}
//
//		unsigned int VBO;
//		unsigned int VAO;
//
//		//����VBO, VAO��, EBO��
//		glGenBuffers(1, &VBO);
//		glGenVertexArrays(1, &VAO);
//		//�󶨶�������
//		glBindVertexArray(VAO);
//		//���嶥������
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferData(GL_ARRAY_BUFFER, length * 3 * 4, vertices, GL_STATIC_DRAW);
//		//���ö������ԣ�λ�ã�ָ��
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		//��������
//		glUseProgram(shaderProgram);
//
//		//����ImGui����
//		ImGui_ImplGlfwGL3_NewFrame();
//		ImGui::Begin("Change Color");
//		ImGui::Checkbox("Show Circle", &Circle);
//		ImGui::Checkbox("Fill Trangle", &isFill);
//		ImGui::SliderInt("radius", &radius, 0, 50);
//		ImGui::End();
//
//		//��Ⱦ������ɫ
//		int view_width, view_height;
//		glfwGetFramebufferSize(window, &view_width, &view_height);
//		glViewport(0, 0, view_width, view_height);
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		//��ȾImGui����
//		ImGui::Render();
//		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
//
//		//���������е����е�
//		glBindVertexArray(VAO);
//		glDrawArrays(GL_POINTS, 0, length);
//
//		//˫����
//		glfwSwapBuffers(window);
//
//		glDeleteVertexArrays(1, &VAO);
//		glDeleteBuffers(1, &VBO);
//		delete[] vertices;
//	}
//}



/*---------------------------------------------BASIC(above)------------------------------*/

int extence(int number) {
	return number * 10;
}

float toFloat(int number) {
	return number / 500.0;
}

/*
Bresenham�㷨���߶�
x1��y1���߶��е���ߵĶ˵�
x2��y2���߶��е��ұߵĶ˵�
flagY���Ƿ���x�ᷭת��1ʱ����ת��-1ʱ��ת
inver���Ƿ񽻻�x��y����ֵ
*/
void  Bresenham(int x1, int x2, int y1, int y2, int flagY, bool inver) {
	int x = x1;
	int y = y1;
	int dx = x2 - x1;
	int dy = y2 - y1;
	int temp2 = 2 * dy - 2 * dx;
	int temp1 = 2 * dy;
	int p = 2 * dy - dx;
	while (x != x2) {
		if (p <= 0) {
			x += 1;
			p += temp1;
		}
		else {
			x += 1;
			y += 1;
			p += temp2;
		}
		//�Խ������д�����������ֵvector
		int pushX = x;
		int pushY = y * flagY;
		if (inver) {
			int temp = pushX;
			pushX = pushY;
			pushY = temp;
		}
		arrayX.push_back(pushX);
		arrayY.push_back(pushY);
		length++;
	}
}

void drawLine(int x1, int x2, int y1, int y2) {
	//б�ʲ�����
	if (x1 == x2) {
		if (y1 > y2) {
			swap(y1, y2);
		}
		for (int i = y1; i <= y2; i++) {
			arrayX.push_back(x1);
			arrayY.push_back(i);
			length++;
		}
	}
	else {
		//��֤x1 < x2
		if (x1 > x2) {
			swap(x1, x2);
			swap(y1, y2);
		}
		arrayX.push_back(x1);
		arrayY.push_back(y1);
		arrayX.push_back(x2);
		arrayY.push_back(y2);
		length += 2;
		int dx = x2 - x1;
		int dy = y2 - y1;
		float k = dy * 1.0f / dx;
		if (fabs(k) <= 1) {
			if (dy >= 0) {
				//б��Ϊ[0��1]
				Bresenham(x1, x2, y1, y2, 1, false);
			}
			else {
				//б��Ϊ[-1��0]
				Bresenham(x1, x2, -y1, -y2, -1, false);
			}
		}
		else {
			if (dy >= 0) {
				//б��Ϊ[1�������
				Bresenham(y1, y2, x1, x2, 1, true);
			}
			else {
				//б��Ϊ(�����-1]
				Bresenham(y2, y1, -x2, -x1, -1, true);
			}
			
		}
		
	}
}

void drawCircle(int radius) {
	int x = 0;
	int y = radius;
	int p = 3 - 2*radius;
	//�ĸ����������ཻ�ĵ�
	arrayX.push_back(0); arrayY.push_back(radius);
	arrayX.push_back(0); arrayY.push_back(-radius);
	arrayX.push_back(-radius); arrayY.push_back(0);
	arrayX.push_back(radius); arrayY.push_back(0);
	length += 4;
	while (y >= x) {
		if (p >= 0) {
			x += 1;
			y -= 1;
			p = p + 4 * (x - y) + 10;
		}
		else {
			x += 1;
			p = p + 4 * x + 6;
		}
		//��x��y����Ӧ�İ˸��ԳƵ�
		arrayX.push_back(x); arrayY.push_back(y);
		arrayX.push_back(-x); arrayY.push_back(-y);
		arrayX.push_back(-x); arrayY.push_back(y);
		arrayX.push_back(x); arrayY.push_back(-y);
		arrayX.push_back(y); arrayY.push_back(x);
		arrayX.push_back(-y); arrayY.push_back(-x);
		arrayX.push_back(-y); arrayY.push_back(x);
		arrayX.push_back(y); arrayY.push_back(-x);
		length += 8;
	}
}

void fillTrangle(int x1, int y1, int x2, int y2, int x3, int y3){
	//��֤y1 < y2 < y3
	if (y1 > y2) {
		swap(x1, x2);
		swap(y1, y2);
	}
	if (y1 > y3) {
		swap(x1, x3);
		swap(y1, y3);
	}
	if (y2 > y3) {
		swap(x2, x3);
		swap(y2, y3);
	}
	int maxHeight = y3 - y1;
	int dy_1 = y2 - y1 + 1;
	int dy_2 = y3 - y2 + 1;

	//�����ε��°벿�ֵ�ɨ��
	for (int y = y1; y < y2; y++) {
		float alpha = (float)(y - y1) / dy_1;
		float beta = (float)(y - y1) / maxHeight;
		int x_left = x1 + (x2 - x1) * alpha;
		int x_right = x1 + (x3 - x1) * beta;
		if (x_left > x_right) { 
			swap(x_left, x_right); 
		}
		for (int x = x_left; x <= x_right; x++) {
			arrayX.push_back(x);
			arrayY.push_back(y);
			length++;
		}
	}
	//�������ϰ벿�ֵ�ɨ��
	for (int y = y2; y <= y3; y++) {
		float alpha = (float)(y - y2) / dy_2;
		float beta = (float)(y - y1) / maxHeight;
		int x_left = x2 + (x3 - x2) * alpha;
		int x_right = x1 + (x3 - x1) * beta;
		if (x_left > x_right) {
			swap(x_left, x_right);
		}
		for (int x = x_left; x <= x_right; x++) {
			arrayX.push_back(x);
			arrayY.push_back(y);
			length++;
		}
	}
}