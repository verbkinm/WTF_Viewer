#include <QDebug>

#include "viewer_widget.h"
#include "ui_viewer_widget.h"
#include "checkFile/checkfile.h"
#include "iostream"
#include "cmath"
#include "fstream"

Viewer_widget::Viewer_widget(std::shared_ptr<const QSettings> spSetting, QWidget *parent) :
    QWidget(parent), _spSettings(spSetting),
    ui(new Ui::Viewer_widget)
{
    ui->setupUi(this);
    makeMaskTab();
    ui->graphicsView->setSettings(spSetting);
    _graphicsView_origin.setScene(ui->graphicsView->getScenePtr());
    _graphicsView_origin.hideAllPanel();
    _graphicsView_origin.hideSettingsButton(true);
    _mask_viewer.hideAllPanel();

    connect(&_mask_settings, SIGNAL(signalOpenTXT(QString)), &_mask_viewer, SLOT(slotSetImageFile(QString)));
    connect(&_mask_settings, SIGNAL(signalSaveTXT()), &_mask_viewer, SLOT(slotSaveTXT()));
    connect(&_mask_settings, SIGNAL(signalGenerated(QString)), &_mask_viewer, SLOT(slotSetImageFile(QString)));
    connect(&_mask_settings, SIGNAL(signalReconstruct_deconv()), SLOT(slotReconstruct_deconv()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(slotTabChanged(int)));
}

Viewer_widget::~Viewer_widget()
{
    delete ui;
}

void Viewer_widget::setImageFile(const QString &path)
{
    ui->graphicsView->setImageFileName(path);
}

std::pair<const Frames &, bool> Viewer_widget::getFrames() const
{
    return ui->graphicsView->getFrames();
}

void Viewer_widget::makeMaskTab()
{
    _main_splitter.setOrientation(Qt::Horizontal);
    _left_splitter.setOrientation(Qt::Vertical);
    _right_splitter.setOrientation(Qt::Vertical);

    _main_splitter.addWidget(&_left_splitter);
    _main_splitter.addWidget(&_right_splitter);

    _left_splitter.addWidget(&_graphicsView_origin);
    _left_splitter.addWidget(&_mask_viewer);
    _right_splitter.addWidget(&_graphicsView_Result);
    _right_splitter.addWidget(&_mask_settings);

    ui->tab_2->layout()->addWidget(&_main_splitter);
    _graphicsView_Result.hideAllPanel();

    _main_splitter.setStretchFactor(0,1);
}

QString Viewer_widget::processing_arrays(const std::vector<std::vector<float> > &origin_array, const std::vector<std::vector<float> > &mask_array)
{
    int h, w;                               // high, width
    float t, u, tmp;                        // Temp coeffisients
    float d1, d2, d3, d4;                   // Coefficients
    unsigned int p1, p2, p3, p4;            // Border pixels
    double origin_array248[248][248];       // Array for reconstruction
    double image240[240][240];              // Decoded array 240x240
    double image256[256][256];              // Resized output array 256x256



    /*______________________Resize_function_____________________*/
    qDebug()<<"Mask rank = "<<mask_array.size();
    qDebug()<<"Resized image to 248. Start.";
    {    for (int j = 0; j < 248; j++)
        {
            tmp = (float)(j) / (float)(248 - 1) * (256 - 1);
            h = (int)floor(tmp);

            if (h < 0)
                h = 0;
            else if (h >= 256 - 1)
                h = 256 - 2;
            u = tmp - h;

            for (int i = 0; i < 248; i++)
            {
                tmp = (float)(i) / (float)(248 - 1) * (256 - 1);
                w = (int)floor(tmp);

                if (w < 0)
                    w = 0;
                else if (w >= 256 - 1)
                    w = 256 - 2;
                t = tmp - w;

                // Coefficients
                d1 = (1 - t) * (1 - u);
                d2 = t * (1 - u);
                d3 = t * u;
                d4 = (1 - t) * u;

                // Border pixels for resized array
                p1 = origin_array[h][w];
                p2 = origin_array[h][w + 1];
                p3 = origin_array[h + 1][w + 1];
                p4 = origin_array[h + 1][w];

                origin_array248[i][j] = p1 * d1 + p2 * d2 + p3 * d3 + p4 * d4;
            }
        }
        qDebug()<<"Done!";
    }
    /*______________________Decoded_function_____________________*/
    qDebug()<<"Decoding function. Start.";
    try
    {
        for (int k = 0; k < 240; k++)
            for (int l = 0; l < 240; l++)
            {
                float sum = 0;
                for (int i = 0; i < 248; i++)
                    for (int j = 0; j < 248; j++)

                        if (mask_array[i+k][j+k]!=0)
                        {
                            {   if (((i+k)>mask_array.size())||((j+k)>mask_array.size()))
                                    throw 666;
                            }
                            sum = sum + origin_array248[i][j] * mask_array[i + k][j + l];
                        }
                        else continue;

                if (sum <= 0)
                    image240[k][l] = 0;
                else
                    image240[k][l] = sum;
            }

    qDebug()<<"Done!";
    /*______________________Resize_function_____________________*/
    qDebug()<<"Resized image to 256. Start.";
    {        for (int j = 0; j < 256; j++)
        {
            tmp = (float)(j) / (float)(256 - 1) * (240 - 1);
            h = (int)floor(tmp);

            if (h < 0)
                h = 0;
            else if (h >= 256 - 1)
                h = 256 - 2;
            u = tmp - h;

            for (int i = 0; i < 256; i++)

            {
                tmp = (float)(i) / (float)(256 - 1) * (240 - 1);
                w = (int)floor(tmp);

                if (w < 0)
                    w = 0;
                else if (w >= 240 - 1)
                    w = 240 - 2;
                t = tmp - w;

                // Coefficients
                d1 = (1 - t) * (1 - u);
                d2 = t * (1 - u);
                d3 = t * u;
                d4 = (1 - t) * u;

                // Border pixels for resized array
                p1 = image240[h][w];
                p2 = image240[h][w + 1];
                p3 = image240[h + 1][w + 1];
                p4 = image240[h + 1][w];

                image256[i][j] = p1 * d1 + p2 * d2 + p3 * d3 + p4 * d4;
            }
        }
        qDebug()<<"Done!";

        {
            qDebug()<<"Start to save file.";
            //Нужно не забыть про сохранение файла в tiff
            //FILE* image_f;
            //image_f = fopen("image.txt", "wb");
            //fwrite(image256, sizeof(image256), 1, image_f);
            //fclose(image_f);
            std::ofstream image_f;
            image_f.open("image.txt");

            for (int i = 0; i < 256; i++)
            {
                for (int j = 0; j < 256; j++)
                {
                    image_f << image256[i][j];
                    if (j<255)
                        image_f << " ";
                }
                image_f << std::endl;
            }
            image_f.close();

            qDebug()<<"Some file was saved somewhere!";
        }
    }
    return("image.txt");
    }
    catch(int erorr_number)
    {
        qDebug() << "Erorr number " << erorr_number << " OUT_OF_RANGE!" << endl;
        return ("666.txt");
    }
}

void Viewer_widget::slotTabChanged(int value)
{
    /* при смене вкладки на Mask делаем так, чтобы на этой вкладке на виджете graphicsView
     * нельзя было рисовать, создавать и перемещать рамку*/
    if(value == MASK)
        ui->graphicsView->setSceneReadOnly(true);
    else if(value == VIEW_AND_EDIT)
        ui->graphicsView->setSceneReadOnly(false);
}

void Viewer_widget::slotReconstruct_deconv()
{
    if(ui->graphicsView->getScenePtr()->items().length() == 1)
    {
        auto item = qgraphicsitem_cast<QGraphicsTextItem*>(ui->graphicsView->getScenePtr()->items().at(0));
        if(item && item->objectName() == "incorrectFile")
            return;
    }

    auto originArray = ui->graphicsView->getVec2D();
    auto maskArray = _mask_viewer.getVec2D();

    _graphicsView_Result.setImageFileName(processing_arrays(originArray, maskArray));
}
