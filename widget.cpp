#include "widget.h"
#include "ui_widget.h"
#include <QtWidgets>
#include <QtDebug>
#include <QtMultimedia>
#include <wavheader.h>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    xlsx=NULL;
    ui->widget_waveform->yAxis->setVisible(true);
    ui->widget_waveform->xAxis->setVisible(true);
    ui->widget_waveform->yAxis->setRange(-1, 1);
    ui->widget_waveform->xAxis->axisRect()->setRangeDrag(Qt::Horizontal);
    recorder=NULL;
    ui->pushButton_go_back->setShortcut(QKeySequence(Qt::Key_Left));
    ui->pushButton_go_next->setShortcut(QKeySequence(Qt::Key_Right));
    ui->pushButton_rec->setShortcut(QKeySequence(Qt::Key_Space));
    ui->pushButton_play->setShortcut(QKeySequence(Qt::Key_P));
}

Widget::~Widget()
{
    delete ui;
    if(xlsx!=NULL) delete xlsx;
    xlsx = NULL;
}

void Widget::on_pushButton_select_excel_clicked()
{
    QString fpath = QFileDialog::getOpenFileName(this,
        tr("Open xlsx"), QDir::currentPath(), tr("Excel Files (*.xlsx *.xls)"));
    if (QFile::exists(fpath)) {
        ui->lineEdit_excel_path->setText(fpath);
    }
}

QString Widget::numberToAlpha(int number, bool isLower = false)
{
    QString returnVal = "";
    char c = isLower ? 'a'-1 : 'A'-1;
    while (number >= 0)
    {
        returnVal = (char)(c + number % 26) + returnVal;
        number /= 26;
        number--;
    }

    return returnVal;
}
void Widget::on_comboBox_select_tab_currentIndexChanged(const QString &arg1)
{
    xlsx->selectSheet(ui->comboBox_select_tab->currentText());
    qInfo() << "select sheet: " << ui->comboBox_select_tab->currentText();
    /*update columns*/
    //找第一個row最後一個有元素的column，決定column數量
    int n_cols = 0;
    for (int col=1;col<=100;col++) {
        QString elem = xlsx->read(1,col).toString();
        if (!elem.isEmpty()) {
            n_cols = col;
        }
    }
    qInfo() << "n_cols=" << n_cols;
    //
    ui->listWidget_show->clear();
    ui->listWidget_filename->clear();
    for (int col=1;col<=n_cols;col++) {
        QString alpha = numberToAlpha(col);
        ui->listWidget_show->addItem(alpha);
        ui->listWidget_filename->addItem(alpha);
    }
    /* 更新總數 */
    int n_rows = 0;
    for (int row=1;row<3000;row++) {
        QString elem = xlsx->read(row,1).toString();
        if (!elem.isEmpty()) {
            n_rows = row;
        }
    }
    qInfo() << "n_rows=" << n_rows;
    ui->lineEdit_total->setText(QString::number(n_rows));
    /* 題號重新開始 */
    ui->spinBox_No->setMaximum(n_rows);
    ui->spinBox_No->setMinimum(1);
    ui->spinBox_No->setValue(1);
    ui->spinBox_No->setEnabled(true);
}


void Widget::on_lineEdit_excel_path_textChanged(const QString &arg1)
{
    if(QFile::exists(ui->lineEdit_excel_path->text())){
        //讀取xml
        if(xlsx!=NULL) delete xlsx;
        xlsx = new QXlsx::Document(ui->lineEdit_excel_path->text(),this);
        //update選項
        ui->comboBox_select_tab->clear();
        ui->comboBox_select_tab->insertItems(0,xlsx->sheetNames());
        //
        ui->comboBox_select_tab->setFocus();
        ui->comboBox_select_tab->setEnabled(true);
        xlsx->selectSheet(ui->comboBox_select_tab->currentText());
    }else{
        ui->comboBox_select_tab->clear();
        ui->comboBox_select_tab->setEnabled(false);
    }
}

