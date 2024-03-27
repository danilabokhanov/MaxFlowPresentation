#include "drawer.h"
#include <math.h>
#include <QwtWeedingCurveFitter>
#include <QEventLoop>

namespace max_flow_app {
Drawer::Drawer(QFrame* frame): layout_(new QVBoxLayout(frame)), plot_(new QwtPlot(frame)) {
    plot_ -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout_ -> addWidget(plot_.get());
    frame -> setLayout(layout_.get());
    plot_ -> setAxisScale(QwtPlot::xBottom, 0, Palette::kMaxX);
    plot_ -> setAxisScale(QwtPlot::yLeft, 0, Palette::kMaxY);
}

void Drawer::DrawGraph(const Data& data) {
    const auto& [edges, vertices] = data;
    ResetState();
    for (const auto& edge : edges) {
        QPointF begin(GetVertexPosById(vertices.size(), edge.u));
        QPointF end(GetVertexPosById(vertices.size(), edge.to));
        AddEdgeWithCapacity(begin, end, edge);
    }
    for (size_t i = 0; i < vertices.size(); i++) {
        AddVertexWithId(GetVertexPosById(vertices.size(), i), i, vertices[i]);
    }
    assert(plot_);
    plot_ -> replot();
}

void Drawer::AddEdgeWithCapacity(const QPointF& begin, const QPointF& end, const Edge& edge) {
    QPointF bend = CalcEdgeBendPos(begin, end);
    QPointF shift_head = SetVectorLength(end - bend, Palette::kCenterPadding);
    QPointF shift_tail = SetVectorLength(bend - begin, Palette::kCenterPadding);
    AddEdge(begin + shift_tail, end - shift_head, edge);
    AddNumber(CalcEdgeNumberPos(begin + shift_tail, end - shift_head),
              Palette::kGraphFont, GetEdgeNumberColor(edge), edge.delta);
}

void Drawer::AddVertexWithId(const QPointF& pos, size_t num, Status status) {
    AddVertex(pos, status);
    AddNumber(pos, Palette::kGraphFont, GetVertexNumberColor(status), num);
}

void Drawer::AddEdge(const QPointF& begin, const QPointF& end, const Edge& data) {
    CurveEdge edge;
    QPointF bend = CalcEdgeBendPos(begin, end);

    QVector<QPointF> tail_points;
    tail_points << begin << bend << end;
    edge.tail.reset(new QwtPlotCurve());
    edge.tail->setPen(GetEdgeColor(data), Palette::kEdgeWidth);
    edge.tail->setCurveAttribute(QwtPlotCurve::Fitted);
    edge.tail->setSamples(tail_points);
    edge.tail->attach(plot_.get());

    QVector<QPointF> head_points;
    QPointF head_begin = RotateVector(end, bend, M_PI / 12 + M_PI / 60, Palette::kEdgeHeadSide);
    QPointF head_end = RotateVector(end, bend, -M_PI / 12 + M_PI / 60, Palette::kEdgeHeadSide);
    head_points << head_begin << end << head_end;
    edge.head.reset(new QwtPlotCurve());
    edge.head->setPen(GetEdgeColor(data), Palette::kEdgeWidth);
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
    edges_.clear();
    numbers_.clear();
    vertices_.clear();
}

void Drawer::AddNumber(const QPointF& pos, const QFont& font, const QColor& color, size_t num) {
    Number number;
    QwtText text(std::to_string(num).data());
    text.setFont(font);
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
    circle -> setSize(Palette::kVertexRadius);
    vertex.base -> setSamples({pos});
    vertex.base -> setSymbol(circle);
    vertex.base -> attach(plot_.get());
    vertices_.push_back(std::move(vertex));
}

QPointF Drawer::CalcEdgeNumberPos(const QPointF& begin, const QPointF& end) {
    return CalcBendPos(begin, end, Palette::kCurveRate / 2);
}

QPointF Drawer::CalcBendPos(const QPointF& begin, const QPointF& end, double rate) {
    QPointF seg_vector = end - begin;
    QPointF normal(seg_vector.y(), -seg_vector.x());
    QPointF bend = (begin + 2 * end) / 3 + SetVectorLength(normal, rate);
    return bend;
}

QPointF Drawer::CalcEdgeBendPos(const QPointF& begin, const QPointF& end) {
    return CalcBendPos(begin, end, Palette::kCurveRate);
}

QColor Drawer::GetEdgeColor(const Edge& edge) const {
    return Palette::kEdgeColor.at(edge.status);
}

QColor Drawer::GetVertexColor(Status status) const {
    auto it = Palette::kVertexColor.find(status);
    assert(it != Palette::kVertexColor.end());
    return it -> second;
}

QColor Drawer::GetBorderColor(Status status) const {
    auto it = Palette::kBasicColor.find(status);
    assert(it != Palette::kBasicColor.end());
    return it -> second;
}

QColor Drawer::GetEdgeNumberColor(const Edge& edge) const {
    auto it = Palette::kBasicColor.find(edge.status);
    assert(it != Palette::kBasicColor.end());
    return it -> second;
}

QColor Drawer::GetVertexNumberColor(Status status) const{
    auto it = Palette::kBasicColor.find(status);
    assert(it != Palette::kBasicColor.end());
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
        return QPointF(Palette::kMaxX * Palette::kSinkPadingRate, Palette::kMaxY / 2);
    }
    if (index + 1 == number) {
        return QPointF(Palette::kMaxX * (1 - Palette::kSinkPadingRate), Palette::kMaxY / 2);
    }
    if (index & 1u) {
        return QPointF(Palette::kMaxX / 2, Palette::kMaxY / 2) +
               RotateVector({0, 0}, {1, 0},
                            M_PI / ((number + 1) / 2) * ((index + 1) / 2), Palette::kMaxX * (0.5 - Palette::kSinkPadingRate));
    }
    return QPointF(Palette::kMaxX / 2, Palette::kMaxY / 2) +
           RotateVector({0, 0}, {1, 0},
                        -M_PI / (number / 2) * ((index + 1) / 2), Palette::kMaxX * (0.5 - Palette::kSinkPadingRate));
}
}
