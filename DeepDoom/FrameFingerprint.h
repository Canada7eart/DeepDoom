#pragma once

#include <opencv2/core.hpp>

class FrameFingerprint
{
public:

	FrameFingerprint(cv::Mat& frame)
	{
		fingerprint = cv::norm(frame, CV_L2) / ((double)frame.rows * (double)frame.cols);
	}

	bool operator==(const FrameFingerprint &other) const {
		if (abs(fingerprint - other.fingerprint) < SIMILARITY_TRESHOLD)
			return true;
		else return false;
	}

	double fingerprint;

private:
	static constexpr double SIMILARITY_TRESHOLD = 0.00900;
};

namespace std
{
	template <>
	struct hash<FrameFingerprint>
	{
		size_t operator()(FrameFingerprint const & fp) const noexcept
		{
			return std::hash<double>()(fp.fingerprint);
		}
	};
}

class Test
{
public:

	Test(int arg)
	{
		x = arg;
	}

	bool operator==(const Test& other) const {
		if (abs(x - other.x) < FUZZY_EQUAL)
			return true;
		else return false;
	}

	int x;

private:
	static const int FUZZY_EQUAL = 5;
};