void Widget::on_pushButton_select_output_dir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr("Select Output Directory"),QDir::currentPath());
    ui->lineEdit_output_dir->setText(dir);
}
void Widget::updateText()
{
    //顯示題目
    QList<QListWidgetItem *> all_item = ui->listWidget_show->selectedItems();
    QString msg="";
    for (auto i=all_item.begin();i!=all_item.end();i++) {
        msg = msg + xlsx->read((*i)->text() + QString::number(ui->spinBox_No->value())).toString() + " ";
    }
    ui->label_text->setText(msg);
}
void Widget::on_spinBox_No_valueChanged(int arg1)
{
    updateText();
    drawWaveform();
    ui->pushButton_play->setEnabled(true);
    ui->pushButton_rec->setEnabled(true);
    ui->pushButton_go_back->setEnabled(true);
    ui->pushButton_go_next->setEnabled(true);
}

void Widget::on_listWidget_show_itemSelectionChanged()
{
    updateText();
}
QString Widget::getTargetFile()
{
    QList<QListWidgetItem *> all_item = ui->listWidget_filename->selectedItems();
    if (all_item.isEmpty()) return "";
    QString ofilename=xlsx->read(all_item.first()->text()+QString::number(ui->spinBox_No->value())).toString();
    for (auto i=all_item.begin()+1;i!=all_item.end();i++) {
        ofilename +=  "_" + xlsx->read((*i)->text() + QString::number(ui->spinBox_No->value())).toString();
    }
    ofilename += ".wav";
    return ui->lineEdit_output_dir->text() + "/" + ofilename;
}
void Widget::drawWaveform()
{
    QString opath = getTargetFile();
    qInfo() << "target: " << opath;
    if (QFile::exists(opath)) {
        QFile fin(opath);
        fin.open(QFile::ReadOnly);
        fin.seek(-1);
        fin.seek(40);//跳過header
        unsigned int nbyte;
        fin.read(reinterpret_cast<char *>(&nbyte), 4);
        unsigned int nsample = nbyte / 2;
        QVector<double> x(nsample),y(nsample);
        //Q_ASSERT(nbyte % nsample == 0);
        {
            short* data = new short[nsample];
            fin.read(reinterpret_cast<char*>(data),nbyte);
            //建立X,Y
            for (unsigned int i=0;i<nsample;i++) {
                x[i] = i / 16000.0;//單位:秒
                y[i] = data[i] / 32768.0;//[-1~1]
            }
            delete [] data;
        }
        fin.close();
        qInfo() << "nsample=" << nsample;
        ui->widget_waveform->addGraph();
        ui->widget_waveform->graph(0)->setData(x,y);
        ui->widget_waveform->replot();
        qInfo() << "replot";
    } else {
        ui->widget_waveform->clearGraphs();
        ui->widget_waveform->replot();
        qInfo() << "clear Graphs";
    }
}

void Widget::on_pushButton_play_clicked()
{
    QString opath = getTargetFile();
    if (!QFile::exists(opath)) {
        qInfo() << "wave file not found";
        return;
    }
    qInfo() << "play " << opath;
    QSound::play(opath);
}

void Widget::on_pushButton_rec_clicked()
{
    ui->pushButton_rec->setEnabled(false);
    ui->pushButton_play->setEnabled(false);
    ui->spinBox_No->setEnabled(false);
    ui->pushButton_go_back->setEnabled(false);
    ui->pushButton_go_next->setEnabled(false);
    QString opath = getTargetFile();
    qInfo() << "target wav: " << opath;
    if (recorder == NULL)
        recorder = new AudioInput(this);

    if (recorder->state() != QAudio::ActiveState) {
        qInfo() << "stop/pause state -> rec";
        QFile fptr(opath);
        fptr.remove();
        recorder->startRecording(opath);
        ui->pushButton_rec->setIcon(QIcon(":/stop"));
    } else {
        qInfo() << "rec state->stop";
        recorder->terminateRecording();
        ui->pushButton_rec->setIcon(QIcon(":/rec"));
    }
    drawWaveform();
    ui->pushButton_rec->setEnabled(true);
    ui->pushButton_play->setEnabled(true);
    ui->spinBox_No->setEnabled(true);
    ui->pushButton_go_back->setEnabled(true);
    ui->pushButton_go_next->setEnabled(true);
}

void Widget::on_lineEdit_output_dir_textChanged(const QString &arg1)
{
    drawWaveform();
}

void Widget::on_pushButton_go_back_clicked()
{
    ui->spinBox_No->setValue(std::max(ui->spinBox_No->minimum(),ui->spinBox_No->value()-1));
}

void Widget::on_pushButton_go_next_clicked()
{
    ui->spinBox_No->setValue(std::min(ui->spinBox_No->maximum(),ui->spinBox_No->value()+1));
}
