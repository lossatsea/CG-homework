#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "Shader.h"
using namespace std;

//响应键盘输入，输入ESC时关闭窗口，输入ASDW移动摄像机
void processInput(GLFWwindow* window);

//等待输入ESC，防止初始化错误时程序立刻关闭
void error_waiting_close(GLFWwindow* window);

//鼠标点击的回调函数
void mouse_callback(GLFWwindow* window, int button, int action, int mods);

//鼠标移动的回调函数
void pos_callback(GLFWwindow* window, double xpos, double ypos);

//当窗口大小改变时，窗口内视图也会被调整
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void getBezier();

void drawBezier();

void drawLine();

void drawDynamicLine();

glm::vec2 getInterPoint(glm::vec2 p0, glm::vec2 p1, float t);

//窗口大小
const unsigned int width = 600;
const unsigned int height = 600;
unsigned int shaderProgram;
int clickPointNum;
int drawPointNum;
int endPointNum;
vector<glm::vec2> clickPoints;
vector<glm::vec2> endPoints;
vector<glm::vec2> drawPoints;
glm::vec2 mousePos;
Shader shader;
const int clock_num = 10;
float dynamic_t = 0;
int clock = 0;

int main() {
	//初始化GLFW窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
	GLFWwindow* window = glfwCreateWindow(width, height, "homework8", NULL, NULL);
	//初始化错误处理
	if (window == NULL) {
		cout << "Failed to creste GLFW window." << endl;
		error_waiting_close(window);
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//初始化glad错误处理
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		error_waiting_close(window);
		return -1;
	}

	shader = Shader("sprite.vs", "sprite.fs");

	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, pos_callback);
	//循环渲染
	while (!glfwWindowShouldClose(window))
	{
		//监听键
		processInput(window);
		//查询IO事件
		glfwPollEvents();

		//渲染背景颜色
		int view_width, view_height;
		glfwGetFramebufferSize(window, &view_width, &view_height);
		glViewport(0, 0, view_width, view_height);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		drawBezier();
		drawLine();
		drawDynamicLine();
		
		//cout << drawPointNum << endl;
		
		//双缓冲
		glfwSwapBuffers(window);
		//查询IO事件
		glfwPollEvents();
	}

	//删除申请的资源
	glfwTerminate();
	return 0;
}

/*------------------------------------------BASIC------------------------------*/

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		clickPointNum++;
		clickPoints.push_back(mousePos);
		getBezier();
		dynamic_t = 0;
		clock = 0;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (clickPointNum > 0) {
			clickPointNum--;
			clickPoints.pop_back();
			getBezier();
			dynamic_t = 0;
			clock = 0;
		}
		break;
	default:
		return;
	}
	return;
}

void pos_callback(GLFWwindow* window, double xpos, double ypos) {
	mousePos.x = ((2*xpos) / width) - 1;
	mousePos.y = 1 - ((2*ypos) / height);
}

glm::vec2 getInterPoint(glm::vec2 p0, glm::vec2 p1, float t) {
	float x = (1 - t) * p0.x + t * p1.x;
	float y = (1 - t) * p0.y + t * p1.y;
	//cout << "push (" << x << ", " << y << ")" << endl;
	return glm::vec2(x, y);
}

void initEndPoints() {
	endPointNum = clickPointNum;
	endPoints.clear();
	for (int i = 0; i < endPointNum; i++)
		endPoints.push_back(clickPoints[i]);
}

