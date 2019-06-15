#include "help_window.h"
#include "ui_help_window.h"

help_window::help_window(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::help_window)
{
    ui->setupUi(this);
    this->setWindowTitle("Help");
    this->setGeometry(
                QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter,
                                     this->size(), qApp->desktop()->availableGeometry()  )   );

    ui->textBrowser_main_win->setStyleSheet("QTextBrowser { background-color : #ffffdb ; color: black }");

    ui->textBrowser_work_win->setStyleSheet("QTextBrowser { background-color : #ffffdb ; color: black }");


}

help_window::~help_window()
{
    delete ui;
}

void help_window::on_prev_page_clicked()
{
    this->setCurrentIndex(0);
}

void help_window::on_next_page_clicked()
{
    this->setCurrentIndex(1);
}
