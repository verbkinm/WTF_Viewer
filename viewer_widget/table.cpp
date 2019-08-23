#include <QDoubleSpinBox>

#include "table.h"
#include "ui_table.h"

Table::Table(int size, std::vector<std::vector<double>> *vec, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Table),
    _vecBin(vec)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::Drawer);

    ui->tableWidget->setRowCount(size);
    ui->tableWidget->setColumnCount(size);

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        for (int column = 0; column < ui->tableWidget->columnCount(); ++column)
        {
            QDoubleSpinBox* spinbox = new QDoubleSpinBox;
            spinbox->setRange(-256.00, 256.00);
            spinbox->setSingleStep(0.01);
            spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
            spinbox->setAlignment(Qt::AlignHCenter);
            spinbox->setValue(_vecBin->at(static_cast<size_t>(row)).at(static_cast<size_t>(column)));
            ui->tableWidget->setCellWidget(row, column, spinbox);
            if(row % 2 == 0)
                spinbox->setStyleSheet("background-color: #c8c8c8;");
        }
    }

    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->resizeColumnsToContents();

    // для плавной прокрутки таблицы
    ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    this->resize(640, 480);
}

Table::~Table()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
        for (int column = 0; column < ui->tableWidget->columnCount(); ++column)
            delete ui->tableWidget->cellWidget(row, column);

    delete ui;
}

void Table::saveData()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
        for (int column = 0; column < ui->tableWidget->columnCount(); ++column)
             _vecBin->at(static_cast<size_t>(row)).at(static_cast<size_t>(column)) = qobject_cast<QDoubleSpinBox*>(ui->tableWidget->cellWidget(row, column))->value();
}

void Table::resizeEvent(QResizeEvent *)
{
    // растягиваем ширину столбцов до предела, если ширина всех столбцов меньше ширины тыблицы
    int widthColumns_TableLessonData = 0;
    for (int column = 0; column < ui->tableWidget->columnCount(); ++column)
        widthColumns_TableLessonData += ui->tableWidget->columnWidth(column);
    if(widthColumns_TableLessonData < ui->tableWidget->width()){
        for (int column = 0; column < ui->tableWidget->columnCount(); ++column)
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(column, QHeaderView::Stretch);
    }
    // растягиваем высоту строк до предела, если высота всех строк меньше высоты тыблицы
    int heightRows_TableLessonData = 0;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
        heightRows_TableLessonData += ui->tableWidget->rowHeight(row);
    if(heightRows_TableLessonData < ui->tableWidget->height())
    {
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
            ui->tableWidget->verticalHeader()->setSectionResizeMode(row, QHeaderView::Stretch);
    }
}
