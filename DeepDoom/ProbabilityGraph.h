#pragma once

#include "FrameFingerprint.h"
#include <memory>
#include <map>
#include "Emulator.h"
#include "FuzzyMap.h"
#include "FrameDatabase.h"

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
	ProbabilityGraph()
	{
	}

	int GetRandomUnmappedAction(ProbabilityVertexPtr vertex);
	double* GetProbabilityForAdjacent(ProbabilityVertexPtr vertex, INPUT_KEY key, ProbabilityVertexPtr adjacent);

	std::vector<int> GetProbable(ProbabilityVertexPtr vertex, std::vector<INPUT_KEY> keys);

	unsigned int GetGroupId(unsigned int V) { return vertices[V]->frame.groupId; }

	ProbabilityVertexPtr AddDisabledVertex(FrameFingerprint& frame);
	ProbabilityVertexPtr AddVertex(FrameFingerprint& frame, ProbabilityVertexPtr previousVertex, INPUT_KEY key, double probability);
	void ConnectVertex(unsigned int V, unsigned int W, INPUT_KEY key, double probability);
	void ConnectFallbackVertex(unsigned int V, INPUT_KEY key, double probability);
	void MultipleConnect(unsigned int V, std::vector<unsigned int> other, double probability);

	FuzzyMap<FrameFingerprint, int, double>& GetFrequencies() { return frequencies; }
	FuzzyMap<FrameFingerprint, ProbabilityVertexPtr, double>& GetVertexFrames() { return vertexFrames; }
	ProbabilityVertexPtr GetVertexFromFrame(FrameFingerprint& frame) { return frameVertices[frame]; }

	FrameDatabase& GetFrameDatabase() { return frames; }
	unsigned int GetNumVertices() { return vertices.size(); }

	void Save(const std::string& filename);
	void Load(const std::string& filename);

private:
	std::vector<ProbabilityVertexPtr> vertices;

	FuzzyMap<FrameFingerprint, int, double> frequencies{ FrameFingerprint::SIMILARITY_TRESHOLD };
	FuzzyMap<FrameFingerprint, ProbabilityVertexPtr, double> vertexFrames{ FrameFingerprint::SIMILARITY_TRESHOLD };

	std::map<FrameFingerprint, ProbabilityVertexPtr> frameVertices;
	FrameDatabase frames;
};

