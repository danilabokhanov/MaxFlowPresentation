#ifndef DRAWER_H
#define DRAWER_H

#include <QFrame>
#include "qwt_plot.h"
#include <QVBoxLayout>
#include <QwtSymbol>
#include <QwtPlotCurve>
#include <QwtPlotMarker>
#include <QwtText>
#include "Kernel/mvc_messages.h"

namespace max_flow_app {
class Drawer {
public:
    using Edge = mvc_messages::Edge;
    using Data = mvc_messages::MaxFlowData;
    using Status = mvc_messages::Status;

    Drawer(QFrame* frame);
    void DrawGraph(const Data& data);
private:

    inline static const double kCurveRate = 0.5, kEdgeHeadSide = 0.2, kVertexRadius = 30,
        kCenterPadding = 0.3, kMaxX = 10, kMaxY = 10, kSinkPadingRate = 0.05;

    inline static const QFont kGraphFont = QFont("Arial", 16, QFont::Bold);
    inline static const std::map<Status, QColor> kVertexColor = {{Status::Basic,  {192, 192, 192}},
                                                                {Status::OnTheNetwork, {102, 178, 255}},
                                                                 {Status::OnThePath, {255, 102, 102}}};
    inline static const std::map<Status, QColor> kEdgeColor = {{Status::Basic,  {30, 30, 30}},
                                                                 {Status::OnTheNetwork, {0, 102, 204}},
                                                                 {Status::OnThePath,{204, 0, 0}}};
    inline static const std::map<Status, QColor> kBasicColor = {{Status::Basic,  {0, 0, 0}},
                                                                     {Status::OnTheNetwork, {0, 0, 102}},
                                                                {Status::OnThePath, {102, 0, 0}}};

    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QwtPlot> plot_;

    struct Vertex {
        std::unique_ptr<QwtPlotCurve> base;
    };

    struct CurveEdge {
        std::unique_ptr<QwtPlotCurve> head;
        std::unique_ptr<QwtPlotCurve> tail;
    };

    struct Number {
        std::unique_ptr<QwtPlotMarker> base;
    };

    std::list<Vertex> vertices_;
    std::list<CurveEdge> edges_;
    std::list<Number> numbers_;

    void AddEdge(const QPointF& begin, const QPointF& end, const Edge& edge);
    void AddVertex(const QPointF& pos, size_t index, Status status);
    void AddNumber(const QPointF& pos, const QFont& font, const QColor& color, size_t num);
    void AddEdgeWithCapacity(const QPointF& begin, const QPointF& end, const Edge& edge);
    void AddVertexWithId(const QPointF& pos, size_t num, Status status);

    QPointF CalcEdgeNumberPos(const QPointF& begin, const QPointF& end);
    QPointF CalcBendPos(const QPointF& begin, const QPointF& end, double rate);
    QPointF CalcEdgeBendPos(const QPointF& begin, const QPointF& end);
    QPointF RotateVector(const QPointF& begin, const QPointF& end,  double angle, double len);
    QPointF SetVectorLength(QPointF vec, double len);
    QPointF GetVertexPosById(size_t number, size_t index);
    QColor GetEdgeColor(const Edge& edge) const;
    QColor GetVertexColor(Status status) const;
    QColor GetBorderColor(Status status) const;
    QColor GetEdgeNumberColor(const Edge& edge) const;
    QColor GetVertexNumberColor(Status status) const;
    void ResetState();
};

}
#endif  // DRAWER_H
