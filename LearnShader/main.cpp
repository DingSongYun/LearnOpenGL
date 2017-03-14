#include <windows.h>
#include "glew.h"
#include <stdio.h>
#include <math.h>
#include <string>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")

const std::string RESOURCE_ROOT = "LearnShader/resource/";
const std::string SHADER_ROOT = "LearnShader/resource/shader/";
const std::string IMAGE_ROOT = "LearnShader/resource/image/";

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

char* LoadFileContent(const char*path)
{
	char *pFileContent = NULL;
	FILE*pFile = fopen(path, "rb");
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		int nLen = ftell(pFile);
		if (nLen > 0)
		{
			rewind(pFile);
			pFileContent = new char[nLen + 1];
			fread(pFileContent,1,nLen,pFile);
			pFileContent[nLen] = '\0';
		}
		fclose(pFile);
	}
	return pFileContent;
}

GLuint CreateProgram(const char*vsPath,const char*fsPath)
{
	GLuint program=glCreateProgram();
	GLuint vsShader, fsShader;
	vsShader = glCreateShader(GL_VERTEX_SHADER);
	fsShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char*vsCode = LoadFileContent((SHADER_ROOT + "sample.vs").c_str());
	const char*fsCode = LoadFileContent((SHADER_ROOT + "sample.fs").c_str());
	glShaderSource(vsShader, 1, &vsCode, nullptr);
	glShaderSource(fsShader, 1, &fsCode, nullptr);
	glCompileShader(vsShader);
	glCompileShader(fsShader);
	glAttachShader(program, vsShader);
	glAttachShader(program, fsShader);
	glLinkProgram(program);
	glDetachShader(program, vsShader);
	glDetachShader(program, fsShader);
	glDeleteShader(vsShader);
	glDeleteShader(fsShader);
	return program;
}

float* CreatePerspective(float fov, float aspect, float zNear, float zFar)
{
	float *matrix = new float[16];
	float half = fov / 2.0f;
	float randiansOfHalf = (half / 180.0f)*3.14f;
	float yscale = cos(randiansOfHalf) / sin(randiansOfHalf);
	float xscale = yscale / aspect;
	memset(matrix, 0, sizeof(float) * 16);
	matrix[0] = xscale;
	matrix[5] = yscale;
	matrix[10] = (zNear + zFar) / (zNear - zFar);
	matrix[11] = -1.0f;
	matrix[14] = (2.0f*zNear*zFar) / (zNear - zFar);
	return matrix;
}

unsigned char* LoadBMP(const char*path, int &width, int &height)
{
	unsigned char*imageData=nullptr;
	FILE *pFile = fopen(path, "rb");
	if (pFile)
	{
		BITMAPFILEHEADER bfh;
		fread(&bfh, sizeof(BITMAPFILEHEADER), 1, pFile);
		if (bfh.bfType==0x4D42)
		{
			BITMAPINFOHEADER bih;
			fread(&bih, sizeof(BITMAPINFOHEADER), 1, pFile);
			width = bih.biWidth;
			height = bih.biHeight;
			int pixelCount = width*height * 3;
			imageData = new unsigned char[pixelCount];
			fseek(pFile, bfh.bfOffBits, SEEK_SET);
			fread(imageData, 1, pixelCount, pFile);

			unsigned char temp;
			for (int i=0;i<pixelCount;i+=3)
			{
				temp = imageData[i+2];
				imageData[i + 2] = imageData[i];
				imageData[i] = temp;
			}
		}
		fclose(pFile);
	}
	return imageData;
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

	GLuint program = CreateProgram("sample.vs", "sample.fs");
	GLint posLoc, colorLoc,texcoordLoc,mLoc,vLoc,ploc,textureLoc;
	posLoc = glGetAttribLocation(program, "pos");
	colorLoc = glGetAttribLocation(program, "color");
	texcoordLoc = glGetAttribLocation(program, "texcoord");

	mLoc = glGetUniformLocation(program,"M");
	vLoc = glGetUniformLocation(program,"V");
	ploc = glGetUniformLocation(program, "P");
	textureLoc = glGetUniformLocation(program, "U_MainTexture");


	float identity[] = {
		1.0f,0,0,0,
		0,1.0f,0,0,
		0,0,1.0f,0,
		0,0,0,1.0f
	};
	float *projection = CreatePerspective(50.0f,800.0f/600.0f,0.1f,1000.0f);


	struct Vertex
	{
		float v[3];
		float rgba[4];
		float texcoord[4];
	};

	Vertex vertex[4];
	memset(vertex, 0, sizeof(Vertex) * 4);
	vertex[0].v[0] = 0.0f;
	vertex[0].v[1] = 0.0f;
	vertex[0].v[2] = -30.0f;
	vertex[0].rgba[0] = 1.0f;
	vertex[0].rgba[1] = 1.0f; 
	vertex[0].rgba[2] = 1.0f; 
	vertex[0].rgba[3] = 1.0f;
	vertex[0].texcoord[0] = 0.0f;
	vertex[0].texcoord[1] = 0.0f;

	vertex[1].v[0] = 10.0f;
	vertex[1].v[1] = 0.0f;
	vertex[1].v[2] = -30.0f;
	vertex[1].rgba[0] = 1.0f;
	vertex[1].rgba[1] = 1.0f;
	vertex[1].rgba[2] = 1.0f;
	vertex[1].rgba[3] = 1.0f;
	vertex[1].texcoord[0] = 1.0f;
	vertex[1].texcoord[1] = 0.0f;

	vertex[2].v[0] = 10.0f;
	vertex[2].v[1] = 10.0f;
	vertex[2].v[2] = -30.0f;
	vertex[2].rgba[0] = 1.0f;
	vertex[2].rgba[1] = 1.0f;
	vertex[2].rgba[2] = 1.0f;
	vertex[2].rgba[3] = 1.0f;
	vertex[2].texcoord[0] = 1.0f;
	vertex[2].texcoord[1] = 1.0f;

	vertex[3].v[0] = 0.0f;
	vertex[3].v[1] = 10.0f;
	vertex[3].v[2] = -30.0f;
	vertex[3].rgba[0] = 1.0f;
	vertex[3].rgba[1] = 1.0f;
	vertex[3].rgba[2] = 1.0f;
	vertex[3].rgba[3] = 1.0f;
	vertex[3].texcoord[0] = 0.0f;
	vertex[3].texcoord[1] = 1.0f;

	unsigned short indexes[] = {0,1,2,3};
	GLuint vbo,ibo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 4, indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
		glUniformMatrix4fv(mLoc, 1,GL_FALSE,identity);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, identity);
		glUniformMatrix4fv(ploc, 1, GL_FALSE, projection);
		
		glUniform1i(textureLoc, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(posLoc);
		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		glEnableVertexAttribArray(colorLoc);
		glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*3));

		glEnableVertexAttribArray(texcoordLoc);
		glVertexAttribPointer(texcoordLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 7));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
		SwapBuffers(dc);
	}
	return 0;
}