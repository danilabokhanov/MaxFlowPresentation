#ifndef VIEW_H
#define VIEW_H

#include "mainwindow.h"
#include "max_flow.h"
#include "drawer.h"

namespace max_flow_app {
class View : public QObject {
    Q_OBJECT
public:
    View();

    using GraphObserver = observer_pattern::Observer<MaxFlow::Data>;

    GraphObserver* GetSubscriberPtr();

    void UpdateGraphView(const MaxFlow::Data* data);

    struct ViewSendingData {
        struct Empty {

        };

        enum {
            CHANGE_VERTICES_NUMBER,
            ADD_EDGE,
            DELETE_EDGE,
            GO_NEXT
        } signal_type;
        std::variant<size_t, MaxFlow::BasicEdge, Empty> args;
    } message_;

    using ViewObservable = observer_pattern::Subject<ViewSendingData>;

    const ViewSendingData* ProduceViewMessage() const;
    void RegisterController(observer_pattern::Observer<ViewSendingData>* observer);

public slots:
    void ApplyButtonPressed();
    void AddButtonPressed();
    void DeleteButtonPressed();
    void GoNextButtonPressed();

private:

    MainWindow main_window_;
    GraphObserver graph_observer_;
    ViewObservable view_observable_;
    Drawer drawer_;
};
}
#endif  // VIEW_H
