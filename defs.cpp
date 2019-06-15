#include "defs.h"

QColor drawing_color(255,255,0);
QColor final_color(0,255,255);

bool draw_in_rect_mode=true;


bool opt_drew_roi =false;
bool opt_review_edit =false;
bool save_res_img=false;

bool show_only_last_label=false;

QString load_img_path  ="";
QString store_img_path ="";
QString load_text_path ="";
QString store_text_path="";

QString enterd_qstr_keys="";

QStringList IN_IMAGES_NAME;
QStringList input_text_names;

bool is_ROI_selected_to_edit=false;
int index_of_edited_ROI;

//bool permision_to_edit_label=false;
//bool permision_to_edit_roi=false;

QString label_of_edited_ROI;
QPolygonF polygon_points_of_edited_ROI;

//bool permision_to_add_roi=false;

//bool opt_review_delete_roi_label=false;

//bool opt_review_new_roi_label=false;

//bool opt_review_edit_roi=false;

//bool opt_review_edit_label=false;

//bool editation_operation_is_selected=false;

QVector<QPair < QGraphicsPolygonItem* , QGraphicsTextItem* > > drawned_rois;
QVector<QPair < QGraphicsPolygonItem* , QGraphicsTextItem* > > drawned_rois_removed;

int select_ROI_if_point_is_inside_it(const QPointF POINT)
{
    //    int index;
    //    bool is_inside=false;
    //    static QColor last_selected_ROI_color;
    //    for(int i=0; i < drawned_rois.size(); i++ )
    //    {
    //        QColor self_color =  drawned_rois[i].first->pen().color();
    //        if(self_color == drawing_color)
    //            self_color = last_selected_ROI_color;
    //            drawned_rois[i].first->setPen(QPen(/*final_color*/self_color,THICKNESS));
    //        drawned_rois[i].second->setDefaultTextColor(/*final_color*/self_color);
    //        if(drawned_rois[i].first->polygon().containsPoint(POINT, Qt::WindingFill))
    //        {
    //            is_inside=true;
    //            index=i;
    //            label_of_edited_ROI=drawned_rois[i].second->toPlainText();
    //        }
    //    }
    //    if(is_inside==true)
    //    {
    //        last_selected_ROI_color = drawned_rois[index].first->pen().color();
    //        drawned_rois[index].first->setPen(QPen(drawing_color,THICKNESS));
    //        drawned_rois[index].second->setDefaultTextColor(drawing_color);
    //        return index;
    //    }
    //    else
    //    {
    //        return -1;
    //    }


    for(int i=0; i < drawned_rois.size(); i++ )
        if(drawned_rois[i].first->polygon().containsPoint(POINT, Qt::WindingFill))
            return i;
    return -1;



}





void draw_shape_on_image(QPixmap &img, const QString str_points , const QPen pen , bool hide_labels)
{
    //    if(str_points.size()==0)
    //        return;

    QList<QString> fields=str_points.split(" ");
    if(fields.size()<3)
        return;

    QPainter painter(&img);
    painter.setPen(pen);

    QVector<QPoint> readed_points;
    for(int i=1; i < fields.size() ; i++ ) //ignor first field (class name)
    {
        QString qstr_point=fields[i];
        qstr_point.remove("(").remove(")");
        int x=qstr_point.split(",").at(0).toInt();
        int y=qstr_point.split(",").at(1).toInt();
        readed_points.push_back(QPoint(x,y));
    }

    if(hide_labels ==false && readed_points.size()>0)
    {
        painter.setFont( QFont("Arial") );
        painter.drawText(readed_points[0], fields[0] );
    }
    if(readed_points.size()==1)  // draw point
    {
        painter.drawPoint(readed_points[0]);
    }
    else if(readed_points.size()==2)  // draw rect not line
    {
        painter.drawRect(QRect(readed_points[0],readed_points[1]).normalized());
    }
    else if (readed_points.size()>2)  // draw polygon
    {
        QPolygon polygon_points;
        polygon_points.append(readed_points);
        painter.drawPolygon(polygon_points);
    }
    else
    {
        qDebug()<<"Could not extract proper points from "<<str_points;
    }
}


QStringList fetch_image_names_from_text_file(const QString text_file_path)
{
    QStringList out_list;

    QFile file( text_file_path );
    if(! file.exists())
    {
        qDebug()<<"Could not find: "<<text_file_path;
        return out_list;
    }
    if ( file.open(QIODevice::ReadOnly) )
    {
        QTextStream in_stream( &file );
        while (!in_stream.atEnd())
        {
            QString line = in_stream.readLine();
            if(line == FIRST_LINE_IN_CLASS_NAMES_FILE)
                continue;

            QString first_word=line.split(" ").first();
            if(first_word == FIRST_WORD_IN_TEXT_FOR_CLASS_NAMES)
                continue;

            QString name=line.split(":").first();
            if(! out_list.contains(name))
                out_list.push_back(name);
        }
    }
    else
    {
        qDebug()<<"Could not read: "<<text_file_path;
        return out_list;
    }
    file.close();

    return out_list;
}

