#pragma once
#include "FrameFingerprint.h"

class FrameDatabase
{
public:
	FrameFingerprint GetSimilarFrame(const FrameFingerprint& frame) const;
	void AddFrame(const FrameFingerprint& frame);

private:
	std::vector<FrameFingerprint> frames;

	int HAMMING_TRESHOLD = 20;
	double HISTOGRAM_TRESHOLD = 0.05;
	double NORM_TRESHOLD = 0.0250;
};

