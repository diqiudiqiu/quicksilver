//
// Created by Nikolay Yakovets on 2018-02-01.
//

#include <set>
#include <string>
#include "SimpleGraph.h"
#include "SimpleEstimator.h"


SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g){

    // works only with SimpleGraph
    graph = g;
}

void SimpleEstimator::prepare() {

    // do your prep here
    uint32_t L=graph->getNoLabels();
    count.resize(L);
    for (int i = 0; i < L; i++) {
        count[i].resize(3, 0);
    }
    std::set<uint32_t > sub;
    std::set<uint32_t > obj;
    for(uint32_t i=0;i<L;i++) {
        count[i][1]=(uint32_t)graph->adj[i].size();
        for (auto labelclass:graph->adj[i]) {
            sub.insert(labelclass.first);
            obj.insert(labelclass.second);
        }
        count[i][0]=(uint32_t)sub.size();
        count[i][2]=(uint32_t)obj.size();
    }
}

uint32_t SimpleEstimator::estimateSize(std::vector<std::string> relations, uint32_t pos)
{
    uint32_t r = relations[pos][0]-'0';
    pos++;
    if(relations[pos-1][1]=='+'){
        return estimateSize(relations, count[r][1], count[r][2],pos);
    } else{
        return estimateSize(relations, count[r][1], count[r][0],pos);
    }
}

uint32_t SimpleEstimator::estimateSize(std::vector<std::string> relations, uint32_t Tr, uint32_t Vry, uint32_t pos)
{
    if(pos == relations.size())
    {
        return Tr;
    }

    uint32_t relation =relations[pos][0]-'0';
    pos++;
    if(relations[pos-1][1]=='+') {
        uint32_t newSize = estimateNaturalJoinSize(Tr, count[relation][1], Vry, count[relation][2]);

        return estimateSize(relations, newSize, count[relation][2], pos);
    } else{
        uint32_t newSize = estimateNaturalJoinSize(Tr, count[relation][1], Vry, count[relation][0]);

        return estimateSize(relations, newSize, count[relation][0], pos);
    }
}

uint32_t SimpleEstimator::estimateNaturalJoinSize(uint32_t Tr, uint32_t Ts, uint32_t Vry, uint32_t Vsy)
{
    return std::min(
            (Tr*Ts)/Vsy,
            (Ts*Tr)/Vry
    );
}


std::vector<std::string> SimpleEstimator::traverseTree(RPQTree *q)
{
    std::vector<std::string> result;

    if(q == nullptr) {
        return result;
    }

    if(q->isLeaf())
    {
        result.push_back(q->data);
    }

    auto newAddition = traverseTree(q->left);
    result.insert(result.end(),newAddition.begin(),newAddition.end());

    newAddition = traverseTree(q->right);
    result.insert(result.end(),newAddition.begin(),newAddition.end());

    return result;
}


cardStat SimpleEstimator::estimate(RPQTree *q) {

    // perform your estimation here
    auto predicates = traverseTree(q);
    uint32_t nopath=estimateSize(predicates,0);
    return cardStat {0, nopath, 0};
}

