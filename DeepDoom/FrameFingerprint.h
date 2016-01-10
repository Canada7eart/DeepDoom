#pragma once

#include <opencv2/core.hpp>
#include "Utilities.h"

class FrameFingerprint
{
public:

	FrameFingerprint(cv::Mat& frame)
	{
		static int channels[] = { 0 };
		static int histSize[] = { 16 };
		static float range[] = { 0, 256 };
		static const float* ranges[] = { range };

		calcHist(&frame, 1, channels, cv::Mat(),
			histogram, 1, histSize, ranges,
			true,
			false);

		histogram = histogram / (frame.rows * frame.cols);

		fingerprint = cv::norm(frame, CV_L2) / ((double)frame.rows * (double)frame.cols);
		pHash = CalculatePHash(frame);
	}

	FrameFingerprint(double fingerprint, uint64_t pHash, cv::Mat histogram)
		: fingerprint(fingerprint), pHash(pHash), histogram(histogram)
	{}

	FrameFingerprint(double fp)
	{
		fingerprint = fp;
	}

	int GetHammingDistance(const FrameFingerprint & other) const {
		uint64_t difference = pHash ^ other.pHash;
		int distance = __popcnt((unsigned int)difference) + __popcnt((unsigned int)(difference >> 32));
		return distance;
	}

	bool operator<(const FrameFingerprint &other) const {
		return (fingerprint - other.fingerprint) < 0.0;
	}
	bool operator>(const FrameFingerprint &other) const {
		return (fingerprint - other.fingerprint) > 0.0;
	}

	bool isValid() const { return fingerprint != 0.0; }

	bool isNormSimilar(const FrameFingerprint& other) const {
		return (abs((double)(fingerprint - other.fingerprint)) < SIMILARITY_TRESHOLD);
	}

	friend FrameFingerprint operator-(const FrameFingerprint& one, const FrameFingerprint& two);
	friend FrameFingerprint operator+(const FrameFingerprint& one, const FrameFingerprint& two);
	friend FrameFingerprint operator-(const FrameFingerprint& one, const double& two);
	friend FrameFingerprint operator+(const FrameFingerprint& one, const double& two);

	double fingerprint;
	uint64_t pHash;
	cv::Mat histogram;

	unsigned int id;
	unsigned int groupId;

	//static constexpr double SIMILARITY_TRESHOLD = 0.00190;
	static constexpr double SIMILARITY_TRESHOLD = 0.0250;

private:

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