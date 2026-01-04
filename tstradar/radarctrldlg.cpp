#include "radarctrldlg.h"

#include "RadarController.h"

RadarCtrlDlg::RadarCtrlDlg(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    m_controller = nullptr;
}

RadarCtrlDlg::~RadarCtrlDlg()
{}

void RadarCtrlDlg::setRadarController(RadarController* c)
{
    Q_ASSERT(c != nullptr);
    m_controller = c;

    connect(m_controller, &RadarController::cursorPosChanged, this, &RadarCtrlDlg::slotCursorPosChanged);
}

void RadarCtrlDlg::on_pushButtonRangeAdd_clicked()
{
    m_controller->addRange();
}

void RadarCtrlDlg::on_pushButtonRangeSub_clicked()
{
    m_controller->subRange();
}

void RadarCtrlDlg::on_checkBoxEBL1_clicked()
{
    m_controller->setEBL1On(!m_controller->EBL1On());
}

void RadarCtrlDlg::on_checkBoxEBL2_clicked()
{
    m_controller->setEBL2On(!m_controller->EBL2On());
}

void RadarCtrlDlg::on_checkBoxVRM1_clicked()
{
    m_controller->setVRM1On(!m_controller->VRM1On());
}

void RadarCtrlDlg::on_checkBoxVRM2_clicked()
{
    m_controller->setVRM2On(!m_controller->VRM2On());
}

void RadarCtrlDlg::on_horizontalSliderGain_valueChanged(int val)
{
    float gain = val / 10.0f;
    m_controller->setGain(gain);
}

void RadarCtrlDlg::on_horizontalSliderTune_valueChanged(int val)
{
    float tune = val / 10.0f;
    m_controller->setTune(tune);
}

void RadarCtrlDlg::on_horizontalSliderSea_valueChanged(int val)
{

}

void RadarCtrlDlg::on_horizontalSliderRain_valueChanged(int val)
{

}

void RadarCtrlDlg::slotCursorPosChanged(float lat, float lon, float bearing, float distNM)
{
    ui.lineEditCursorLat->setText(QString::number(lat));
    ui.lineEditCursorLon->setText(QString::number(lon));
    ui.lineEditBRG->setText(QString::number(bearing));
    ui.lineEditDIST->setText(QString::number(distNM));
}
