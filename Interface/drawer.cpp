#include "drawer.h"
#include <numbers>
#include <QwtWeedingCurveFitter>
#include <QEventLoop>
#include "drawer_helper.h"

namespace max_flow_app {
Drawer::Drawer(QFrame* frame): layout_(new QVBoxLayout(frame)), plot_(new QwtPlot(frame)) {
    layout_ -> addWidget(plot_);
    frame -> setLayout(layout_);
    plot_ -> setAxisScale(QwtPlot::xBottom, 0, DrawerHelper::kMaxX);
    plot_ -> setAxisScale(QwtPlot::yLeft, 0, DrawerHelper::kMaxY);
    plot_->setAxisVisible(QwtAxis::YLeft, false);
    plot_->setAxisVisible(QwtAxis::XBottom, false);
}

void Drawer::DrawGraph(const Data& data) {
    const auto& [edges, vertices, pos, edge_id, frame_id,
                 frames_number, flow_rate, pushed_flow, selected_vertex] = data;
    ResetState();
    AddFlowInfo(flow_rate, pushed_flow, DrawerHelper::kBasicColor.begin() -> second);
    for (size_t i = 0; i < edges.size(); i++) {
        QPointF begin(pos[edges[i].u]);
        QPointF end(pos[edges[i].to]);
        if (i == edge_id) {
            AddDynamicEdgeWithCapacity(begin, end, edges[i], frame_id, frames_number);
        } else {
            AddStaticEdgeWithCapacity(begin, end, edges[i]);
        }
    }
    for (size_t i = 0; i < vertices.size(); i++) {
        AddVertexWithId(pos[i], i, vertices[i], i == selected_vertex);
    }
    assert(plot_);
    plot_ -> replot();
}

void Drawer::AddStaticEdgeWithCapacity(const QPointF& begin, const QPointF& end,
                                 const Edge& edge) {
    QPointF bend = CalcEdgeBendPos(begin, end);
    QPointF shift_head = DrawerHelper::SetVectorLength(end - bend, DrawerHelper::kCenterPadding);
    QPointF shift_tail = DrawerHelper::SetVectorLength(bend - begin, DrawerHelper::kCenterPadding);
    AddStaticEdge(begin + shift_tail, end - shift_head, edge);
    AddNumber(CalcEdgeNumberPos(begin + shift_tail, end - shift_head),
              GetEdgeNumberColor(edge), edge.delta);
}

void Drawer::AddDynamicEdgeWithCapacity(const QPointF& begin, const QPointF& end,
                                const Edge& edge, size_t frame_id, size_t frames_number) {
    QPointF bend = CalcEdgeBendPos(begin, end);
    QPointF shift_head = DrawerHelper::SetVectorLength(end - bend, DrawerHelper::kCenterPadding);
    QPointF shift_tail = DrawerHelper::SetVectorLength(bend - begin, DrawerHelper::kCenterPadding);
    AddDynamicEdge(begin + shift_tail, end - shift_head, edge, frame_id, frames_number);
    AddNumber(CalcEdgeNumberPos(begin + shift_tail, end - shift_head),
              GetEdgeNumberColor(edge), edge.delta);
}

void Drawer::AddVertexWithId(const QPointF& pos, size_t num, Status status, bool is_selected) {
    AddVertex(pos, status, is_selected);
    AddNumber(pos, GetVertexNumberColor(status), num);
}

void Drawer::AddStaticEdge(const QPointF& begin, const QPointF& end, const Edge& data) {
    CurveEdge edge;
    QPointF bend = CalcEdgeBendPos(begin, end);

    QVector<QPointF> tail_points;
    tail_points << begin << bend << end;
    edge.tail.reset(new QwtPlotCurve());
    edge.tail->setPen(GetEdgeColor(data.status), DrawerHelper::kEdgeWidth);
    edge.tail->setCurveAttribute(QwtPlotCurve::Fitted);
    edge.tail->setSamples(tail_points);
    edge.tail->attach(plot_);

    QVector<QPointF> head_points;
    QPointF head_begin = DrawerHelper::RotateVector(end, bend,
            std::numbers::pi / 12 + std::numbers::pi / 60, DrawerHelper::kEdgeHeadSide);
    QPointF head_end = DrawerHelper::RotateVector(end, bend,
           -std::numbers::pi / 12 + std::numbers::pi / 60, DrawerHelper::kEdgeHeadSide);
    head_points << head_begin << end << head_end;
    edge.head.reset(new QwtPlotCurve());
    edge.head->setPen(GetEdgeColor(data.status), DrawerHelper::kEdgeWidth);
    edge.head->setCurveAttribute(QwtPlotCurve::Fitted);
    edge.head->setSamples(head_points);
    edge.head->attach(plot_);

    edges_.push_back(std::move(edge));
}

void Drawer::ResetState() {
    for (auto& edge: edges_) {
        edge.tail -> detach();
        edge.head -> detach();
    }
    for (auto& num : numbers_) {
        num.base -> detach();
    }
    for (auto& vertex: vertices_) {
        vertex.base -> detach();
    }
    if (flow_info_) {
        flow_info_ -> detach();
    }
    edges_.clear();
    numbers_.clear();
    vertices_.clear();
    flow_info_.reset(nullptr);
}

void Drawer::AddNumber(const QPointF& pos, const QColor& color, size_t num) {
    Number number;
    QwtText text(std::to_string(num).data());
    text.setFont(DrawerHelper::kGraphFont);
    text.setColor(color);
    number.base.reset(new QwtPlotMarker());
    number.base -> setLabel(text);
    number.base -> setValue(pos);
    number.base-> attach(plot_);

    numbers_.push_back(std::move(number));
}

void Drawer::AddVertex(const QPointF& pos, Status status, bool is_selected) {
    Vertex vertex;
    vertex.base.reset(new QwtPlotCurve());
    QwtSymbol* circle = new QwtSymbol();
    circle -> setStyle(QwtSymbol::Ellipse);
    if (!is_selected) {
        circle -> setPen(GetBorderColor(status), 3);
        circle -> setSize(DrawerHelper::kVertexRadius);
    } else {
        circle -> setPen(GetBorderColor(status), 2);
        circle -> setSize(DrawerHelper::kVertexRadius * 1.5);
    }
    circle -> setBrush(GetVertexColor(status));
    vertex.base -> setSamples({pos});
    vertex.base -> setSymbol(circle);
    vertex.base -> attach(plot_);
    vertices_.push_back(std::move(vertex));
}

void Drawer::AddFlowInfo(size_t flow_rate, size_t pushed_flow, const QColor& color) {
    QwtText text(("boundary:" + std::to_string(1u << flow_rate) +
                  "\nflow pushed out:"
                  + std::to_string(pushed_flow)).data());
    text.setFont(DrawerHelper::kFlowRateFont);
    text.setColor(color);
    text.setRenderFlags(Qt::AlignLeft | Qt::AlignTop);
    flow_info_.reset(new QwtPlotMarker());
    flow_info_ -> setLabel(text);
    flow_info_ -> setValue(DrawerHelper::kFlowInfoPos);
    flow_info_ -> attach(plot_);
}

QPointF Drawer::CalcEdgeNumberPos(const QPointF& begin, const QPointF& end) {
    return CalcBendPos(begin, end, DrawerHelper::kCurveRate / 2);
}

QPointF Drawer::CalcBendPos(const QPointF& begin, const QPointF& end, double rate) {
    QPointF seg_vector = end - begin;
    QPointF normal(seg_vector.y(), -seg_vector.x());
    QPointF bend = (begin + 2 * end) / 3 + DrawerHelper::SetVectorLength(normal, rate);
    return bend;
}

QPointF Drawer::CalcEdgeBendPos(const QPointF& begin, const QPointF& end) {
    return CalcBendPos(begin, end, DrawerHelper::kCurveRate);
}

QColor Drawer::GetEdgeColor(Status status) const {
    return DrawerHelper::kEdgeColor.at(status);
}

QColor Drawer::GetVertexColor(Status status) const {
    auto it = DrawerHelper::kVertexColor.find(status);
    assert(it != DrawerHelper::kVertexColor.end());
    return it -> second;
}

QColor Drawer::GetBorderColor(Status status) const {
    auto it = DrawerHelper::kBasicColor.find(status);
    assert(it != DrawerHelper::kBasicColor.end());
    return it -> second;
}

QColor Drawer::GetEdgeNumberColor(const Edge& edge) const {
    auto it = DrawerHelper::kBasicColor.find(edge.status);
    assert(it != DrawerHelper::kBasicColor.end());
    return it -> second;
}

QColor Drawer::GetVertexNumberColor(Status status) const{
    auto it = DrawerHelper::kBasicColor.find(status);
    assert(it != DrawerHelper::kBasicColor.end());
    return it -> second;
}

void Drawer::AddDynamicEdge(const QPointF& begin, const QPointF& end, const Edge& data,
                            size_t frame_id, size_t frames_number) {
    CurveEdge edge;
    QPointF bend = CalcEdgeBendPos(begin, end);

    QColor new_color = GetEdgeColor(data.status);
    QColor prev_color = GetEdgeColor(kernel_messages::GetPreviousStatus(data.status));
    QVector<QPointF> tail_points;
    tail_points << begin << bend << end;
    edge.tail.reset(new QwtPlotCurve());
    if (frame_id + 1 == frames_number) {
        edge.tail->setPen(new_color, DrawerHelper::kEdgeWidth);
    } else {
        QLinearGradient gradient(begin.x() / DrawerHelper::kMaxX,
                                 (DrawerHelper::kMaxY - begin.y()) / DrawerHelper::kMaxY,
                                 end.x() / DrawerHelper::kMaxX,
                                 (DrawerHelper::kMaxY - end.y()) / DrawerHelper::kMaxY);
        gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
        gradient.setColorAt(std::max((frame_id + 1.0) / (frames_number + 1) - 0.1, 0.0), new_color);
        gradient.setColorAt(std::min((frame_id + 1.0) / (frames_number + 1) + 0.1, 1.0), prev_color);
        QPen pen(gradient, DrawerHelper::kEdgeWidth);
        edge.tail->setPen(pen);
    }
    edge.tail->setCurveAttribute(QwtPlotCurve::Fitted);
    edge.tail->setSamples(tail_points);
    edge.tail->attach(plot_);

    QVector<QPointF> head_points;
    QPointF head_begin = DrawerHelper::RotateVector(end, bend,
                std::numbers::pi / 12 + std::numbers::pi / 60, DrawerHelper::kEdgeHeadSide);
    QPointF head_end = DrawerHelper::RotateVector(end, bend,
               -std::numbers::pi / 12 + std::numbers::pi / 60, DrawerHelper::kEdgeHeadSide);
    head_points << head_begin << end << head_end;
    edge.head.reset(new QwtPlotCurve());
    if (frame_id + 1 == frames_number) {
        edge.head->setPen(new_color, DrawerHelper::kEdgeWidth);
    } else {
        edge.head->setPen(prev_color, DrawerHelper::kEdgeWidth);
    }
    edge.head->setCurveAttribute(QwtPlotCurve::Fitted);
    edge.head->setSamples(head_points);
    edge.head->attach(plot_);

    edges_.push_back(std::move(edge));
}

QwtPlot* Drawer::GetQwtPlotPtr() {
    return plot_;
}
}
