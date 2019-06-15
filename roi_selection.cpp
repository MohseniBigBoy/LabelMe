#include "roi_selection.h"
#include "ui_roi_selection.h"

vec_class_name_color CLASSES_CHARS; // full class names and Abbreviation class names

bool permision_to_enter_class_key = false;
bool hide_labels_on_image = false;

bool hide_mouse_position = false;


ROI_SELECTION::ROI_SELECTION(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::ROI_SELECTION)
{
    ui->setupUi(this);

    this->setGeometry(
                QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter,
                                     this->size(), qApp->desktop()->availableGeometry()  )   );

    connect(&scene, SIGNAL(roi_deleted()), this, SLOT(get_deleted_roi_signal()));


    if(opt_review_edit ==true)
    {
        this->setWindowTitle("Review and Edit");

        CLASSES_CHARS = fetch_class_names_from_text_file(load_text_path);

        if(CLASSES_CHARS.size()<1)
        {
            QMessageBox::StandardButton reply;
            reply=QMessageBox::warning(this, "Warning", "Can not fetch proper Class Names from \n " + load_text_path,
                                       QMessageBox::Ok);
        }
        if(save_res_img == false)
        {
            ui->action_Submit_All_Exit->setEnabled(false);
        }

        prepare_drew_page();

    }

    else if ( opt_drew_roi == true)
    {
        this->setWindowTitle("Drew ROI (rectangle/polygon)");

        this->setCurrentIndex(0);

        ui->rdB_class_manually->setAutoExclusive(false);
        ui->rdB_class_manually->setChecked(true);
        ui->rdB_class_manually->setAutoExclusive(true);
        ui->lnE_class_text_file_path->setEnabled(false);
        ui->btn_start->setEnabled(false);
        ui->action_Submit_All_Exit->setEnabled(false);

        ui->spinB_class_count->setMaximum(MAX_CLASS_COUNT);
    }
    else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, "Error", "Undefined Operation!",QMessageBox::Ok);
        exit(EXIT_FAILURE);
    }

}

ROI_SELECTION::~ROI_SELECTION()
{
    delete ui;
}

void ROI_SELECTION::wheelEvent(QWheelEvent *event)
{
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.1;
    if(event->delta() > 0)
        ui->graphicsView->scale(scaleFactor, scaleFactor); // Zoom in
    else
        ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);    // Zooming out
}

