#include "stdafx.h"
#include "Emulator.h"

Emulator::Emulator(std::string& partialWindowName)
{
	windowName = partialWindowName;

	EnumWindows(Emulator::EnumWindowsStaticCallback, (LPARAM) this);

	if (window == nullptr)
	{
		std::cerr << "The emulator window could not be found. Make sure the emulator is running in the background.\n";
		exit(-2);
	}

	GetClientRectangle();

	CreateDCBitmap();
}

void Emulator::SendKey(INPUT_KEY key, unsigned long msPressRelease, unsigned int numFrames /*= 1*/, bool setFocus /*= true*/)
{
	// Set up a keyboard event.
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.wVk = key;

	for (unsigned int i = 0; i < numFrames; ++i)
	{
		if (setFocus)
		{
			Focus();
		}

		// Send key press.
		ip.ki.dwFlags = 0;
		SendInput(1, &ip, sizeof(INPUT));

		Sleep(msPressRelease);


	}

	if (setFocus)
	{
		Focus();
	}

	// Send key release.
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void Emulator::GetFrame(cv::Mat& frame)
{
	BitBlt(compatibleHdc, 0, 0, width, height, windowHdc, 0, menuHeight, SRCCOPY);



	unsigned char* pixels = new unsigned char[4 * width * height];

	GetDIBits(windowHdc, bitmapHandle, 0, height, pixels, (LPBITMAPINFO)&bitmapHeader, DIB_RGB_COLORS);

	frame = cv::Mat(cv::Size((int)width, (int)height), CV_8UC4, pixels).clone();
	cv::cvtColor(frame, frame, CV_BGRA2BGR);
	cv::flip(frame, frame, 0);

	/*// Copy to clipboard
	OpenClipboard(NULL);

	EmptyClipboard();
	SetClipboardData(CF_BITMAP, bitmapHandle);
	CloseClipboard(); */

	delete[] pixels;
}

void Emulator::Focus()
{
	SetForegroundWindow(window);
	EnableWindow(window, TRUE);
	SetFocus(window);
}

void Emulator::GetClientRectangle()
{
	RECT rc;
	GetWindowRect(window, &rc);

	// Subtract the menu height. GetClientRect() does not work correctly.
	menuHeight = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION); // +GetSystemMetrics(SM_CXPADDEDBORDER));

	width = rc.right - rc.left;
	height = (rc.bottom - rc.top) - menuHeight;
}

void Emulator::CreateDCBitmap()
{
	windowHdc = GetWindowDC(window);

	compatibleHdc = CreateCompatibleDC(windowHdc);

	if (bitmapHandle == nullptr)
		bitmapHandle = CreateCompatibleBitmap(windowHdc,
			width, height);

	SelectObject(compatibleHdc, bitmapHandle);

	bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapHeader.biPlanes = 1;
	bitmapHeader.biBitCount = 32;
	bitmapHeader.biWidth = width;
	bitmapHeader.biHeight = height;
	bitmapHeader.biCompression = BI_RGB;
}

BOOL Emulator::EnumCallback(__in HWND hWnd)
{
	int length = GetWindowTextLength(hWnd);
	if (0 == length) return TRUE;

	char* buffer;
	buffer = new char[length + 1];
	memset(buffer, 0, (length + 1) * sizeof(char));

	GetWindowText(hWnd, buffer, length + 1);
	std::string windowTitle = buffer;
	delete[] buffer;

	if (windowTitle.substr(0, windowName.size()) == windowName)
	{
		windowName = windowTitle;
		window = hWnd;
		return FALSE; // Stop enumerating.
	}
	return TRUE;
}

BOOL CALLBACK Emulator::EnumWindowsStaticCallback(__in HWND hWnd, __in LPARAM lParam)
{
	Emulator* emulator = (Emulator*)lParam;
	return emulator->EnumCallback(hWnd);
}

Emulator::~Emulator()
{
	if (windowHdc != nullptr)
		ReleaseDC(NULL, windowHdc);

	if (bitmapHandle != nullptr)
		DeleteObject(bitmapHandle);

	if (compatibleHdc != nullptr)
		DeleteDC(compatibleHdc);
}

INPUT_KEY Emulator::GetInverseKey(INPUT_KEY key)
{
	switch (key)
	{
	case INPUT_UP:
		return INPUT_DOWN;
		break;
	case INPUT_DOWN:
		return INPUT_UP;
		break;
	case INPUT_LEFT:
		return INPUT_RIGHT;
		break;
	case INPUT_RIGHT:
		return INPUT_LEFT;
		break;
	default:
		return INPUT_INVALID;
		break;
	}
}

int Emulator::GetKeyIndex(INPUT_KEY key)
{
	switch (key)
	{
	case INPUT_UP:
		return 2;
		break;
	case INPUT_LEFT:
		return 0;
		break;
	case INPUT_RIGHT:
		return 1;
		break;
	case INPUT_DOWN:
		return 3;
		break;
	case INPUT_SPACE:
		return 4;
		break;
	case INPUT_CTRL:
		return 5;
		break;
	default:
		return -1;
		break;
	}
}
