#include "stdafx.h"
#include "Learner.h"
#include <chrono>
#include "FrameFingerprint.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

const std::array<std::string, NUM_INPUTS> keyDescriptions = { "LEFT", "RIGHT", "UP", "DOWN",   "SPACE", "CTRL" };
std::vector<INPUT_KEY> keys = { INPUT_LEFT , INPUT_RIGHT,INPUT_UP ,INPUT_DOWN,  INPUT_SPACE , INPUT_CTRL };

std::vector<unsigned int> traversed = { 0,0,0,0,0,0,0,0 };
std::vector<bool> predictions;

void Learner::Train()
{
	{
		cv::Mat initialFrame;
		emulator.GetFrame(initialFrame);
		cv::Rect screen(0, 0, emulator.GetWidth(), 335);
		initialFrame = initialFrame(screen);
		cv::cvtColor(initialFrame, initialFrame, CV_BGR2GRAY);
		GaussianBlur(initialFrame, initialFrame, cv::Size(11, 11), 3);

		if (false)
		{
			std::string graphFile = "graphs_05_01/graph_48000.bin";
			probabilityGraph.Load(graphFile);

			FrameFingerprint similar = probabilityGraph.GetFrameDatabase().GetSimilarFrame(FrameFingerprint(initialFrame));

			lastProbabilityVertex = probabilityGraph.GetVertexFromFrame(similar);

			int y = 0;
		}
		else
		{

			lastProbabilityVertex = probabilityGraph.AddVertex(FrameFingerprint(initialFrame), nullptr, INPUT_INVALID, 100.0);
			lastFingerprint = lastProbabilityVertex->frame;
			lastKey = INPUT_INVALID;
		}
	}

	while (true)
	{
		int key = -1;

		//if (lastProbabilityVertex != nullptr && GetRandom(0, 1) == 0)
		//	key = probabilityGraph.GetRandomUnmappedAction(lastProbabilityVertex);

		if (key == -1)
		{
			key = GetRandom(0, 3);
		}


		emulator.SendKey(keys[key], 60, 8, false);
		lastKey = keys[key];

		ProbabilityVertexPtr previous = lastProbabilityVertex;

		Sleep(250);

		std::vector<int> predicted, predictedAny;
		if (previous != nullptr)
		{
			predicted = probabilityGraph.GetProbable(previous, { keys[key] });
		}

		static unsigned long long numFrames = 0;

		if (!ProcessFrame())
		{

		}
		traversed.push_back(lastProbabilityVertex->id);

		numFrames++;


		//	if (numFrames % 1500 == 0)
		if (false)
		{
			std::string graphFile = "graphs_06_01/graph_" + std::to_string(numFrames) + ".bin";
			probabilityGraph.Save(graphFile);
			std::cout << "Saved to file " << graphFile << "\n";

			std::string traversedFile = "graphs_06_01/traversed_" + std::to_string(numFrames) + ".bin";
			SaveTraversedToFile(traversed, traversedFile);

			std::string predictionsFile = "graphs_06_01/predictions_" + std::to_string(numFrames) + ".bin";
			SavePredictionsToFile(predictions, predictionsFile);
		}

		std::cout << "Now at state " << lastProbabilityVertex->id << "\n";

		if (previous != nullptr)
		{
			//predictedAny = probabilityGraph.GetProbable(previous, { INPUT_ANY });
		}

		static std::vector<bool> lastPredictions;
		int nPredictions = predicted.size() + predictedAny.size();
		if (previous != nullptr && lastProbabilityVertex != nullptr && previous->id != lastProbabilityVertex->id && nPredictions > 1)
		{
			bool predictionCorrect = false;
			bool isAccurate = false;

			for (int pred : predicted)
			{
				if (pred == lastProbabilityVertex->id)
				{
					predictionCorrect = true;
					isAccurate = true;
					break;
				}
			}

			if (!predictionCorrect)
			{
				for (int pred : predictedAny)
				{
					if (pred == lastProbabilityVertex->id)
					{
						predictionCorrect = true;
						break;
					}
				}
			}

			if (predictionCorrect)
			{
				lastPredictions.push_back(true);
				predictions.push_back(true);

				double* probability = nullptr;
				if (previous != nullptr)
					probability = probabilityGraph.GetProbabilityForAdjacent(previous, lastKey, lastProbabilityVertex);

				std::cout << "(" << GetPredictionPercentage(lastPredictions) << "%) Prediction CORRECT "
					<< "(n: " << nPredictions << "/" << probabilityGraph.GetNumVertices() << ") "
					<< "from " << previous->id << " to " << lastProbabilityVertex->id
					<< " with probability: ";
				if (!isAccurate || probability == nullptr)
					std::cout << "unknown";
				else
					std::cout << *probability;
				std::cout << "\n";
			}
			else
			{
				lastPredictions.push_back(false);
				predictions.push_back(false);
				std::cout << "(" << GetPredictionPercentage(lastPredictions) << "%) Prediction failed "
					<< "(n: " << nPredictions << "/" << probabilityGraph.GetNumVertices() << ") "
					<< "from " << previous->id << " to " << lastProbabilityVertex->id << "\n";
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
		//probabilityGraph.MultipleConnect(lastProbabilityVertex->id, { traversed.end() - 5, traversed.end() }, 20);
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
					//std::cout << "Connection\n";
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
