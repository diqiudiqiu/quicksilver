//
// Created by Nikolay Yakovets on 2018-02-02.
//

#include "SimpleEstimator.h"
#include "SimpleEvaluator.h"

std::vector<std::map<int,std::shared_ptr<SimpleGraph>>> table;

SimpleEvaluator::SimpleEvaluator(std::shared_ptr<SimpleGraph> &g) {

    // works only with SimpleGraph
    graph = g;
//    est = nullptr;
}

void SimpleEvaluator::attachEstimator(std::shared_ptr<SimpleEstimator> &e) {
    est = e;
}

void SimpleEvaluator::prepare() {

    // if attached, prepare the estimator
     if(est != nullptr) est->prepare();
    table.resize(2);
    // prepare other things here.., if necessary

}

cardStat SimpleEvaluator::computeStats(std::shared_ptr<SimpleGraph> &g) {

    cardStat stats {};

    std::set<uint32_t > In;
    std::set<uint32_t > Out;
    stats.noPaths=(uint32_t )g->adj[0].size();
    for(auto card:g->adj[0])
    {
        Out.insert(card.first);
        In.insert(card.second);
    }
    stats.noOut=(uint32_t )Out.size();
    stats.noIn=(uint32_t )In.size();
    return stats;
}


std::shared_ptr<SimpleGraph> SimpleEvaluator::project(uint32_t projectLabel, bool inverse, std::shared_ptr<SimpleGraph> &in) {

    auto out = std::make_shared<SimpleGraph>(1);
    out->setNoVertices(in->getNoVertices());
    int k=projectLabel;

    if(!inverse)
    {
        if(table[0].find(k)!=table[0].end())
        {
            return table[0].at(k);
        } else
        {
            for (auto edge: in->adj[projectLabel]) {
                out->addEdge(edge.first, edge.second, 0);
            }
            table[0].insert(std::map<int , std::shared_ptr<SimpleGraph>>::value_type(k,out));
            return out;
        }
    } else{

        if(table[1].find(k)!=table[1].end())
        {
            return table[1].at(k);
        } else
        {
            for (auto edge: in->adj[projectLabel]) {
                out->addEdge(edge.second, edge.first, 0);
            }
            table[1].insert(std::map<int , std::shared_ptr<SimpleGraph>>::value_type(k,out));
            return out;
        }
    }

//
//    if(table.find(k)!=table.end())
//    {
//        return table.at(k);
//    } else {
//        if (!inverse) {
//            for (auto edge: in->adj[projectLabel]) {
//                out->addEdge(edge.first, edge.second, 0);
//            }
//        } else {
//            for (auto edge: in->adj[projectLabel]) {
//                out->addEdge(edge.second, edge.first, 0);
//            }
//        }
//        table.insert(std::map<int , std::shared_ptr<SimpleGraph>>::value_type(k,out));
//        return out;
//    }

}

bool rightcmp(const std::pair<uint32_t, uint32_t> &firstElem, const std::pair<uint32_t, uint32_t> &secondElem) {
    if(firstElem.first==secondElem.first)
    {
        return firstElem.second<secondElem.second;
    }
    return firstElem.first < secondElem.first;
}
bool leftcmp(const std::pair<uint32_t, uint32_t> &firstElem, const std::pair<uint32_t, uint32_t> &secondElem) {
    if(firstElem.second==secondElem.second)
    {
        return firstElem.first<secondElem.first;
    }
    return firstElem.second < secondElem.second;
}


std::shared_ptr<SimpleGraph> SimpleEvaluator::join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right) {

    auto out = std::make_shared<SimpleGraph>(1);
    out->setNoVertices(left->getNoVertices());
    std::vector<std::vector<uint32_t >> righttable;
    righttable.resize(right->getNoVertices());
    for(auto rightsource:right->adj[0]){
        righttable[rightsource.first].emplace_back(rightsource.second);
    }
    std::sort(left->adj[0].begin(), left->adj[0].end(), leftcmp);
//    std::sort(right->adj[0].begin(), right->adj[0].end(), rightcmp);
//    auto rightSource=right->adj[0].begin();
    for(auto leftSource:left->adj[0]) {
        auto leftTarget = leftSource.second;
        for (auto rightLabelTarget : righttable[leftTarget]) {

            auto rightTarget = rightLabelTarget;
            out->addEdge(leftSource.first, rightTarget, 0);

        }
//        if (rightSource->first < leftTarget && rightSource!=right->adj[0].end()) {
//            rightSource++;
//        } else {
//            if (rightSource->first == leftSource.second) {
//                out->addEdge(leftSource.first, rightSource->second, 0);
//                continue;
//            } else {
//                continue;
//            }
//        }
    }

    return out;
}

std::shared_ptr<SimpleGraph> SimpleEvaluator::evaluate_aux(RPQTree *q) {

    // evaluate according to the AST bottom-up

    if(q->isLeaf()) {
        // project out the label in the AST
        std::regex directLabel (R"((\d+)\+)");
        std::regex inverseLabel (R"((\d+)\-)");

        std::smatch matches;

        uint32_t label;
        bool inverse;

        if(std::regex_search(q->data, matches, directLabel)) {
            label = (uint32_t) std::stoul(matches[1]);
            inverse = false;
        } else if(std::regex_search(q->data, matches, inverseLabel)) {
            label = (uint32_t) std::stoul(matches[1]);
            inverse = true;
        } else {
            std::cerr << "Label parsing failed!" << std::endl;
            return nullptr;
        }
        return SimpleEvaluator::project(label, inverse, graph);
    }

    if(q->isConcat()) {

        // evaluate the children
        auto leftGraph = SimpleEvaluator::evaluate_aux(q->left);
        auto rightGraph = SimpleEvaluator::evaluate_aux(q->right);
        return SimpleEvaluator::join(leftGraph, rightGraph);

    }

    return nullptr;
}




cardStat SimpleEvaluator::evaluate(RPQTree *query) {
//    bool jflag=true;
    auto res = evaluate_aux(query);
//
    return SimpleEvaluator::computeStats(res);
}