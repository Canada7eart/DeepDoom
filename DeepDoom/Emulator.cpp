#include "stdafx.h"
#include "Emulator.h"

const std::array<std::string, NUM_INPUTS> keyDescriptions = { "UP", "DOWN", "LEFT", "RIGHT", "SPACE", "CTRL" };

Emulator::Emulator(std::string& partialWindowName)
{
	EnumWindows(Emulator::EnumWindowsStaticCallback, (LPARAM) this);

	if (window == nullptr)
	{
		std::cerr << "The emulator window could not be found. Make sure the emulator is running in the background.\n";
		exit(-2);
	}

	GetClientRectangle();
}


void Emulator::SendKey(INPUT_KEY key, unsigned long msPressRelease, bool setFocus /*= true*/)
{
	// Set up a keyboard event.
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.wVk = key;

	// Send key press.
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(msPressRelease);

	// Send key release.
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void Emulator::GetClientRectangle()
{
	RECT rc;
	GetWindowRect(window, &rc);

	// Subtract the menu height. GetClientRect() does not work correctly.
	int menuHeight = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION); // +GetSystemMetrics(SM_CXPADDEDBORDER));

	width = rc.right - rc.left;
	height = (rc.bottom - rc.top) - menuHeight;
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

