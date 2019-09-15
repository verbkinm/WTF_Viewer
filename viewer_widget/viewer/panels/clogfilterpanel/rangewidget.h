#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <vector>
#include <QVBoxLayout>

namespace Ui {
class RangeWidget;
}

class RangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RangeWidget(QWidget *parent = nullptr);
    ~RangeWidget();

    void setRange(const std::vector<double> &vector);

    double getRangeBegin() const;
    double getRangeEnd() const;

protected:
    QGridLayout *layout();

private:
    Ui::RangeWidget *ui;

    void fillBeginComboBox();
    void fillEndComboBox();

    bool containsInVector(QString str_value);

    void disconnectSignals();
    void connectSignals();

    std::vector<double> _vector;
    QString _beginLast, _endLast;

private slots:
    void slotChangeBeginRange(QString currentText);
    void slotChangeEndRange(QString currentText);
//    void slotRangeGroup(bool checked);

};

#endif // WIDGET_H
