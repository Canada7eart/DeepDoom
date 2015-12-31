#pragma once
#include "FrameFingerprint.h"

class FrameDatabase
{
public:
	FrameFingerprint GetSimilarFrame(const FrameFingerprint& frame) const;
	int AddFrame( FrameFingerprint& frame);

private:
	FrameFingerprint GetSimilarFrame(const FrameFingerprint& frame, const std::vector<FrameFingerprint>& dataset, int strictness) const;

	std::vector<FrameFingerprint> frames;

	std::vector<int> HAMMING_TRESHOLD = { 35, 32, 27 };
	std::vector<double> HISTOGRAM_TRESHOLD = { 0.10, 0.09, 0.05 };
	std::vector<double> NORM_TRESHOLD = { 0.0950, 0.0800, 0.0550 };

	const int MAX_STRICTNESS = 2;
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

