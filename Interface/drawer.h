#ifndef DRAWER_H
#define DRAWER_H

#include <QVBoxLayout>
#include <QwtSymbol>
#include <QwtPlotCurve>
#include <QwtPlotMarker>
#include <QwtText>
#include "qwt_plot.h"
#include "palette.h"

namespace max_flow_app {
class Drawer {
public:
    using Edge = mvc_messages::Edge;
    using Data = mvc_messages::MaxFlowData;
    using Status = mvc_messages::Status;

    Drawer(QFrame* frame);
    void DrawGraph(const Data& data);
private:
    inline static const Palette pattle;

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
    void AddVertex(const QPointF& pos, Status status);
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
