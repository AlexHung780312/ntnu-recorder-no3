#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtXlsx>
#include <QtMultimedia>
#include <audioinput.h>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    QString numberToAlpha(int number, bool isLower);
    QString getTargetFile();
    ~Widget();

private slots:
    void on_pushButton_select_excel_clicked();
    void on_comboBox_select_tab_currentIndexChanged(const QString &arg1);
    void on_lineEdit_excel_path_textChanged(const QString &arg1);

    void on_pushButton_select_output_dir_clicked();

    void on_spinBox_No_valueChanged(int arg1);
    void updateText();
    void drawWaveform();

    void on_listWidget_show_itemSelectionChanged();

    void on_pushButton_play_clicked();

    void on_pushButton_rec_clicked();

    void on_lineEdit_output_dir_textChanged(const QString &arg1);

    void on_pushButton_go_back_clicked();

    void on_pushButton_go_next_clicked();

private:
    QXlsx::Document *xlsx;
    AudioInput *recorder;
    Ui::Widget *ui;
};

#endif // WIDGET_H
