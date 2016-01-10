#include "stdafx.h"
#include "ProbabilityGraph.h"
#include <fstream>
#include <time.h>

using std::make_shared;
using std::make_pair;

extern std::vector<INPUT_KEY> keys;


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

std::vector<int> ProbabilityGraph::GetProbable(ProbabilityVertexPtr vertex, std::vector<INPUT_KEY> keys)
{
	std::vector<int> targets;

	for (const INPUT_KEY key : keys)
	{
		auto& range = vertex->adj.equal_range(key);
		auto& iterator = range.first;
		while (iterator != range.second)
		{
			auto& probPair = iterator->second;

			if (probPair.second != nullptr)
				targets.push_back(probPair.second->id);

			iterator++;
		}
	}

	return targets;
}

ProbabilityVertexPtr ProbabilityGraph::AddDisabledVertex(FrameFingerprint& frame)
{
	ProbabilityVertexPtr newVertex = make_shared<ProbabilityVertex>(frame, vertices.size());
	vertices.push_back(newVertex);

	frameVertices[frame] = newVertex;

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

	frameVertices[frame] = newVertex;

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

void ProbabilityGraph::MultipleConnect(unsigned int V, std::vector<unsigned int> other, double probability)
{
	std::map<unsigned int, int> alreadyInserted;
	for (const unsigned int W : other)
	{
		if (alreadyInserted[W] == 0)
		{
			if (W != V)
			{
				
				ConnectVertex(V, W, INPUT_ANY, probability);
				ConnectVertex(W, V, INPUT_ANY, probability);
			}
			else
				ConnectFallbackVertex(V, INPUT_ANY, probability);

			alreadyInserted[W] = 1;
		}
	}
}

void ProbabilityGraph::Save(const std::string& filename)
{
	std::ofstream file(filename, std::ios::out | std::ios::trunc | std::ios::binary);

	if (!file.is_open())
	{
		std::string tempFilename = "graph_temp_";
		tempFilename += std::to_string(time(NULL));
		tempFilename += ".bin";

		std::cout << "Could not open " << filename << ", trying with " << tempFilename << "\n";
		file = std::ofstream(tempFilename, std::ios::out | std::ios::trunc | std::ios::binary);
		if (!file.is_open())
			return;
	}

	unsigned int numVertices = GetNumVertices();
	file.write((const char*)&numVertices, sizeof(numVertices));

	for (ProbabilityVertexPtr& vertex : vertices)
	{
		unsigned int adjSize = vertex->adj.size();
		file.write((const char*)&vertex->id, sizeof(vertex->id));
		file.write((const char*)&vertex->frame.fingerprint, sizeof(vertex->frame.fingerprint));
		file.write((const char*)&vertex->frame.pHash, sizeof(vertex->frame.pHash));
		file.write((const char*)vertex->frame.histogram.data, vertex->frame.histogram.step *vertex->frame.histogram.rows);

		file.write((const char*)&adjSize, sizeof(adjSize));
		for (auto& entry : vertex->adj)
		{
			int nullptrValue = -1;
			file.write((const char*)&entry.first, sizeof(entry.first)); // Key.
			file.write((const char*)&entry.second.first, sizeof(entry.second.first)); // Probability.
			if (entry.second.second != nullptr) // Adjacent id.
				file.write((const char*)&entry.second.second->id, sizeof(entry.second.second->id));
			else
				file.write((const char*)&nullptrValue, sizeof(nullptrValue));
		}
	}

	file.close();
}

void ProbabilityGraph::Load(const std::string& filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);

	std::map<int, std::vector< std::tuple<INPUT_KEY, int, double> > > edges;

	unsigned int numVertices = -1;
	file.read((char*)&numVertices, sizeof(numVertices));

	for (unsigned int i = 0; i < numVertices; ++i)
	{
		int id = -1;
		unsigned int adjSize = -1;
		double fingerprint = 0.0;
		uint64_t pHash = 0;
		cv::Mat histogram{ cv::Size(1, 16), CV_32F };
		file.read((char*)&id, sizeof(id));
		file.read((char*)&fingerprint, sizeof(fingerprint));
		file.read((char*)&pHash, sizeof(pHash));
		file.read((char*)histogram.data, histogram.step * histogram.rows);

		FrameFingerprint frame{ fingerprint, pHash, histogram };
		ProbabilityVertexPtr newVertex = make_shared<ProbabilityVertex>(frame, id);
		vertices.push_back(newVertex);
		frameVertices[frame] = newVertex;
		frames.AddFrame(frame);
		frames.IncreaseFrequency(frame.id);

		file.read((char*)&adjSize, sizeof(adjSize));
		for (unsigned int i = 0; i < adjSize; ++i)
		{
			INPUT_KEY key = INPUT_INVALID;
			int otherId = -1;
			double probability = 0.0;
			file.read((char*)&key, sizeof(key));
			file.read((char*)&probability, sizeof(probability));
			file.read((char*)&otherId, sizeof(otherId));

			//	if (probability > 60)
			edges[id].emplace_back(key, otherId, probability);
		}
	}

	for (const auto& pair : edges)
	{
		for (const std::tuple<INPUT_KEY, int, double>& edge : pair.second)
		{
			int vertId = std::get<1>(edge);
			if (vertId != -1)
				ConnectVertex(pair.first, vertId, std::get<0>(edge), std::get<2>(edge));
			else ConnectFallbackVertex(pair.first, std::get<0>(edge), std::get<2>(edge));
		}
	}

	file.close();
}

