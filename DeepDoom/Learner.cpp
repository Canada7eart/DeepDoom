#include "stdafx.h"
#include "Learner.h"
#include <chrono>
#include "FrameFingerprint.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

const std::array<std::string, NUM_INPUTS> keyDescriptions = { "LEFT", "RIGHT", "UP", "DOWN",   "SPACE", "CTRL" };
std::vector<INPUT_KEY> keys = { INPUT_LEFT , INPUT_RIGHT,INPUT_UP ,INPUT_DOWN,  INPUT_SPACE , INPUT_CTRL };

void Learner::Train()
{
	{
		cv::Mat initialFrame;
		emulator.GetFrame(initialFrame);
		cv::Rect screen(0, 0, emulator.GetWidth(), 335);
		initialFrame = initialFrame(screen);
		cv::cvtColor(initialFrame, initialFrame, CV_BGR2GRAY);


		lastProbabilityVertex = probabilityGraph.AddVertex(FrameFingerprint(initialFrame), nullptr, INPUT_INVALID, 100.0);
		lastFingerprint = lastProbabilityVertex->frame;
		lastKey = INPUT_INVALID;
	}

	while (true)
	{
		int key = -1;

		//if (GetRandom(0, 1) == 0)
		//	key = probabilityGraph.GetRandomUnmappedAction(lastProbabilityVertex);

		if (key == -1)
		{
			key = GetRandom(0, 1);
		}

		std::vector<int> predicted;
		if (lastProbabilityVertex != nullptr)
			predicted = probabilityGraph.GetProbable(lastProbabilityVertex, keys[key]);

		emulator.SendKey(keys[key], 60, 8, false);
		lastKey = keys[key];

		ProbabilityVertexPtr previous = lastProbabilityVertex;

		Sleep(200);

		if (!ProcessFrame())
		{

		}

		//std::cout << "Predicted size: " << predicted.size() << "\n";

		static std::vector<bool> lastPredictions;
		if (lastProbabilityVertex != nullptr && predicted.size() > 0)
		{
			bool predictionCorrect = false;
			for (int pred : predicted)
			{
				if (pred == lastProbabilityVertex->id)
				{
					predictionCorrect = true;
					break;
				}
			}
			if (predictionCorrect)
			{
				lastPredictions.push_back(true);

				double* probability = nullptr;
				if (previous != nullptr)
					probability = probabilityGraph.GetProbabilityForAdjacent(previous, lastKey, lastProbabilityVertex);

				std::cout << "(" << GetPredictionPercentage(lastPredictions) << "%) Prediction CORRECT (n: " << predicted.size() << ") "
					<< "from " << previous->id << " to " << lastProbabilityVertex->id << " with probability: " << *probability << "\n";
			}
			else
			{
				lastPredictions.push_back(false);
				std::cout << "(" << GetPredictionPercentage(lastPredictions) << "%) Prediction failed (n: " << predicted.size() << ")\n";
			}

			if (lastPredictions.size() == 10)
				lastPredictions.erase(lastPredictions.begin());

		}

	}
}

bool Learner::ProcessFrame()
{
	cv::Mat frame;
	emulator.GetFrame(frame);

	cv::Rect screen(0, 0, emulator.GetWidth(), 335);
	frame = frame(screen);
	cv::cvtColor(frame, frame, CV_BGR2GRAY);
	GaussianBlur(frame, frame, cv::Size(11, 11), 3);

	FrameFingerprint frameFingerprint{ frame };

	probabilityGraph.GetFrequencies()[frameFingerprint]++;

	FrameFingerprint similar = probabilityGraph.GetFrameDatabase().GetSimilarFrame(frameFingerprint);

	bool hasMoved = false;

	int frequency = probabilityGraph.GetFrequencies()[frameFingerprint];

	//imshow("Frame", frame);

	//cv::waitKey(30);

	static int totalEnabled = 0;

	if (!similar.isValid()) // New frame.
	{
		probabilityGraph.GetFrameDatabase().AddFrame(frameFingerprint);
		lastProbabilityVertex = probabilityGraph.AddDisabledVertex(frameFingerprint);
		hasMoved = true;

		std::cout << "New state (" << lastProbabilityVertex->id << ")\n";
	}
	else
	{
		probabilityGraph.GetFrameDatabase().IncreaseFrequency(similar.id);
		int frameFrequency = probabilityGraph.GetFrameDatabase().GetFrameFrequency(similar.id);

		ProbabilityVertexPtr vertex = probabilityGraph.GetVertexFromFrame(similar);
		double* probability = probabilityGraph.GetProbabilityForAdjacent(lastProbabilityVertex, lastKey, vertex);

		bool isSimilarToLast = FrameDatabase::AreFramesSimilar(frameFingerprint, vertex->frame, 2);
		isSimilarToLast = false;


		if (frameFrequency >= 1)
		{
			if (isSimilarToLast)
			{
				if (probability == nullptr)
				{
					//	std::cout << "Fallback\n";
					probabilityGraph.ConnectFallbackVertex(lastProbabilityVertex->id, lastKey, 40);
				}
				else
					*probability *= 1.2;
			}
			else
			{
				if (probability == nullptr)
				{
					//	std::cout << "Connection\n";
					probabilityGraph.ConnectVertex(lastProbabilityVertex->id, vertex->id, lastKey, 60);
				}
				else
					*probability *= 1.3;

				hasMoved = true;
			}
		}

		lastProbabilityVertex = vertex;
	}

	lastFingerprint = lastProbabilityVertex->frame;
	return hasMoved;
}

int Learner::GetRandom(int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(random);
}
