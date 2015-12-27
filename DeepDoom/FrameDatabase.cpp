#include "stdafx.h"
#include "FrameDatabase.h"


FrameFingerprint FrameDatabase::GetSimilarFrame(const FrameFingerprint& frame) const
{
	for (const FrameFingerprint& databaseFrame : frames)
	{
		if (abs((double)(frame.fingerprint - databaseFrame.fingerprint)) < NORM_TRESHOLD)
		{
			if (frame.GetHammingDistance(databaseFrame) < HAMMING_TRESHOLD)
			{
				if (cv::compareHist(frame.histogram, databaseFrame.histogram, CV_COMP_BHATTACHARYYA) < HISTOGRAM_TRESHOLD)
				{
					return databaseFrame;
				}
			}
		}
	}

	return FrameFingerprint(0.0);
}

void FrameDatabase::AddFrame(const FrameFingerprint& frame)
{
	frames.push_back(frame);
}
