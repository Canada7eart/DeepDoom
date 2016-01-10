#pragma once
#include <array>

constexpr int NUM_INPUTS = 6;

//#define USE_STRAFE

#ifndef USE_STRAFE
#define LEFT_KEY VK_LEFT
#define RIGHT_KEY VK_RIGHT
#else
#define LEFT_KEY 0x41 // "A"
#define RIGHT_KEY 0x44 // "D"
#endif

enum INPUT_KEY {
	INPUT_UP = VK_UP, INPUT_DOWN = VK_DOWN, INPUT_LEFT = LEFT_KEY, INPUT_RIGHT = RIGHT_KEY,
	INPUT_SPACE = VK_SPACE, INPUT_CTRL = VK_CONTROL, INPUT_ANY, INPUT_INVALID = -1
};

class Emulator
{
public:
	Emulator(std::string& partialWindowName);

	~Emulator();

	void SendKey(INPUT_KEY key, unsigned long msPressRelease, unsigned int numFrames = 1, bool setFocus = true);
	void GetFrame(cv::Mat& frame);

	unsigned int GetWidth() { return width; }
	unsigned int GetHeight() { return height; }

	void Focus();

	static INPUT_KEY GetInverseKey(INPUT_KEY key);
	static int GetKeyIndex(INPUT_KEY key);

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

