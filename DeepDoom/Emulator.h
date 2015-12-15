#pragma once
#include <array>

constexpr int NUM_INPUTS = 6;

enum INPUT_KEY {
	INPUT_UP = VK_UP, INPUT_DOWN = VK_DOWN, INPUT_LEFT = VK_LEFT, INPUT_RIGHT = VK_RIGHT,
	INPUT_SPACE = VK_SPACE, INPUT_CTRL = VK_CONTROL
};

class Emulator
{
public:
	Emulator(std::string& partialWindowName);

	~Emulator();

	void SendKey(INPUT_KEY key, unsigned long msPressRelease, bool setFocus = true);
	void GetFrame(cv::Mat& frame);

	unsigned int GetWidth() { return width; }
	unsigned int GetHeight() { return height; }

	void Focus();

private:
	BOOL EnumCallback(__in HWND hWnd);
	static BOOL CALLBACK EnumWindowsStaticCallback(__in  HWND hWnd, __in  LPARAM lParam);

	void GetClientRectangle();
	void CreateDCBitmap();

	HWND window = nullptr;
	HDC windowHdc = nullptr;
	HDC compatibleHdc = nullptr;
	HBITMAP bitmapHandle = nullptr;
	BITMAPINFOHEADER bitmapHeader = { 0 };

	std::string windowName;

	unsigned int width = 0, height = 0, menuHeight = 0;
};

