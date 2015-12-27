#pragma once
#include <stdint.h>

// Image hashing.
uint64_t CalculateHash(cv::Mat& img);
uint64_t CalculatePHash(cv::Mat& img);