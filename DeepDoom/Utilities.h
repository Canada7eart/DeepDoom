#pragma once
#include <stdint.h>

void SaveTraversedToFile(const std::vector<unsigned int> vertices, const std::string& filename);
void SavePredictionsToFile(const std::vector<bool> predictions, const std::string& filename);
int GetPredictionPercentage(std::vector<bool>& predictions);

// Image hashing.
uint64_t CalculateHash(cv::Mat& img);
uint64_t CalculatePHash(cv::Mat& img);