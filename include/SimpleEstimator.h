//
// Created by Nikolay Yakovets on 2018-02-01.
//

#ifndef QS_SIMPLEESTIMATOR_H
#define QS_SIMPLEESTIMATOR_H

#include "Estimator.h"
#include "SimpleGraph.h"
#include <map>
static std::deque<uint32_t> convertTreeToDeque(RPQTree *q);

class SimpleEstimator : public Estimator {
private:
    std::shared_ptr<SimpleGraph> graph;

	std::map<uint32_t, uint32_t> T;
    std::map<uint32_t, uint32_t> V;

	uint32_t getNumberOfDistinctValuesForAttribute(uint32_t attribute);

    uint32_t estimateSize(std::deque<uint32_t> relations);
	uint32_t estimateSize(std::deque<uint32_t> relations, uint32_t Tr, uint32_t Vry);
	uint32_t estimateNaturalJoinSize(uint32_t Ts, uint32_t Tr, uint32_t Vsy = 10, uint32_t Vry = 10);

public:
    explicit SimpleEstimator(std::shared_ptr<SimpleGraph> &g);
    ~SimpleEstimator() = default;

    void prepare() override ;
    cardStat estimate(RPQTree *q) override ;

};


#endif //QS_SIMPLEESTIMATOR_H
