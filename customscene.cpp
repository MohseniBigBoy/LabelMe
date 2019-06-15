#include "customscene.h"

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event){
    pressed=true;
    try
    {

        //    if( is_first_selection==false /*&& draw_in_line_mode_auto==true*/ /*&& draw_in_line_mode_auto ==true*/)
        //    {
        //        item_ellipse->setPen(QPen(final_color,THICKNESS*2));
        //        item_ellipse->setBrush(QBrush(final_color,Qt::SolidPattern));
        //    }

        if(is_first_selection==false && draw_in_rect_mode == false)
        {
            if(item_ellipse == NULL)
                item_ellipse = new QGraphicsEllipseItem();
            item_ellipse->setPen(QPen(final_color,THICKNESS*2));
            item_ellipse->setBrush(QBrush(final_color,Qt::SolidPattern));
        }

        enterd_qstr_keys.clear();

        //    if(draw_in_line_mode==false)  // get command from another class
        //    {
        //        draw_in_line_mode_auto=false;
        //    }

        //if(draw_in_line_mode_auto==false)
        //{
        if(temp_write_text_into_roi != NULL)
            removeItem(temp_write_text_into_roi);
        temp_write_text_into_roi = new QGraphicsTextItem;
        //}

        begin_point = event->scenePos();
        if(is_first_selection==false)
            removeItem(item_rect);

        if(draw_in_rect_mode == false)
        {
            //            if(item_line !=NULL)
            //                delete item_line;
            if(item_rect !=NULL)
                removeItem(item_rect);

            item_line = new QGraphicsLineItem();
            vec_item_line.push_back(item_line);
            addItem(item_line);
            //  addItem(item_line);
        }
        else
        {
            if(item_rect != NULL)
                removeItem(item_rect);

            //            if(item_line != NULL)
            for(QGraphicsLineItem *l_item : vec_item_line)
            {
                removeItem(l_item);
            }

            item_rect = new QGraphicsRectItem();
            addItem(item_rect);
            last_drawn_roi_points.clear();
            last_drawn_roi_points.push_back(begin_point);
        }

    }

    catch (std::exception& e)
    {
        qDebug() << "Standard exception: " << e.what() ;
    }

    //    qDebug() << "begin_point: "<< begin_point.x() <<" , "<< begin_point.y();
}

void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    //    const QPointF curr_point = item_rect->mapFromScene(event->scenePos());
    const QPointF curr_point = event->scenePos();

    if( hide_mouse_position == false)
        QToolTip::showText(event->screenPos(),
                           QString::number( (int)event->scenePos().x()) + ","
                           + QString::number( (int)event->scenePos().y()));

    //      QGraphicsScene::mouseMoveEvent(event);

    if(pressed==true  /*&& opt_review_edit==false*/)
    {
        //        if(opt_review_edit == true && is_ROI_selected_to_edit==false /*&& permision_to_edit_label==false && permision_to_edit_roi==false && opt_review_edit_new_roi_label==false*/)
        //            return;
        if(draw_in_rect_mode == false && select_first_point == true)
        {
            QLineF l ; //= item_line->line();
            l.setP1(last_prev_point);
            l.setP2(curr_point);
            if(item_line != NULL)
            {
                item_line->setPen(QPen(drawing_color,THICKNESS));
                item_line->setLine(l);
            }
        }
        else
        {
            if(item_rect != NULL)
            {
                QRectF correct_rec=QRectF(begin_point,curr_point).normalized(); // return positive W and H
                item_rect->setRect(correct_rec);
                item_rect->setPen(QPen(drawing_color,THICKNESS));
            }
        }
    }
}

void CustomScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){

    pressed=false;

    //    qDebug()<< " is_first_selection "<< is_first_selection <<  "  draw_in_rect_mode " << draw_in_rect_mode;

    if(draw_in_rect_mode == true )
        end_point =item_rect->mapFromScene(event->scenePos());
    else
        end_point=event->scenePos();

    //    end_point=begin_point;
    //    removeItem(item_ellipse);
    //    QRectF circle_rect;
    //    circle_rect.setX(end_point.x()-THICKNESS*2);
    //    circle_rect.setY(end_point.y()-THICKNESS*2);
    //    circle_rect.setWidth(THICKNESS*4);
    //    circle_rect.setHeight(THICKNESS*4);
    //    item_ellipse=new QGraphicsEllipseItem();
    //    item_ellipse->setRect(circle_rect);
    //    item_ellipse->setPen(QPen(drawing_color,THICKNESS*2));
    //    item_ellipse->setBrush(QBrush(drawing_color,Qt::SolidPattern));
    //    addItem(item_ellipse);
    //    return;

//    qDebug() << "end_point: "<< end_point.x() <<" , " << end_point.y();
    // scene->temp_write_text_into_roi->setPlainText(label_of_edited_ROI);

    //    if (opt_review_edit==true /*&& draw_in_rect_mode == false*/ && (qAbs(begin_point.x()-end_point.x()) < MIN_DST_TO_DETECT_RECT_FROM_POINT) && (qAbs(begin_point.y()-end_point.y()) < MIN_DST_TO_DETECT_RECT_FROM_POINT))
    //    {
    //        //            qDebug()<<"point selected: "<<begin_point;
    //        //            removeItem(item_ellipse);
    //        //            QRectF circle_rect;
    //        //            circle_rect.setX(end_point.x()-THICKNESS*2);
    //        //            circle_rect.setY(end_point.y()-THICKNESS*2);
    //        //            circle_rect.setWidth(THICKNESS*4);
    //        //            circle_rect.setHeight(THICKNESS*4);
    //        //            item_ellipse=new QGraphicsEllipseItem();
    //        //            item_ellipse->setRect(circle_rect);
    //        //            item_ellipse->setPen(QPen(drawing_color,THICKNESS*2));
    //        //            item_ellipse->setBrush(QBrush(drawing_color,Qt::SolidPattern));
    //        //            addItem(item_ellipse);

    //        index_of_edited_ROI=select_ROI_if_point_is_inside_it(end_point);
    //        if(index_of_edited_ROI>-1)
    //        {
    //            is_ROI_selected_to_edit=true;
    //        }
    //        else
    //        {
    //            is_ROI_selected_to_edit=false;
    //            label_of_edited_ROI.clear();
    //        }
    //        return;
    //    }

    //    if(opt_review_edit==true && is_ROI_selected_to_edit==false && permision_to_edit_label==false && permision_to_edit_roi==false)
    //        return;

    //    qDebug()<<"permision_to_edit_label: "<<permision_to_edit_label;

    //      qDebug()<<"permision_to_edit_roi: "<<permision_to_edit_roi;
    //        qDebug()<<"is_ROI_selected_to_edit: "<<is_ROI_selected_to_edit;

    //    if(opt_review_edit==true && permision_to_edit_label==false && permision_to_edit_roi==false)
    //        return;

    if( vec_item_line.size() > 1)
    {
        vec_item_line[vec_item_line.size()-2]->setPen(QPen(final_color,THICKNESS));
    }

    //        if(is_first_selection==true)
    //        {
    //            if(qAbs(begin_point.x()-end_point.x()) < MIN_DST_TO_DETECT_RECT_FROM_POINT || qAbs(begin_point.y()-end_point.y()) < MIN_DST_TO_DETECT_RECT_FROM_POINT )
    //            {
    //                draw_in_line_mode_auto=true;
    //                qDebug()<<"You choose line mode";
    //            }
    //            else
    //            {
    //                draw_in_line_mode_auto=false;
    //                qDebug()<<"You choose rectangle mode";
    //            }
    //        }

    //        if(draw_in_line_mode==false)  // get command from another class
    //        {
    //            draw_in_line_mode_auto=false;
    //        }

    bool polygon_drawing_started=false;

    if( draw_in_rect_mode == false )
    {
        last_prev_point=end_point;
        //        qDebug()<<"you choose: "<<end_point;

        if( select_first_point==false &&  (qAbs(begin_point.x()-end_point.x()) < MIN_DST_TO_DETECT_RECT_FROM_POINT || qAbs(begin_point.y()-end_point.y()) < MIN_DST_TO_DETECT_RECT_FROM_POINT ))
        {

            last_drawn_roi_points.clear();

            select_first_point=true;
            QRectF circle_rect;
            circle_rect.setX(end_point.x()-THICKNESS*2);
            circle_rect.setY(end_point.y()-THICKNESS*2);
            circle_rect.setWidth(THICKNESS*4);
            circle_rect.setHeight(THICKNESS*4);
            //            addEllipse(circle_rect,QPen(Qt::red,radius),QBrush(Qt::red,Qt::SolidPattern));
            if (is_first_selection==false)
            {
                if(item_ellipse!=NULL)
                    removeItem(item_ellipse);
            }
            item_ellipse=new QGraphicsEllipseItem(0);
            item_ellipse->setRect(circle_rect);
            item_ellipse->setPen(QPen(drawing_color,THICKNESS*2));
            item_ellipse->setBrush(QBrush(drawing_color,Qt::SolidPattern));
            addItem(item_ellipse);
            for(int i=0; i<vec_item_line.size(); i++)
                removeItem(vec_item_line[i]);
            vec_item_line.clear();

            polygon_drawing_started=true;
        }
        //        qDebug()<<"Last point: "<<end_point;
    }
    else
    {
        qDebug()<<"Drawn Rect: "<<begin_point<<" to "<<end_point;
    }

    //    if(opt_drew_roi==true && (polygon_drawing_started==true || draw_in_rect_mode == true) )
    //    {
    //        if(temp_write_text_into_roi!=NULL)
    //        {
    //            //            if(draw_in_line_mode_auto==true )
    //            //                temp_write_text_into_roi->setPos(last_prev_point.x()+5,last_prev_point.y()+5);
    //            //            else
    //            temp_write_text_into_roi->setPos(begin_point.x()+5,begin_point.y()+5);

    //            temp_write_text_into_roi->setPlainText("ROI");
    //            temp_write_text_into_roi->setDefaultTextColor(drawing_color);
    //            addItem(temp_write_text_into_roi);
    //        }
    //    }

    last_drawn_roi_points.push_back(end_point);

    is_first_selection=false;

    if(draw_in_rect_mode == true)
    {
        QRectF rect=QRectF(begin_point,end_point).normalized();
        if( rect.width()*rect.height() < MIN_RECT_AREA)
        {
            qDebug()<<"The regtangle area is smaller than "<<MIN_RECT_AREA;
            removeItem(item_rect);
            removeItem(temp_write_text_into_roi);
            //            if(temp_write_text_into_roi->textWidth()>0)
            if(temp_write_text_into_roi!=NULL)
            {
                //            delete temp_write_text_into_roi;
                //                temp_write_text_into_roi->clearFocus();
                removeItem(temp_write_text_into_roi);
            }

            last_drawn_roi_points.clear();
        }

    }

    wait_labeling=true;
}

void CustomScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(opt_review_edit == false || drawned_rois.size() < 1 )
        return ;

//    qDebug() << " double clicked pressed ";
    pressed=false;
//    if(draw_in_rect_mode == true )
//        end_point = item_rect->mapFromScene(event->scenePos());
//    else
        end_point=event->scenePos();

    //            qDebug()<<"point selected: "<<begin_point;
    //            removeItem(item_ellipse);
    //            QRectF circle_rect;
    //            circle_rect.setX(end_point.x()-THICKNESS*2);
    //            circle_rect.setY(end_point.y()-THICKNESS*2);
    //            circle_rect.setWidth(THICKNESS*4);
    //            circle_rect.setHeight(THICKNESS*4);
    //            item_ellipse=new QGraphicsEllipseItem();
    //            item_ellipse->setRect(circle_rect);
    //            item_ellipse->setPen(QPen(drawing_color,THICKNESS*2));
    //            item_ellipse->setBrush(QBrush(drawing_color,Qt::SolidPattern));
    //            addItem(item_ellipse);

    index_of_edited_ROI=select_ROI_if_point_is_inside_it(end_point);
    //        qDebug()<< "index_of_edited_ROI : "<< index_of_edited_ROI;
    if(index_of_edited_ROI>-1)
    {
        drawned_rois_removed.push_back(drawned_rois[index_of_edited_ROI]);

        if(drawned_rois[index_of_edited_ROI].first != NULL)
            removeItem(drawned_rois[index_of_edited_ROI].first);
        if(drawned_rois[index_of_edited_ROI].second != NULL)
            removeItem(drawned_rois[index_of_edited_ROI].second);

        drawned_rois.remove(index_of_edited_ROI);
        wait_labeling = false;
        emit roi_deleted();
    }
}

