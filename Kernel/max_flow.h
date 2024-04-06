#ifndef MAX_FLOW_H
#define MAX_FLOW_H
#include <vector>
#include <cstddef>
#include <deque>
#include "Library/observer_pattern.h"
#include "mvc_messages.h"
#include <random>
#include <string>

namespace max_flow_app {
class MaxFlow {
public:
    using BasicEdge = mvc_messages::BasicEdge;
    using Edge = mvc_messages::Edge;
    using Data = mvc_messages::MaxFlowData;
    using Status = mvc_messages::Status;

    MaxFlow() = default;
    MaxFlow(size_t n, size_t m, const std::vector<BasicEdge>& edges);

    void ChangeVerticesNumberRequest(size_t new_number);
    void AddEdgeRequest(const BasicEdge& edge);
    void DeleteEdgeRequest(const BasicEdge& egde);
    void RunRequest();
    void GenRandomSampleRequest();
    void RecoverPrevStateRequest();

    void RegisterNetworkObserver(observer_pattern::Observer<Data>* observer);
    void RegisterFlowObserver(observer_pattern::Observer<Data>* observer);
    void RegisterCleanupObserver(observer_pattern::Observer<void>* observer);
    void RegisterUnlockObserver(observer_pattern::Observer<void>* observer);

    Data GetData() const;

private:
    inline static const size_t kMinVerticesNum = 2, kMaxVerticesNum = 10,
        kMaxEdgeCapacity = 100, kStatesStorageSize = 10;

    bool FindNetwork();
    void SetPathToBasicStatus(const std::vector<size_t>& path);
    void SetGraphToBasicStatus(bool is_flow_notification);
    void ExtendNetwork(size_t vertex, std::vector<bool>& used,
                       std::vector<ssize_t>& parent, std::deque<size_t>& queue);
    void ChangeNewEdgeStatus(size_t vertex, const std::vector<ssize_t>& parent);
    size_t ExtractVertice(std::deque<size_t>& queue);
    void FindingNetworkInit(std::deque<size_t>& queue, std::vector<ssize_t>& parent,
                            std::vector<bool>& used);
    bool FindPath(size_t vertex, std::vector<size_t>& path);
    void ProcessPath(const std::vector<size_t>& path);

    Edge& GetEdge(size_t index);
    Edge& GetReverseEdge(size_t index);
    const Edge& GetEdge(size_t index) const;
    const Edge& GetReverseEdge(size_t index) const;
    void AddEdges(const std::vector<BasicEdge>& edges);
    void AddEdge(const BasicEdge& edge);
    size_t FindEdge(const MaxFlow::BasicEdge& edge);
    void SetEdgeStatus(size_t index, Status status);
    bool isValid(const BasicEdge& edge);

    void ResetState();
    void ResetFlowInfo();
    void SaveState();

    size_t GenRandNum(size_t l, size_t r);

    size_t n_ = 2, m_ = 0;
    std::vector<std::vector<size_t>> graph_ = std::vector<std::vector<size_t>>(n_);
    std::vector<size_t> dist_ = std::vector<size_t>(n_);
    std::vector<size_t> processed_neighbors_ = std::vector<size_t>(n_);
    std::vector<Edge> edges_;
    std::vector<Status> vertices_ = std::vector<Status>(n_, Status::Basic);
    size_t updated_edge_ = std::string::npos;

    size_t flow_rate_ = 0, pushed_flow_ = 0;

    observer_pattern::Observable<Data> network_observable_ =
        observer_pattern::Observable<Data> ([this] () {return GetData();});
    observer_pattern::Observable<Data> flow_observable_ =
        observer_pattern::Observable<Data> ([this] () {return GetData();});
    observer_pattern::Observable<void> сleanup_observable_;
    observer_pattern::Observable<void> unlock_observable_;

    std::mt19937 rand_generator_;

    struct State {
        size_t n, m, flow_rate = 0;
        std::vector<size_t> capacity;
        std::vector<std::vector<size_t>> graph;
        std::vector<BasicEdge> edges;
    };
    std::deque<State> previous_states_;
};

}
#endif // MAX_FLOW_H
