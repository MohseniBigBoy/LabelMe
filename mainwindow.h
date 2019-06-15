#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "defs.h"
#include "roi_selection.h"

#include <QStyle>
#include <QDesktopWidget>

namespace Ui {
class MainWindow;
}

class ROI_SELECTION;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_lnEusername_textEdited(const QString &arg1);

    void on_rdbtn_edit_clicked(bool checked);

    //    void on_rdbtn_cls_clicked(bool checked);

    void on_rdbtn_roi_clicked(bool checked);

    void on_chBox_save_result_images_clicked(bool checked);

    void on_btn_load_img_clicked();

    void on_btn_load_text_clicked();

    void on_btn_store_img_clicked();

    void on_btn_store_text_clicked();

    void on_btn_enter_clicked();



    void on_action_Help_triggered();

private:
    Ui::MainWindow *ui;

    bool valid_load_img_path=false;
    bool valid_store_img_path=false;
    bool valid_load_text_path=false;
    bool valid_store_text_path=false;
    void let_to_enter();
    void clear_input_data();

    help_window help_win ;

protected:

};

#endif // MAINWINDOW_H
