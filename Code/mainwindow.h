#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_PhongButton_toggled(bool checked);

    void on_height_valueChanged(int value);
    void on_speed_valueChanged(int value);
    void on_fov_valueChanged(int value);
    void on_angle_valueChanged(int value);
};

#endif // MAINWINDOW_H
