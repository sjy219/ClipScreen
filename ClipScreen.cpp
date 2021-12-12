#include <windows.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	WNDCLASS wndclass;
	MSG msg;
	TCHAR szAppName[] = "Window";
	SetProcessDPIAware();


	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = NULL;
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wndclass);

	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW,
		szAppName,
		"2333",
		WS_POPUP,
		0,
		0,
		cx,
		cy,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, hdctemp;
	PAINTSTRUCT ps;
	HBITMAP hBitmap;
	static int iAlpha = 0;
	static long iCount = 0;
	COLORREF TransparentColor = RGB(233, 233, 233);
	static POINT start, now;
	HBRUSH hBrushRect, hBrushFrame;
	RECT rect;
	int minx, miny;
	int absx, absy;

	switch (message)
	{
	case WM_CREATE:

		start.x = now.x = -1;
		start.y = now.y = -1;
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		SetTimer(hwnd, 1, 1, NULL);
		return 0;
	case WM_LBUTTONDOWN:
		SetCapture(hwnd);
		GetCursorPos(&start);
		return 0;
	case WM_MOUSEMOVE:

		if (wParam & MK_LBUTTON)
		{
			now.x = LOWORD(lParam);
			now.y = HIWORD(lParam);
			if (iCount++ % 4 == 0)
				InvalidateRect(hwnd, NULL, true);
		}

		return 0;
	case WM_TIMER:

		SetLayeredWindowAttributes(hwnd, 0, iAlpha, LWA_ALPHA);
		if (iAlpha > 128)
			KillTimer(hwnd, 1);
		iAlpha += 10;
		return 0;

	case WM_LBUTTONUP:
		hdc = GetDC(NULL);
		absx = abs(start.x - now.x);
		absy = abs(start.y - now.y);
		minx = min(start.x, now.x);
		miny = min(start.y, now.y);

		hdctemp = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, absx, absy);
		SelectObject(hdctemp, hBitmap);

		BitBlt(hdctemp, 0, 0, absx, absy, hdc, minx, miny, SRCCOPY);

		OpenClipboard(hwnd);
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, hBitmap);
		CloseClipboard();


		ReleaseCapture();
		DeleteDC(hdctemp);
		DeleteObject(hBitmap);
		ReleaseDC(NULL, hdc);
		DestroyWindow(hwnd);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		hBrushRect = CreateSolidBrush(TransparentColor);
		hBrushFrame = CreateSolidBrush(RGB(255, 255, 255));

		SetRect(&rect, min(start.x, now.x), min(start.y, now.y), max(start.x, now.x), max(start.y, now.y));
		FillRect(hdc, &rect, hBrushRect);
		FrameRect(hdc, &rect, hBrushFrame);
		SetLayeredWindowAttributes(hwnd, TransparentColor, iAlpha, LWA_COLORKEY | LWA_ALPHA);



		DeleteObject(hBrushFrame);
		DeleteObject(hBrushRect);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