void ROI_SELECTION::keyPressEvent(QKeyEvent *event)
{

    //    qDebug() <<"You pressed"<<event->text()<<"  "<<event->key() ;

    int keyInt = event->key();

    if(permision_to_enter_class_key==false)
        return;


    if(permision_to_jump==true &&  ((keyInt==Qt::Key_Return) || (keyInt==Qt::Key_Enter)) )
    {
        go_to_img_N_from_current(jump_to);
        permision_to_jump=false;
        return;
    }


    if(keyInt == Qt::Key_unknown){
        qDebug() << "Unknown key from a macro probably ! ";
        return;
    }

    bool is_en_num =(bool) (keyInt>=48 && keyInt<=57) ;
    bool is_en_alph=(bool) (keyInt>=65 && keyInt<=90);

    bool is_per_num =(bool) (keyInt>=1776 && keyInt<=1785) ;
    bool is_per_alph=(bool) (keyInt==1575 || keyInt==1576 || keyInt==1662 || keyInt==1578 || keyInt==1579 || keyInt==1580 ||
                             keyInt==1670 || keyInt==1581 || keyInt==1582 || keyInt==1583 || keyInt==1584 || keyInt==1585 ||
                             keyInt==1586 || keyInt==1688 || keyInt==1587 || keyInt==1588 || keyInt==1589 || keyInt==1590 ||
                             keyInt==1591 || keyInt==1592 || keyInt==1593 || keyInt==1594 || keyInt==1601 || keyInt==1602 ||
                             keyInt==1705 || keyInt==1711 || keyInt==1604 || keyInt==1605 || keyInt==1606 || keyInt==1608 ||
                             keyInt==1607 || keyInt==1740 );

    bool special_keys= (bool) ( (keyInt==Qt::Key_Control) || (keyInt==Qt::Key_Enter) ||
                                (keyInt==Qt::Key_Return) || (keyInt==Qt::Key_Backspace) );

    if ( ! ( is_en_num || is_en_alph || is_per_num || is_per_alph || special_keys ) )
    {
        qDebug()<<" You pressed an illegal key: "<<event->text()<<"  "<<keyInt;
        return;
    }

    if(keyInt == Qt::Key_Control) // return if hold Ctrl key
    {
        qDebug() << "Single Ctrl pressed";
        return;
    }

    // check for a combination of user clicks
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if(modifiers & Qt::ShiftModifier)
        keyInt += Qt::SHIFT;
    if(modifiers & Qt::ControlModifier)
        keyInt += Qt::CTRL;

    qDebug() << "New KeySequence:" << QKeySequence(keyInt).toString(/*QKeySequence::NativeText*/);

    if(QKeySequence(keyInt).matches(QKeySequence::Undo)) // Ctrl+Z
    {
        qDebug()<<"you  pressed undo";
        undo_scene();
        return;
    }

    if(QKeySequence(keyInt).matches(QKeySequence::Redo)) // Ctrl+Shift+Z
    {
        qDebug()<<"you  pressed redo";
        redo_scene();
        return;
    }

    QMessageBox::StandardButton reply;
    if( scene.wait_labeling == false /*scene.last_drawn_roi_points.size()<2*/)
    {
        reply = QMessageBox::critical(this, "Error", "Draw proper ROI !",QMessageBox::Ok);
        return;
    }


    if( CLASSES_CHARS.size()==1 && (event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return) && enterd_qstr_keys.isEmpty() )
    {
        enterd_qstr_keys=CLASSES_CHARS.values()[0];
    }

    QString entered_class_name;

    if((event->key()==Qt::Key_Enter) || (event->key()==Qt::Key_Return))
    {

        qDebug()<<"Enter key pressed.";
        entered_class_name=enterd_qstr_keys;
        enterd_qstr_keys.clear();
    }
    else
    {
        if (event->key()==Qt::Key_Backspace)
            enterd_qstr_keys.remove(enterd_qstr_keys.size()-1,enterd_qstr_keys.size()); //remove last char
        else
        {
            QChar pressed_key=event->text().at(0);
            enterd_qstr_keys+=pressed_key;
        }

        if(scene.last_drawn_roi_points.size()>0)
        {
            scene.temp_write_text_into_roi->setPos(scene.last_drawn_roi_points[0].x()+5,scene.last_drawn_roi_points[0].y()+5);
            scene.temp_write_text_into_roi->setPlainText(enterd_qstr_keys);
            scene.temp_write_text_into_roi->setDefaultTextColor(drawing_color);
            scene.addItem(scene.temp_write_text_into_roi);
        }
        //        }
        return;
    }

    QString qstr_wite_line="";

    if( CLASSES_CHARS.values().contains(entered_class_name) && ! entered_class_name.isEmpty() ) // enter key pressed
        qstr_wite_line=CLASSES_CHARS.key(entered_class_name);

    if(qstr_wite_line.isEmpty() )
    {
        reply = QMessageBox::critical(this, "Error", "Invalid class name !",QMessageBox::Ok);

        return ;
    }

    scene.wait_labeling = false;

    scene.initial_to_draw_roi();

    QPolygonF current_polygon;
    for(int i=0; i<scene.last_drawn_roi_points.size(); i++)
    {
        if(scene.last_drawn_roi_points[i].x()<0) scene.last_drawn_roi_points[i].setX(0);
        if(scene.last_drawn_roi_points[i].x()>image.width()) scene.last_drawn_roi_points[i].setX(image.width());
        if(scene.last_drawn_roi_points[i].y()<0) scene.last_drawn_roi_points[i].setY(0);
        if(scene.last_drawn_roi_points[i].y()>image.height()) scene.last_drawn_roi_points[i].setY(image.height());

        if(scene.last_drawn_roi_points.size()!=2)
            current_polygon.append(scene.last_drawn_roi_points[i]);
    }


    if(scene.last_drawn_roi_points.size()==2)
    {
        QRectF correct_rec=QRectF(scene.last_drawn_roi_points[0],scene.last_drawn_roi_points[1]).normalized(); // return positive W and H
        QPointF p_up_left=QPointF(correct_rec.x(),correct_rec.y());
        QPointF p_up_right=QPointF(correct_rec.x()+correct_rec.width(),correct_rec.y());
        QPointF p_down_left=QPointF(correct_rec.x(),correct_rec.y()+correct_rec.height());
        QPointF p_down_right=QPointF(correct_rec.x()+correct_rec.width(),correct_rec.y()+correct_rec.height());
        current_polygon.append(p_up_left);
        current_polygon.append(p_up_right);
        current_polygon.append(p_down_right);
        current_polygon.append(p_down_left);
    }

    QColor f_color(CLASSES_CHARS.get_color_by_key(qstr_wite_line));
    QGraphicsTextItem * write_text_into_roi = new QGraphicsTextItem;
    write_text_into_roi->setPos(scene.last_drawn_roi_points[0].x()+5,scene.last_drawn_roi_points[0].y()+5);
    write_text_into_roi->setPlainText(qstr_wite_line);
    write_text_into_roi->setDefaultTextColor(/*final_color*/f_color);


    QGraphicsPolygonItem *item_polygon = new QGraphicsPolygonItem();
    item_polygon->setPolygon(current_polygon);
    item_polygon->setPen(QPen(/*final_color*/f_color,2));

    drawned_rois.push_back(QPair < QGraphicsPolygonItem* , QGraphicsTextItem* > (item_polygon , write_text_into_roi ));

    update_scene();

    ui->graphicsView->show();
    ui->btn_submit_next->setEnabled(true);

    must_be_submit=true;


}


void ROI_SELECTION::on_rdB_class_manually_clicked(bool checked)
{
    //    insert_class_names_manually=checked;
    if(!ui->rdB_class_manually->isChecked())
    {
        ui->rdB_class_manually->setAutoExclusive(false);
        ui->rdB_class_manually->setChecked(true);
        ui->rdB_class_manually->setAutoExclusive(true);
    }

    ui->lbl_class_count->setEnabled(checked);
    ui->spinB_class_count->setEnabled(checked);
    ui->lnE_class_text_file_path->setEnabled(!checked);
    ui->lbl_class_text_file_path->setEnabled(!checked);

}

