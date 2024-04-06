#include "drawer.h"
#include <math.h>
#include <QwtWeedingCurveFitter>
#include <QEventLoop>

namespace max_flow_app {
Drawer::Drawer(QFrame* frame): layout_(new QVBoxLayout(frame)), plot_(new QwtPlot(frame)) {
    layout_ -> addWidget(plot_.get());
    frame -> setLayout(layout_.get());
    plot_ -> setAxisScale(QwtPlot::xBottom, 0, DrawerSetup::kMaxX);
    plot_ -> setAxisScale(QwtPlot::yLeft, 0, DrawerSetup::kMaxY);
    plot_->setAxisVisible(QwtAxis::YLeft, false);
    plot_->setAxisVisible(QwtAxis::XBottom, false);
}

void Drawer::DrawGraph(const Data& data) {
    const auto& [edges, vertices, edge_id, frame_id,
                 frames_number, flow_rate, pushed_flow, _] = data;
    ResetState();
    AddFlowInfo(flow_rate, pushed_flow, DrawerSetup::kBasicColor.begin() -> second);
    for (size_t i = 0; i < edges.size(); i++) {
        QPointF begin(GetVertexPosById(vertices.size(), edges[i].u));
        QPointF end(GetVertexPosById(vertices.size(), edges[i].to));
        if (i == edge_id) {
            AddDynamicEdgeWithCapacity(begin, end, edges[i], frame_id, frames_number);
        } else {
            AddStaticEdgeWithCapacity(begin, end, edges[i]);
        }
    }
    for (size_t i = 0; i < vertices.size(); i++) {
        AddVertexWithId(GetVertexPosById(vertices.size(), i), i, vertices[i]);
    }
    assert(plot_);
    plot_ -> replot();
}

void Drawer::AddStaticEdgeWithCapacity(const QPointF& begin, const QPointF& end,
                                 const Edge& edge) {
    QPointF bend = CalcEdgeBendPos(begin, end);
    QPointF shift_head = SetVectorLength(end - bend, DrawerSetup::kCenterPadding);
    QPointF shift_tail = SetVectorLength(bend - begin, DrawerSetup::kCenterPadding);
    AddStaticEdge(begin + shift_tail, end - shift_head, edge);
    AddNumber(CalcEdgeNumberPos(begin + shift_tail, end - shift_head),
              GetEdgeNumberColor(edge), edge.delta);
}

void Drawer::AddDynamicEdgeWithCapacity(const QPointF& begin, const QPointF& end,
                                const Edge& edge, size_t frame_id, size_t frames_number) {
    QPointF bend = CalcEdgeBendPos(begin, end);
    QPointF shift_head = SetVectorLength(end - bend, DrawerSetup::kCenterPadding);
    QPointF shift_tail = SetVectorLength(bend - begin, DrawerSetup::kCenterPadding);
    AddDynamicEdge(begin + shift_tail, end - shift_head, edge, frame_id, frames_number);
    AddNumber(CalcEdgeNumberPos(begin + shift_tail, end - shift_head),
              GetEdgeNumberColor(edge), edge.delta);
}

void Drawer::AddVertexWithId(const QPointF& pos, size_t num, Status status) {
    AddVertex(pos, status);
    AddNumber(pos, GetVertexNumberColor(status), num);
}

void Drawer::AddStaticEdge(const QPointF& begin, const QPointF& end, const Edge& data) {
    CurveEdge edge;
    QPointF bend = CalcEdgeBendPos(begin, end);

    QVector<QPointF> tail_points;
    tail_points << begin << bend << end;
    edge.tail.reset(new QwtPlotCurve());
    edge.tail->setPen(GetEdgeColor(data.status), DrawerSetup::kEdgeWidth);
    edge.tail->setCurveAttribute(QwtPlotCurve::Fitted);
    edge.tail->setSamples(tail_points);
    edge.tail->attach(plot_.get());

    QVector<QPointF> head_points;
    QPointF head_begin = RotateVector(end, bend, M_PI / 12 + M_PI / 60, DrawerSetup::kEdgeHeadSide);
    QPointF head_end = RotateVector(end, bend, -M_PI / 12 + M_PI / 60, DrawerSetup::kEdgeHeadSide);
    head_points << head_begin << end << head_end;
    edge.head.reset(new QwtPlotCurve());
    edge.head->setPen(GetEdgeColor(data.status), DrawerSetup::kEdgeWidth);
    edge.head->setCurveAttribute(QwtPlotCurve::Fitted);
    edge.head->setSamples(head_points);
    edge.head->attach(plot_.get());

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
    text.setFont(DrawerSetup::kGraphFont);
    text.setColor(color);
    number.base.reset(new QwtPlotMarker());
    number.base -> setLabel(text);
    number.base -> setValue(pos);
    number.base-> attach(plot_.get());

    numbers_.push_back(std::move(number));
}

void Drawer::AddVertex(const QPointF& pos, Status status) {
    Vertex vertex;
    vertex.base.reset(new QwtPlotCurve());
    QwtSymbol* circle = new QwtSymbol();
    circle -> setStyle(QwtSymbol::Ellipse);
    circle -> setPen(GetBorderColor(status), 3);
    circle -> setBrush(GetVertexColor(status));
    circle -> setSize(DrawerSetup::kVertexRadius);
    vertex.base -> setSamples({pos});
    vertex.base -> setSymbol(circle);
    vertex.base -> attach(plot_.get());
    vertices_.push_back(std::move(vertex));
}

void Drawer::AddFlowInfo(size_t flow_rate, size_t pushed_flow, const QColor& color) {
    QwtText text(("boundary:" + std::to_string(1u << flow_rate) +
                  "\nflow pushed out:"
                  + std::to_string(pushed_flow)).data());
    text.setFont(DrawerSetup::kFlowRateFont);
    text.setColor(color);
    text.setRenderFlags(Qt::AlignLeft | Qt::AlignTop);
    flow_info_.reset(new QwtPlotMarker());
    flow_info_ -> setLabel(text);
    flow_info_ -> setValue(DrawerSetup::kFlowInfoPos);
    flow_info_ -> attach(plot_.get());
}

QPointF Drawer::CalcEdgeNumberPos(const QPointF& begin, const QPointF& end) {
    return CalcBendPos(begin, end, DrawerSetup::kCurveRate / 2);
}

QPointF Drawer::CalcBendPos(const QPointF& begin, const QPointF& end, double rate) {
    QPointF seg_vector = end - begin;
    QPointF normal(seg_vector.y(), -seg_vector.x());
    QPointF bend = (begin + 2 * end) / 3 + SetVectorLength(normal, rate);
    return bend;
}

QPointF Drawer::CalcEdgeBendPos(const QPointF& begin, const QPointF& end) {
    return CalcBendPos(begin, end, DrawerSetup::kCurveRate);
}

QColor Drawer::GetEdgeColor(Status status) const {
    return DrawerSetup::kEdgeColor.at(status);
}

QColor Drawer::GetVertexColor(Status status) const {
    auto it = DrawerSetup::kVertexColor.find(status);
    assert(it != DrawerSetup::kVertexColor.end());
    return it -> second;
}

QColor Drawer::GetBorderColor(Status status) const {
    auto it = DrawerSetup::kBasicColor.find(status);
    assert(it != DrawerSetup::kBasicColor.end());
    return it -> second;
}

QColor Drawer::GetEdgeNumberColor(const Edge& edge) const {
    auto it = DrawerSetup::kBasicColor.find(edge.status);
    assert(it != DrawerSetup::kBasicColor.end());
    return it -> second;
}

QColor Drawer::GetVertexNumberColor(Status status) const{
    auto it = DrawerSetup::kBasicColor.find(status);
    assert(it != DrawerSetup::kBasicColor.end());
    return it -> second;
}

QPointF Drawer::RotateVector(const QPointF& begin, const QPointF& end,  double angle, double len) {
    QPointF direction(end - begin);
    double polar = atan2(direction.y(), direction.x());
    QPointF res = begin + QPointF(cos(polar + angle) * len, sin(polar + angle) * len);
    return res;
}

QPointF Drawer::SetVectorLength(QPointF vec, double len) {
    double cur_len = sqrt(vec.x() * vec.x() + vec.y() * vec.y());
    return QPointF(vec.x() / cur_len * len, vec.y() / cur_len * len);
}

QPointF Drawer::GetVertexPosById(size_t number, size_t index) {
    if (!index) {
        return QPointF(DrawerSetup::kMaxX * DrawerSetup::kSinkPadingRate, DrawerSetup::kMaxY / 2);
    }
    if (index + 1 == number) {
        return QPointF(DrawerSetup::kMaxX * (1 - DrawerSetup::kSinkPadingRate), DrawerSetup::kMaxY / 2);
    }
    if (index & 1u) {
        return QPointF(DrawerSetup::kMaxX / 2, DrawerSetup::kMaxY / 2) +
               RotateVector({0, 0}, {1, 0},
                            M_PI / ((number + 1) / 2) * ((index + 1) / 2), DrawerSetup::kMaxX * (0.5 - DrawerSetup::kSinkPadingRate));
    }
    return QPointF(DrawerSetup::kMaxX / 2, DrawerSetup::kMaxY / 2) +
           RotateVector({0, 0}, {1, 0},
                        -M_PI / (number / 2) * ((index + 1) / 2), DrawerSetup::kMaxX * (0.5 - DrawerSetup::kSinkPadingRate));
}

void Drawer::AddDynamicEdge(const QPointF& begin, const QPointF& end, const Edge& data,
                            size_t frame_id, size_t frames_number) {
    CurveEdge edge;
    QPointF bend = CalcEdgeBendPos(begin, end);

    QColor new_color = GetEdgeColor(data.status);
    QColor prev_color = GetEdgeColor(mvc_messages::GetPreviousStatus(data.status));
    QVector<QPointF> tail_points;
    tail_points << begin << bend << end;
    edge.tail.reset(new QwtPlotCurve());
    if (frame_id + 1 == frames_number) {
        edge.tail->setPen(new_color, DrawerSetup::kEdgeWidth);
    } else {
        QLinearGradient gradient(begin.x() / DrawerSetup::kMaxX,
                                 (DrawerSetup::kMaxY - begin.y()) / DrawerSetup::kMaxY,
                                 end.x() / DrawerSetup::kMaxX,
                                 (DrawerSetup::kMaxY - end.y()) / DrawerSetup::kMaxY);
        gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
        gradient.setColorAt(std::max((frame_id + 1.0) / (frames_number + 1) - 0.1, 0.0), new_color);
        gradient.setColorAt(std::min((frame_id + 1.0) / (frames_number + 1) + 0.1, 1.0), prev_color);
        QPen pen(gradient, DrawerSetup::kEdgeWidth);
        edge.tail->setPen(pen);
    }
    edge.tail->setCurveAttribute(QwtPlotCurve::Fitted);
    edge.tail->setSamples(tail_points);
    edge.tail->attach(plot_.get());

    QVector<QPointF> head_points;
    QPointF head_begin = RotateVector(end, bend, M_PI / 12 + M_PI / 60, DrawerSetup::kEdgeHeadSide);
    QPointF head_end = RotateVector(end, bend, -M_PI / 12 + M_PI / 60, DrawerSetup::kEdgeHeadSide);
    head_points << head_begin << end << head_end;
    edge.head.reset(new QwtPlotCurve());
    if (frame_id + 1 == frames_number) {
        edge.head->setPen(new_color, DrawerSetup::kEdgeWidth);
    } else {
        edge.head->setPen(prev_color, DrawerSetup::kEdgeWidth);
    }
    edge.head->setCurveAttribute(QwtPlotCurve::Fitted);
    edge.head->setSamples(head_points);
    edge.head->attach(plot_.get());

    edges_.push_back(std::move(edge));
}

}
