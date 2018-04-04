//
// Created by Nikolay Yakovets on 2018-02-02.
//

#include "SimpleEstimator.h"
#include "SimpleEvaluator.h"

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
    // if(est != nullptr) est->prepare();

    // prepare other things here.., if necessary

}

/*
cardStat SimpleEvaluator::computeStats(std::shared_ptr<SimpleGraph> &g) {

    cardStat stats {};

    for(int source = 0; source < g->getNoVertices(); source++) {
        if(!g->adj[source].empty()) stats.noOut++;
    }

   // stats.noPaths = g->getNoDistinctEdges();

    for(int target = 0; target < g->getNoVertices(); target++) {
        if(!g->reverse_adj[target].empty()) stats.noIn++;
    }
    return stats;
}*/

cardStat SimpleEvaluator::computeStats1(std::map<uint32_t ,std::set<uint32_t >> table) {
    cardStat stats {};
    std::set<uint32_t > In;
    stats.noOut=table.size();
    for(auto list:table){

        stats.noPaths+=list.second.size();
        for(auto in:list.second){
                In.insert(in);
        }
    }
        //stats.noPaths+=a1.size()*a2.size();

    stats.noIn=(uint32_t )In.size();
    return stats;
}
/*
std::shared_ptr<SimpleGraph> SimpleEvaluator::project(uint32_t projectLabel, bool inverse, std::shared_ptr<SimpleGraph> &in) {

    auto out = std::make_shared<SimpleGraph>(in->getNoVertices());
    out->setNoLabels(in->getNoLabels());

    if(!inverse) {
        // going forward
        for(uint32_t source = 0; source < in->getNoVertices(); source++) {
            for (auto labelTarget : in->adj[source]) {

                auto label = labelTarget.first;
                auto target = labelTarget.second;

                if (label == projectLabel)
                    out->addEdge(source, target, label);
            }
        }
    } else {
        // going backward
        for(uint32_t source = 0; source < in->getNoVertices(); source++) {
            for (auto labelTarget : in->reverse_adj[source]) {

                auto label = labelTarget.first;
                auto target = labelTarget.second;

                if (label == projectLabel)
                    out->addEdge(source, target, label);
            }
        }
    }

    return out;
}

std::shared_ptr<SimpleGraph> SimpleEvaluator::join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right) {

    auto out = std::make_shared<SimpleGraph>(left->getNoVertices());
    out->setNoLabels(1);

    for(uint32_t leftSource = 0; leftSource < left->getNoVertices(); leftSource++) {
        for (auto labelTarget : left->adj[leftSource]) {

            int leftTarget = labelTarget.second;
            // try to join the left target with right source
            for (auto rightLabelTarget : right->adj[leftTarget]) {

                auto rightTarget = rightLabelTarget.second;
                out->addEdge(leftSource, rightTarget, 0);

            }
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

        // join left with right
        return SimpleEvaluator::join(leftGraph, rightGraph);

    }

    return nullptr;
}
*/

std::map<uint32_t ,std::set<uint32_t >> SimpleEvaluator::project1(uint32_t projectLabel, bool inverse ){
    std::map<uint32_t ,std::set<uint32_t >> table;
    if(!inverse) {
        // going forward
        for(uint32_t source = 0; source < graph->getNoVertices(); source++) {
            std::set<uint32_t > From;
            for (auto labelTarget : graph->adj[source]) {

                auto label = labelTarget.first;
                auto target = labelTarget.second;

                if (label == projectLabel)
                    From.insert(target);
            }
            if(!From.empty())
            {
                table.insert(std::make_pair(source,From));
            }
        }
    } else {
        // going backward

        for(uint32_t source = 0; source < graph->getNoVertices(); source++) {
            std::set<uint32_t > From;
            for (auto labelTarget : graph->reverse_adj[source]) {

                auto label = labelTarget.first;
                auto target = labelTarget.second;

                if (label == projectLabel)
                    From.insert(target);
            }
            if(!From.empty())
            {
                table.insert(std::make_pair(source,From));
            }
        }
    }
    return table;
}
std::map<uint32_t ,std::set<uint32_t >> SimpleEvaluator::join1(std::map<uint32_t ,std::set<uint32_t >> &l ,std::map<uint32_t ,std::set<uint32_t >> &r){
    std::vector<uint32_t > indexes;
    for(auto left=l.begin();left!=l.end();left++)
    {
        //auto left=l[i];
        std::set<uint32_t > target;
        uint32_t flag=0;
        for (auto source:left->second)
        {
            if(r.find(source)!=r.end())
            {
                flag=1;
                target.insert(r.at(source).begin(),r.at(source).end());
            }
        }
        if(flag)
        {
            l[left->first]=target;
        } else{
            indexes.push_back(left->first);
//            l.erase(left->first);
//            --left;
        }
    }
    for(auto index:indexes)
    {
        l.erase(index);
    }
    return l;
}

std::map<uint32_t ,std::set<uint32_t >> SimpleEvaluator::evaluate_aux1(RPQTree *q){
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

        }

        return SimpleEvaluator::project1(label, inverse);
    }

    if(q->isConcat()) {

        // evaluate the children
        auto leftGraph = SimpleEvaluator::evaluate_aux1(q->left);
        auto rightGraph = SimpleEvaluator::evaluate_aux1(q->right);

        // join left with right
        return SimpleEvaluator::join1(leftGraph, rightGraph);

    }


}




cardStat SimpleEvaluator::evaluate(RPQTree *query) {
    //auto res = evaluate_aux(query);
    //table.resize(0);
    auto res = evaluate_aux1(query);
    return SimpleEvaluator::computeStats1(res);
    //return SimpleEvaluator::computeStats(res);
}