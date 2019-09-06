#ifndef CLOGRANGES_H
#define CLOGRANGES_H

#include <QStringList>
#include <vector>
#include <QModelIndexList>

class ClogRanges
{
public:
    ClogRanges();

    void setListModel(const std::vector<size_t> &vector);
    void setListModelBegin(const QStringList &listModelBegin);
    void setListModelEnd(const QStringList &listModelEnd);

    QStringList getListModel() const;
    QStringList getListModelBegin() const;
    QStringList getListModelEnd() const;
    QString getListBeginLast() const;
    QString getListEndLast() const;

    void clear();

private:
    QStringList _listModel, _listModelBegin, _listModelEnd;
    QString _listModelBeginLast, _listModelEndLast;
};

#endif // CLOGRANGES_H
