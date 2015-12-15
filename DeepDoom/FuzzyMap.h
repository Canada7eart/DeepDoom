#pragma once
#include <map>

template< class K, class V, class E>
class FuzzyMap
{
public:

	FuzzyMap(const E& epsilon) : epsilon(epsilon)
	{
	}

	std::map<K, V>& GetMap() { return map; }

	V& operator[](const K& key) {
		auto existant = find(key);
		if (existant != map.end())
		{
			return existant->second;
		}

		return (map[key] = V());
	}

	typename std::map<K, V>::iterator find(const K& value)
	{
		auto lower = map.lower_bound(value - epsilon);
		auto upper = map.upper_bound(value + epsilon);

		if (lower != upper)
			return lower;
		else
			return map.end();
	}

	typename std::map<K, V>::iterator begin() const
	{
		return map.begin();
	}

	typename std::map<K, V>::iterator end() const
	{
		return map.end();
	}

	E& GetEpislon() { return epsilon; }
	void SetEpsilon(E& newEpsilon) { epsilon = newEpsilon; }

private:
	std::map<K, V> map;
	E epsilon;
};

