#include "arm_view.h"
#include "ui_arm_view.h"

arm_view::arm_view(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::arm_view)
{
    ui->setupUi(this);

    claw_x = 18.5; //same initial value as in gamepadmonitor.h, except the y coordinate is *(-1)
    claw_y = -9.5;

    frame_timer = new QTimer(this);
    connect(frame_timer, SIGNAL(timeout()), this, SLOT(update()));
    frame_timer->start(33);
}

arm_view::~arm_view()
{
    delete ui;
}

void arm_view::clawUpdatePos(float x, float y)
{
    claw_x = x;
    claw_y = -y;
}

void arm_view::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    float scale = 15.0;
    QPointF origin(150, 300);
    //origin
    painter.drawLine(origin.x()-10, origin.y(), origin.x()+10, origin.y());
    painter.drawLine(origin.x(), origin.y()-10, origin.x(), origin.y()+10);
    //bounds
    painter.drawEllipse(origin, 30.93*scale, 30.93*scale);
    painter.drawEllipse(origin, 20.67*scale, 20.67*scale);
    painter.drawEllipse(origin+QPointF(14.8678*scale, -1.9870*scale), 18.01*scale, 18.01*scale);
    painter.drawEllipse(origin+QPointF(1.2187*scale, -14.9504*scale), 18.01*scale, 18.01*scale);
    //claw position
    painter.drawLine(origin.x()+claw_x*scale-5, origin.y()+claw_y*scale-5, origin.x()+claw_x*scale+5, origin.y()+claw_y*scale+5);
    painter.drawLine(origin.x()+claw_x*scale-5, origin.y()+claw_y*scale+5, origin.x()+claw_x*scale+5, origin.y()+claw_y*scale-5);
    //old bounds
    //painter.drawEllipse(origin + QPointF(14.85*scale, .1*scale), 14.46*scale, 13.78*scale);
    //painter.drawEllipse(origin + QPointF(10*scale, -9.3*scale), 14.83*scale, 14.14*scale);
}
