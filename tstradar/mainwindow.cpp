#include "mainwindow.h"

#include "radarwidget.h"
#include "RadarController.h"
#include "radarctrldlg.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    m_widget = new RadarWidget();
    auto controller = m_widget->controller();
    controller->setRange(12.0f);
    setCentralWidget(m_widget);

    m_ctrlDlg = new RadarCtrlDlg(this);
    m_ctrlDlg->setMinimumSize(150, -1);
    m_ctrlDlg->setRadarController(controller);

    QWidget* central_widget = new QWidget(this);
    {
        QHBoxLayout* horizontalLayout = new QHBoxLayout(central_widget);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->addWidget(m_widget);
        horizontalLayout->addWidget(m_ctrlDlg);
        horizontalLayout->setStretch(0, 1);
    }
    setCentralWidget(central_widget);

    this->setStyleSheet("background: rgb(85, 85, 127)");
    controller->setBackgroundColor(QColor(85, 85, 127));

}

MainWindow::~MainWindow()
{}
