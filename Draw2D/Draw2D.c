#include "Draw2D.h"
#include "Define.h"

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
		L"在main函数中创建窗口",         // 窗口标题
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
void SendBufferToDisplay(HWND hwnd, BITMAPINFO bmi) {
	uint32_t width = GameEngine_GetFrameWidth();
	uint32_t height = GameEngine_GetFrameHeight();

	HDC hdc = GetDC(hwnd);
	// 将位图数据传输到设备上下文
	SetDIBitsToDevice(
		hdc,
		0, 0,
		(DWORD)width, (DWORD)height,
		0, 0,
		0, (UINT)height,
		GameEngine_GetFrameData(),
		&bmi,
		DIB_RGB_COLORS
	);
	ReleaseDC(hwnd, hdc);
}

int main()
{
	Vect2 v = MakeVect2(2, 3);
	PrintVect2(v);

	return;

	HWND hwnd = CreateRenderWindow(800, 600);
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
	//每像素字节数
	uint8_t bytepp = bpp / 8;

	/*引擎初始化*/
	uint8_t fps = 30;
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
	bmi.bmiHeader.biWidth = (LONG)GameEngine_GetFrameWidth(); // 位图宽度
	bmi.bmiHeader.biHeight = -(LONG)GameEngine_GetFrameHeight(); // 负值表示顶向下位图
	bmi.bmiHeader.biPlanes = 1; // 位图平面数，必须为1
	bmi.bmiHeader.biBitCount = (WORD)GameEngine_GetFrameBytepp() * 8; // 每像素位数
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
		SendBufferToDisplay(hwnd, bmi);

		/*消息循环*/
		while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/*场景循环*/
		GameEngin_SceneLoop(delta);

		QueryPerformanceCounter(&curTime);
		sleepTime = fmax(targetFrameTime - (curTime.QuadPart - lastTime.QuadPart) * invFreq, 0.0) * 1000;
		//printf("sleep:%f\n", targetFrameTime - (curTime.QuadPart - lastTime.QuadPart) * invFreq);
		Sleep(sleepTime);

		QueryPerformanceCounter(&frameEndTime);
		delta = (float)(frameEndTime.QuadPart - lastTime.QuadPart) * invFreq;
		printf("deltatime:%f\n", delta);
	}
}

void AppRelease() {
	GameEngine_Release();
	printf("AppRelease\n");
}