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



public:

    explicit SimpleEvaluator(std::shared_ptr<SimpleGraph> &g);
    ~SimpleEvaluator() = default;

    void prepare() override ;
    cardStat evaluate(RPQTree *query) override ;

    void attachEstimator(std::shared_ptr<SimpleEstimator> &e);

//    std::shared_ptr<SimpleGraph> evaluate_aux(RPQTree *q);
    std::string evaluate_aux1(std::vector<std::string> q, uint32_t pos);
//    static std::shared_ptr<SimpleGraph> project(uint32_t label, bool inverse, std::shared_ptr<SimpleGraph> &g);
//    static std::shared_ptr<SimpleGraph> join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right);
    std::string join1(std::string left, std::string right);
    std::string leftpart(std::vector<std::string> q, uint32_t &pos);
    std::string rightpart(uint32_t begin,std::vector<std::string> q, uint32_t &pos);
//     static cardStat computeStats(std::shared_ptr<SimpleGraph> &g);
    cardStat computeStats1(std::string q);

};


#endif //QS_SIMPLEEVALUATOR_H
