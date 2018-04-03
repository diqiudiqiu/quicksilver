#include <set>
#include "SimpleGraph.h"
#include "SimpleEstimator.h"

SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g){

    // works only with SimpleGraph
    graph = g;
}

void SimpleEstimator::prepare() {
    std::map<uint32_t, std::set<uint32_t>> attributes;

    //Determine T for all relations
	for(const auto& fromNode : graph->adj)
	{
		for(auto outgoingEdge : fromNode)
		{
			T[outgoingEdge.first] += 1;
            attributes[outgoingEdge.first].insert(outgoingEdge.second);
		}
	}

    for(const auto& attributePair : attributes)
    {
        V[attributePair.first] = attributePair.second.size();
    }
}

uint32_t SimpleEstimator::getNumberOfDistinctValuesForAttribute(uint32_t attribute)
{
	return V[attribute];
}

uint32_t SimpleEstimator::estimateSize(std::deque<uint32_t> relations, uint32_t Tr, uint32_t Vry)
{
    if(relations.size() < 1)
    {
        return Tr;
    }

    uint32_t relation = relations.front();
    relations.pop_front();

    uint32_t newSize = estimateNaturalJoinSize(Tr, T[relation], Vry, getNumberOfDistinctValuesForAttribute(relation));

    return estimateSize(relations, newSize, V[relation]);
}

uint32_t SimpleEstimator::estimateSize(std::deque<uint32_t> relations)
{
    uint32_t r = relations.front();
    relations.pop_front();

    return estimateSize(relations, T[r], V[r]);
}

uint32_t SimpleEstimator::estimateNaturalJoinSize(uint32_t Tr, uint32_t Ts, uint32_t Vry, uint32_t Vsy)
{
	return std::min(
			(Tr*Ts)/Vsy,
			(Ts*Tr)/Vry
	);
}

cardStat SimpleEstimator::estimate(RPQTree *q)
{
    auto relations = convertTreeToDeque(q);

	uint32_t noPaths = estimateSize(relations);

    return cardStat {0, noPaths, 0};
}

static std::deque<uint32_t> convertTreeToDeque(RPQTree *q)
{
    std::deque<uint32_t> result;

    if (q == nullptr) {
        return result;
    }

    if (q->isLeaf()) {
        result.push_back(q->data[0] - '0'); //convert first character of string to int
    }


    auto newAddition = convertTreeToDeque(q->left);
    result.insert(result.end(), newAddition.begin(), newAddition.end());

    newAddition = convertTreeToDeque(q->right);
    result.insert(result.end(), newAddition.begin(), newAddition.end());

    return result;
}