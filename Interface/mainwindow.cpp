#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace max_flow_app {
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui_(std::make_unique<Ui::MainWindow>()) {
    ui_->setupUi(this);
    setWindowTitle("MaxFlow");
}

MainWindow::~MainWindow() = default;

QFrame* MainWindow::GetQwtFramePtr() const {
    return ui_->frame_qwt;
}

QSpinBox* MainWindow::GetVerticesSpinBoxPtr() const {
    return ui_->spinBox_vertices;
}

QPushButton* MainWindow::GetVerticesButtonPtr() const {
    return ui_->button_apply;
}

QSpinBox* MainWindow::GetUSpinBoxPtr() const {
    return ui_->spinBox_u;
}

QSpinBox* MainWindow::GetVSpinBoxPtr() const {
    return ui_->spinBox_v;
}

QSpinBox* MainWindow::GetWeightSpinBoxPtr() const {
    return ui_->spinBox_weight;
}

QPushButton* MainWindow::GetAddButtonPtr() const {
    return ui_->button_add;
}

QPushButton* MainWindow::GetDeleteButtonPtr() const {
    return ui_->button_delete;
}

QPushButton* MainWindow::GetRunButtonPtr() const {
    return ui_->button_run;
}

QPushButton* MainWindow::GetRandomSampleButtonPtr() const {
    return ui_->button_random_sample;
}

QPushButton* MainWindow::GetSkipButtonPtr() const {
    return ui_->button_skip;
}

QPushButton* MainWindow::GetCancelButtonPtr() const {
    return ui_->button_cancel;
}

size_t MainWindow::GetUSpinBoxNumber() const {
    return ui_->spinBox_u->value();
}

size_t MainWindow::GetVSpinBoxNumber() const {
    return ui_->spinBox_v->value();
}

size_t MainWindow::GetVerticesSpinBoxNumber() const {
    return ui_->spinBox_vertices->value();
}

size_t MainWindow::GetWeightBoxNumber() const {
    return ui_->spinBox_weight->value();
}
}  // namespace max_flow_app
