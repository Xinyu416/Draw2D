#define _CRT_SECURE_NO_WARNINGS
#include "Draw2D.h"
#include "Define.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Camera.h"
#include <string.h>
#include "Thread.h"
#include "Queue.h"
#include "MemManager.h"
#include "Shader.h"


//窗口过程函数(消息回调)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		printf("VM_CREATE\n");
		break;
	case WM_SIZE:
	{
		//lParam 将窗口的新宽度和高度作为 16 位值纳入一个 32 位或 64 位数字中
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		printf("WM_SIZE\n");
	}
	break;
	case WM_PAINT:
	{
		printf("WM_PAINT\n");
		/*OnPaint(hwnd);*/
	}
	break;
	case WM_KEYDOWN:
	{
		//键盘按下事件
		GameEngine_MouseKeyEvent(KEYDOWN, wParam);

		if ((char)wParam == '1')
		{
			printf("start save bmp\n");
			// 定义位图信息结构体
			HDC hdc = GetDC(hwnd);
			BITMAPINFO bmi;
			memset(&bmi, 0, sizeof(BITMAPINFO)); // 初始化结构体
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); // 设置位图信息头大小
			bmi.bmiHeader.biWidth = (LONG)Renderer_GetFrameWidth(); // 位图宽度
			bmi.bmiHeader.biHeight = -(LONG)Renderer_GetFrameHeight(); // 负值表示顶向下位图
			bmi.bmiHeader.biPlanes = 1; // 位图平面数，必须为1
			bmi.bmiHeader.biBitCount = (WORD)Renderer_GetFrameBytepp() * 8; // 每像素位数
			bmi.bmiHeader.biCompression = BI_RGB; // 无压缩
			ReleaseDC(hwnd, hdc);

			uint32_t image_size = Renderer_GetFrameWidth() * Renderer_GetFrameHeight() * Renderer_GetFrameBytepp();
			uint32_t file_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + image_size;
			// 初始化文件头
			BITMAPFILEHEADER file_header = {
				.bfType = 0x4D42,        // "BM"
				.bfSize = file_size,
				.bfReserved1 = 0,
				.bfReserved2 = 0,
				.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
			};
			const char* file = "C:\\Users\\DRF\\Desktop\\Temp\\level.bmp";
			FILE* f = fopen(file, "wb");
			// 写入文件头和信息头
			fwrite(&file_header, 1, sizeof(file_header), f);
			fwrite(&bmi, 1, sizeof(BITMAPINFO), f);
			//fwrite(GameEngine_GetFrameData(), 1, image_size, f);
			fclose(f);
			printf("save bmp\n");
		}
	}
	break;
	case WM_KEYUP:
		//键盘释放事件
		GameEngine_MouseKeyEvent(KEYUP, wParam);
		break;
	case WM_LBUTTONDOWN:
		/*int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		printf("LeftMouseButtonDown: (%d, %d)\n", x, y);*/
		//鼠标左键按下事件
		GameEngine_MouseKeyEvent(LMBD, lParam);
		break;
	case WM_LBUTTONUP:
		//鼠标左键释事件
		GameEngine_MouseKeyEvent(LMBU, wParam);
		break;
	case WM_RBUTTONDOWN:
		//鼠标右键按下事件
		GameEngine_MouseKeyEvent(RMBD, wParam);
		break;
	case WM_RBUTTONUP:
		//鼠标右键释放事件
		GameEngine_MouseKeyEvent(RMBU, wParam);
		break;
	case WM_MOUSEMOVE:
		//int xPos = GET_X_LPARAM(lParam);
		//int yPos = GET_Y_LPARAM(lParam);
		//// 例如，打印鼠标位置到控制台或进行其他处理
		//printf("--------- Mouse position: X=%d, Y=%d\n", xPos, yPos);
		break;
	case WM_DESTROY:
	{
		printf("WM_DESTROY\n");
		PostQuitMessage(0);
		EngineClose();
	}
	return 0;
	case WM_CLOSE:
	{
		printf("WM_CLOSE\n");
		WindowsClose();
		if (MessageBox(hwnd, L"Really quit?", L"Draw2D", MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hwnd);
		}
	}
	return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/*创建渲染窗口*/
HWND CreateRenderWindow(uint32_t width, uint32_t height) {
	// 注册窗口类
	HINSTANCE hInstance = GetModuleHandle(NULL);
	const wchar_t CLASS_NAME[] = L"Sample Window Class";
	WNDCLASS wc = {
		.lpfnWndProc = WindowProc,
		.hInstance = hInstance,
		.lpszClassName = CLASS_NAME,
		.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME + 1),
		.hCursor = LoadCursor(NULL, IDC_ARROW)
	};

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, L"窗口类注册失败!", L"错误", MB_ICONERROR);
		return 1;
	}

	// 创建窗口
	HWND hwnd = CreateWindowEx(
		0,                              // 扩展窗口样式
		CLASS_NAME,                     // 窗口类名
		L"Draw2D",         // 窗口标题
		WS_OVERLAPPEDWINDOW,            // 窗口样式
		CW_USEDEFAULT, CW_USEDEFAULT,	// 位置和大小
		width, height,
		NULL,							// 父窗口    
		NULL,							// 菜单
		hInstance,						// 实例句柄
		NULL							// 附加数据
	);
	return hwnd;
}