void ROI_SELECTION::on_rdB_class_from_file_clicked(bool checked)
{
    //    insert_class_names_manually=!checked;
    ui->lbl_class_count->setEnabled(!checked);
    ui->spinB_class_count->setEnabled(!checked);
    ui->lnE_class_text_file_path->setEnabled(checked);
    ui->lbl_class_text_file_path->setEnabled(checked);

    QString class_file= QFileDialog::getOpenFileName(this, tr("Select Class Names File"),QDir::homePath(), tr("Text Files (*.txt)"));

    if(QFile(class_file).exists())
    {
        if(class_file==ui->lnE_class_text_file_path->text())
            on_lnE_class_text_file_path_textChanged(class_file);
        ui->lnE_class_text_file_path->setText(class_file);
        ui->lnE_class_text_file_path->setEnabled(true);
    }
    else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, "Error", "File not found!",QMessageBox::Ok);
    }

}



void ROI_SELECTION::fill_QLineEdits_in_class_names_scroll_using_class_chars(vec_class_name_color &class_chars)
{
    //    qDebug()<<"CLASSES_CHARS: "<<class_chars.size();

    if(class_chars.size()<1)
        return;

    if(ui->scrollAreaWidgetContents->children().size()>0)
    {
        qDeleteAll(ui->scrollAreaWidgetContents->children());
    }

    lnE_classes_pair.clear();
    btn_s_select_class_color.clear();

    QGridLayout *gridLayout = new QGridLayout();

    int min_h=20;

    for (int i=0; i < class_chars.size() ;i++)
    {
        QString name_1="lnE_cls_"+QString::number(i)+"_1";
        QString name_2="lnE_cls_"+QString::number(i)+"_2";
        QLineEdit *lnE_i_1=new QLineEdit();
        lnE_i_1->setMinimumHeight(min_h);
        lnE_i_1->setObjectName(name_1);
        lnE_i_1->setMaximumWidth((int)(0.5*(ui->scrollArea_class_labels->geometry().width())));
        if(class_chars.keys()[i].isEmpty())
            lnE_i_1->setPlaceholderText("Class Full Name");
        else
            lnE_i_1->setText(class_chars.keys()[i]);


        QLineEdit *lnE_i_2=new QLineEdit();
        lnE_i_2->setObjectName(name_2);
        lnE_i_2->setMinimumHeight(min_h);
        lnE_i_2->setMaximumWidth((int)(0.2*(ui->scrollArea_class_labels->geometry().width())));

        if(class_chars.values()[i].isEmpty())
            lnE_i_2->setPlaceholderText("Abbreviation");
        else
            lnE_i_2->setText(class_chars.values()[i]);

        lnE_classes_pair.push_back(lnE_i_1);
        lnE_classes_pair.push_back(lnE_i_2);

        QLabel *lbl_class_naames_count=new QLabel();
        lbl_class_naames_count->setText(QString::number(i+1));
        lbl_class_naames_count->setMaximumWidth((int)(0.1*(ui->scrollArea_class_labels->geometry().width())));


        QPushButton *btn_select_color=new QPushButton();
        btn_select_color->setMinimumHeight(min_h);
        btn_select_color->setMaximumWidth((int)(0.2*(ui->scrollArea_class_labels->geometry().width())));
        QString color=class_chars.colors()[i];
        QColor qcolor(color);
        if( qcolor.isValid())
        {
            btn_select_color->setText(color);
            btn_select_color->setStyleSheet("QPushButton { background-color : "+color+"; color: black }");
        }
        else
        {
            btn_select_color->setText("set color");
            QPalette pal = btn_select_color->palette();
            pal.setColor(QPalette::Button, Qt::white);
            btn_select_color->setAutoFillBackground(true);
            btn_select_color->setPalette(pal);
            btn_select_color->update();
        }


        btn_select_color->setToolTip("click to set/change color");
        btn_s_select_class_color.push_back(btn_select_color);
        connect(btn_s_select_class_color.back(), SIGNAL(clicked()), this, SLOT(change_btn_color()));

        gridLayout->addWidget(lbl_class_naames_count,i,0);
        gridLayout->addWidget(lnE_i_1,i,1);
        gridLayout->addWidget(lnE_i_2,i,2);
        gridLayout->addWidget(btn_select_color,i,3);

    }
    ui->scrollAreaWidgetContents->setLayout(gridLayout);

}


void ROI_SELECTION::create_blank_QLineEdit_in_class_names_scroll(int rows)
{
    if(rows<1)
        return;

    vec_class_name_color class_names;
    for(int i=0; i< rows; i++)
    {
        s_class_name_color scnc;
        scnc.color="";
        class_names.push_back(scnc);
    }

    CLASSES_CHARS.clear();

    fill_QLineEdits_in_class_names_scroll_using_class_chars(class_names);

}