void update_store_text_file(const QVector<QString> qlines,const QString img_name)
{
    if(qlines.size()<1)
        return;

    QFile file( store_text_path );
    file.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream stream( &file );
    foreach (QString line, qlines)
    {
        stream <<img_name<<": "<<line << endl;
    }

    file.close();
}


QVector<QString> fetch_image_data_from_text_file(const QString text_path,const QString image_name)
{
    QVector<QString> vec_data;

    QFile text_file(text_path);

    if(! text_file.exists())
    {
        qDebug()<<"Could not find: "<<text_path;
        return vec_data;
    }
    if ( text_file.open(QIODevice::ReadOnly) )
    {
        QTextStream in_stream( &text_file );
        while (!in_stream.atEnd())
        {
            QString line = in_stream.readLine();
            QString name=line.split(":").first();
            if(name==image_name)
            {
                line.remove(name + ": ");
                vec_data.push_back(line);
            }
        }
    }
    text_file.close();
    return vec_data;

}

void append_class_names_to_begin_of_file(const QString text_path, const vec_class_name_color &data)
{
    QFile text_file(text_path);
    QStringList class_keys=data.keys();
    QVector<QString> lines;

    lines.push_back(FIRST_LINE_IN_CLASS_NAMES_FILE);

    for(int i=0; i<data.size(); i++)
        lines.push_back(QString(FIRST_WORD_IN_TEXT_FOR_CLASS_NAMES)+ " "+ data.keys()[i] + " " + data.values()[i] + " " +data.colors()[i]);

    if(text_file.exists())
    {
        if(!text_file.open(QIODevice::ReadOnly))
        {
            qDebug()<<"Could not read "<<text_path;
            return;
        }
        QTextStream in_stream( &text_file );
        while (!in_stream.atEnd())
        {
            QString line = in_stream.readLine();
            QString first_word=line.split(" ").first();
            if(line == FIRST_LINE_IN_CLASS_NAMES_FILE )
                continue;
            if(first_word == FIRST_WORD_IN_TEXT_FOR_CLASS_NAMES)
            {
                QString second_word=line.split(" ").at(1);
                if(class_keys.contains(second_word))
                    continue;
            }

            lines.push_back(line);
        }
        text_file.close();
    }

    if(! text_file.open(QIODevice::WriteOnly)) // clear and write
    {
        qDebug()<<"Could not create "<<text_path;
        return;
    }
    QTextStream out_stream( &text_file );
    foreach (QString line, lines) {
        out_stream<<line<<"\n";
    }
    text_file.close();
}

//void write_class_names_in_text_file(const QString text_path, const QVector<s_class_name_color> class_names)
//{
//    QFile text_file(text_path);
//    QVector<QString> lines;
//    lines.push_back(FIRST_LINE_IN_CLASS_NAMES_FILE);
//    for(s_class_name_color cnc:class_names)
//    {
//        lines.push_back(QString(FIRST_WORD_IN_TEXT_FOR_CLASS_NAMES) + " " + cnc.full_name + " " + cnc.abr_name + " " + cnc.color);
//    }

//    if(text_file.exists())
//    {
//        if(!text_file.open(QIODevice::ReadOnly))
//        {
//            qDebug()<<"Could not read "<<text_path;
//            return;
//        }
//        QTextStream in_stream( &text_file );
//        while (!in_stream.atEnd())
//        {
//            QString line = in_stream.readLine();
//            QString first_word=line.split(" ").first();
//            if( line == FIRST_LINE_IN_CLASS_NAMES_FILE || first_word == FIRST_WORD_IN_TEXT_FOR_CLASS_NAMES)
//                continue;
//            lines.push_back(line);
//        }
//        text_file.close();
//    }

//    if(! text_file.open(QIODevice::WriteOnly)) // clear and write
//    {
//        qDebug()<<"Could not create "<<text_path;
//        return;
//    }
//    QTextStream out_stream( &text_file );
//    foreach (QString line, lines) {
//        out_stream<<line<<"\n";
//    }
//    text_file.close();
//}


vec_class_name_color fetch_class_names_from_text_file(const QString text_path)
{
    QFile text_file(text_path);
    vec_class_name_color class_names;
    if(!text_file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Could not read "<<text_path;
        return class_names;
    }

    QTextStream in_stream( &text_file );
    while (!in_stream.atEnd())
    {
        QString line = in_stream.readLine();
        if(line.size()<1)
            continue;
        QString first_word=line.split(" ").first();
        if(first_word==FIRST_WORD_IN_TEXT_FOR_CLASS_NAMES)
        {
            s_class_name_color cnc;
            cnc.full_name=line.split(" ").at(1);
            cnc.abr_name=line.split(" ").at(2);
            if(! line.split(" ").at(3).isEmpty())
                cnc.color=line.split(" ").at(3);

            class_names.push_back(cnc);
        }
        else
        {
            int number_of_words=line.split(" ").count();
            if(number_of_words==2 && ! line.contains(":"))
            {
                s_class_name_color cnc;
                cnc.full_name=line.split(" ").first();
                cnc.abr_name=line.split(" ").last();
                class_names.push_back(cnc);
            }

        }
    }
    text_file.close();
    return class_names;
}

