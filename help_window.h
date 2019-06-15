#ifndef HELP_WINDOW_H
#define HELP_WINDOW_H

#include <QStackedWidget>
#include <QStyle>
#include <QDesktopWidget>

namespace Ui {
class help_window;
}

class help_window : public QStackedWidget
{
    Q_OBJECT

public:
    explicit help_window(QWidget *parent = 0);
    ~help_window();

private slots:
    void on_prev_page_clicked();

    void on_next_page_clicked();

private:
    Ui::help_window *ui;
};

#endif // HELP_WINDOW_H
