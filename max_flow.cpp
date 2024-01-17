#include "max_flow.h"
#include <deque>
#include <sys/types.h>

namespace max_flow_app {
MaxFlow::MaxFlow()
    : n_(kDefaultN), m_(kDefaultM), graph_(n_), network_(n_), processed_neighbors_(n_),
      data_({{}, std::vector<Status> (n_, Status::Basic)}), graph_ovservable_([this] () {return GetData();}) {
}

MaxFlow::MaxFlow(size_t n, size_t m, const std::vector<BasicEdge> &edges)
    : n_(n), m_(m), graph_(n), network_(n), processed_neighbors_(n), graph_ovservable_([this] () {return GetData();}) {
    data_.edges_.reserve(m << 1);
    data_.vertices_.resize(n, Status::Basic);
    AddEdges(edges);
}

bool MaxFlow::GoNext() {
    if (is_path_processed_successfully_) {
        if ((is_path_processed_successfully_ = FindPath(0))) {
            ProcessPath();
        }
        return true;
    }
    if (!FindNetwork()) {
        current_flow_rate_ >>= 1;
        if (current_flow_rate_ == 0) {
            SetGraphToBasicStatus();
            return false;
        }
        SetGraphToBasicStatus();
    } else {
        is_path_processed_successfully_ = true;
    }
    return true;
}

void MaxFlow::AddEdge(const MaxFlow::BasicEdge &edge) {
    ResetState();
    SetGraphToBasicStatus();

    for (Edge &e : data_.edges_) {
        if (e.u == edge.u && e.to == edge.to) {
            e.delta += edge.delta;
            return;
        }
    }

    data_.edges_.push_back({.u = edge.u, .to = edge.to, .delta = edge.delta});
    data_.edges_.push_back({.u = edge.to, .to = edge.u, .delta = 0});
    graph_[edge.u].push_back(m_ * 2 - 2);
    graph_[edge.to].push_back(m_ * 2 - 1);
    m_++;
    graph_ovservable_.Notify();
}

void MaxFlow::DeleteEdge(const MaxFlow::BasicEdge &edge) {
    size_t index = 0;
    for (Edge &e : data_.edges_) {
        if (e.u == edge.u && e.to == edge.to) {
            break;
        }
        index++;
    }
    if (index == data_.edges_.size()) {
        return;
    }

    ResetState();
    SetGraphToBasicStatus();

    for (auto edges : graph_) {
        size_t deleted_index = edges.size();
        for (size_t i = 0; i < edges.size(); i++) {
            if (edges[i] == index) {
                deleted_index = i;
                break;
            }
        }
        if (deleted_index != edges.size()) {
            for (size_t i = 0; i < edges.size() - 1 - deleted_index; i++) {
                std::swap(edges[i + deleted_index], edges[i + deleted_index + 1]);
            }
            edges.pop_back();
        }
        for (auto &e : edges) {
            if (e > index) {
                e--;
            }
        }
    }
    m_--;
    graph_ovservable_.Notify();
}

bool MaxFlow::FindNetwork() {
    std::deque<size_t> queue = {0};
    std::vector<ssize_t> parent(n_, -1);
    std::vector<size_t> dist(n_, n_ + 1);
    std::vector<bool> used(n_, 0);
    used[0] = 1;
    dist[0] = 0;
    SetVertexStatus(0, Status::OnTheNetwork, true);  // Notify
    while (!queue.empty()) {
        size_t i = queue.front();
        queue.pop_front();

        if (i) {
            SetVertexAndEdgeStatus(i, parent[i], Status::OnTheNetwork);
        }
        for (auto edge_id : graph_[i]) {
            auto [u, to, delta, _] = GetEdge(edge_id);
            if (delta < current_flow_rate_) {
                continue;
            }
            if (!used[to]) {
                parent[to] = edge_id;
                dist[to] = dist[u] + 1;
                used[to] = 1;
                queue.push_back(to);
            }
        }
    }
    if (!used[n_ - 1]) {
        return false;
    }
    for (size_t i = 0; i < n_; i++) {
        network_[i].clear();
    }
    for (size_t i = 0; i < 2 * m_; i++) {
        auto [u, to, delta, _] = data_.edges_[i];
        if (dist[u] + 1 == dist[to] && delta > 0) {
            network_[u].push_back(i);
        }
    }
    for (size_t i = 0; i < n_; i++) {
        processed_neighbors_[i] = 0;
    }
    return true;
}

bool MaxFlow::FindPath(size_t vertex) {
    if (vertex == n_ - 1) {
        reversed_path_.clear();
        return true;
    }
    while (processed_neighbors_[vertex] < network_[vertex].size()) {
        size_t edge_id = network_[vertex][processed_neighbors_[vertex]++];
        auto [u, to, delta, _] = GetEdge(edge_id);
        if (delta < current_flow_rate_) {
            continue;
        }
        if (FindPath(to)) {
            reversed_path_.push_back(edge_id);
            return true;
        }
    }
    reversed_path_.clear();
    return false;
}

void MaxFlow::ProcessPath() {
    SetVertexStatus(0, Status::OnThePath, true);
    for (auto it = reversed_path_.rbegin(); it != reversed_path_.rend(); it++) {
        size_t edge_id = *it;
        SetVertexAndEdgeStatus(GetEdge(edge_id).to, edge_id, Status::OnThePath);
        GetEdge(edge_id).delta -= current_flow_rate_;
        GetReverseEdge(edge_id).delta += current_flow_rate_;
    }
    current_flow_ += current_flow_rate_;
}

MaxFlow::Edge &MaxFlow::GetEdge(size_t index) {
    return data_.edges_[index];
}

MaxFlow::Edge &MaxFlow::GetReverseEdge(size_t index) {
    return data_.edges_[index ^ 1];
}

const MaxFlow::Edge &MaxFlow::GetEdge(size_t index) const {
    return data_.edges_[index];
}

const MaxFlow::Edge &MaxFlow::GetReverseEdge(size_t index) const {
    return data_.edges_[index ^ 1];
}

void MaxFlow::ResetState() {
    current_flow_rate_ = kMaxCost;
    is_path_processed_successfully_ = false;
}

void MaxFlow::AddEdges(const std::vector<BasicEdge> &edges) {
    for (auto [u, to, delta] : edges) {
        data_.edges_.push_back({.u = u, .to = to, .delta = delta});
        data_.edges_.push_back({.u = to, .to = u, .delta = 0});
        graph_[u].push_back(data_.edges_.size() - 2);
        graph_[to].push_back(data_.edges_.size() - 1);
    }
}

void MaxFlow::SetVertexStatus(size_t vertex, const MaxFlow::Status &status, bool should_notify) {
    data_.vertices_[vertex] = status;
    if (should_notify) {
        graph_ovservable_.Notify();
    }
}

void MaxFlow::SetEdgeStatus(size_t edge_id, const MaxFlow::Status &status, bool should_notify) {
    data_.edges_[edge_id].status = status;
    if (should_notify) {
        graph_ovservable_.Notify();
    }
}

void MaxFlow::SetVertexAndEdgeStatus(size_t vertex, size_t edge_id, const MaxFlow::Status &status) {
    data_.vertices_[vertex] = status;
    data_.edges_[edge_id].status = status;
    graph_ovservable_.Notify();
}

void MaxFlow::SetGraphToBasicStatus() {
    for (auto &vertex_status : data_.vertices_) {
        vertex_status = Status::Basic;
    }
    for (auto &edge : data_.edges_) {
        edge.status = Status::Basic;
    }
    graph_ovservable_.Notify();
}

size_t MaxFlow::GetCurrentFlow() const {
    return current_flow_;
}

const MaxFlow::Data *MaxFlow::GetData() const {
    return &data_;
}

void MaxFlow::RegisterView(observer_pattern::Observer<Data>* observer) {
    graph_ovservable_.Subscribe(observer);
}

void MaxFlow::ChangeVerticesNumber(size_t new_number) {
    data_.vertices_.resize(new_number, Status::Basic);
    std::vector<Edge> new_edges;
    std::vector<std::vector<size_t>> new_graph(new_number);
    for (const auto& edge : data_.edges_) {
        if (edge.u < new_number && edge.to < new_number) {
            new_graph[edge.u].push_back(new_edges.size());
            new_edges.push_back(edge);
        }
    }
    n_ = new_number;
    m_ = (new_edges.size() >> 1);
    graph_ = std::move(new_graph);
    data_.edges_ = std::move(new_edges);
    ResetState();
    SetGraphToBasicStatus();
}
}
