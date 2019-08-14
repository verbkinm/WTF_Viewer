#ifndef VIEWER_PROCESSOR_H
#define VIEWER_PROCESSOR_H

#include <QSettings>
#include "viewer_widget/frames/frames.h"

class Viewer_Processor : public QObject
{
public:
    Viewer_Processor();

    enum marker
    {
        NO_MARKERS = 0x0,
        BORDER = 0x1,
        MASKING = 0x2,
        GENERAL_CALIBRATION = 0x4
    };
    Q_DECLARE_FLAGS(Markers_Flags, marker)
    Viewer_Processor::Markers_Flags _markers;

    QString getFileName() const;
    double getDataInVec2D(size_t column, size_t row);
    size_t getColumns();
    size_t getRows();

    void setDataInVec2D(size_t column, size_t row, double value);
    void setFileName(const QString &fileName);
    void setSettings(QSettings* const settings);

    QImage getImage();
    void   cutVec2D(int cutX, int cutY, size_t width, size_t height);

private:
    enum class fileType {UNDEFINED, TXT, CLOG};
    fileType _fileType;

    QSettings* _settings = nullptr;

    QString _fileName;
    Frames _frames;

    std::vector<std::vector<double>> _vec2D;
    std::vector<std::vector<double>> _vec2DMask;

    size_t _rows;
    size_t _columns;


    double findMaxInArrayOrigin();
    double findMinInArrayOrigin();

    double convert(double value,
                   double From1, double From2,
                   double To1, double To2);



    //void applyClogFilter(QImage& image);
    //void applyClogFilterAdditionalFunction(ePoint &point);

    void rebuildVec2DAccordingToSettings();
    void rebuildImageAccordingToSettings(QImage &image);

//    void generalCalibrationSettingsForArray(ePoint &point);
    void createFrameInVec2D();
    void createMaskInVec2D();

    //повороты
//    QImage rotatePlus();
//    QImage rotateMinus();

    void setFileType(const QString &fileName);
    void allocateEmptyVec2D(std::vector<std::vector<double> > &vec2D, size_t columns, size_t rows);
    void createVec2D();
    void createVec2DFromTXT();
    void createVec2DFromCLOG();
    void resetDataToDefault();

    QImage getImageFromVec2D();

    bool checkSettingsPtr();

};
Q_DECLARE_OPERATORS_FOR_FLAGS(Viewer_Processor::Markers_Flags)

#endif // VIEWER_PROCESSOR_H
