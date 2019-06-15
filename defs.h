#ifndef DEFS_H
#define DEFS_H

//#include <QtCore>
//#include <QtGui>
//#include <QLayout>
//#include <QtGlobal>
#include <QtWidgets>
//#include <QWidget>

#include <QGraphicsScene>
//#include <QGraphicsSceneMouseEvent>
//#include <QMouseEvent>
//#include <QGraphicsView>
//#include <QGraphicsItem>
#include <QGraphicsRectItem>
//#include <QGraphicsLineItem>
//#include <QGraphicsEllipseItem>
//#include <qtest.h>
#include <QMessageBox>
#include <QKeyEvent>

#include <QVector>
#include <QString>
#include <QMap>


#include <QDebug>

#define FIRST_LINE_IN_CLASS_NAMES_FILE "# image-name, class_full_name, class_abbreviation_name, class_color"

#define MAX_LOG_FILES_COUNT 50
#define MIN_RECT_AREA 50

#define MAX_CLASS_COUNT 99

#define MSG_SEEN_TO_WRITE_IN_TEXT_FILE "ONLY_DISPLAYED"

#define MIN_DST_TO_DETECT_RECT_FROM_POINT 2   // pixel

#define THICKNESS 2

#define FIRST_WORD_IN_TEXT_FOR_CLASS_NAMES "#class_names:"

#define STATISTICS_WIN_W 250

#define STATISTICS_WIN_H 300

extern bool permision_to_enter_class_key;

extern bool hide_labels_on_image;
extern bool hide_mouse_position;

const QStringList valid_image_formats{"*.png", "*.jpg" ,"*.jpeg" , "*.bmp"};

extern bool show_only_last_label;

extern QColor drawing_color;
extern QColor final_color;


extern QString load_img_path;
extern QString store_img_path;
extern QString load_text_path;
extern QString store_text_path;

extern QStringList IN_IMAGES_NAME;

extern QStringList input_text_names;

extern QStringList prc_imgs;   // processed images
extern QStringList unprc_imgs; // unprocessed images

extern QString LOG_FILE_PATH;

extern bool opt_drew_roi;
extern bool opt_review_edit;
extern bool save_res_img;

extern bool draw_in_rect_mode;

struct s_class_name_color
{
    QString full_name;
    QString abr_name;
    QString color=final_color.name();
};

struct vec_class_name_color
{
    QVector <s_class_name_color> vec ;
    int size() const
    {
        return vec.size();
    }

    QStringList keys() const
    {
        QStringList k;
        for(s_class_name_color cnc : vec)
            k.push_back(cnc.full_name);
        return k;
    }
    QString key(QString v) const
    {
        for(s_class_name_color cnc : vec)
            if(cnc.abr_name == v)
                return cnc.full_name;

        return NULL;
    }
    QStringList values() const
    {
        QStringList v;
        for(s_class_name_color cnc : vec)
            v.push_back(cnc.abr_name);
        return v;
    }
    QString value(QString k) const
    {
        for(s_class_name_color cnc : vec)
            if(cnc.full_name == k)
                return cnc.abr_name;

        return NULL;
    }
    QStringList colors() const
    {
        QStringList c;
        for(s_class_name_color cnc : vec)
            c.push_back(cnc.color);
        return c;
    }

    void insert(QString f_name, QString abr_name , QString color=NULL)
    {
        s_class_name_color cnc;
        cnc.full_name=f_name;
        cnc.abr_name=abr_name;
        if(!color.isEmpty())
            cnc.color=color;
        vec.push_back(cnc);
    }
    void clear()
    {
        vec.clear();
    }
    bool isEmpty() const
    {
        return vec.isEmpty();
    }
    void push_back(s_class_name_color cnc)
    {
        vec.push_back(cnc);
    }

    QString get_color_by_key(QString k) const
    {
        for(s_class_name_color cnc : vec)
            if(cnc.full_name == k)
                return cnc.color;

        return NULL;
    }
    QString get_color_by_value(QString v) const
    {
        for(s_class_name_color cnc : vec)
            if(cnc.abr_name == v)
                return cnc.color;

        return NULL;
    }

};


extern QString enterd_qstr_keys;

extern vec_class_name_color CLASSES_CHARS; // full class names and Abbreviation class names

QStringList fetch_file_names(QString PATH, QStringList suffix_s);

QStringList fetch_image_file_names(QString PATH);

void update_store_text_file(const QVector<QString > qlines, const QString img_name);

void draw_shape_on_image(QPixmap &img, const QString str_points, const QPen pen, bool hide_labels=false);

QStringList fetch_image_names_from_text_file(const QString text_file_path);

QVector<QString> fetch_image_data_from_text_file(const QString text_path, const QString image_name);

extern bool is_ROI_selected_to_edit;

extern int index_of_edited_ROI;

//extern bool permision_to_edit_roi;

//extern bool permision_to_edit_label;

extern QString label_of_edited_ROI;

extern QPolygonF polygon_points_of_edited_ROI;

extern QVector<QPair < QGraphicsPolygonItem* , QGraphicsTextItem* > > drawned_rois;

extern QVector<QPair < QGraphicsPolygonItem* , QGraphicsTextItem* > > drawned_rois_removed;

void append_class_names_to_begin_of_file(const QString text_path , const vec_class_name_color &data) ;

//void write_class_names_in_text_file(const QString text_path, const QVector<s_class_name_color> class_names);

vec_class_name_color fetch_class_names_from_text_file(const QString text_path);

int select_ROI_if_point_is_inside_it(const QPointF POINT);

void rewrite_data_in_text_file(QVector<QPair < QGraphicsPolygonItem* , QGraphicsTextItem* > > roi_data, QString text_path, QString img_name);

QVector<QString> convert_roi_data_to_qvec_qstr(const QVector<QPair<QGraphicsPolygonItem *, QGraphicsTextItem *> > roi_data);

QMap<QString,int> fetch_statictics_from_text(QString text_path);

void append_to_file(const QString file_path, const QString &data);

void append_to_file(const QString file_path,const QVector<QString> &data);

#endif // DEFS_H
