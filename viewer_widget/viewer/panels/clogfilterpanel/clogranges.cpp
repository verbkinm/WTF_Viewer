#include "clogranges.h"

ClogRanges::ClogRanges()
{

}

void ClogRanges::setListModel(const std::vector<size_t> &vector)
{
    for(const auto &value : vector)
        _listModel << QString::number(value);
}

QStringList ClogRanges::getListModel() const
{
    return _listModel;
}

QString ClogRanges::getListBeginLast() const
{
    return _listModelBeginLast;
}

QString ClogRanges::getListEndLast() const
{
    return _listModelEndLast;
}

void ClogRanges::clear()
{
    _listModel.clear();
    _listModelBegin.clear();
    _listModelEnd.clear();
}

QStringList ClogRanges::getListModelBegin() const
{
    return _listModelBegin;
}

void ClogRanges::setListModelBegin(const QStringList &listModelBegin)
{
    _listModelBegin = listModelBegin;
}

QStringList ClogRanges::getListModelEnd() const
{
    return _listModelEnd;
}

void ClogRanges::setListModelEnd(const QStringList &listModelEnd)
{
    _listModelEnd = listModelEnd;
}
