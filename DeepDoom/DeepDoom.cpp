// DeepDoom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "Emulator.h"
#include <chrono>
#include "FrameFingerprint.h"
#include <unordered_map>
#include <map>

const std::array<std::string, NUM_INPUTS> keyDescriptions = { "UP", "DOWN", "LEFT", "RIGHT", "SPACE", "CTRL" };

std::vector<FrameFingerprint> fingerprints;
std::map<int, int> fingerprintFrequencies;

void ProcessFrame(Emulator& emulator)
{
	auto start = std::chrono::high_resolution_clock::now();

	cv::Mat frame;
	emulator.GetFrame(frame);

	cv::Rect screen(0, 0, emulator.GetWidth(),  335);
	frame = frame(screen);
	cv::cvtColor(frame, frame, CV_BGR2GRAY);

	FrameFingerprint frameFingerprint{ frame };

	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Frame acquired in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";
	std::cout << "Fingerprint: " << frameFingerprint.fingerprint << "\n";

	auto found = std::find(fingerprints.begin(), fingerprints.end(), frameFingerprint);
	int i = 0;
	if (found != fingerprints.end())
	{
		i = std::distance(fingerprints.begin(), found);
		fingerprintFrequencies[i] ++;
	}
	else {
		fingerprints.push_back(frameFingerprint);
		fingerprintFrequencies[i = fingerprints.size() - 1] ++;
	}



	std::cout << "Frequency: " << fingerprintFrequencies[i] << "\n";
}


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

	std::vector<INPUT_KEY> keys = { INPUT_UP , INPUT_DOWN , INPUT_LEFT , INPUT_RIGHT,
		INPUT_SPACE , INPUT_CTRL };

	srand((unsigned int)time(NULL));

	emulator.Focus();

	while (true)
	{
		//Sleep(60);

		ProcessFrame(emulator);

		Sleep(2000);

		int key = 0;
		/* key = rand() % keys.size(); */

		static int invert = 0;
		if (invert == 0)
		{
			key = 0;
			invert++;
		}
		else
		{
			key = 1;
			invert--;
		}


		//emulator.SendKey(keys[key], 60);
		std::cout << "Sent keystroke " << keyDescriptions[key] << "\n";

		//Sleep(60);
	}

	return 0;
}

