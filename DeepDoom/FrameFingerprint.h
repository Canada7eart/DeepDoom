#pragma once

#include <opencv2/core.hpp>

class FrameFingerprint
{
public:

	FrameFingerprint(cv::Mat& frame)
	{
		fingerprint = cv::norm(frame, CV_L2) / ((double)frame.rows * (double)frame.cols);
	}

	FrameFingerprint(double fp)
	{
		fingerprint = fp;
	}

	bool operator==(const FrameFingerprint &other) const {
		return fingerprint == other.fingerprint;
	}
	bool operator<(const FrameFingerprint &other) const {
		return (fingerprint - other.fingerprint) < 0.0;
	}
	bool operator>(const FrameFingerprint &other) const {
		return (fingerprint - other.fingerprint) > 0.0;
	}

	bool isSimilar(const FrameFingerprint& other) const {
		return abs(fingerprint - other.fingerprint) < SIMILARITY_TRESHOLD;
	}

	friend FrameFingerprint operator-(const FrameFingerprint& one, const FrameFingerprint& two);
	friend FrameFingerprint operator+(const FrameFingerprint& one, const FrameFingerprint& two);
	friend FrameFingerprint operator-(const FrameFingerprint& one, const double& two);
	friend FrameFingerprint operator+(const FrameFingerprint& one, const double& two);

	double fingerprint;

	static constexpr double SIMILARITY_TRESHOLD = 0.00200;

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