void ROI_SELECTION::show_statistics()
{

    scroll_area_show_statistics->setVisible(true);

    scroll_area_show_statistics->setAutoFillBackground(true);

    //    if(widget_statistics_win==NULL)
    widget_statistics_win = new QWidget();

    scroll_area_show_statistics->setWidget( widget_statistics_win );
    //    if(gridLayout_statistics_win==NULL)
    gridLayout_statistics_win = new QGridLayout();
    widget_statistics_win->setLayout( gridLayout_statistics_win );

    int min_h=20;

    QString input_text = opt_review_edit==true ? load_text_path : store_text_path ;

    QMap<QString, int> statistics_counts=fetch_statictics_from_text(input_text);
    qDebug() << statistics_counts.size();

    for (int i=0; i < CLASSES_CHARS.size() ;i++)
    {
        QLabel *counter=new QLabel();
        counter->setMinimumHeight(min_h);
        counter->setMaximumWidth(20);
        counter->setText(QString::number(i+1));

        QLabel *class_name=new QLabel();
        class_name->setMinimumHeight(min_h);
        class_name->setMaximumWidth((int)(0.65*(scroll_area_show_statistics->geometry().width())));
        class_name->setText(CLASSES_CHARS.keys()[i]);

        QLabel *class_abb=new QLabel();
        class_abb->setMinimumHeight(min_h);
        class_abb->setMaximumWidth((int)(0.35*(scroll_area_show_statistics->geometry().width())));
        class_abb->setText(CLASSES_CHARS.values()[i]);

        QLabel *class_i_statistics=new QLabel();
        class_i_statistics->setMinimumHeight(min_h);
        class_i_statistics->setMaximumWidth(40);
        class_i_statistics->setText(QString::number(statistics_counts.value(CLASSES_CHARS.keys()[i])));

        QString color=CLASSES_CHARS.colors()[i];
        if(color.isEmpty())
            color=s_class_name_color().color;

        QLabel *class_color=new QLabel();
        class_color->setFixedSize(min_h*2.5,min_h);
        class_color->setStyleSheet("QLabel { background-color : "+color+"; color: black }");
        class_color->setToolTip(color);

        gridLayout_statistics_win->addWidget(counter,i,0,Qt::AlignLeft);
        gridLayout_statistics_win->addWidget(class_name,i,1, Qt::AlignLeft);
        gridLayout_statistics_win->addWidget(class_abb,i,2, Qt::AlignCenter);
        gridLayout_statistics_win->addWidget(class_i_statistics,i,3, Qt::AlignCenter);
        gridLayout_statistics_win->addWidget(class_color,i,4, Qt::AlignRight);

        q_labels_sttistics_win.push_back(counter);
        q_labels_sttistics_win.push_back(class_name);
        q_labels_sttistics_win.push_back(class_abb);
        q_labels_sttistics_win.push_back(class_i_statistics);
        q_labels_sttistics_win.push_back(class_color);
    }


}

void ROI_SELECTION::close_statistics_window()
{
    scroll_area_show_statistics->setVisible(false);

    foreach (QLabel *ql, q_labels_sttistics_win) {
        //            delete ql;
        gridLayout_statistics_win->removeWidget(ql);
    }
    q_labels_sttistics_win.clear();

    if(gridLayout_statistics_win->children().size()>0)
    {
        qDeleteAll(gridLayout_statistics_win->children());
    }
    delete gridLayout_statistics_win->widget();

    delete widget_statistics_win;
}

