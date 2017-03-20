#include <windows.h>
#include "glew.h"
#include <stdio.h>
#include <math.h>
#include <string>
#include "../GPUProgram/Utils.h"
#include "../GPUProgram/GPUProgram.h"
#include "../GPUProgram/ObjModel.h"
#include "../Glm/glm.hpp"
#include "../Glm/ext.hpp"
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")

const std::string RESOURCE_ROOT = "LearnShader/resource/";
const std::string SHADER_ROOT = "LearnShader/resource/shader/";
const std::string IMAGE_ROOT = "LearnShader/resource/image/";
const std::string MODEL_ROOT = "LearnShader/resource/model/";

LRESULT CALLBACK GLWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

float* CreatePerspective(float fov, float aspect, float zNear, float zFar)
{
	float *matrix = new float[16];
	float half = fov / 2.0f;
	float randiansOfHalf = (half / 180.0f)*3.14f;
	float yscale = cosf(randiansOfHalf) / sinf(randiansOfHalf);
	float xscale = yscale / aspect;
	memset(matrix, 0, sizeof(float) * 16);
	matrix[0] = xscale;
	matrix[5] = yscale;
	matrix[10] = (zNear + zFar) / (zNear - zFar);
	matrix[11] = -1.0f;
	matrix[14] = (2.0f*zNear*zFar) / (zNear - zFar);
	return matrix;
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = NULL;
	wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndClass.hIcon = NULL;
	wndClass.hIconSm = NULL;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc=GLWindowProc;
	wndClass.lpszClassName = L"OpenGL";
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;
	ATOM atom = RegisterClassEx(&wndClass);

	HWND hwnd = CreateWindowEx(NULL, L"OpenGL", L"RenderWindow", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, hInstance, NULL);
	HDC dc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_TYPE_RGBA | PFD_DOUBLEBUFFER;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int pixelFormatID = ChoosePixelFormat(dc, &pfd);

	SetPixelFormat(dc,pixelFormatID,&pfd);

	HGLRC rc = wglCreateContext(dc);
	wglMakeCurrent(dc, rc);

	glewInit();
	unsigned char*imageData;
	int width, height;
	imageData = LoadBMP((IMAGE_ROOT + "sample.bmp").c_str(), width, height);

	GLuint mainTexture;
	glGenTextures(1, &mainTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glBindTexture(GL_TEXTURE_2D,0);

    GPUProgram gpuProgram;
    gpuProgram.AttachShader(GL_VERTEX_SHADER, (SHADER_ROOT + "sample.vs").c_str());
    gpuProgram.AttachShader(GL_FRAGMENT_SHADER, (SHADER_ROOT + "sample.fs").c_str());
    gpuProgram.Link();

    gpuProgram.DetectAttribute("pos");
    gpuProgram.DetectAttribute("texcoord");
    gpuProgram.DetectAttribute("normal");

    gpuProgram.DetectUniform("M");
    gpuProgram.DetectUniform("V");
    gpuProgram.DetectUniform("P");
    gpuProgram.DetectUniform("U_MainTexture");
    //init 3D model
    ObjModel model;
    model.Init((MODEL_ROOT + "Cube.obj").c_str());

	float identity[] = {
		1.0f,0,0,0,
		0,1.0f,0,0,
		0,0,1.0f,0,
		0,0,0,1.0f
	};
    glm::mat4 modelMatrix = glm::translate<float>(0.0f, 0.0f, -2.0f) * glm::rotate<float>(-30.0f, 0.0f, 1.0f, 1.0f); // ïΩà⁄ + ê˘?
	float *projection = CreatePerspective(50.0f,800.0f/600.0f,0.1f,1000.0f);

	glClearColor(41.0f/255.0f,  71.0f/255.0f, 121.0f / 255.0f, 1.0f);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // ?ûyñÕéÆ
        glEnable(GL_DEPTH_TEST);

		glUseProgram(gpuProgram.mProgram);
		glUniformMatrix4fv(gpuProgram.getLocation("M"), 1,GL_FALSE,glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(gpuProgram.getLocation("V"), 1, GL_FALSE, identity);
		glUniformMatrix4fv(gpuProgram.getLocation("P"), 1, GL_FALSE, projection);
		
		glUniform1i(gpuProgram.getLocation("U_MainTexture"), 0);

        model.BindModel(gpuProgram.getLocation("pos"), gpuProgram.getLocation("texcoord"), gpuProgram.getLocation("normal"));
        model.Draw();

		glUseProgram(0);
        glFinish(); // TODO: ?ò¢îüêîëzä±èY?
		SwapBuffers(dc);
	}
	return 0;
}