void rewrite_data_in_text_file(QVector<QPair<QGraphicsPolygonItem *, QGraphicsTextItem *> > roi_data, QString text_path,QString img_name)
{
    // if roi_data was empty, that means clear img_name data

    QVector<QString> readed_data_from_roi_data=convert_roi_data_to_qvec_qstr(roi_data);

    QFile text_file(text_path);
    if(!text_file.exists())
    {
        qDebug()<<" Could not find "<<text_path;
        return;
    }

    QVector<QString> rewrite_lines;

    if(text_file.exists())
    {
        if(!text_file.open(QIODevice::ReadOnly))
        {
            qDebug()<<"Could not read "<<text_path;
            return;
        }
        QTextStream out_stream( &text_file );
        while (!out_stream.atEnd())
        {
            QString line = out_stream.readLine();
            QString first_word=line.split(":").first();
            if(first_word==img_name)
                continue;
            rewrite_lines.push_back(line);
        }
        text_file.close();
    }

    foreach (QString data, readed_data_from_roi_data) {
        QString new_data=img_name + ": " + data;
        rewrite_lines.push_back(new_data);
    }
    if(readed_data_from_roi_data.size()==0)
    {
        QString new_data=img_name + ": " + MSG_SEEN_TO_WRITE_IN_TEXT_FILE;
        rewrite_lines.push_back(new_data);
    }

    if(!text_file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Could not write "<<text_path;
        return;
    }
    QTextStream in_stream( &text_file );

    foreach (QString line, rewrite_lines) {
        in_stream<<line<<"\n";
    }
    text_file.close();
}

QVector<QString> convert_roi_data_to_qvec_qstr(const QVector<QPair<QGraphicsPolygonItem *, QGraphicsTextItem *> > roi_data)
{
    QVector<QString> output_vector;
    if(roi_data.size()<1)
        return output_vector;


    for(int i=0;i<roi_data.size();i ++ )
    {
        QString class_name=roi_data[i].second->toPlainText();

        QString qstr_wite_line=class_name;

        foreach (QPointF p, roi_data[i].first->polygon()) {
            qstr_wite_line +=" (" + QString::number((int)p.x()) + "," + QString::number((int)p.y()) + ")";
        }
        output_vector.push_back(qstr_wite_line);
    }
    return output_vector;
}

QMap<QString, int> fetch_statictics_from_text(QString text_path)
{
    QFile text_file(text_path);
    QMap<QString, int> statistics;
    if(!text_file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Could not read "<<text_path;
        return statistics;
    }

    QStringList keys=CLASSES_CHARS.keys();
    for(QString k:keys )
        statistics.insert(k,0); // Initialization

    QTextStream in_stream( &text_file);
    while (!in_stream.atEnd())
    {
        QString line = in_stream.readLine();
        if(line.size()<1)
            continue;
        QStringList words=line.split(" ");
        QString first_word=words[0];

        if(first_word==FIRST_WORD_IN_TEXT_FOR_CLASS_NAMES)
            continue;
        QString second_word=words[1];
        if(statistics.keys().contains(second_word) && words.size() > 3)
            statistics[second_word]++ ;
    }

    text_file.close();
    return statistics;
}

void append_to_file(const QString file_path, const QString &data)
{
    QFile file(file_path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug()<<"ERROR: Can not open file ("<<file_path<<") to append!";
        return;
    }
    if(data.isEmpty())
        return;

    QTextStream stream( &file );
    stream <<data;

    stream << endl;
    file.close();
}

void append_to_file(const QString file_path, const QVector<QString> &data)
{
    QFile file(file_path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug()<<"ERROR: Can not open file ("<<file_path<<") to append!";
        return;
    }

    if(data.isEmpty())
        return;

    QTextStream stream( &file );
    foreach (QString line, data)
    {
        stream <<line<< endl;
    }
    stream << endl;
    file.close();
}

QStringList fetch_file_names(QString PATH, QStringList suffix_s)
{
    //    QProcess Myproc;
    //    Myproc.setWorkingDirectory(PATH);
    //    Myproc.start("ls --sort=version");
    //    Myproc.waitForBytesWritten();
    //    Myproc.waitForFinished();
    //    QByteArray FileNames = Myproc.readAll();
    //    QStringList ImageNames;

    //    for( ; FileNames.size()>0 ; )
    //    {
    //        int Index = FileNames.indexOf("\n",0);
    //        QString File = FileNames.mid(0,Index);
    //        ImageNames.push_back(File);
    //        FileNames.remove(0,Index + 1);
    //    }
    //    return ImageNames;

    QStringList file_list=QDir(PATH).entryList(suffix_s, QDir::Files|QDir::NoDotAndDotDot);
    file_list.sort(Qt::CaseSensitive); //default
    return file_list;
}

QStringList fetch_image_file_names(QString PATH)
{
    return fetch_file_names(PATH,valid_image_formats);
}




