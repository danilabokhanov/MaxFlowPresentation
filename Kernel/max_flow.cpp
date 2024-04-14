#include "max_flow.h"
#include <deque>
#include <sys/types.h>
#include <chrono>
#include <string>

namespace max_flow_app {
MaxFlow::MaxFlow(size_t n, size_t m, std::initializer_list<BasicEdge> edges)
    : n_(n),
      m_(m),
      graph_(n),
      dist_(n_),
      processed_neighbors_(n),
      vertices_(n_, Status::Basic),
      rand_generator_(std::chrono::steady_clock::now().time_since_epoch().count()) {
    AddEdges(std::move(edges));
}

void MaxFlow::RunRequest() {
    SaveState();
    while (true) {
        while (!FindNetwork()) {
            if (flow_rate_ == 0) {
                SetGraphToBasicStatus(false);
                unlock_observable_.Notify();
                return;
            }
            SetGraphToBasicStatus(true);
            flow_rate_--;
        }
        std::vector<size_t> path;
        processed_neighbors_.assign(n_, 0);
        while (FindPath(0, path)) {
            ProcessPath(path);
            SetPathToBasicStatus(path);
            path.clear();
        }
        SetGraphToBasicStatus(true);
    }
}

size_t MaxFlow::FindEdge(const MaxFlow::BasicEdge& edge) {
    for (auto e = edges_.begin(); e != edges_.end(); e++) {
        if ((*e).u == edge.u && (*e).to == edge.to) {
            return e - edges_.begin();
        }
    }
    return std::string::npos;
}

void MaxFlow::AddEdgeRequest(const MaxFlow::BasicEdge& edge) {
    if (!IsValid(edge)) {
        return;
    }
    SaveState();
    AddEdge(edge);
    ResetState();
    ResetFlowInfo();
}

bool MaxFlow::IsValid(const BasicEdge& edge) {
    return edge.u < n_ && edge.to < n_ && edge.u != edge.to && edge.delta;
}

void MaxFlow::DeleteEdgeRequest(const MaxFlow::BasicEdge& edge) {
    SaveState();
    size_t index = 0;
    for (Edge& e : edges_) {
        if (e.u == edge.u && e.to == edge.to) {
            break;
        }
        index++;
    }
    if (index == edges_.size()) {
        return;
    }
    index = std::min(index, index ^ 1);
    edges_.erase(edges_.begin() + index);
    edges_.erase(edges_.begin() + index);

    for (auto& edges : graph_) {
        size_t deleted_index = edges.size();
        for (size_t i = 0; i < edges.size(); i++) {
            if ((edges[i] ^ index) <= 1) {
                deleted_index = i;
                break;
            }
        }
        if (deleted_index != edges.size()) {
            edges.erase(edges.begin() + deleted_index);
        }
        for (auto& edge : edges) {
            if (edge > index) {
                edge -= 2;
            }
        }
    }
    m_--;
    ResetState();
    ResetFlowInfo();
}

void MaxFlow::ExtendNetwork(size_t vertex, std::vector<bool>& used, std::vector<ssize_t>& parent,
                            std::deque<size_t>& queue) {
    for (auto edge_id : graph_[vertex]) {
        auto [u, to, delta, _] = GetEdge(edge_id);
        if (delta < (1u << flow_rate_)) {
            continue;
        }
        if (!used[to]) {
            parent[to] = edge_id;
            dist_[to] = dist_[u] + 1;
            used[to] = 1;
            queue.push_back(to);
        }
    }
}

void MaxFlow::ChangeNewEdgeStatus(size_t vertex, const std::vector<ssize_t>& parent) {
    if (vertex) {
        edges_[parent[vertex]].status = Status::OnTheNetwork;
        updated_edge_ = parent[vertex];
        flow_observable_.Notify();
        vertices_[vertex] = Status::OnTheNetwork;
        updated_edge_ = std::string::npos;
        flow_observable_.Notify();
    }
}

size_t MaxFlow::ExtractVertice(std::deque<size_t>& queue) {
    size_t i = queue.front();
    queue.pop_front();
    return i;
}

void MaxFlow::FindingNetworkInit(std::deque<size_t>& queue, std::vector<ssize_t>& parent,
                                 std::vector<bool>& used) {
    queue = {0};
    parent.assign(n_, -1);
    dist_.assign(n_, n_ + 1);
    used.assign(n_, 0);
    used[0] = 1;
    dist_[0] = 0;
    vertices_[0] = Status::OnTheNetwork;
    updated_edge_ = std::string::npos;
    flow_observable_.Notify();
}

bool MaxFlow::FindNetwork() {
    std::deque<size_t> queue;
    std::vector<ssize_t> parent(n_);
    std::vector<bool> used(n_);

    FindingNetworkInit(queue, parent, used);
    while (!queue.empty()) {
        size_t i = ExtractVertice(queue);
        ChangeNewEdgeStatus(i, parent);
        ExtendNetwork(i, used, parent, queue);
    }
    return used[n_ - 1];
}

bool MaxFlow::FindPath(size_t vertex, std::vector<size_t>& path) {
    if (vertex == n_ - 1) {
        return true;
    }
    while (processed_neighbors_[vertex] < graph_[vertex].size()) {
        size_t edge_id = graph_[vertex][processed_neighbors_[vertex]++];
        auto [u, to, delta, _] = GetEdge(edge_id);
        if (delta < (1 << flow_rate_) || dist_[u] + 1 != dist_[to]) {
            continue;
        }
        if (FindPath(to, path)) {
            path.push_back(edge_id);
            if (!vertex) {
                reverse(path.begin(), path.end());
            }
            return true;
        }
    }
    return false;
}

void MaxFlow::ProcessPath(const std::vector<size_t>& path) {
    vertices_[0] = Status::OnThePath;
    updated_edge_ = std::string::npos;
    flow_observable_.Notify();

    for (size_t edge_id : path) {
        GetEdge(edge_id).status = Status::OnThePath;
        updated_edge_ = edge_id;
        flow_observable_.Notify();
        GetEdge(edge_id).delta -= (1 << flow_rate_);
        GetReverseEdge(edge_id).delta += (1 << flow_rate_);
        vertices_[GetEdge(edge_id).to] = Status::OnThePath;
        updated_edge_ = std::string::npos;
        flow_observable_.Notify();
    }
    pushed_flow_ += (1 << flow_rate_);
    network_observable_.Notify();
}

MaxFlow::Edge& MaxFlow::GetEdge(size_t index) {
    return edges_[index];
}

MaxFlow::Edge& MaxFlow::GetReverseEdge(size_t index) {
    return edges_[index ^ 1];
}

const MaxFlow::Edge& MaxFlow::GetEdge(size_t index) const {
    return edges_[index];
}

const MaxFlow::Edge& MaxFlow::GetReverseEdge(size_t index) const {
    return edges_[index ^ 1];
}

void MaxFlow::ResetState() {
    for (auto& vertex_status : vertices_) {
        vertex_status = Status::Basic;
    }
    flow_rate_ = 0;
    for (auto& edge : edges_) {
        edge.status = Status::Basic;
        while ((1u << flow_rate_) < edge.delta) {
            flow_rate_++;
        }
    }
    updated_edge_ = std::string::npos;
    network_observable_.Notify();
}

void MaxFlow::ResetFlowInfo() {
    pushed_flow_ = 0;
    network_observable_.Notify();
}

void MaxFlow::AddEdges(std::initializer_list<BasicEdge> edges) {
    edges_.reserve(m_ << 1);
    for (auto [u, to, delta] : edges) {
        edges_.push_back({.u = u, .to = to, .delta = delta});
        edges_.push_back({.u = to, .to = u, .delta = 0});
        graph_[u].push_back(edges_.size() - 2);
        graph_[to].push_back(edges_.size() - 1);
    }
}

MaxFlow::Data MaxFlow::GetData() const {
    return {.edges = edges_,
            .vertices = vertices_,
            .update_edge = updated_edge_,
            .flow_rate = flow_rate_,
            .pushed_flow = pushed_flow_};
}

void MaxFlow::RegisterNetworkObserver(observer_pattern::Observer<Data>* observer) {
    assert(network_observable_.Subscribe(observer));
}

void MaxFlow::RegisterFlowObserver(observer_pattern::Observer<Data>* observer) {
    assert(flow_observable_.Subscribe(observer));
}

void MaxFlow::RegisterCleanupObserver(observer_pattern::Observer<void>* observer) {
    assert(cleanup_observable_.Subscribe(observer));
}

void MaxFlow::RegisterUnlockObserver(observer_pattern::Observer<void>* observer) {
    assert(unlock_observable_.Subscribe(observer));
}

void MaxFlow::ChangeVerticesNumberRequest(size_t new_number) {
    SaveState();
    vertices_.resize(new_number, Status::Basic);
    std::vector<Edge> new_edges;
    std::vector<std::vector<size_t>> new_graph(new_number);
    for (const auto& edge : edges_) {
        if (edge.u < new_number && edge.to < new_number) {
            new_graph[edge.u].push_back(new_edges.size());
            new_edges.push_back(edge);
        }
    }
    n_ = new_number;
    m_ = (new_edges.size() >> 1);
    graph_ = std::move(new_graph);
    edges_ = std::move(new_edges);
    ResetState();
    ResetFlowInfo();
}

size_t MaxFlow::GenRandNum(size_t l, size_t r) {
    return rand_generator_() % (r - l + 1) + l;
}

void MaxFlow::AddEdge(const BasicEdge& edge) {
    if (edge.u == edge.to) {
        return;
    }
    size_t index = FindEdge(edge);
    if (index == std::string::npos) {
        edges_.push_back({.u = edge.u, .to = edge.to, .delta = edge.delta});
        edges_.push_back({.u = edge.to, .to = edge.u, .delta = 0});
        graph_[edge.u].push_back(m_ << 1);
        graph_[edge.to].push_back((m_ << 1) + 1);
        m_++;
        return;
    }
    edges_[index].delta += edge.delta;
}

void MaxFlow::GenRandomSampleRequest() {
    SaveState();
    n_ = GenRandNum(kMinVerticesNum, kMaxVerticesNum);
    m_ = 0;
    edges_.clear();
    graph_.clear();
    graph_.resize(n_);
    vertices_.resize(n_);
    for (size_t i = 1; i < n_; i++) {
        AddEdge({.u = GenRandNum(0, i - 1), .to = i, .delta = GenRandNum(1, kMaxEdgeCapacity)});
    }
    ResetState();
    ResetFlowInfo();
}

void MaxFlow::SetEdgeStatus(size_t index, Status status) {
    Edge& edge = GetEdge(index);
    vertices_[edge.u] = status;
    edge.status = status;
    vertices_[edge.to] = status;
}

void MaxFlow::SetPathToBasicStatus(const std::vector<size_t>& path) {
    for (auto index : path) {
        SetEdgeStatus(index, Status::OnTheNetwork);
    }
    updated_edge_ = std::string::npos;
    flow_observable_.Notify();
}

void MaxFlow::SetGraphToBasicStatus(bool is_flow_notification) {
    for (Status& status : vertices_) {
        status = Status::Basic;
    }
    for (Edge& edge : edges_) {
        edge.status = Status::Basic;
    }
    updated_edge_ = std::string::npos;
    if (is_flow_notification) {
        flow_observable_.Notify();
        return;
    }
    network_observable_.Notify();
}

void MaxFlow::SaveState() {
    State state{.n = n_, .m = m_, .flow_rate = flow_rate_, .graph = graph_};
    std::vector<BasicEdge> edges;
    for (auto [u, to, delta, _] : edges_) {
        edges.push_back({u, to, delta});
    }
    state.edges = std::move(edges);
    previous_states_.push_back(std::move(state));
    while (previous_states_.size() > kStatesStorageSize) {
        previous_states_.pop_front();
    }
}

void MaxFlow::RecoverPrevStateRequest() {
    if (previous_states_.empty()) {
        return;
    }
    State state = std::move(previous_states_.back());
    previous_states_.pop_back();
    n_ = state.n;
    m_ = state.m;
    flow_rate_ = state.flow_rate;
    pushed_flow_ = 0;
    graph_ = std::move(state.graph);
    edges_.clear();
    vertices_.resize(n_);
    for (auto [u, to, delta] : state.edges) {
        edges_.push_back({.u = u, .to = to, .delta = delta, .status = Status::Basic});
    }
    network_observable_.Notify();
    cleanup_observable_.Notify();
    unlock_observable_.Notify();
}
}  // namespace max_flow_app
