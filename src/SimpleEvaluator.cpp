//
// Created by Nikolay Yakovets on 2018-02-02.
//

#include "SimpleEstimator.h"
#include "SimpleEvaluator.h"
#include <sstream>
#include <string>

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
            right[edge.first].emplace_back(edge.second);
            right1[edge.second].emplace_back(edge.first);
        }

        est->lefttable.insert(std::make_pair(query.str(),right1));
        est->lefttable.insert(std::make_pair(query1.str(),right));
        est->righttable.insert(std::make_pair(query.str(),right));
        est->righttable.insert(std::make_pair(query1.str(),right1));
    }


bool sortPairs(const uint32_t &a, const uint32_t &b) {
    return a<b;
}

uint32_t getNoDistinctEdges(std::vector<std::vector<uint32_t>> adj) {

    uint32_t sum = 0;

    for (uint32_t i=0;i<(uint32_t)adj.size();i++)
    {
        std::sort(adj[i].begin(), adj[i].end(), sortPairs);
        uint32_t prevTarget = 0;
        bool first = true;
        for (uint32_t j=0;j<(uint32_t)adj[i].size();j++) {
            if (first || prevTarget != adj[i][j]) {
                first = false;
                sum++;
                prevTarget = adj[i][j];
            }
        }
    }

    return sum;
}

cardStat SimpleEvaluator::computeStats1(std::string q){
    cardStat stats {};
//    stats.noPaths=est->lefttable.find(q)->second->getNoDistinctEdges();
    std::set<uint32_t > In;
    auto right=est->righttable.find(q);
    stats.noPaths=getNoDistinctEdges(right->second);
    for(uint32_t i=0;i<graph->getNoVertices();i++)
    {
        if(!right->second[i].empty()){
            stats.noOut++;
        }
    }
    auto left=est->lefttable.find(q);
    for(uint32_t i=0;i<graph->getNoVertices();i++)
    {
        if(!left->second[i].empty()){
            stats.noIn++;
        }
    }
//    stats.noIn=(uint32_t )In.size();
    return stats;
}

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
    std::vector<std::vector<uint32_t >> leftout;
    leftout.resize(graph->getNoVertices());
    std::vector<std::vector<uint32_t >> rightin;
    rightin.resize(graph->getNoVertices());

    auto leftp=est->lefttable.find(left);
    auto rightp=est->righttable.find(right);
    for(uint32_t i=0;i<graph->getNoVertices();i++)
    {
        if(!leftp->second[i].empty()&&!rightp->second[i].empty())
        {
            for(auto leftTarget:leftp->second[i]){
                for(auto rightTarget:rightp->second[i])
                {
                    leftout[rightTarget].emplace_back(leftTarget);
                    rightin[leftTarget].emplace_back(rightTarget);
                }
            }
        }
    }

    est->lefttable.insert(std::make_pair(joinq,leftout));
    est->righttable.insert(std::make_pair(joinq,rightin));
    return joinq;
}

std::string SimpleEvaluator::evaluate_aux1(std::vector<std::string> q, uint32_t pos){
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
    return qq.str();
}


cardStat SimpleEvaluator::evaluate(RPQTree *query) {
    auto querylist=est->traverseTree(query);
    std::string query1=evaluate_aux1(querylist,(uint32_t)querylist.size());
    return computeStats1(query1);
}