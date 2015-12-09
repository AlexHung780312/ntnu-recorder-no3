#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtXlsx>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    QString numberToAlpha(int number, bool isLower);
    ~Widget();

private slots:
    void on_pushButton_select_excel_clicked();
    void on_comboBox_select_tab_currentIndexChanged(const QString &arg1);
    void on_lineEdit_excel_path_textChanged(const QString &arg1);

    void on_pushButton_select_output_dir_clicked();

    void on_spinBox_No_valueChanged(int arg1);
    void updateText();

    void on_listWidget_show_itemSelectionChanged();

private:
    QXlsx::Document *xlsx;
    Ui::Widget *ui;
};

#endif // WIDGET_H
