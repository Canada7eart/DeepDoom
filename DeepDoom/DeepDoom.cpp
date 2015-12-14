// DeepDoom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "Emulator.h"
#include <chrono>



int main(int argc, char** argv)
{
	std::string partialEmulatorName = "DOSBox 0";

	if (argc > 1)
	{
		partialEmulatorName = argv[1];
	}

	BOOL result = SetProcessDPIAware();

	if (result == FALSE)
	{
		std::cout << "Could not make the application DPI-aware, wrong window sizes could be returned.\n";
	}

	Emulator emulator{ partialEmulatorName };

	while (true)
	{
		Sleep(1000);

		//emulator.SendKey(INPUT_CTRL, 60);

		auto start = std::chrono::high_resolution_clock::now();

		cv::Mat frame;
		emulator.GetFrame(frame);

		auto end = std::chrono::high_resolution_clock::now();

		std::cout << "Frame acquired in " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "\n";

		int x = 0;
	}

	return 0;
}

