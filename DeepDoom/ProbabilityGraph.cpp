#include "stdafx.h"
#include "ProbabilityGraph.h"

using std::make_shared;
using std::make_pair;

extern std::vector<INPUT_KEY> keys;

ProbabilityGraph::ProbabilityGraph()
{
}

int ProbabilityGraph::GetRandomUnmappedAction(ProbabilityVertexPtr vertex)
{
	std::vector<int> unmappedKeys;
	for (int i = 0; i < 4; ++i)
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

double* ProbabilityGraph::GetProbabilityForAdjacent(ProbabilityVertexPtr vertex, INPUT_KEY key, ProbabilityVertexPtr adjacent)
{
	auto& range = vertex->adj.equal_range(key);
	auto& iterator = range.first;
	while (iterator != range.second)
	{
		auto& probPair = iterator->second;
		if (probPair.second == adjacent)
		{
			return &(probPair.first);
		}

		iterator++;
	}

	return nullptr;
}

std::vector<int> ProbabilityGraph::GetProbable(ProbabilityVertexPtr vertex, INPUT_KEY key)
{
	std::vector<int> targets;

	auto& range = vertex->adj.equal_range(key);
	auto& iterator = range.first;
	while (iterator != range.second)
	{
		auto& probPair = iterator->second;

		if (probPair.second != nullptr)
			targets.push_back(probPair.second->id);

		iterator++;
	}

	return targets;
}

ProbabilityVertexPtr ProbabilityGraph::AddDisabledVertex(FrameFingerprint& frame)
{
	ProbabilityVertexPtr newVertex = make_shared<ProbabilityVertex>(frame, vertices.size());
	vertices.push_back(newVertex);

	return newVertex;
}

ProbabilityVertexPtr ProbabilityGraph::AddVertex(FrameFingerprint& frame, ProbabilityVertexPtr previousVertex, INPUT_KEY key, double probability)
{
	ProbabilityVertexPtr newVertex = make_shared<ProbabilityVertex>(frame, vertices.size());
	vertices.push_back(newVertex);

	if (previousVertex != nullptr)
	{
		previousVertex->adj.insert(make_pair(key, make_pair(probability, newVertex)));
	}

	return newVertex;
}

void ProbabilityGraph::ConnectVertex(unsigned int V, unsigned int W, INPUT_KEY key, double probability)
{
	if (V >= vertices.size() || W >= vertices.size())
		return;

	vertices[V]->adj.insert(make_pair(key, make_pair(probability, vertices[W])));
}

void ProbabilityGraph::ConnectFallbackVertex(unsigned int V, INPUT_KEY key, double probability)
{
	if (V >= vertices.size())
		return;

	vertices[V]->adj.insert(make_pair(key, make_pair(probability, nullptr)));
}

