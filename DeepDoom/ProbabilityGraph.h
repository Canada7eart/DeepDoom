#pragma once

#include "FrameFingerprint.h"
#include <memory>
#include <map>
#include "Emulator.h"
#include "FuzzyMap.h"

struct ProbabilityVertex {
	int id;
	FrameFingerprint frame;
	std::multimap<INPUT_KEY, std::pair<double, std::shared_ptr<ProbabilityVertex>>> adj;

	ProbabilityVertex(FrameFingerprint& frame, int id)
		: id(id), frame(frame)
	{}

	bool HasAdjacent(INPUT_KEY key)
	{
		return adj.find(key) != adj.end();
	}
};

using ProbabilityVertexPtr = std::shared_ptr<ProbabilityVertex>;

class ProbabilityGraph
{
public:
	ProbabilityGraph();

	int GetRandomUnmappedAction(ProbabilityVertexPtr vertex);
	double* GetProbabilityForAdjacent(ProbabilityVertexPtr vertex, INPUT_KEY key, ProbabilityVertexPtr adjacent);

	std::vector<int> GetProbable(ProbabilityVertexPtr vertex, INPUT_KEY key);

	ProbabilityVertexPtr AddVertex(FrameFingerprint& frame, ProbabilityVertexPtr previousVertex, INPUT_KEY key, double probability);
	void ConnectVertex(unsigned int V, unsigned int W, INPUT_KEY key, double probability);
	void ConnectFallbackVertex(unsigned int V, INPUT_KEY key, double probability);

	FuzzyMap<FrameFingerprint, int, double>& GetFrequencies() { return frequencies; }
	FuzzyMap<FrameFingerprint, ProbabilityVertexPtr, double>& GetVertexFrames() { return vertexFrames; }

private:
	std::vector<ProbabilityVertexPtr> vertices;

	FuzzyMap<FrameFingerprint, int, double> frequencies{ FrameFingerprint::SIMILARITY_TRESHOLD };
	FuzzyMap<FrameFingerprint, ProbabilityVertexPtr, double> vertexFrames{ FrameFingerprint::SIMILARITY_TRESHOLD };


};

