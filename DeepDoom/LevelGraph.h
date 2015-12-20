#pragma once
#include "FrameFingerprint.h"
#include <memory>
#include <map>
#include "Emulator.h"
#include "FuzzyMap.h"

struct LevelVertex {
	int id;
	FrameFingerprint frame;
	std::map<INPUT_KEY, std::shared_ptr<LevelVertex>> adj;

	LevelVertex(FrameFingerprint& frame, int id)
		: id(id), frame(frame)
	{}

	bool HasAdjacent(INPUT_KEY key)
	{
		return adj.find(key) != adj.end();
	}

	bool IsFallback(INPUT_KEY key)
	{
		return adj[key] == nullptr;
	}
};

using LevelVertexPtr = std::shared_ptr<LevelVertex>;

class LevelGraph
{
public:
	LevelGraph() {}
	LevelGraph(FrameFingerprint& rootFrame);

	int GetRandomUnmappedAction(LevelVertexPtr vertex);
	int GetFirstUnmappedPath(LevelVertexPtr vertex);

	LevelVertexPtr AddVertex(FrameFingerprint& frame, LevelVertexPtr previousVertex, INPUT_KEY key);
	void AddFallbackVertex(LevelVertexPtr previousVertex, INPUT_KEY key);

	void ConnectVertex(int V, int W, INPUT_KEY key);
	void ConnectFallbackVertex(int V, INPUT_KEY key);

	void SaveToFile(const std::string& filename);
	void LoadFromFile(const std::string& filename);

	unsigned int GetNumVertices() { return vertices.size(); }
	LevelVertexPtr GetVertex(unsigned int id) { if (vertices.size() > id) return vertices[id]; else return nullptr; }

	FuzzyMap<FrameFingerprint, int, double>& GetFrequencies() { return frequencies; }
	FuzzyMap<FrameFingerprint, LevelVertexPtr, double>& GetVertexFrames() { return vertexFrames; }
private:
	std::vector<LevelVertexPtr> vertices;

	FuzzyMap<FrameFingerprint, int, double> frequencies{ FrameFingerprint::SIMILARITY_TRESHOLD };
	FuzzyMap<FrameFingerprint, LevelVertexPtr, double> vertexFrames{ FrameFingerprint::SIMILARITY_TRESHOLD };

};

