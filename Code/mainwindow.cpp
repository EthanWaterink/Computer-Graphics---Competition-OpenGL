#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "math.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->mainView->comboBox_lookingAt = ui->lookAt;
    ui->mainView->comboBox_lookingFrom = ui->lookFrom;
}

MainWindow::~MainWindow() {
    delete ui;
}

// --- Functions that listen for widget events
// forewards to the mainview


void MainWindow::on_PhongButton_toggled(bool checked) {
    if (checked) {
        ui->mainView->setShadingMode(MainView::PHONG);
        ui->mainView->update();
    }
}

void MainWindow::on_height_valueChanged(int value) {
    ui->mainView->setHeight(value/100.0f);
}


void MainWindow::on_speed_valueChanged(int value) {
    ui->mainView->setSpeed(value/100.0f);
}

void MainWindow::on_fov_valueChanged(int value) {
    ui->mainView->setCameraFOV(value/1000.0f);
}

void MainWindow::on_angle_valueChanged(int value) {
    ui->mainView->setAngle(value/10.0f);
}