/*
SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g){

    // works only with SimpleGraph
    graph = g;
}

void SimpleEstimator::prepare() {
    CharacteristicSet characteristicSets;
    characteristicSets.CreateCharacteristicSet(graph);
    CharacSet=characteristicSets;
}

cardStat SimpleEstimator::estimate(RPQTree *q)
{
    //(noOut, noPaths, noIn)
    auto predicates = traverseTree(q);
    std::pair<uint32_t,uint32_t > card = EstimateQueryCardinality(predicates,graph->getNoLabels());
    uint32_t nopath=estimateSize(predicates,0);

    return cardStat {card.first, nopath, card.second};
}

uint32_t SimpleEstimator::estimateSize(std::vector<std::string> relations, uint32_t pos)
{
    uint32_t r = relations[pos][0]-'0';
    pos++;
    return estimateSize(relations, CharacSet.count[r][1], CharacSet.count[r][2],pos);
}

uint32_t SimpleEstimator::estimateSize(std::vector<std::string> relations, uint32_t Tr, uint32_t Vry, uint32_t pos)
{
    if(pos == relations.size())
    {
        return Tr;
    }

    uint32_t relation =relations[pos][0]-'0';
    pos++;

    uint32_t newSize = estimateNaturalJoinSize(Tr, CharacSet.count[relation][1], Vry, CharacSet.count[relation][2]);

    return estimateSize(relations, newSize, CharacSet.count[relation][2],pos);
}

uint32_t SimpleEstimator::estimateNaturalJoinSize(uint32_t Tr, uint32_t Ts, uint32_t Vry, uint32_t Vsy)
{
    return std::min(
            (Tr*Ts)/Vsy,
            (Ts*Tr)/Vry
    );
}


std::vector<std::string> traverseTree(RPQTree *q)
{
    std::vector<std::string> result;

    if(q == nullptr) {
        return result;
    }

    if(q->isLeaf())
    {
        result.push_back(q->data);
    }

    auto newAddition = traverseTree(q->left);
    result.insert(result.end(),newAddition.begin(),newAddition.end());

    newAddition = traverseTree(q->right);
    result.insert(result.end(),newAddition.begin(),newAddition.end());

    return result;
}



std::pair<uint32_t ,uint32_t > SimpleEstimator::EstimateQueryCardinality(std::vector<std::string> Q, uint32_t L)
{
    float card;
    float card1;
    for(int i=0;i<Q.size();i++)
    {
        uint32_t S=CharacSet.count[Q[i][0]-'0'][0];
        uint32_t O=CharacSet.count[Q[i][0]-'0'][2];
        uint32_t cl=CharacSet.count[Q[i][0]-'0'][1];
        if(Q[i][1]=='-')
        {
            S=CharacSet.count[Q[i][0]-'0'][2];
            O=CharacSet.count[Q[i][0]-'0'][0];
        }
        float sel;
        if(i==0)
        {
            sel=1.0;
        } else {
            sel = CCC(card, S,cl,O);
        }
        uint32_t isel=std::ceil(sel);
        while(CharacSet.frecount[Q[i][0]-'0'].find(isel)==CharacSet.frecount[Q[i][0]-'0'].end())
        {
            isel++;
        }
        if(Q[i][1]=='+')
        {
            card=CharacSet.frecount[Q[i][0]-'0'][isel];
        }
        if(Q[i][1]=='-')
        {
            card=CharacSet.Sfrecount[Q[i][0]-'0'][isel];
        }


    }
    for(int i=Q.size()-1;i>=0;i--)
    {
        uint32_t S=CharacSet.count[Q[i][0]-'0'][2];
        uint32_t O=CharacSet.count[Q[i][0]-'0'][0];
        uint32_t cl=CharacSet.count[Q[i][0]-'0'][1];
        if(Q[i][1]=='-')
        {
            S=CharacSet.count[Q[i][0]-'0'][0];
            O=CharacSet.count[Q[i][0]-'0'][2];
        }
        float sel;
        if(i==Q.size()-1)
        {
            sel=1.0;
        } else {
            sel = CCC(card1, S,cl,O);
        }
        uint32_t isel=std::ceil(sel);
        while(CharacSet.frecount[Q[i][0]-'0'].find(isel)==CharacSet.Sfrecount[Q[i][0]-'0'].end())
        {
            isel++;
        }
        if(Q[i][1]=='+')
        {
            card1=CharacSet.Sfrecount[Q[i][0]-'0'][isel];
        }
        if(Q[i][1]=='-')
        {
            card1=CharacSet.frecount[Q[i][0]-'0'][isel];
        }

    }

    return std::make_pair(card1,card);
}

float SimpleEstimator::CCC(uint32_t O, uint32_t S1, uint32_t cl, uint32_t O1)
{
    float k=std::min(O,S1)/1.0;
    float s1=S1/1.0;
    float c=cl/1.0;
    float o1=O1/1.0;
    return  ((s1-k/2.0)/s1*c)/o1;
}


void CharacteristicSet::setNoVertices(uint32_t n) {
    V = n;
}

void CharacteristicSet::setNoLabels(uint32_t noLabels) {
    L = noLabels;
}

void CharacteristicSet::CreateCharacteristicSet(std::shared_ptr<SimpleGraph> &g) {
    setNoVertices(g->getNoVertices());
    setNoLabels(g->getNoLabels());
    count.resize(L);
    frecount.resize(L);
    Sfrecount.resize(L);
    for (int i = 0; i < L; i++) {
        count[i].resize(3, 0);
    }
    //std::map<uint32_t, std::set<uint32_t>> attributes;
    for (int i = 0; i < V; i++) {
        std::vector<uint32_t> flag(4, 1);
        std::vector<uint32_t> rflag(4, 1);
        std::vector<uint32_t> Sfreq(L, 0);
        auto outgoingEdges = g->adj.at(i);
        for (auto outgoingEdge : outgoingEdges) {
            // attributes[outgoingEdge.first].insert(outgoingEdge.second);
            auto property = outgoingEdge.first;
            if (flag[property]) {
                count[property][0]++;
                flag[property] = 0;
            }
            count[property][1]++;
            Sfreq[property]++;
        }
        auto incomingEdges = g->reverse_adj.at(i);
        std::vector<uint32_t> freq(L, 0);
        for (auto incomingEdge : incomingEdges) {
            auto property = incomingEdge.first;
            if (rflag[property]) {
                count[property][2]++;
                rflag[property] = 0;
            }
            freq[property]++;
        }
        for (int j = 0; j < L; j++) {
            if (freq[j]) {
                if (frecount[j].find(freq[j]) == frecount[j].end()) {
                    frecount[j][freq[j]] = 1;
                } else {
                    frecount[j][freq[j]]++;
                }
                for (auto iter = frecount[j].begin(); iter != frecount[j].find(freq[j]); iter++) {
                    frecount[j][iter->first]++;
                }
            }
            if (Sfreq[j]) {
                if (Sfrecount[j].find(Sfreq[j]) == Sfrecount[j].end()) {
                    Sfrecount[j][Sfreq[j]] = 1;
                } else {
                    Sfrecount[j][Sfreq[j]]++;
                }
                for (auto iter = Sfrecount[j].begin(); iter != Sfrecount[j].find(Sfreq[j]); iter++) {
                    Sfrecount[j][iter->first]++;
                }
            }
        }
    }
}
*/