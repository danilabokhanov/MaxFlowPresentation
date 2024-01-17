#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace max_flow_app {
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui_(std::make_unique<Ui::MainWindow>())
{
    ui_->setupUi(this);
}


MainWindow::~MainWindow() {

}

QFrame* MainWindow::GetQwtFramePtr() const {
    return ui_ ->frame_qwt;
}

QSpinBox* MainWindow::GetVerticesSpinBoxPtr() const {
    return ui_ ->spinBox_vertices;
}

QPushButton* MainWindow::GetVerticesButtonPtr() const {
    return ui_ -> button_apply;
}

QSpinBox* MainWindow::GetUSpinBoxPtr() const {
    return ui_ -> spinBox_u;
}

QSpinBox* MainWindow::GetVSpinBoxPtr() const {
    return ui_ -> spinBox_v;
}

QSpinBox* MainWindow::GetWeightSpinBoxPtr() const {
    return ui_ -> spinBox_weight;
}

QPushButton* MainWindow::GetAddButtonPtr() const {
    return ui_ -> button_add;
}

QPushButton* MainWindow::GetDeleteButtonPtr() const {
    return ui_ -> button_delete;
}

QPushButton* MainWindow::GetGoNextButtonPtr() const {
    return ui_ -> button_go_next;
}
}
