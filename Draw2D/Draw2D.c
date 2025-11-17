#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "GameEngine.h"

void AppLoop(HWND hwnd);

void OnResize(uint32_t width,uint32_t height) {
	printf("width:%d,height:%d\n",width,height);
}

void OnPaint(HWND hwnd)
{
	printf("OnPaint\n");
	PAINTSTRUCT ctx;
	HDC hdc = BeginPaint(hwnd, &ctx);

#pragma region sample
	// 实心颜色画刷
	HBRUSH hSolidBrush = CreateSolidBrush(RGB(255, 0, 0));
	// 阴影画刷
	HBRUSH hHatchBrush = CreateHatchBrush(HS_CROSS, RGB(0, 0, 255));
	// 模式画刷（需要位图）
	HBITMAP hBitmap = CreateBitmap(80, 80, 1, 24, NULL);
	if (hBitmap)
	{
		SelectObject(hdc, hBitmap);
	}
	//HBRUSH hPatternBrush = CreatePatternBrush(hBitmap);

#pragma endregion

#pragma region sample
	HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 200, 255));

	/*RECT rect = { 100, 100, 300, 200 };
	FillRect(hdc, &rect, hRedBrush);
	FillRect(hdc, &ctx.rcPaint, hRedBrush);*/

#pragma endregion

#pragma region sample

	static RECT rects[4];
	static HBRUSH brushes[4];
	static int initialized = 0;

	if (!initialized) {
		// 初始化矩形位置
		for (int i = 0; i < 4; i++) {
			rects[i].left = 50 + i * 80;
			rects[i].top = 50;
			rects[i].right = rects[i].left + 60;
			rects[i].bottom = 150;
		}

		// 创建不同颜色的画刷
		brushes[0] = CreateSolidBrush(RGB(255, 0, 0));     // 红色
		brushes[1] = CreateSolidBrush(RGB(0, 255, 0));     // 绿色
		brushes[2] = CreateSolidBrush(RGB(0, 0, 255));     // 蓝色
		brushes[3] = CreateSolidBrush(RGB(255, 255, 0));   // 黄色

		initialized = 1;
	}
	// 填充所有矩形
	for (int i = 0; i < 4; i++) {
		FillRect(hdc, &rects[i], brushes[i]);
	}

#pragma endregion

	EndPaint(hwnd, &ctx);
	// 清理资源
	DeleteObject(hRedBrush);
	ReleaseDC(NULL, hdc);
}

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		printf("VM_CREATE\n");
		/*初始化GameEngine*/
		break;
	case WM_SIZE:
	{
		//lParam 将窗口的新宽度和高度作为 16 位值纳入一个 32 位或 64 位数字中
		int width = LOWORD(lParam); 
		int height = HIWORD(lParam);
		OnResize(width,height);
		printf("WM_SIZE");
	}
	break;
	case WM_PAINT:
	{
		printf("WM_PAINT\n");
		OnPaint(hwnd);
	}
	return 0;
	case WM_DESTROY:
	{
		printf("WM_DESTROY\n");
		PostQuitMessage(0);
	}
	return 0;
	case WM_CLOSE:
	{
		printf("WM_CLOSE\n");
		if (MessageBox(hwnd, L"Really quit?", L"Draw2D", MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hwnd);
		}
	}
	return 0;
	default:
		//此函数对消息执行默认操作，该操作因消息类型而异。
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

int main()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	// 注册窗口类
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
		L"在main函数中创建窗口",         // 窗口标题
		WS_OVERLAPPEDWINDOW,            // 窗口样式
		CW_USEDEFAULT, CW_USEDEFAULT,	// 位置和大小
		800, 600,
		NULL,							// 父窗口    
		NULL,							// 菜单
		hInstance,						// 实例句柄
		NULL							// 附加数据
	);

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
	ReleaseDC(hwnd, hdc);


	//BitBlt();
	/*窗口变化通知*/

	// 显示窗口
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	GameEngineInit(width,height,30, OnResize);

	/*消息循环*/
	AppLoop(hwnd);
	
	return 0;
}

void AppLoop(HWND hwnd) {
	LARGE_INTEGER curTime;
	LARGE_INTEGER lastTime;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	double invFreq = 1.0 / (double)freq.QuadPart;
	double targetFrameTime = 1.0 / (double)GameEngine_GetFPS();
	DWORD sleepTime = 0;
	MSG msg = { 0 };
	while (GameEngine_IsRuning()) {
		QueryPerformanceCounter(&lastTime);
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			GameEnginSceneLoop();
		}
		QueryPerformanceCounter(&curTime);
		sleepTime = fmax(targetFrameTime - (curTime.QuadPart - lastTime.QuadPart) * invFreq, 0.0) * 1000;
		printf("sleep:%f\n", targetFrameTime - (curTime.QuadPart - lastTime.QuadPart) * invFreq);
		Sleep(sleepTime);
	}
}

void temp() {
	HDC hdc = GetDC(window);
	BITMAPINFO bmi; // 定义位图信息结构体
	memset(&bmi, 0, sizeof(BITMAPINFO)); // 初始化结构体
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); // 设置位图信息头大小
	bmi.bmiHeader.biWidth = (LONG)FramBuffer.width; // 位图宽度
	bmi.bmiHeader.biHeight = -(LONG)FramBuffer.height; // 负值表示顶向下位图
	bmi.bmiHeader.biPlanes = 1; // 位图平面数，必须为1
	bmi.bmiHeader.biBitCount = (WORD)FramBuffer.bpp; // 每像素位数
	bmi.bmiHeader.biCompression = BI_RGB; // 无压缩
	SetDIBitsToDevice( // 将位图数据传输到设备上下文
		hdc,
		0, 0,
		(DWORD)FramBuffer.width, (DWORD)FramBuffer.height,
		0, 0,
		0, (UINT)FramBuffer.height,
		FramBuffer.memory,
		&bmi,
		DIB_RGB_COLORS
	);
	ReleaseDC(window, hdc); // 释放设备上下文
}

void onPaint(HWND hwnd, const uint8_t* frameBufferData, BITMAPINFO* pbmi) {
	uint32_t width = engine_getFrameWidth();
	uint32_t height = engine_getFrameHeight();
	HDC hdc = GetDC(hwnd);
	// 将位图数据传输到设备上下文
	SetDIBitsToDevice(
		hdc,
		0, 0,
		(DWORD)width, (DWORD)height,
		0, 0,
		0, (UINT)height,
		frameBufferData,
		pbmi,
		DIB_RGB_COLORS
	);
	ReleaseDC(hwnd, hdc);
}