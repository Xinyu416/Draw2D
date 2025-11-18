#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>


LRESULT CALLBACK WindowProcSample(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


void OnPaint_(HWND hwnd)
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

int main_()
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";
	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProcSample;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, SW_SHOW);

	// Run the message loop.

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//	// 可以在这里添加其他逻辑
		//	if (msg.message == WM_KEYDOWN) {
		//		printf("按键按下: %c\n", (char)msg.wParam);
		//	}
	}

	return 0;
}

LRESULT CALLBACK WindowProcSample(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}