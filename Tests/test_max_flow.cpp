#include "catch.hpp"
#include "../Kernel/max_flow.h"
#include <iostream>

using namespace max_flow_app;
using namespace kernel_messages;
using namespace observer_pattern;

class StateQueue {
public:
    StateQueue() = default;
    StateQueue(std::vector<MaxFlowData> states) : states_(std::move(states)) {
    }
    MaxFlowData GetState() {
        assert(it_ < states_.size());
        return states_[it_++];
    }

    bool IsEndOfSequence() {
        return it_ == states_.size();
    }

private:
    size_t it_ = 0;
    std::vector<MaxFlowData> states_;
};

TEST_CASE("Test basic") {
    StateQueue expected({
        {.edges = {},
         .vertices = {Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {},
         .vertices = {Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::Basic},
                   {2, 0, 0, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 1,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::Basic},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 1,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::Basic},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 1,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::Basic},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 1,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::Basic},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 1,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = 2,
         .flow_rate = 1,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::OnTheNetwork, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 1,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::Basic},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 1,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::Basic},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::Basic},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::OnTheNetwork},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::Basic},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = 0,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::OnTheNetwork},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::Basic},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::OnTheNetwork},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::Basic, Status::Basic},
         .updated_edge = 2,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::OnTheNetwork},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::Basic},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::OnTheNetwork},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::OnTheNetwork},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork, Status::Basic},
         .updated_edge = 6,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::OnTheNetwork},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::OnTheNetwork},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::OnTheNetwork},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::OnTheNetwork},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 1, Status::OnThePath},
                   {1, 0, 0, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::OnTheNetwork},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = 0,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 0, Status::OnThePath},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::OnTheNetwork},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnThePath, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 0, Status::OnThePath},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 2, Status::OnThePath},
                   {3, 1, 0, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnThePath, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = 6,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 0, Status::OnThePath},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnThePath},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnThePath, Status::OnTheNetwork, Status::OnThePath},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 0},
        {.edges = {{0, 1, 0, Status::OnThePath},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnThePath},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnThePath, Status::OnTheNetwork, Status::OnThePath},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 1},
        {.edges = {{0, 1, 0, Status::OnTheNetwork},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 1},
        {.edges = {{0, 1, 0, Status::OnTheNetwork},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 2, Status::OnTheNetwork},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 1},
        {.edges = {{0, 1, 0, Status::OnTheNetwork},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 2, Status::OnThePath},
                   {2, 0, 0, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = 2,
         .flow_rate = 0,
         .pushed_flow = 1},
        {.edges = {{0, 1, 0, Status::OnTheNetwork},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnThePath},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 1, Status::Basic},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnThePath, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 1},
        {.edges = {{0, 1, 0, Status::OnTheNetwork},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnThePath},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 1, Status::OnThePath},
                   {3, 2, 0, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnThePath, Status::OnTheNetwork},
         .updated_edge = 8,
         .flow_rate = 0,
         .pushed_flow = 1},
        {.edges = {{0, 1, 0, Status::OnTheNetwork},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnThePath},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::OnThePath},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnThePath, Status::OnThePath},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 1},
        {.edges = {{0, 1, 0, Status::OnTheNetwork},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnThePath},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::OnThePath},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnThePath, Status::OnThePath},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::OnTheNetwork},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnTheNetwork},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::OnTheNetwork},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::Basic},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::Basic},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::Basic},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::Basic},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnTheNetwork},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::Basic},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = 2,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnTheNetwork},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::Basic},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::Basic},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::OnTheNetwork, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnTheNetwork},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::OnTheNetwork},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::Basic},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::OnTheNetwork, Status::Basic},
         .updated_edge = 4,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnTheNetwork},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::OnTheNetwork},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::Basic},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnTheNetwork},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::OnTheNetwork},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork, Status::Basic},
         .updated_edge = 6,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnTheNetwork},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::OnTheNetwork},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnTheNetwork},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::OnTheNetwork},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 1, Status::OnThePath},
                   {2, 0, 1, Status::Basic},
                   {2, 1, 1, Status::OnTheNetwork},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = 2,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::OnThePath},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 1, Status::OnTheNetwork},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnThePath, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::OnThePath},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 1, Status::OnThePath},
                   {1, 2, 0, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnTheNetwork, Status::OnThePath, Status::OnTheNetwork},
         .updated_edge = 4,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::OnThePath},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::OnThePath},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 1, Status::OnTheNetwork},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnThePath, Status::OnThePath, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::OnThePath},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::OnThePath},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 1, Status::OnThePath},
                   {3, 1, 1, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnThePath, Status::OnThePath, Status::OnTheNetwork},
         .updated_edge = 6,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::OnThePath},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::OnThePath},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 0, Status::OnThePath},
                   {3, 1, 2, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnThePath, Status::OnThePath, Status::OnThePath},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 2},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::OnThePath},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::OnThePath},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 0, Status::OnThePath},
                   {3, 1, 2, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnThePath, Status::OnThePath, Status::OnThePath, Status::OnThePath},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 3},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::OnTheNetwork},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::OnTheNetwork},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 0, Status::OnTheNetwork},
                   {3, 1, 2, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork, Status::OnTheNetwork},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 3},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::Basic},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::Basic},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 0, Status::Basic},
                   {3, 1, 2, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 3},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::Basic},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::Basic},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 0, Status::Basic},
                   {3, 1, 2, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::OnTheNetwork, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 3},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::Basic},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::Basic},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 0, Status::Basic},
                   {3, 1, 2, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 3},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::Basic},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::Basic},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 0, Status::Basic},
                   {3, 1, 2, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 3},
        {.edges = {{0, 1, 0, Status::Basic},
                   {1, 0, 1, Status::Basic},
                   {0, 2, 0, Status::Basic},
                   {2, 0, 2, Status::Basic},
                   {2, 1, 0, Status::Basic},
                   {1, 2, 1, Status::Basic},
                   {1, 3, 0, Status::Basic},
                   {3, 1, 2, Status::Basic},
                   {2, 3, 0, Status::Basic},
                   {3, 2, 1, Status::Basic}},
         .vertices = {Status::Basic, Status::Basic, Status::Basic, Status::Basic},
         .updated_edge = std::string::npos,
         .flow_rate = 0,
         .pushed_flow = 3}
    });
    MaxFlow max_flow;
    Observer<MaxFlowData> flow_observer([&expected](const MaxFlowData& message) {
        REQUIRE_FALSE(expected.IsEndOfSequence());
        REQUIRE(expected.GetState() == message);
    });
    Observer<MaxFlowData> network_observer([&expected](const MaxFlowData& message) {
        REQUIRE_FALSE(expected.IsEndOfSequence());
        REQUIRE(expected.GetState() == message);
    });
    max_flow.RegisterFlowObserver(&flow_observer);
    max_flow.RegisterNetworkObserver(&network_observer);
    max_flow.ChangeVerticesNumberRequest(4);
    max_flow.AddEdgeRequest({0, 1, 1});
    max_flow.AddEdgeRequest({0, 2, 2});
    max_flow.AddEdgeRequest({2, 1, 1});
    max_flow.AddEdgeRequest({1, 3, 2});
    max_flow.AddEdgeRequest({2, 3, 1});
    max_flow.RunRequest();
}