/*buffer显示*/
void SendBufferToDisplay(HWND hwnd, BITMAPINFO bmi, uint8_t* frameBuffer) {
	uint32_t width = Renderer_GetFrameWidth();
	uint32_t height = Renderer_GetFrameHeight();

	HDC hdc = GetDC(hwnd);
	// 将位图数据传输到设备上下文
	SetDIBitsToDevice(
		hdc,
		0, 0,
		(DWORD)width, (DWORD)height,
		0, 0,
		0, (UINT)height,
		frameBuffer,
		&bmi,
		DIB_RGB_COLORS
	);
	ReleaseDC(hwnd, hdc);
}

int main()
{
	//Task_Main();

	//printf("MemManager size:%lu\n",sizeof(MemManager));
	//printf("uint32_t size:%lu\n",sizeof(uint32_t));

	//MemManager memM = Mem_Create(40);
	//PrintMemManager(&memM);
	//uint8_t* a = (uint8_t*)Mem_AllocateStatic(&memM, sizeof(uint8_t));
	////ClearMemStatic(&memM);
	//uint16_t* ca = (uint16_t*)Mem_AllocateStatic(&memM, sizeof(uint16_t)*3);
	//uint32_t* aa = (uint32_t*)Mem_AllocateStatic(&memM, sizeof(uint32_t));
	//ca[0] = 0xAABB;
	//ca[1] = 0xCCDD;
	//ca[2] = 0xABCD;
	//uint8_t* c = (uint8_t*)Mem_AllocateDynamic(&memM, sizeof(uint8_t));

	//Mem_ClearDynamic(&memM);
	//PrintMemManager(&memM);
	//Color4* cc = (Color4*)Mem_AllocateDynamic(&memM, sizeof(Color4));
	//uint8_t* dd = (uint8_t*)Mem_AllocateDynamic(&memM, sizeof(uint8_t));
	//uint32_t* d = (uint32_t*)Mem_AllocateDynamic(&memM, sizeof(uint32_t));
	//*a = 0xaa;
	//*aa = 0xcccc;
	//*c = 0xcc;
	//cc->r = 0xdd;
	//cc->g = 0xdc;
	//cc->b = 0xdb;
	//cc->a = 0xda;
	//*dd = 0xaa;
	//*d = 0xee;
	//PrintMemManager(&memM);
	//PrintMemData(&memM);
	//Mem_Release(&memM);
	//GetNumberOfProcessors();
	//return;

	HWND hwnd = CreateRenderWindow(700 + 16, 775 + 39);
	if (hwnd == NULL)
	{
		MessageBox(NULL, L"窗口创建失败!", L"错误", MB_ICONERROR);
		return 1;
	}

	/*获取窗口设备信息*/
	HDC hdc = GetDC(hwnd);
	int bpp = GetDeviceCaps(hdc, BITSPIXEL);
	RECT windowrc = { 0 };
	GetClientRect(hwnd, &windowrc);
	int width = windowrc.right;
	int height = windowrc.bottom;

	printf("DeviceInfo width:%d,height:%d,bpp:%d\n", width, height, bpp);

	//每像素字节数
	uint8_t bytepp = bpp / 8;

	/*引擎初始化*/
	uint8_t fps = 2;
	GameEngineInit(width, height, fps, bytepp);

	/*显示窗口*/
	ShowWindow(hwnd, SW_SHOW);

	/*消息循环*/
	AppLoop(hwnd);

	/*清理释放*/
	AppRelease();

	return 0;
}

void AppLoop(HWND hwnd) {

	// 定义位图信息结构体
	HDC hdc = GetDC(hwnd);
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO)); // 初始化结构体
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); // 设置位图信息头大小
	bmi.bmiHeader.biWidth = (LONG)Renderer_GetFrameWidth(); // 位图宽度
	bmi.bmiHeader.biHeight = -(LONG)Renderer_GetFrameHeight(); // 负值表示顶向下位图
	bmi.bmiHeader.biPlanes = 1; // 位图平面数，必须为1
	bmi.bmiHeader.biBitCount = (WORD)Renderer_GetFrameBytepp() * 8; // 每像素位数
	bmi.bmiHeader.biCompression = BI_RGB; // 无压缩
	ReleaseDC(hwnd, hdc);

	LARGE_INTEGER frameEndTime;
	LARGE_INTEGER curTime;
	LARGE_INTEGER lastTime;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);//频率计数
	double invFreq = 1.0 / (double)freq.QuadPart;//每个计数的时间
	double targetFrameTime = 1.0 / (double)GameEngine_GetFPS();//目标的每帧时间
	DWORD sleepTime = 0;
	MSG msg = { 0 };

	/*实际运行的每帧的时间*/
	float delta = 0;
	/*当前帧结束时间*/
	QueryPerformanceCounter(&frameEndTime);

	while (GameEngine_IsRuning()) {
		QueryPerformanceCounter(&lastTime);

		/*帧开始时发送画面数据供显示 帧间处理画面数据*/
		SendBufferToDisplay(hwnd, bmi, Renderer_GetFrameBuffer());


		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 游戏主循环，可以在这里处理输入状态
			// 例如检查键盘/鼠标的当前状态
		}

		/*场景循环*/
		GameEngine_SceneLoop(delta);

		QueryPerformanceCounter(&curTime);
		sleepTime = fmax(targetFrameTime - (curTime.QuadPart - lastTime.QuadPart) * invFreq, 0.0) * 1000;
		//printf("sleep:%f\n", targetFrameTime - (curTime.QuadPart - lastTime.QuadPart) * invFreq);
		Sleep(sleepTime);

		QueryPerformanceCounter(&frameEndTime);
		delta = (float)(frameEndTime.QuadPart - lastTime.QuadPart) * invFreq;
		//printf("deltatime:%f\n", delta);

	}
}

void AppRelease() {
	GameEngine_Release();
	printf("AppRelease\n");
}