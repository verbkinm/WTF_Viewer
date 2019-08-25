#ifndef TABLE_H
#define TABLE_H

#include <QDialog>
#include <vector>
#include <memory>

namespace Ui {
class Table;
}

class Table : public QDialog
{
    Q_OBJECT

public:
    explicit Table(int size, std::vector<std::vector<double>> *vec, QWidget *parent = nullptr);
    ~Table();

    void saveData();

private:
    Ui::Table *ui;
    std::vector<std::vector<double>> *_vecBin;

protected:
    virtual void resizeEvent(QResizeEvent *);
};

#endif // TABLE_H
