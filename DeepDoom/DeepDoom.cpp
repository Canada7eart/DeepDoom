// DeepDoom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "Emulator.h"
#include <chrono>
#include "FrameFingerprint.h"
#include <map>
#include "FuzzyMap.h"
#include "Learner.h"

int main(int argc, char** argv)
{
/*	FuzzyMap<FrameFingerprint, int, double> test{ FrameFingerprint::SIMILARITY_TRESHOLD };

	test[FrameFingerprint(0.085951950804441610)] = 1;
	test[FrameFingerprint(0.083057393916518105)] = 2;
	test[FrameFingerprint(0.090380971665016532)] = 3;

	int x = test[FrameFingerprint(0.082161949313406077)];

	int y = 0;

	return 0; */

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
	emulator.Focus();

	Learner learner{ emulator };
	learner.Train();

	return 0;
}

