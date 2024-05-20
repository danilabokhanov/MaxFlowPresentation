#ifndef DRAWER_H
#define DRAWER_H

#include <QVBoxLayout>
#include <QwtSymbol>
#include <QwtPlotCurve>
#include <QwtPlotMarker>
#include <QwtText>
#include <QwtPlotTextLabel>
#include <memory>
#include "qwt_plot.h"
#include "Kernel/kernel_messages.h"
#include "interface_messages.h"

namespace max_flow_app {
class Drawer {
public:
    using Data = interface_messages::GeomModelData;

    Drawer(QFrame* frame);

    void DrawGraph(const Data& data);
    QwtPlot* GetQwtPlotPtr();

private:
    using Edge = kernel_messages::Edge;
    using Status = kernel_messages::Status;

    void AddStaticEdge(const QPointF& begin, const QPointF& end, const Edge& edge);
    void AddDynamicEdge(const QPointF& begin, const QPointF& end, const Edge& edge, size_t frame_id,
                        size_t frames_number);
    void AddVertex(const QPointF& pos, Status status, bool is_selected);
    void AddNumber(const QPointF& pos, const QColor& color, size_t num);
    void AddStaticEdgeWithCapacity(const QPointF& begin, const QPointF& end, const Edge& edge);
    void AddDynamicEdgeWithCapacity(const QPointF& begin, const QPointF& end, const Edge& edge,
                                    size_t frame_id, size_t frames_number);
    void AddVertexWithId(const QPointF& pos, size_t num, Status status, bool is_selected);
    void AddFlowInfo(size_t flow_rate, size_t pushed_flow, const QColor& color);
    QPointF CalcEdgeNumberPos(const QPointF& begin, const QPointF& end);
    QPointF CalcBendPos(const QPointF& begin, const QPointF& end, double rate);
    QPointF CalcEdgeBendPos(const QPointF& begin, const QPointF& end);
    QColor GetEdgeColor(Status status) const;
    QColor GetVertexColor(Status status) const;
    QColor GetBorderColor(Status status) const;
    QColor GetEdgeNumberColor(const Edge& edge) const;
    QColor GetVertexNumberColor(Status status) const;
    void ResetState();

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

    QVBoxLayout* layout_;
    QwtPlot* plot_;
};

}  // namespace max_flow_app
#endif  // DRAWER_H