void ROI_SELECTION::prepare_drew_page()
{
    this->setCurrentIndex(1);


    permision_to_enter_class_key=true;

    QMenuBar *menu_bar=new QMenuBar(this);

    QMenu *menu_tools = new QMenu("&Tools",menu_bar);

    menu_tools->setToolTipsVisible(true);

    QAction *act_separator = new QAction(this);
    act_separator->setSeparator(true);

    ui->action_Redo->setIcon(QIcon("redo.png"));
    ui->action_Undo->setIcon(QIcon("undo.png"));

    ui->action_Hide_Labels->setToolTip("Hide ROI Labels");
    ui->action_Hide_Mouse_Position->setToolTip("hide mouse position tooltip");
    ui->action_Drew_in_Polygon_Mode ->setToolTip("change drawing mode from rectangle to polygon");
    ui->action_show_statistics->setToolTip("show statistics up to now");
    ui->action_Undo->setToolTip("remove previous ROI and label");
    ui->action_Redo->setToolTip("return previous removed ROI and label");

    menu_tools->addActions(QList<QAction *>() << ui->action_Hide_Labels << ui->action_Hide_Mouse_Position <<
                           ui->action_show_statistics << ui->action_Drew_in_Polygon_Mode   << act_separator << ui->action_Undo << ui->action_Redo);

    //    menu_bar->addAction(menu_tools->menuAction());
    menu_bar->addMenu(menu_tools);
    //    this->layout()->setMenuBar(menu_bar);


    QMenu *menu_analyze = new QMenu("&Analyze",menu_bar);
    menu_analyze->setToolTipsVisible(true);
    ui->action_Submit_All_Exit->setToolTip("read image data from the loaded text \nfile and drew them on image then save\nimage in the Store Image Directory, do \nthis for all images then exit.");
    menu_analyze->addAction(ui->action_Submit_All_Exit);
    menu_bar->addMenu(menu_analyze);

    QMenu *menu_help = new QMenu("&Help",menu_bar);
    menu_help->setToolTipsVisible(true);
    menu_help->addAction(ui->action_Help);
    menu_bar->addMenu(menu_help);

    this->layout()->setMenuBar(menu_bar);

    ui->qle_img_name->setToolTip("click here and insert relative image number,\nthen push the Enter key to jump it!");

    ui->btn_prev->setToolTip("go to previous image");
    ui->btn_next->setToolTip("go to next image");
    ui->btn_submit_next->setToolTip("submit result then go to next image");

    scroll_area_show_statistics=new QScrollArea(this);
    scroll_area_show_statistics->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    scroll_area_show_statistics->setWidgetResizable( true );

    scroll_area_show_statistics->setGeometry(21,85
                                             ,STATISTICS_WIN_W,STATISTICS_WIN_H);

    scroll_area_show_statistics->setMaximumHeight(/*ui->graphicsView->height()-20*/STATISTICS_WIN_H);
    scroll_area_show_statistics->setVisible(false);

    ui->btn_submit_next->setEnabled(false);

    start_time_process_image=QDateTime::currentDateTime();

    ui->progressBar->setValue(0); // With a current value of
    ui->progressBar->setFormat("Processed Images: 0 /"+
                               QString::number(IN_IMAGES_NAME.size())+" ( 0 %)");
    //     ui->progressBar->setFormat("Processed Images: "+QString::number(100*(current_img_num+1)/unprc_imgs.size())+"%");
    ui->qle_img_name->setText("");

    QVector<QString> log_date;
    log_date.push_back(FIRST_LINE_IN_CLASS_NAMES_FILE);
    for(int i=0; i<CLASSES_CHARS.size(); i++)
    {
        log_date.push_back(QString(FIRST_WORD_IN_TEXT_FOR_CLASS_NAMES)+ " "+ CLASSES_CHARS.keys()[i] + " " + CLASSES_CHARS.values()[i] + " " +CLASSES_CHARS.colors()[i]);
    }

    append_class_names_to_begin_of_file(store_text_path,CLASSES_CHARS);

    log_date.push_back("\n**********************************************");
    log_date.push_back("\nImageInfo _ #ShapePerImage _ ElapsedTime (sec) \n");

    append_to_file(LOG_FILE_PATH,log_date);

    show_img();
}

void ROI_SELECTION::submit_results_go_next()
{

    QString out_text_file_path=store_text_path;
    if(opt_review_edit==true)
        out_text_file_path=load_text_path;

    if(submit_all_and_exit == false)
    {
        // The out_text_file_path file has already been created by write_class_names_in_text_file function.
        rewrite_data_in_text_file(drawned_rois,out_text_file_path,unprc_imgs[current_img_num]);

        qint64 elapsed_time = start_time_process_image.secsTo(QDateTime::currentDateTime());
        QString log_data=QString::number(prc_imgs.size()+current_img_num+1)+"/"+QString::number(prc_imgs.size()+unprc_imgs.size())+" : "+
                unprc_imgs[current_img_num]+"  _  "+QString::number(drawned_rois.size())+"  _  "+
                QString::number(elapsed_time);

        append_to_file(LOG_FILE_PATH,log_data);
        start_time_process_image=QDateTime::currentDateTime();
    }

    if (save_res_img==true)
    {
        res_image=image;
        QVector<QString> roi_data=convert_roi_data_to_qvec_qstr(drawned_rois);
        QVector<QColor> colors;
        for(int i=0; i< drawned_rois.size() ; i++)
            colors.push_back(drawned_rois[i].second->defaultTextColor());
        for (int i=0; i<roi_data.size() ; i++ )
        {
            QString qstr=roi_data[i];
            draw_shape_on_image(res_image,qstr,QPen(/*final_color*/colors[i],THICKNESS),hide_labels_on_image);
        }

        QString stored_img_name=store_img_path+"/"+unprc_imgs[current_img_num];
        res_image.save(stored_img_name);
    }

    clear_image_date();

    current_img_num++;
    current_img_num%=unprc_imgs.size();

    show_img();
    if(scroll_area_show_statistics->isVisible())
        show_statistics();

}

void ROI_SELECTION::go_to_img_N_from_current(int N)
{
    if(must_be_submit==true)
    {
        QMessageBox::StandardButton reply;
        reply=QMessageBox::question(this, "Warning", "You forget sibmit results! \n Do you want submit them?",
                                    QMessageBox::Cancel|QMessageBox::Yes|QMessageBox::No,QMessageBox::Cancel);
        if(reply == QMessageBox::Cancel)
        {
            return;
        }
        else if (reply == QMessageBox::Yes)
        {
            on_btn_submit_next_clicked();
        }
    }
    else
    {
        QString seen_line=unprc_imgs[current_img_num] + ": " + MSG_SEEN_TO_WRITE_IN_TEXT_FILE;
        append_to_file(store_text_path,seen_line);
    }

    clear_image_date();
    ui->btn_submit_next->setEnabled(false);

    current_img_num += N;
    current_img_num %= unprc_imgs.size();
    show_img();

}

