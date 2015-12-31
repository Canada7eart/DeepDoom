#include "stdafx.h"
#include "FrameDatabase.h"


FrameFingerprint FrameDatabase::GetSimilarFrame(const FrameFingerprint& frame) const
{
	return GetSimilarFrame(frame, frames, 0);
}

FrameFingerprint FrameDatabase::GetSimilarFrame(const FrameFingerprint& frame,
	const std::vector<FrameFingerprint>& dataset, int strictness) const
{
	std::vector<FrameFingerprint> framesFound;

	for (unsigned int i = 0; i < dataset.size(); ++i)
	{
		const FrameFingerprint& databaseFrame = frames[i];

		//	if (abs((double)(frame.fingerprint - databaseFrame.fingerprint)) < NORM_TRESHOLD[strictness])
		//	{
		if (frame.GetHammingDistance(databaseFrame) < HAMMING_TRESHOLD[strictness])
		{
			if (cv::compareHist(frame.histogram, databaseFrame.histogram, CV_COMP_BHATTACHARYYA) < HISTOGRAM_TRESHOLD[strictness])
			{

				framesFound.push_back(databaseFrame);
			}
		}
		//	}
	}

	std::cout << "Found: " << framesFound.size() << "\n";

	if (framesFound.size() == 1)
	{
		return framesFound[0];
	}
	else if (framesFound.size() > 1)
	{
		if ((strictness + 1) <= MAX_STRICTNESS)
		{
			FrameFingerprint best = GetSimilarFrame(frame, framesFound, strictness + 1);
			if (best.fingerprint == 0.0)
				return framesFound[0];
			else return best;
		}
		else return framesFound[0];
	}

	return FrameFingerprint(0.0);
}

int FrameDatabase::AddFrame(FrameFingerprint& frame)
{
	frame.id = frames.size();
	frames.push_back(frame);
	return frame.id;
}
