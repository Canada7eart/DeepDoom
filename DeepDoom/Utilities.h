#pragma once
#include <stdint.h>

int GetPredictionPercentage(std::vector<bool>& predictions);

// Image hashing.
uint64_t CalculateHash(cv::Mat& img);
uint64_t CalculatePHash(cv::Mat& img);