void ROI_SELECTION::check_image()
{
    if(current_img_num<0 || current_img_num==unprc_imgs.size() )
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Warning", "Input image number is out of range!",QMessageBox::Ok);
    }
    if(current_img_num<0)
        current_img_num=unprc_imgs.size()-1; // last picture
    if(current_img_num==unprc_imgs.size())
        current_img_num=0; //first pic
}

void ROI_SELECTION::show_img()
{
    check_image();

    //    QString image_full_path= load_img_path + "/" + input_images_names[current_img_num] ;
    QString image_full_path= load_img_path + "/" + unprc_imgs[current_img_num] ;
    //    qDebug()<<image_full_path;
    image.load(image_full_path);
    //    qDebug()<<" image size  "<<image.width()<<"*"<<image.height();
    if(image.width() < 1 || image.height() < 1 )
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, "Error", "This Item is unvisible !",QMessageBox::Ok);
        qDebug()<<"Could not show "<<image_full_path;
        return;
    }

    ui->progressBar->setValue((prc_imgs.size()+current_img_num+1)*100/IN_IMAGES_NAME.size()); // With a current value of
    ui->progressBar->setFormat("Processed Images: "+QString::number(prc_imgs.size()+current_img_num+1)+"/"+
                               QString::number(IN_IMAGES_NAME.size())+"  ("+QString::number((prc_imgs.size()+current_img_num+1)*100/IN_IMAGES_NAME.size())+"%)");

    ui->qle_img_name->setText(unprc_imgs[current_img_num]);

    scene.setSceneRect(image.rect());
    scene.addPixmap(image);

    scene.temp_write_text_into_roi =  new QGraphicsTextItem;

    ui->groupBox_roi->setTitle(image_full_path);
    ui->graphicsView->ensureVisible(scene.sceneRect());
    //        ui->graphicsView_cls->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView->fitInView(scene.itemsBoundingRect() ,Qt::KeepAspectRatio);
    ui->graphicsView->setScene(&scene);

    if(opt_review_edit ==true)
    {
        QVector<QString> image_data=fetch_image_data_from_text_file(load_text_path,unprc_imgs[current_img_num]);


        for(QString nn:image_data)
            qDebug()<<" image_data "<< nn;
        if(image_data.size()>0)
        {
            read_text_prepare_to_draw(image_data);
        }

    }
    ui->graphicsView->show();
}

void ROI_SELECTION::undo_scene()
{
    //    qDebug()<<drawned_rois.size()<<drawned_rois_removed.size();
    if(drawned_rois.size()>0)
    {
        drawned_rois_removed.push_back(drawned_rois.last());
        drawned_rois.pop_back();
    }
    else if(drawned_rois.size()==0)
    {
        ui->btn_submit_next->setEnabled(false);
        scene.is_first_selection=true;
    }
    update_scene();
}

void ROI_SELECTION::redo_scene()
{
    if(drawned_rois_removed.size()>0)
    {
        drawned_rois.push_back(drawned_rois_removed.last());
        drawned_rois_removed.pop_back();
    }
    else if(drawned_rois.size()>0)
    {
        ui->btn_submit_next->setEnabled(true);
        scene.is_first_selection=false;
    }
    update_scene();
}

void ROI_SELECTION::update_scene()
{
    for(int i=0; i<drawned_rois_removed.size(); i++)
    {
        scene.removeItem(drawned_rois_removed[i].first);
        scene.removeItem(drawned_rois_removed[i].second);
    }

    //    qDebug()<<"drawned_rois.size() "<<drawned_rois.size() << " hide: "<<hide_labels_on_image;

    if(hide_labels_on_image==true)
    {
        for(int i=0; i< drawned_rois.size(); i++)
        {
            scene.removeItem(drawned_rois[i].first);
            scene.removeItem(drawned_rois[i].second);
        }
    }

    for(int i=0; i< drawned_rois.size(); i++)
    {
        scene.addItem(drawned_rois[i].first);
        if(hide_labels_on_image==false /*|| show_only_last_label==false*/ )
            scene.addItem(drawned_rois[i].second);
    }

    for(int i=0;i<scene.vec_item_line.size();i++)
        delete scene.vec_item_line[i];

    scene.vec_item_line.clear();

    if(scene.temp_write_text_into_roi!=NULL)
    {
        scene.removeItem(scene.temp_write_text_into_roi);
    }

    scene.wait_labeling=false;
}

void ROI_SELECTION::clear_image_date()
{
    must_be_submit=false;

    for(int i=0;i<drawned_rois.size();i++)
    {
        delete drawned_rois[i].first;
        delete drawned_rois[i].second;
    }

    for(int i=0;i<drawned_rois_removed.size();i++)
    {
        delete drawned_rois_removed[i].first;
        delete drawned_rois_removed[i].second;
    }

    scene.initial_to_draw_roi();

    drawned_rois.clear();
    drawned_rois_removed.clear();

    scene.last_drawn_roi_points.clear();
    scene.clear();
}


void ROI_SELECTION::on_spinB_class_count_valueChanged(int arg1)
{

    if( arg1 > 0 )
        ui->btn_start->setEnabled(true);
    else
    {
        ui->btn_start->setEnabled(false);
        return;
    }

    create_blank_QLineEdit_in_class_names_scroll(ui->spinB_class_count->text().toInt());

}

