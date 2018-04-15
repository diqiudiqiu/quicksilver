//
// Created by Nikolay Yakovets on 2018-02-01.
//
/*
#ifndef QS_SIMPLEESTIMATOR_H
#define QS_SIMPLEESTIMATOR_H

#include <map>
#include <set>
#include <string>
#include "Estimator.h"
#include "SimpleGraph.h"


std::vector<std::string> traverseTree(RPQTree *q);


class CharacteristicSet {
public:


    std::vector<std::vector<uint32_t>> count;
    std::vector<std::map<uint32_t,uint32_t>> frecount;
    std::vector<std::map<uint32_t,uint32_t>> Sfrecount;

protected:
    uint32_t L;
    uint32_t V;

public:
    void CreateCharacteristicSet(std::shared_ptr<SimpleGraph> &g);

    void setNoVertices(uint32_t n);
    void setNoLabels(uint32_t noLabels);


    CharacteristicSet()= default;
    ~CharacteristicSet() = default;


};


class SimpleEstimator : public Estimator {
private:
    std::shared_ptr<SimpleGraph> graph;
    CharacteristicSet CharacSet;

public:
    explicit SimpleEstimator(std::shared_ptr<SimpleGraph> &g);
    ~SimpleEstimator() = default;

    void prepare() override ;
    cardStat estimate(RPQTree *q) override ;

    std::pair<uint32_t ,uint32_t > EstimateQueryCardinality(std::vector<std::string>, uint32_t L);
    uint32_t estimateSize(std::vector<std::string> relations, uint32_t pos);
    uint32_t estimateSize(std::vector<std::string> relations, uint32_t Tr, uint32_t Vry,uint32_t pos);
    uint32_t estimateNaturalJoinSize(uint32_t Ts, uint32_t Tr, uint32_t Vsy = 10, uint32_t Vry = 10);
    float CCC(uint32_t O, uint32_t S,uint32_t cl,uint32_t O1);

};



#endif //QS_SIMPLEESTIMATOR_H
*/
#ifndef QS_SIMPLEESTIMATOR_H
#define QS_SIMPLEESTIMATOR_H

#include "Estimator.h"
#include "SimpleGraph.h"

class SimpleEstimator : public Estimator {

    std::shared_ptr<SimpleGraph> graph;

public:
    explicit SimpleEstimator(std::shared_ptr<SimpleGraph> &g);
    ~SimpleEstimator() = default;

    void prepare() override ;
    cardStat estimate(RPQTree *q) override ;

};


#endif //QS_SIMPLEESTIMATOR_H