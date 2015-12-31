#pragma once
#include "Emulator.h"
#include "LevelGraph.h"
#include "FuzzyMap.h"
#include <random>
#include <ctime>
#include "ProbabilityGraph.h"
#include "FrameDatabase.h"

class Learner
{
public:
	Learner(Emulator& emulator)
		: emulator(emulator)
	{
		random = std::mt19937{ (unsigned int)time(NULL) };
	}

	void Train();

private:
	bool ProcessFrame();

	int GetRandom(int min, int max);


	LevelGraph level;
	LevelVertexPtr lastVertex = nullptr;
	ProbabilityGraph probabilityGraph;
	ProbabilityVertexPtr lastProbabilityVertex = nullptr;
	FrameDatabase frames;

	INPUT_KEY lastKey = INPUT_INVALID;
	FrameFingerprint lastFingerprint = FrameFingerprint(0.0);

	std::mt19937 random;

	Emulator emulator;
};

