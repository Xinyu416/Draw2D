#define _CRT_SECURE_NO_WARNINGS
#include "Draw2D.h"
#include "Define.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Camera.h"
#include <string.h>
#include "Thread.h"
#include "Queue.h"


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
			bmi.bmiHeader.biWidth = (LONG)GameEngine_GetFrameWidth(); // 位图宽度
			bmi.bmiHeader.biHeight = -(LONG)GameEngine_GetFrameHeight(); // 负值表示顶向下位图
			bmi.bmiHeader.biPlanes = 1; // 位图平面数，必须为1
			bmi.bmiHeader.biBitCount = (WORD)GameEngine_GetFrameBytepp() * 8; // 每像素位数
			bmi.bmiHeader.biCompression = BI_RGB; // 无压缩
			ReleaseDC(hwnd, hdc);

			uint32_t image_size = GameEngine_GetFrameWidth() * GameEngine_GetFrameHeight() * GameEngine_GetFrameBytepp();
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
			fwrite(GameEngine_GetFrameData(), 1, image_size, f);
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
		frameBuffer,
		&bmi,
		DIB_RGB_COLORS
	);
	ReleaseDC(hwnd, hdc);
}

void MatrixTest() {
	//测试点
	Vect2 pos = MakeVect2(40, 80);

	//缩放旋转结果矩阵
	Matrix srm = CreateStandardMatrix();
	//缩放旋转位移结果矩阵
	Matrix srtm = CreateStandardMatrix();

	//缩放
	Matrix ms = MakeScaMatrix(-4, 2);
	//旋转
	Matrix mr = MakeRotMatrix(Deg2Rad(50));
	//位移
	Matrix mt = MakeTranslataMatrix(100, 200);

	//分开算
	Vect2 vs = Vect2MultMatrix(pos, ms.m);
	Vect2 vsr = Vect2MultMatrix(vs, mr.m);
	Vect2 vsrt = Vect2MultMatrix(vsr, mt.m);
	PrintVect2(vsrt);

	//一起算
	Multi2Matrix(mr.m, ms.m, srm.m);
	Multi2Matrix(mt.m, srm.m, srtm.m);
	Vect2 ov = Vect2MultMatrix(pos, srtm.m);
	PrintVect2(ov);
}

//逆时针原则
void MeshTest() {
	//create Quad
	Quad quad1 = { 0 };
	quad1.vertices[0] = MakeVect2(0, 0);
	quad1.vertices[1] = MakeVect2(100, 0);
	quad1.vertices[2] = MakeVect2(100, 100);
	quad1.vertices[3] = MakeVect2(0, 100);
	quad1.uvs[0] = MakeVect2(0, 0);
	quad1.uvs[1] = MakeVect2(0.2, 0.3);
	quad1.uvs[2] = MakeVect2(0.8, 0.5);
	quad1.uvs[3] = MakeVect2(1, 1);
	quad1.color[0] = MakeColor4(100, 100, 100, 100);
	quad1.color[1] = MakeColor4(200, 200, 200, 200);
	quad1.color[2] = MakeColor4(150, 150, 150, 150);
	quad1.color[3] = MakeColor4(10, 10, 10, 10);

	Quad quad2 = { 0 };
	quad2.vertices[0] = MakeVect2(10, 10);
	quad2.vertices[1] = MakeVect2(20, 10);
	quad2.vertices[2] = MakeVect2(20, 20);
	quad2.vertices[3] = MakeVect2(10, 20);
	quad2.color[0] = MakeColor4(100, 100, 100, 100);
	quad2.color[1] = MakeColor4(200, 200, 200, 200);
	quad2.color[2] = MakeColor4(150, 150, 150, 150);
	quad2.color[3] = MakeColor4(10, 10, 10, 10);

	Geometry geo = CreateGeometry(4);
	GeometryAddQuad(&geo, quad1);
	GeometryAddQuad(&geo, quad2);

	printf("num:%d\n", geo.numOfQuad);

	//test point in or out
	Quad quad3 = { 0 };
	quad3.vertices[0] = MakeVect2(10, 10);
	quad3.vertices[1] = MakeVect2(30, 10);
	quad3.vertices[2] = MakeVect2(30, 30);
	quad3.vertices[3] = MakeVect2(10, 30);
	quad3.color[0] = MakeColor4(100, 100, 100, 100);
	quad3.color[1] = MakeColor4(200, 200, 200, 200);
	quad3.color[2] = MakeColor4(150, 150, 150, 150);
	quad3.color[3] = MakeColor4(10, 10, 10, 10);

	GeometryAddQuad(&geo, quad3);

	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(255,255,255,255),.textureId = 1 };
	Mesh mesh = CreateMesh(1, MakeVect2(1, 1), 50.f, MakeVect2(1, 1), geo, tm, mat);

	PrintMesh(&mesh);
}

int main()
{
	//MeshTest();
	//GameIns_Init();
	//ThreadTest();
	//Camera_Init();
	//QueueTest();
	Task_Main();
	return;
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
		SendBufferToDisplay(hwnd, bmi, GameEngine_GetFrameData());


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
		GameEngin_SceneLoop(delta);

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