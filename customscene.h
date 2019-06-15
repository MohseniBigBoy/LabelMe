#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H


//#include <QGraphicsSceneMouseEvent>
//#include <QGraphicsView>
//#include <QGraphicsLineItem>
//#include <QGraphicsRectItem>
//#include <QDebug>

#include <exception>

#include "defs.h"

class CustomScene : public QGraphicsScene
{
    Q_OBJECT

public:

    QVector<QPointF> last_drawn_roi_points;

    QGraphicsTextItem *temp_write_text_into_roi = NULL ; //= new QGraphicsTextItem;

    QVector<QGraphicsLineItem *> vec_item_line;

    QGraphicsEllipseItem *item_ellipse = NULL; // beginning point in line_mode

    bool is_first_selection=true;

    inline void initial_to_draw_roi()
    {
        pressed=true;
        select_first_point=false;
        removeItem(item_rect);
        for(QGraphicsLineItem * grl: vec_item_line)
             removeItem(grl);
        removeItem(temp_write_text_into_roi);
        removeItem(item_ellipse);
    }

    bool wait_labeling = false;


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

 signals:
      void roi_deleted() ;

private:

    bool pressed=false;

    bool select_first_point=false;

    bool draw_in_line_mode_auto=false;

    QPointF begin_point;
    QPointF end_point;
    QPointF last_prev_point;

    QGraphicsLineItem *item_line=NULL;

    QGraphicsRectItem *item_rect=NULL;


};


#endif // CUSTOMSCENE_H
