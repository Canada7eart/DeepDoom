#include "stdafx.h"
#include "FrameDatabase.h"

std::vector<int> FrameDatabase::HAMMING_TRESHOLD = { 36, 33, 27 };
std::vector<double> FrameDatabase::HISTOGRAM_TRESHOLD = { 0.12, 0.10, 0.06 };
std::vector<double>  FrameDatabase::NORM_TRESHOLD = { 0.0950, 0.0800, 0.0550 };


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

	//std::cout << "Found: " << framesFound.size() << "\n";

	if (framesFound.size() == 1)
	{
		return framesFound[0];
	}
	else if (framesFound.size() > 1)
	{
		if ((strictness + 1) <= MAX_STRICTNESS)
		{
			FrameFingerprint best = GetSimilarFrame(frame, framesFound, strictness + 1);
			if (!best.isValid())
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
	frequencies.push_back(1);
	return frame.id;
}

bool FrameDatabase::AreFramesSimilar(const FrameFingerprint& frame1, const FrameFingerprint& frame2, int strictness /* = 0 */)
{
	//if (abs((double)(frame1.fingerprint - frame2.fingerprint)) < NORM_TRESHOLD[strictness])
	//{
	if (frame1.GetHammingDistance(frame2) < HAMMING_TRESHOLD[strictness])
	{
		if (cv::compareHist(frame1.histogram, frame2.histogram, CV_COMP_BHATTACHARYYA) < HISTOGRAM_TRESHOLD[strictness])
		{
			return true;
		}
	}
	//}

	return false;
}
