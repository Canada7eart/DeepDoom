#include "stdafx.h"
#include <numeric>
#include <fstream>

using cv::Mat;
using cv::Size;

void SaveTraversedToFile(const std::vector<unsigned int> vertices, const std::string& filename)
{
	std::ofstream file(filename, std::ios::out | std::ios::trunc | std::ios::binary);

	if (!file.is_open())
	{
		return;
	}

	for (const unsigned int& vertex : vertices)
	{
		file.write((const char*)&vertex, sizeof(vertex));
	}

	file.close();
}

void SavePredictionsToFile(const std::vector<bool> predictions, const std::string& filename)
{
	std::ofstream file(filename, std::ios::out | std::ios::trunc | std::ios::binary);

	if (!file.is_open())
	{
		return;
	}

	for (const bool& pred : predictions)
	{
		file.write((const char*)&pred, sizeof(pred));
	}

	file.close();
}

int GetPredictionPercentage(std::vector<bool>& predictions)
{
	int numCorrect = 0;
	for (unsigned int i = 0; i < predictions.size(); ++i)
	{
		if (predictions[i] == true)
			numCorrect++;
	}

	return numCorrect * 100 / predictions.size();
}

uint64_t CalculateHash(Mat& img)
{
	Mat temp;

	GaussianBlur(img, temp, Size(3, 3), 0, 0);
	resize(temp, temp, cv::Size(8, 8));

	temp.convertTo(temp, CV_32F);

	float* pixelData = (float*)temp.data;

	float sum = std::accumulate(pixelData, pixelData + 64, 0.0f);

	float average = sum / 64;

	uint64_t hash = 0;

	for (int i = 0; i < 64; ++i)
	{
		if (pixelData[i] >= average)
		{
			hash |= ((uint64_t)1 << i);
		}
	}

	return hash;

}

uint64_t CalculatePHash(Mat& img)
{
	Mat temp;

	resize(img, temp, cv::Size(32, 32));

	temp.convertTo(temp, CV_32F);

	Mat dctResult;
	dct(temp, dctResult);

	// imshow("DCT", dctResult);

	float* dctData = new float[8 * 8];

	for (int i = 0; i < 8; ++i)
	{
		memcpy(dctData + i * 8, dctResult.data + i * dctResult.step, sizeof(float) * 8);
	}

	float sum = std::accumulate(dctData + 1, dctData + 64, 0.0f);

	float average = sum / 64;

	uint64_t pHash = 0;

	//cout << "BITS: ";
	int numAbove = 0, numBelow = 0;
	for (int i = 0; i < 64; ++i)
	{


		if (dctData[i] >= average)
		{
			pHash |= ((uint64_t)1 << i);
			numAbove++;
			//cout << "1";
		}
		else {
			numBelow++;
			//cout << "0";
		}

		/*	if (i == 31)
		cout << " "; */
	}
	//cout << "\n";

	delete[] dctData;

	return pHash;
}