void ROI_SELECTION::on_btn_start_clicked()
{
    CLASSES_CHARS.clear();

    QMessageBox::StandardButton reply;

    bool fields_are_correct=true;

    for(int i=0; i< lnE_classes_pair.size(); i=i+2)
    {
        QString full_name=lnE_classes_pair[i]->text();
        if(full_name.size() < 1){
            reply = QMessageBox::critical(this, "Error", "Class full name field is empty in row "+QString::number(i/2+1),QMessageBox::Ok);
            fields_are_correct=false;
            return;
        }

        QString Abbreviation_name=lnE_classes_pair[i+1]->text();
        if(Abbreviation_name.size() < 1)
        {
            reply = QMessageBox::critical(this, "Error", "Class abbreviation name field is empty in row "+QString::number(i/2+1),QMessageBox::Ok);
            fields_are_correct=false;
            return;
        }

        //check repeatation in keys
        if(CLASSES_CHARS.keys().contains(full_name) /*|| CLASSES_CHARS.keys().contains(Abbreviation_name)*/ )
        {
            reply = QMessageBox::critical(this, "Error", "Class full name has been duplicated in row "+QString::number(i/2+1),QMessageBox::Ok);
            fields_are_correct=false;
            lnE_classes_pair[i]->setText("");
            return;
        }
        //check repeatation in values
        if(CLASSES_CHARS.values().contains(Abbreviation_name) /*|| CLASSES_CHARS.values().contains(full_name)*/)
        {
            reply = QMessageBox::critical(this, "Error", "Class abbreviation name has been duplicated in row "+QString::number(i/2+1),QMessageBox::Ok);
            fields_are_correct=false;
            lnE_classes_pair[i+1]->setText("");
            return;
        }

        QString color=btn_s_select_class_color[i/2]->text();

        if(color == "set color")
        {
            color=s_class_name_color().color; // default color
        }

        CLASSES_CHARS.insert(full_name , Abbreviation_name , color);
    }

    if(CLASSES_CHARS.isEmpty())
    {
        reply = QMessageBox::critical(this, "Error", "Set at least one Class Name!",QMessageBox::Ok);
        CLASSES_CHARS.clear();
        return ;
    }

    if(fields_are_correct==false)
    {
        reply = QMessageBox::critical(this, "Error", "Invalid Fields!",QMessageBox::Ok);
        CLASSES_CHARS.clear();
        return ;
    }


    prepare_drew_page();

}

void ROI_SELECTION::on_lnE_class_text_file_path_textChanged(const QString &arg1)
{
    QMessageBox::StandardButton reply;

    if(!QFile(arg1).exists())
    {
        reply = QMessageBox::critical(this, "Error", "File not found!",QMessageBox::Ok);
        return;
    }

    ui->grpB_class_names->setEnabled(true);
    ui->scrollArea_class_labels->setEnabled(true);

    vec_class_name_color class_names=fetch_class_names_from_text_file(arg1);

    if(class_names.size() < 1)
    {
        reply = QMessageBox::critical(this, "Error", "Can not fetch proper class names!",QMessageBox::Ok);
        qDebug()<<"Class names are invalid! "<<arg1;
        ui->btn_start->setEnabled(false);
        return;
        //        CLASSES_CHARS.clear();
    }

    CLASSES_CHARS=class_names;
    fill_QLineEdits_in_class_names_scroll_using_class_chars(CLASSES_CHARS);
    ui->btn_start->setEnabled(true);

}

void ROI_SELECTION::on_action_show_statistics_toggled(bool arg1)
{
    if(arg1==true)
        show_statistics();
    else
        close_statistics_window();
}


void ROI_SELECTION::on_btn_submit_next_clicked()
{
    submit_results_go_next();
    ui->btn_submit_next->setEnabled(false);
}

void ROI_SELECTION::on_btn_prev_clicked()
{
    go_to_img_N_from_current(-1);
}

void ROI_SELECTION::on_btn_next_clicked()
{
    go_to_img_N_from_current(+1);
}


void ROI_SELECTION::change_btn_color()
{
    QObject* button = QObject::sender();
    for(int i=0;i<btn_s_select_class_color.size();i++)
    {
        if (button == btn_s_select_class_color[i])
        {
            set_btn_color(btn_s_select_class_color[i]);
            break;
        }
    }
}

void ROI_SELECTION::set_btn_color(QPushButton *btn)
{
    QColor color = QColorDialog::getColor(/*Qt::yellow*/QColor("#00ffff"), this );
    if( color.isValid() )
    {
        //        qDebug() << "Color Choosen : " << color.name();
        QPalette pal = btn->palette();
        pal.setColor(QPalette::Button, QColor(color));
        btn->setAutoFillBackground(true);
        btn->setPalette(pal);
        btn->setToolTip("ckick to change color");
        btn->setText(color.name());
        btn->update();
    }

}

