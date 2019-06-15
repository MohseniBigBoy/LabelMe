#ifndef ROI_SELECTION_H
#define ROI_SELECTION_H

#include <QStackedWidget>
#include "defs.h"
#include "customscene.h"

#include "help_window.h"

#include <exception>

//#include <QMainWindow>
//#include <QApplication>

//#define _alto  300
//#define _ancho 700



namespace Ui {
class ROI_SELECTION;
}

class ROI_SELECTION : public QStackedWidget //, private  QGraphicsScene
{
    Q_OBJECT

public:
    /*explicit*/ ROI_SELECTION(QWidget *parent = 0);
    ~ROI_SELECTION();


    help_window help_win ;

protected:
    //    inline void dropEvent(QDropEvent *e)
    //     {
    //         foreach (const QUrl &url, e->mimeData()->urls()) {
    //             QString fileName = url.toLocalFile();
    //             qDebug() << "Dropped file:" << fileName;
    //         }
    //     }

protected:

    virtual void wheelEvent(QWheelEvent* event);

    virtual void keyPressEvent(QKeyEvent *event);


private slots:

    void on_rdB_class_manually_clicked(bool checked);

    void on_rdB_class_from_file_clicked(bool checked);

    void on_spinB_class_count_valueChanged(int arg1);

    void on_btn_start_clicked();

    void on_lnE_class_text_file_path_textChanged(const QString &arg1);

    // signals:
    //    void set_value_spinB_class_count(int val);

    void on_action_show_statistics_toggled(bool arg1);

    void on_btn_submit_next_clicked();

    void on_btn_prev_clicked();

    void on_btn_next_clicked();

    void change_btn_color();

    void on_action_Hide_Labels_toggled(bool arg1);

    void on_qle_img_name_textChanged(const QString &arg1);


    void on_action_Drew_in_Polygon_Mode_toggled(bool arg1);

    void on_btn_submit_all_clicked();

    void on_action_Undo_triggered();

    void on_action_Redo_triggered();

    void on_action_Hide_Mouse_Position_toggled(bool arg1);

    void on_action_Submit_All_Exit_triggered();

    void on_action_Help_triggered();


private slots:
    void get_deleted_roi_signal();

signals:
  virtual void clicked(QPushButton*);

private:
    Ui::ROI_SELECTION *ui;

    bool permision_to_jump=false;
    int jump_to;
    bool insert_class_names_manually;

      bool submit_all_and_exit=false;

    void fill_QLineEdits_in_class_names_scroll_using_class_chars(vec_class_name_color &class_chars);

        void create_blank_QLineEdit_in_class_names_scroll(int rows);

    QVector<QLineEdit* > lnE_classes_pair;  // read text of Line Edited Items

    QVector<QPushButton* > btn_s_select_class_color;

    QWidget *widget_statistics_win=NULL;
    QGridLayout *gridLayout_statistics_win=NULL;
    QVector<QLabel *> q_labels_sttistics_win;
    QScrollArea *scroll_area_show_statistics;

    void show_statistics();
    void close_statistics_window();

    int current_img_num=0;


    void prepare_drew_page();

    CustomScene scene;

    void submit_results_go_next();

    void go_to_img_N_from_current(int N);

    bool must_be_submit=false;

    QDateTime start_time_process_image;

    QPixmap image;

    QPixmap res_image;

    void check_image();
    void show_img();

    void undo_scene();

    void redo_scene();

    void update_scene();

    void clear_image_date();

    void set_btn_color(QPushButton *btn);

    void read_text_prepare_to_draw(QVector<QString> img_data_lines);

};

#endif //ROI_SELECTION_H
