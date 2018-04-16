//
// Created by Nikolay Yakovets on 2018-02-02.
//

#include "SimpleEstimator.h"
#include "SimpleEvaluator.h"
#include <sstream>
#include <string>
//std::map<std::string,std::shared_ptr<SimpleGraph>> lefttable;
//std::map<std::string,std::vector<std::vector<uint32_t>>> righttable;

SimpleEvaluator::SimpleEvaluator(std::shared_ptr<SimpleGraph> &g) {

    // works only with SimpleGraph
    graph = g;
    est = nullptr;
}

void SimpleEvaluator::attachEstimator(std::shared_ptr<SimpleEstimator> &e) {
    est = e;
}

void SimpleEvaluator::prepare() {

    // if attached, prepare the estimator
    if(est != nullptr) est->prepare();
    for(uint32_t i=0;i<graph->getNoLabels();i++)
    {
        auto out = std::make_shared<SimpleGraph>(1);
        out->setNoVertices(graph->getNoVertices());
        auto in = std::make_shared<SimpleGraph>(1);
        in->setNoVertices(graph->getNoVertices());
        std::vector<std::vector<uint32_t>> right;
        std::vector<std::vector<uint32_t>> right1;
        right.resize(graph->getNoVertices());
        right1.resize(graph->getNoVertices());
        std::stringstream query;
        query<<i<<'+';
        std::stringstream query1;
        query1<<i<<'-';
        for(auto edge:graph->adj[i])
        {
            out->addEdge(edge.first,edge.second,0);
            in->addEdge(edge.second,edge.first,0);
            right[edge.first].emplace_back(edge.second);
            right1[edge.second].emplace_back(edge.first);
        }

        est->lefttable.insert(std::make_pair(query.str(),out));
        est->lefttable.insert(std::make_pair(query1.str(),in));
        est->righttable.insert(std::make_pair(query.str(),right));
        est->righttable.insert(std::make_pair(query1.str(),right1));
    }

    std::cout<<"hi"<<std::endl;
    // prepare other things here.., if necessary

}
//bool rightcmp(const std::pair<uint32_t, uint32_t> &firstElem, const std::pair<uint32_t, uint32_t> &secondElem) {
//    if(firstElem.first==secondElem.first)
//    {
//        return firstElem.second<secondElem.second;
//    }
//    return firstElem.first < secondElem.first;
//}
//bool leftcmp(const std::pair<uint32_t, uint32_t> &firstElem, const std::pair<uint32_t, uint32_t> &secondElem) {
//    if(firstElem.second==secondElem.second)
//    {
//        return firstElem.first<secondElem.first;
//    }
//    return firstElem.second < secondElem.second;
//}

cardStat SimpleEvaluator::computeStats(std::shared_ptr<SimpleGraph> &g) {

    cardStat stats {};

    std::set<uint32_t > In;
    std::set<uint32_t > Out;
//    stats.noPaths=(uint32_t )g->adj[0].size();
    stats.noPaths = g->getNoDistinctEdges();

    for(auto card:g->adj[0])
    {
        Out.insert(card.first);
        In.insert(card.second);
    }
    stats.noOut=(uint32_t )Out.size();
    stats.noIn=(uint32_t )In.size();
    return stats;
}

/*
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
//            for (auto edge: in->adj[projectLabel]) {
//                out->addEdge(edge.first, edge.second, 0);
//            }
            out->adj[0]=in->adj[projectLabel];
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
//            out->adj[0]=in->reverse_adj[projectLabel];
            table[1].insert(std::map<int , std::shared_ptr<SimpleGraph>>::value_type(k,out));
            return out;
        }
    }


}

std::shared_ptr<SimpleGraph> SimpleEvaluator::join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right) {

    auto out = std::make_shared<SimpleGraph>(1);
    out->setNoVertices(left->getNoVertices());
    std::vector<std::vector<uint32_t >> righttable;
    righttable.resize(right->getNoVertices());
    for(auto rightsource:right->adj[0]){
        righttable[rightsource.first].emplace_back(rightsource.second);
    }
    for(auto leftSource:left->adj[0]) {
        auto leftTarget = leftSource.second;
        for (auto rightLabelTarget : righttable[leftTarget]) {

            auto rightTarget = rightLabelTarget;
            out->addEdge(leftSource.first, rightTarget, 0);

        }
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
*/
std::string SimpleEvaluator::leftpart(std::vector<std::string> q, uint32_t &pos)
{
    std::stringstream qq;
    std::string query;
    for(uint32_t i=0;i<pos;i++)
    {
        qq<<q[i];
    }
    query=qq.str();
    if(est->lefttable.find(query)==est->lefttable.end())
    {
        pos--;
        query=leftpart(q,pos);
    } else{
        return query;
    }
    return query;
}
std::string SimpleEvaluator::rightpart(uint32_t begin,std::vector<std::string> q, uint32_t &pos)
{
    std::stringstream qq;
    std::string query;
    for(uint32_t i=begin;i<pos;i++)
    {
        qq<<q[i];
    }
    query=qq.str();
    if(est->lefttable.find(query)==est->lefttable.end())
    {
        pos--;
        query=rightpart(begin,q,pos);
    } else{
        return query;
    }
    return query;
}
std::string SimpleEvaluator::join1(std::string left, std::string right) {

    std::string joinq;
    joinq=left+right;
    auto out = std::make_shared<SimpleGraph>(1);
    out->setNoVertices(graph->getNoVertices());
    std::vector<std::vector<uint32_t >> rightin;
    rightin.resize(graph->getNoVertices());
    auto leftp=est->lefttable.find(left);
    auto rightp=est->righttable.find(right);

    for(auto leftSource:leftp->second->adj[0]) {
        auto leftTarget = leftSource.second;
        for (auto rightLabelTarget : rightp->second[leftTarget]) {
            auto rightTarget = rightLabelTarget;
            out->addEdge(leftSource.first, rightTarget, 0);
            rightin[leftSource.first].emplace_back(rightTarget);
        }
    }
    est->lefttable.insert(std::make_pair(joinq,out));
    est->righttable.insert(std::make_pair(joinq,rightin));
    return joinq;
}

std::shared_ptr<SimpleGraph> SimpleEvaluator::evaluate_aux1(std::vector<std::string> q, uint32_t pos){
    uint32_t begin=0;
    uint32_t lpos=pos;
    auto leftq=leftpart(q,lpos);
    begin=lpos;
    while(begin!=pos)
    {
        uint32_t rpos=pos;
        auto rightq=rightpart(begin,q,rpos);
        begin=rpos;
        leftq=join1(leftq,rightq);
    }
    std::stringstream qq;
    std::string query;
    for(uint32_t i=0;i<pos;i++)
    {
        qq<<q[i];
    }
    return est->lefttable.find(qq.str())->second;
}


cardStat SimpleEvaluator::evaluate(RPQTree *query) {
//    bool jflag=true;
//    auto res = evaluate_aux(query);
    auto querylist=est->traverseTree(query);
    auto res=evaluate_aux1(querylist,(uint32_t)querylist.size());
    return SimpleEvaluator::computeStats(res);
}