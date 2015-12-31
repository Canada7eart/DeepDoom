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
		frames.AddFrame(FrameFingerprint(initialFrame));

		lastProbabilityVertex = probabilityGraph.AddVertex(FrameFingerprint(initialFrame), nullptr, INPUT_INVALID, 100.0);
		lastFingerprint = lastProbabilityVertex->frame;
		lastKey = INPUT_INVALID;
	}

	while (true)
	{
		int key = -1;

		//	if (GetRandom(0, 1) == 0)
			//	key = probabilityGraph.GetRandomUnmappedAction(lastProbabilityVertex);

		if (key == -1)
		{
			key = GetRandom(0, 1);
		}

		std::vector<int> predicted;
		if (lastProbabilityVertex != nullptr)
			predicted = probabilityGraph.GetProbable(lastProbabilityVertex, keys[key]);

		emulator.SendKey(keys[key], 60, 10, false);
		lastKey = keys[key];

		ProbabilityVertexPtr previous = lastProbabilityVertex;

		Sleep(250);

		if (!ProcessFrame())
		{

		}

		if (lastProbabilityVertex != nullptr && predicted.size() > 1)
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
				double* probability = nullptr;
				if (previous != nullptr)
					probability = probabilityGraph.GetProbabilityForAdjacent(previous, lastKey, lastProbabilityVertex);

				//	std::cout << "Prediction CORRECT (n: " << predicted.size() << ") with probability: " << *probability << "\n";
			}
			else
			{
				//	std::cout << "Prediction failed (n: " << predicted.size() << ")\n";
			}
		}

	}



	/*
	bool hasMoved = true;
	bool triedInverse = false;

	int key = INPUT_INVALID;
	int keyRetries = 0;
	const int MAX_RETRIES = 3;

	const bool loadGraph = false;

	if (loadGraph) {

		level.LoadFromFile("graph_2.bin");
		lastVertex = level.GetVertex(0);
		lastFingerprint = lastVertex->frame;
	}

	unsigned long long numFrames = 0;

	LevelVertexPtr predicted = nullptr;
	LevelVertexPtr previous = nullptr;
	bool isPredicted = false;

	while (true)
	{
		if (hasMoved)
		{
			if (lastVertex != nullptr)
			{
				key = level.GetRandomUnmappedAction(lastVertex);
				if (key != -1)
					std::cout << "Unmapped key: " << keyDescriptions[key] << "\n";
			}

			if (key == -1)
			{
				//	std::cout << "Searching unmapped path.\n";

			//	if (loadGraph)
			//	{
				if (lastVertex != nullptr)
				{
					key = level.GetFirstUnmappedPath(lastVertex);

					if (key == -1)
					{
						key = GetRandom(0, 1);

						std::cout << "Random key: " << key << "\n";
						if (lastVertex != nullptr)
							std::cout << "No unmapped action at vertex " << lastVertex->id << "\n";
					}
					else
					{
						std::cout << "Unmapped path: " << keyDescriptions[key] << "\n";

						if (key == -1)
						{
							for (int i = 0; i < 5; ++i)
								std::cout << "ERROR\n";
						}
					}
				}
				else
				{

					key = GetRandom(0, 1);

					std::cout << "Random key: " << key << "\n";
					if (lastVertex != nullptr)
						std::cout << "No unmapped action at vertex " << lastVertex->id << "\n";
				}
			}
			else
			{
				//std::cout << "Unmapped key: " << keyDescriptions[key] << "\n";
			}
		}
		else
		{
			// Repeat last key.
			std::cout << "Repeating " << keyDescriptions[key] << "\n";
		}

		if (lastVertex != nullptr && lastVertex->adj.find(keys[key]) != lastVertex->adj.end())
		{
			if (lastVertex->adj[keys[key]] != nullptr)
			{
				previous = lastVertex;
				predicted = lastVertex->adj[keys[key]];
				isPredicted = true;
			}
		}

		emulator.SendKey(keys[key], 60, 4, false);

		if (lastVertex != nullptr)
			std::cout << "Sent keystroke " << keyDescriptions[key] << " at vertex " << lastVertex->id << "\n";

		//Sleep(1500);

		lastKey = keys[key];

		//Sleep(60 * 4);

		if (!ProcessFrame())
		{
			hasMoved = false;
			keyRetries++;
			if (keyRetries > MAX_RETRIES)
			{
				keyRetries = 0;

				if (triedInverse)
				{
					hasMoved = true;
				}
				else
				{
					INPUT_KEY inverse = Emulator::GetInverseKey(keys[key]);
					if (inverse != INPUT_INVALID)
					{
						triedInverse = true;
						auto id = std::find(keys.begin(), keys.end(), inverse);
						key = std::distance(keys.begin(), id);
						std::cout << "Trying inverse key " << keyDescriptions[key] << "\n";
					}
					else
						hasMoved = true;
				}
			}
		}
		else
		{
			triedInverse = false;
			keyRetries = 0;
			hasMoved = true;
		}

		if (isPredicted)
		{
			if (lastVertex->id != predicted->id)
			{
				std::cout << "Prediction failed (should be " << predicted->id << ", was " << lastVertex->id << ")\n";
				if (previous->id != lastVertex->id)
					level.ConnectVertex(previous->id, lastVertex->id, keys[key]);
				else
					level.ConnectFallbackVertex(previous->id, keys[key]);
			}
			else
			{
				std::cout << "Prediction CORRECT.\n";
			}

			isPredicted = false;
		}

		numFrames++;

		if (numFrames % 100 == 0)
		{
			std::cout << "Saving to file.\n";
			level.SaveToFile("graph_2.bin");
			numFrames = 0;
		}


	} */
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

	bool hasMoved = false;

	int frequency = probabilityGraph.GetFrequencies()[frameFingerprint];

	unsigned int id = 0;
	FrameFingerprint database = frames.GetSimilarFrame(frameFingerprint);

	if (database.fingerprint > 0)
	{
		id = database.id;
		if (frequency == 1)
		{
			std::cout << "Found frame in database (" << id << ") but it's supposed to be new\n";
		}
		else
		{
			std::cout << "Found frame in database (" << id << ")\n";
		}
	}
	else
	{
		int newFrameId = frames.AddFrame(frameFingerprint);
		std::cout << "Frame ADDED to database (" << newFrameId << ")\n";
	}

	imshow("Frame", frame);

	cv::waitKey(30);

	static int totalEnabled = 0;

	if (frequency == 1)
	{
		lastProbabilityVertex = probabilityGraph.AddDisabledVertex(frameFingerprint);
		probabilityGraph.GetVertexFrames()[frameFingerprint] = lastProbabilityVertex;
		hasMoved = true;
		//std::cout << "New disabled state (" << lastProbabilityVertex->id << "), fingerprint: " << frameFingerprint.fingerprint << "\n";
	}
	else if (frequency >= 3)
	{
		ProbabilityVertexPtr newVertex = probabilityGraph.GetVertexFrames()[frameFingerprint];

		if (frequency == 3)
		{
			totalEnabled++;
			//std::cout << "Vertex " << newVertex->id << " enabled (total: " << totalEnabled << ")\n";
		}

		bool similar = lastFingerprint.isNormSimilar(frameFingerprint);

		double* probability = probabilityGraph.GetProbabilityForAdjacent(lastProbabilityVertex, lastKey, newVertex);

		if (similar)
		{
			if (probability == nullptr)
			{
				probabilityGraph.ConnectFallbackVertex(lastProbabilityVertex->id, lastKey, 30.0);
			}
			else
			{
				*probability *= 1.2;
			}
		}
		else
		{
			if (probability == nullptr)
			{
				probabilityGraph.ConnectVertex(lastProbabilityVertex->id, newVertex->id, lastKey, 40.0);
			}
			else
			{
				*probability *= 1.3;
			}
			hasMoved = true;
		}

		lastProbabilityVertex = newVertex;
	}

	lastFingerprint = lastProbabilityVertex->frame;
	return hasMoved;

	/*auto start = std::chrono::high_resolution_clock::now();

	cv::Mat frame;
	emulator.GetFrame(frame);

	cv::Rect screen(0, 0, emulator.GetWidth(), 335);
	frame = frame(screen);
	cv::cvtColor(frame, frame, CV_BGR2GRAY);

	FrameFingerprint frameFingerprint{ frame };

	auto end = std::chrono::high_resolution_clock::now();

	//std::cout << "Frame acquired in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";
	//std::cout << "Fingerprint: " << frameFingerprint.fingerprint << "\n";

	level.GetFrequencies()[frameFingerprint]++;

	//std::cout << "Frequency (map): " << frequencies[frameFingerprint] << "\n";

	bool hasMoved = false;

	if (level.GetFrequencies()[frameFingerprint] == 1)
	{
		lastVertex = level.AddVertex(frameFingerprint, lastVertex, lastKey);
		level.GetVertexFrames()[frameFingerprint] = lastVertex;

		std::cout << "New state (" << (lastVertex->id + 1) << ")\n";

		hasMoved = true;
	}
	else
	{
		LevelVertexPtr newVertex = level.GetVertexFrames()[frameFingerprint];


		bool similar = lastFingerprint.isSimilar(frameFingerprint);

		if (similar) // We did't move a lot (or at all), roughly the same frame.
		{

			if (!lastVertex->HasAdjacent(lastKey))
				level.AddFallbackVertex(lastVertex, lastKey);
			hasMoved = false;
			//	std::cout << "No effect.\n";
		}
		else // We arrived at a state we already knew.
		{
			level.ConnectVertex(lastVertex->id, newVertex->id, lastKey);
			hasMoved = true;
		}

		lastVertex = newVertex;
	}

	lastFingerprint = lastVertex->frame;
	return hasMoved; */
}

int Learner::GetRandom(int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(random);
}