void getBezier() {
	initEndPoints();
	drawPointNum = 0;
	drawPoints.clear();
	if (clickPointNum < 1)
		return;
	else if (clickPointNum == 1) {
		drawPointNum = 1;
		drawPoints.push_back(clickPoints[0]);
		return;
	}
	for (float t = 0; t < 1.001f; t += 0.001f) {
		//cout << t << ":" <<endl;
		//会有clickPointNum-1层
		int layerNum = clickPointNum - 1;
		int current_index = 0;
		int point_num_in_layer = clickPointNum;
		for (int i = 0; i < layerNum; i++) {
			//cout << "layer: " << i << endl;
			point_num_in_layer--;
			for (int j = 0; j < point_num_in_layer; j++) {
				//cout << "index: " << current_index << endl;
				glm::vec2 p0 = endPoints[current_index];
				glm::vec2 p1 = endPoints[current_index + 1];
				endPoints.push_back(getInterPoint(p0, p1, t));
				endPointNum++;
				current_index++;
			}
			current_index++;
		}
		drawPoints.push_back(endPoints[endPointNum - 1]);
		drawPointNum++;
		initEndPoints();
	}
}

	void drawBezier() {
		float* vertics = new float[drawPointNum * 3];
		for (int i = 0; i < drawPointNum; i++) {
			vertics[i * 3] = drawPoints[i].x;
			vertics[i * 3 + 1] = drawPoints[i].y;
			vertics[i * 3 + 2] = 0.0f;
		}

		

		unsigned int VBO;
		unsigned int VAO;

		//生成VBO, VAO对, EBO象
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		//绑定顶点数组
		glBindVertexArray(VAO);
		//缓冲顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, drawPointNum * 3 * 4, vertics, GL_STATIC_DRAW);
		//设置顶点属性（位置）指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		shader.use();
		shader.setVec3("spriteColor", glm::vec3(0.0f, 1.0f, 0.0f));

		//画出数组中的所有点
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, drawPointNum);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		
		if (drawPointNum > 0)
			delete[] vertics;
	}

	void drawLine() {
		float* vertics_click_line = new float[clickPointNum * 3];
		for (int i = 0; i < clickPointNum; i++) {
			vertics_click_line[i * 3] = clickPoints[i].x;
			vertics_click_line[i * 3 + 1] = clickPoints[i].y;
			vertics_click_line[i * 3 + 2] = 0.0f;
		}

		unsigned int VBO_line;
		unsigned int VAO_line;

		//生成VBO, VAO对, EBO象
		glGenBuffers(1, &VBO_line);
		glGenVertexArrays(1, &VAO_line);
		//绑定顶点数组
		glBindVertexArray(VAO_line);
		//缓冲顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
		glBufferData(GL_ARRAY_BUFFER, clickPointNum * 3 * 4, vertics_click_line, GL_STATIC_DRAW);
		//设置顶点属性（位置）指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		shader.use();
		shader.setVec3("spriteColor", glm::vec3(1.0f, 1.0f, 1.0f));

		//画出数组中的所有点
		glBindVertexArray(VAO_line);
		glDrawArrays(GL_LINE_STRIP, 0, clickPointNum);

		glDeleteVertexArrays(1, &VAO_line);
		glDeleteBuffers(1, &VBO_line);

		if (clickPointNum > 0)
			delete[] vertics_click_line;
	}

	void drawDynamicLine() {
		if (clickPointNum <= 2)
			return;
		int dynamicPointNum = (clickPointNum / 2.0f - 1)*(clickPointNum + 1);
		float* vertics_dynamic = new float[dynamicPointNum * 3];

		int layerNum = clickPointNum - 2;
		int current_index = 0;
		int dynamic_index = 0;
		//cout << endPoints.size() << endl;
		int point_num_in_layer = clickPointNum;
		for (int i = 0; i < layerNum; i++) {
			//cout << "layer: " << i << endl;
			point_num_in_layer--;
			for (int j = 0; j < point_num_in_layer; j++) {
				//cout << "index: " << current_index << endl;
				glm::vec2 p0 = endPoints[current_index];
				glm::vec2 p1 = endPoints[current_index + 1];
				glm::vec2 pi = getInterPoint(p0, p1, dynamic_t);
				vertics_dynamic[dynamic_index * 3] = pi.x;
				vertics_dynamic[dynamic_index * 3 + 1] = pi.y;
				vertics_dynamic[dynamic_index * 3 + 2] = 0.0f;
				endPoints.push_back(pi);
				dynamic_index++;
				current_index++;
			}
			current_index++;
		}
		initEndPoints();

		//cout << dynamic_t << ": " << dynamicPointNum << endl;

		unsigned int VBO_dynamic;
		unsigned int VAO_dynamic;

		//生成VBO, VAO对, EBO象
		glGenBuffers(1, &VBO_dynamic);
		glGenVertexArrays(1, &VAO_dynamic);
		//绑定顶点数组
		glBindVertexArray(VAO_dynamic);
		//缓冲顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO_dynamic);
		glBufferData(GL_ARRAY_BUFFER, dynamicPointNum* 3 * 4, vertics_dynamic, GL_STATIC_DRAW);
		//设置顶点属性（位置）指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		layerNum = clickPointNum - 2;
		point_num_in_layer = clickPointNum - 1;
		current_index = 0;
		for (int i = 0; i < layerNum; i++) {
			shader.use();
			shader.setVec3("spriteColor", glm::vec3(1.0f, 1.0f, 1.0f));

			//画出数组中的所有点
			glBindVertexArray(VAO_dynamic);
			glDrawArrays(GL_LINE_STRIP, current_index, point_num_in_layer);
			//cout << "draw layer " << i << " from " << current_index <<  " with " << point_num_in_layer << " points" << endl;
			current_index += point_num_in_layer;
			point_num_in_layer--;
		}

		glDeleteVertexArrays(1, &VAO_dynamic);
		glDeleteBuffers(1, &VBO_dynamic);

		if (dynamicPointNum > 0)
			delete[] vertics_dynamic;
		if (clock > clock_num) {
			if (dynamic_t <= 0.99) {
				//cout << "draw, t = " << dynamic_t << " clock = " << clock << endl;
				dynamic_t += 0.01;
				clock = 0;
			}
			else {
				dynamic_t = 0;
				clock = 0;
			}
		}
		else {
			clock++;
		}
		return;
	}