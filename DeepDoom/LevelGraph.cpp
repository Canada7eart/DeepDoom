#include "stdafx.h"
#include "LevelGraph.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <utility>
#include <map>
#include <queue>
#include <unordered_map>

using std::make_shared;

extern std::vector<INPUT_KEY> keys;

LevelGraph::LevelGraph(FrameFingerprint& rootFrame)
{
	vertices.push_back(std::make_shared<LevelVertex>(rootFrame, 0));
}

int LevelGraph::GetRandomUnmappedAction(LevelVertexPtr vertex)
{
	std::vector<int> unmappedKeys;
	for (int i = 0; i < 2; ++i)
	{
		if (vertex->HasAdjacent(keys[i]))
			continue;
		else
		{
			unmappedKeys.push_back(i);
		}
	}

	if (unmappedKeys.size() == 0)
		return -1;

	std::random_shuffle(unmappedKeys.begin(), unmappedKeys.end());
	return unmappedKeys[0];
}

int LevelGraph::GetFirstUnmappedPath(LevelVertexPtr vertex)
{
	if (vertices.size() == 0)
		return -1;

	std::unordered_map<LevelVertexPtr, std::pair<LevelVertexPtr, INPUT_KEY>> parents;
	std::unordered_map<LevelVertexPtr, int> closed;

	std::queue<LevelVertexPtr> openVertices;
	openVertices.push(vertex);
	parents[vertex] = std::make_pair(nullptr, INPUT_INVALID);

	//std::cout << "Searching from vertex: " << vertex->id << "\n";

	bool found = false;
	LevelVertexPtr target = nullptr;
	while (openVertices.size() > 0 && !found)
	{
		LevelVertexPtr current = openVertices.front();
		openVertices.pop();


		int unmapped = GetRandomUnmappedAction(current);
		if (unmapped == -1)
		{
			for (auto& adj : current->adj)
			{
				if (closed[adj.second] == 0 && adj.second != nullptr)
				{
					openVertices.push(adj.second);
					parents[adj.second] = std::make_pair(current, adj.first);

					if (adj.second->id == current->id)
					{
						int x = 0;
					}
				}
			}
		}
		else
		{
			target = current;
			found = true;
		}

		closed[current] = 1;
	}

	if (target == nullptr)
		return -1;

	LevelVertexPtr iterator = target;

	INPUT_KEY lastAction = INPUT_INVALID;

	while (iterator != vertex)
	{
		LevelVertexPtr parent = parents[iterator].first;
		lastAction = parents[iterator].second;
		iterator = parent;
	//	std::cout << "Parent: " << parent->id << "\n";
	}

	//std::cout << "Target vertex: " << target->id << ", Next: " << vertex->adj[lastAction]->id << "\n";

	//std::cout << "Next key: " << Emulator::GetKeyIndex(lastAction) << "\n";

	return Emulator::GetKeyIndex(lastAction);
}

LevelVertexPtr LevelGraph::AddVertex(FrameFingerprint& frame,
	LevelVertexPtr previousVertex, INPUT_KEY key)
{
	LevelVertexPtr newVertex = make_shared<LevelVertex>(frame, vertices.size());
	vertices.push_back(newVertex);

	if (previousVertex != nullptr)
	{
		previousVertex->adj[key] = newVertex;

		INPUT_KEY inverseKey = Emulator::GetInverseKey(key);
		if (inverseKey != INPUT_INVALID)
			newVertex->adj[inverseKey] = previousVertex;
	}

	if (vertices.size() % 10 == 0)
	{
		std::cout << "Saving to file.\n";
		SaveToFile("graph_2.bin");
	}

	return newVertex;
}

void LevelGraph::AddFallbackVertex(LevelVertexPtr previousVertex, INPUT_KEY key)
{
	previousVertex->adj[key] = nullptr;
}

void LevelGraph::ConnectVertex(int V, int W, INPUT_KEY key)
{
	vertices[V]->adj[key] = vertices[W];
}

void LevelGraph::ConnectFallbackVertex(int V, INPUT_KEY key)
{
	vertices[V]->adj[key] = nullptr;
}

void LevelGraph::SaveToFile(const std::string& filename)
{
	std::ofstream file(filename, std::ios::out | std::ios::trunc | std::ios::binary);

	if (!file.is_open())
	{
		std::cout << "Could not open " << filename << ", trying with graph_temp.bin\n";
		file = std::ofstream("graph_temp.bin", std::ios::out | std::ios::trunc | std::ios::binary);
		if (!file.is_open())
			return;
	}

	unsigned int numVertices = GetNumVertices();
	file.write((const char*)&numVertices, sizeof(numVertices));

	double epsilon = frequencies.GetEpislon();
	file.write((const char*)&epsilon, sizeof(epsilon));

	for (LevelVertexPtr& vertex : vertices)
	{
		unsigned int adjSize = vertex->adj.size();
		file.write((const char*)&vertex->id, sizeof(vertex->id));
		file.write((const char*)&vertex->frame.fingerprint, sizeof(vertex->frame.fingerprint));
		file.write((const char*)&adjSize, sizeof(adjSize));

		for (auto& pair : vertex->adj)
		{
			int nullptrValue = -1;
			file.write((const char*)&pair.first, sizeof(pair.first));
			if (pair.second != nullptr)
				file.write((const char*)&pair.second->id, sizeof(pair.second->id));
			else
				file.write((const char*)&nullptrValue, sizeof(nullptrValue));
		}
	}

	file.close();
}

void LevelGraph::LoadFromFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);

	std::map<int, std::vector< std::pair<INPUT_KEY, int> > > edges;

	unsigned int numVertices = -1;
	file.read((char*)&numVertices, sizeof(numVertices));

	double epsilon = 0.0;
	file.read((char*)&epsilon, sizeof(epsilon));

	frequencies = FuzzyMap<FrameFingerprint, int, double>{ epsilon };
	vertexFrames = FuzzyMap<FrameFingerprint, LevelVertexPtr, double>{ epsilon };

	for (unsigned int i = 0; i < numVertices; ++i)
	{
		int id = -1;
		unsigned int adjSize = -1;
		double fingerprint = 0.0;
		file.read((char*)&id, sizeof(id));
		file.read((char*)&fingerprint, sizeof(fingerprint));
		file.read((char*)&adjSize, sizeof(adjSize));

		LevelVertexPtr newVertex = make_shared<LevelVertex>(FrameFingerprint(fingerprint), id);
		vertices.push_back(newVertex);

		frequencies[FrameFingerprint(fingerprint)] = 1;
		vertexFrames[FrameFingerprint(fingerprint)] = newVertex;

		for (unsigned int i = 0; i < adjSize; ++i)
		{
			INPUT_KEY key = INPUT_INVALID;
			int otherId = -1;
			file.read((char*)&key, sizeof(key));
			file.read((char*)&otherId, sizeof(otherId));

			edges[id].emplace_back(key, otherId);
		}
	}

	for (const auto& pair : edges)
	{
		for (const std::pair<INPUT_KEY, int>& edge : pair.second)
		{
			if (edge.second != -1)
				ConnectVertex(pair.first, edge.second, edge.first);
			else ConnectFallbackVertex(pair.first, edge.first);
		}
	}

	file.close();
}
