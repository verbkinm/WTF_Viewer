#ifndef TRIPLESPINBOX_H
#define TRIPLESPINBOX_H

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>

class TripleSpinBox : public QWidget
{
    Q_OBJECT

public:
    TripleSpinBox(QWidget *parent = nullptr);

    void setReadOnly(bool left, bool center, bool right);
    void setMaximum(int value);
    void setTitle(const QString &left_title, const QString &center_title, const QString &right_title);

    int getLeftValue() const;
    int getCenterValue() const;
    int getRightValue() const;


private:
    QSpinBox _left, _center, _right;
    QLabel _left_label, _center_label, _right_label;
    QGridLayout _layout;
    int _maximum;

private slots:
    void slotValueChanged();

signals:
    void signalValueChanged(int, int, int);
};
#endif // TRIPLESPINBOX_H
