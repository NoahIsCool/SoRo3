#ifndef DRILL_H
#define DRILL_H

#include <QWidget>

namespace Ui {
class drill;
}

class drill : public QWidget
{
    Q_OBJECT

public:
    explicit drill(QWidget *parent = 0);
    ~drill();

private:
    Ui::drill *ui;
};

#endif // DRILL_H
