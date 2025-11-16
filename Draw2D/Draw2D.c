#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

static uint8_t* __frameBuffer = NULL;

double GameFPS = 30.0f;

void OnPaint(HWND hwnd)
{
	printf("OnPaint\n");
	PAINTSTRUCT ctx;
	HDC hdc = BeginPaint(hwnd, &ctx);

#pragma region sample
	/**/
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
	HBRUSH hPatternBrush = CreatePatternBrush(hBitmap);

#pragma endregion


#pragma region sample
	HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 200, 255));

	/*RECT rect = { 100, 100, 300, 200 };
	FillRect(hdc, &rect, hRedBrush);
	FillRect(hdc, &ctx.rcPaint, hRedBrush);*/

#pragma endregion

#pragma region sample

	//static RECT rects[4];
	//static HBRUSH brushes[4];
	//static int initialized = 0;

	//if (!initialized) {
	//	// 初始化矩形位置
	//	for (int i = 0; i < 4; i++) {
	//		rects[i].left = 50 + i * 80;
	//		rects[i].top = 50;
	//		rects[i].right = rects[i].left + 60;
	//		rects[i].bottom = 150;
	//	}

	//	// 创建不同颜色的画刷
	//	brushes[0] = CreateSolidBrush(RGB(255, 0, 0));     // 红色
	//	brushes[1] = CreateSolidBrush(RGB(0, 255, 0));     // 绿色
	//	brushes[2] = CreateSolidBrush(RGB(0, 0, 255));     // 蓝色
	//	brushes[3] = CreateSolidBrush(RGB(255, 255, 0));   // 黄色

	//	initialized = 1;
	//}
	//// 填充所有矩形
	//for (int i = 0; i < 4; i++) {
	//	FillRect(hdc, &rects[i], brushes[i]);
	//}

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
		break;
	case WM_SIZE:
	{
		//lParam 将窗口的新宽度和高度作为 16 位值纳入一个 32 位或 64 位数字中
		int width = LOWORD(lParam);  // Macro to get the low-order word.
		int height = HIWORD(lParam); // Macro to get the high-order word.

		printf("WM_SIZE");
		printf("width:%d,height:%d\n", width, height);
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
		//printf("default\n");
		//此函数对消息执行默认操作，该操作因消息类型而异。
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

int main()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// 注册窗口类
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

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

		// 位置和大小
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		NULL,       // 父窗口    
		NULL,       // 菜单
		hInstance,  // 实例句柄
		NULL        // 附加数据
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
	printf("    window bpp:%d rect:%d %d %d %d\n", bpp, windowrc.left, windowrc.top, windowrc.right, windowrc.bottom);
	ReleaseDC(hwnd, hdc);


	uint32_t bitDepth = 3;
	__frameBuffer = (uint8_t*)malloc(width * height * bitDepth);

	for (size_t i = 0; i < width * height; i++)
	{
		__frameBuffer[i] = 100;
		__frameBuffer[i + 1] = 100;
		__frameBuffer[i + 2] = 100;

	}

	// 显示窗口
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	printf("窗口已创建，可以同时使用控制台输出！\n");

	// 消息循环
	//MSG msg;
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);

	//	// 可以在这里添加其他逻辑
	//	if (msg.message == WM_KEYDOWN) {
	//		printf("按键按下: %c\n", (char)msg.wParam);
	//	}
	//}

	MSG msg;
	LARGE_INTEGER freq; // 计时器频率
	LARGE_INTEGER now; // 当前时间
	LARGE_INTEGER last; // 上一帧时间
	QueryPerformanceFrequency(&freq); // 获取计时器频率
	double invFreq = 1.0 / (double)freq.QuadPart;
	double targetFrameTime = 1.0 / GameFPS; // 目标帧时间
	bool isQuit = false;
	QueryPerformanceCounter(&last); //  只在循环前初始化一次
	while (!isQuit)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// 有消息:处理它
			if (msg.message == WM_QUIT)
			{
				isQuit = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (isQuit) break;
		QueryPerformanceCounter(&now);
		//判断当前是否满足帧率，如果没有则跳过
		if ((double)(now.QuadPart - last.QuadPart) * invFreq >= targetFrameTime)
		{
			//这里可以添加每帧需要执行的逻辑代码
			//printf("Frame executed.\n");
		}
	}

	printf("程序退出\n");
	return 0;
}