void ROI_SELECTION::read_text_prepare_to_draw(QVector<QString> img_data_lines)
{
    drawned_rois.clear();
    drawned_rois_removed.clear();

    for(int i=0;i<img_data_lines.size();i++)
    {
        QStringList words=img_data_lines[i].split(" ");
        if (words.size()<3)
            continue;
        QString class_fullName=words[0];
        QString class_color_qstr = CLASSES_CHARS.get_color_by_key(class_fullName);
        QColor class_color=final_color;
        if(! class_color_qstr.isEmpty())
            if(QColor(class_color_qstr).isValid())
                class_color=QColor(class_color_qstr);

        QVector<QPoint> readed_points;
        for(int k=1;k<words.size();k++)
        {
            QString qstr_point=words[k];
            //            if(qstr_point.contains("#")) // first letter of colors is # sign
            //                continue;
            qstr_point.remove("(").remove(")");
            int x=qstr_point.split(",").at(0).toInt();
            int y=qstr_point.split(",").at(1).toInt();

            readed_points.push_back(QPoint(x,y));
        }

        QGraphicsTextItem * write_text_into_roi = new QGraphicsTextItem;
        write_text_into_roi->setPos(readed_points[0].x()+5,readed_points[0].y()+5);
        write_text_into_roi->setPlainText(class_fullName);
        write_text_into_roi->setDefaultTextColor(class_color);

        //                scene.initial_to_draw_roi();


        QPolygon current_polygon;
        if(readed_points.size()>2)
        {
            foreach (QPoint p, readed_points)
                current_polygon.append(p);
        }
        else if(readed_points.size()==2)
        {
            QRect correct_rec=QRect(readed_points[0],readed_points[1]).normalized(); // return positive W and H
            QPoint p_up_left=QPoint(correct_rec.x(),correct_rec.y());
            QPoint p_up_right=QPoint(correct_rec.x()+correct_rec.width(),correct_rec.y());
            QPoint p_down_left=QPoint(correct_rec.x(),correct_rec.y()+correct_rec.height());
            QPoint p_down_right=QPoint(correct_rec.x()+correct_rec.width(),correct_rec.y()+correct_rec.height());
            current_polygon.append(p_up_left);
            current_polygon.append(p_up_right);
            current_polygon.append(p_down_right);
            current_polygon.append(p_down_left);
        }


        QGraphicsPolygonItem *item_polygon = new QGraphicsPolygonItem();
        item_polygon->setPolygon(current_polygon);
        item_polygon->setPen(QPen(class_color,THICKNESS));

        drawned_rois.push_back(QPair < QGraphicsPolygonItem* , QGraphicsTextItem* > (item_polygon , write_text_into_roi ));

    }

    update_scene();
}


void ROI_SELECTION::on_action_Hide_Labels_toggled(bool arg1)
{
    if(arg1)
        hide_labels_on_image=true;
    else
        hide_labels_on_image=false;
    //    qDebug()<<"hide_labels_on_image " << hide_labels_on_image;

    update_scene();
}

void ROI_SELECTION::on_qle_img_name_textChanged(const QString &arg1)
{
    int jump_img_number=arg1.toInt();

    if(jump_img_number != 0)
        permision_to_jump=true;
    else
        permision_to_jump=false;

    jump_to = jump_img_number /*- current_img_num */  ;

}


void ROI_SELECTION::on_action_Drew_in_Polygon_Mode_toggled(bool arg1)
{
    qDebug()<<"drew in polygon mode";
    qDebug()<<"prev draw_in_rect_mode "<< draw_in_rect_mode;
    draw_in_rect_mode = ! arg1;

    scene.is_first_selection = true;
    qDebug()<<"next draw_in_rect_mode "<< draw_in_rect_mode;
}

void ROI_SELECTION::on_btn_submit_all_clicked()
{
    QMessageBox::StandardButton reply;
    reply=QMessageBox::question(this, "Warning", "Are you sure process all images and exit?",
                                QMessageBox::Cancel|QMessageBox::Yes,QMessageBox::Cancel);
    if (reply == QMessageBox::Cancel)
    {
        return;
    }

    submit_all_and_exit=true;
    for(int i=0; i<IN_IMAGES_NAME.size(); i++)
    {

        submit_results_go_next();
    }

    exit(EXIT_SUCCESS);
}

void ROI_SELECTION::on_action_Undo_triggered()
{
    undo_scene();
}

void ROI_SELECTION::on_action_Redo_triggered()
{
    redo_scene();
}

void ROI_SELECTION::on_action_Hide_Mouse_Position_toggled(bool arg1)
{
    hide_mouse_position = arg1 ;
}

void ROI_SELECTION::on_action_Submit_All_Exit_triggered()
{
    QMessageBox::StandardButton reply;
    reply=QMessageBox::question(this, "Warning", "Are you sure process all images and exit?",
                                QMessageBox::Cancel|QMessageBox::Yes,QMessageBox::Cancel);
    if (reply == QMessageBox::Cancel)
    {
        return;
    }

    submit_all_and_exit=true;
    for(int i=0; i<IN_IMAGES_NAME.size(); i++)
    {

        submit_results_go_next();
    }

    exit(EXIT_SUCCESS);
}

void ROI_SELECTION::on_action_Help_triggered()
{
    help_win.setCurrentIndex(1);
    help_win.show();
}

void ROI_SELECTION::get_deleted_roi_signal()
{
    qDebug()<<" signal received";
    ui->btn_submit_next->setEnabled(true);
}

