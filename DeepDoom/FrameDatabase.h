#pragma once
#include "FrameFingerprint.h"
#include <time.h>

class FrameDatabase
{
public:
	FrameDatabase()
	{
		//std::srand(time(NULL));
	}

	FrameFingerprint GetSimilarFrame(const FrameFingerprint& frame) const;
	int AddFrame(FrameFingerprint& frame);

	int GetFrameFrequency(int frameId) { return frequencies[frameId]; }
	void IncreaseFrequency(int frameId) { frequencies[frameId]++; }
	void DecreaseFrequency(int frameId) { frequencies[frameId]--; }

	static bool AreFramesSimilar(const FrameFingerprint& frame1, const FrameFingerprint& frame2, int strictness = 0);

private:
	FrameFingerprint GetSimilarFrame(const FrameFingerprint& frame, const std::vector<FrameFingerprint>& dataset, int strictness) const;

	std::vector<FrameFingerprint> frames;
	std::vector<int> frequencies;

	static std::vector<int> HAMMING_TRESHOLD;
	static std::vector<double> HISTOGRAM_TRESHOLD;
	static std::vector<double> NORM_TRESHOLD;

	static constexpr int MAX_STRICTNESS = 4;
	/*
	// Also seems to work.
	int HAMMING_TRESHOLD = 35;
	double HISTOGRAM_TRESHOLD = 0.10;
	double NORM_TRESHOLD = 0.0950;

	// Seems to work OK.
	int HAMMING_TRESHOLD = 25;
	double HISTOGRAM_TRESHOLD = 0.15;
	double NORM_TRESHOLD = 0.0900; */
};

