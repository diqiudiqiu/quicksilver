//
// Created by Nikolay Yakovets on 2018-02-02.
//

#ifndef QS_SIMPLEEVALUATOR_H
#define QS_SIMPLEEVALUATOR_H


#include <memory>
#include <cmath>
#include "SimpleGraph.h"
#include "RPQTree.h"
#include "Evaluator.h"
#include "Graph.h"
#include <set>
#include <map>


class SimpleEvaluator : public Evaluator {

    std::shared_ptr<SimpleGraph> graph;
    std::shared_ptr<SimpleEstimator> est;

    std::vector<std::pair<std::vector<uint32_t >,std::vector<uint32_t >>> table1;
    //std::vector<uint32_t > index;
    uint32_t flag=0;


public:

    explicit SimpleEvaluator(std::shared_ptr<SimpleGraph> &g);
    ~SimpleEvaluator() = default;

    void prepare() override ;
    cardStat evaluate(RPQTree *query) override ;

    void attachEstimator(std::shared_ptr<SimpleEstimator> &e);

    //std::shared_ptr<SimpleGraph> evaluate_aux(RPQTree *q);
    //static std::shared_ptr<SimpleGraph> project(uint32_t label, bool inverse, std::shared_ptr<SimpleGraph> &g);
    // static std::shared_ptr<SimpleGraph> join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right);

    std::map<uint32_t ,std::set<uint32_t >> evaluate_aux1(RPQTree *q);
    std::map<uint32_t ,std::set<uint32_t >> project1(uint32_t label, bool inverse);
    std::map<uint32_t ,std::set<uint32_t >>join1(std::map<uint32_t ,std::set<uint32_t >> &l ,std::map<uint32_t ,std::set<uint32_t >> &r);

    // static cardStat computeStats(std::shared_ptr<SimpleGraph> &g);
    cardStat computeStats1(std::map<uint32_t ,std::set<uint32_t >> table);

};


#endif //QS_SIMPLEEVALUATOR_H
