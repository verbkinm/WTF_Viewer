#ifndef TABLE_H
#define TABLE_H

#include <QDialog>

namespace Ui {
class Table;
}

class Table : public QDialog
{
    Q_OBJECT

public:
    explicit Table(int size, double** array, QWidget *parent = nullptr);
    ~Table();

    void saveData();

private:
    Ui::Table *ui;

    double** arrayBin = nullptr;

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *);
};

#endif // TABLE_H
