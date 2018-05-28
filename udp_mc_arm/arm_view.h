#ifndef ARM_VIEW_H
#define ARM_VIEW_H

#include <QWidget>
#include <QtGui>
#include <QTimer>

namespace Ui {
class arm_view;
}

class arm_view : public QWidget
{
    Q_OBJECT

public:
    explicit arm_view(QWidget *parent = 0);
    ~arm_view();

public slots:
    void clawUpdatePos(float x, float y);

private:
    Ui::arm_view *ui;
    float claw_x;
    float claw_y;
    QTimer* frame_timer;

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // ARM_VIEW_H
