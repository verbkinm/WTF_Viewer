#ifndef VIEWER_CLOG_PROCESSOR_H
#define VIEWER_CLOG_PROCESSOR_H

#include "../../frames/frames.h"
#include "filter_clog.h"
#include "viewer_processor.h"

class Viewer_Clog_Processor : public Viewer_Processor
{
public:
    const static int CLOG_SIZE = 256;

    Viewer_Clog_Processor();

    virtual QImage getImage() override;
    virtual QImage getRedrawnImage() override;

    std::vector<double> getClustersLengthVector() const;
    std::vector<double> getVectorOfLengthsOfTots() const;
    std::vector<double> getVectorOfSumOfTots() const;
    Frames const &getFrames() const;

    void setFilter(const Filter_Clog &filter);

private:
    Frames _frames;
    Filter_Clog _filter;

//    bool isWithinRanges(size_t frameNumber, size_t clusterNumber);

    void modifyPointAccordingFilter(size_t frameNumber, size_t clusterNumber);
    void modifyPointAccordingPixMode(OneFrame::ePoint &point);
    void modifyPoint(size_t frameNumber, size_t clusterNumber);
    void generalCalibrationSettingsForEPoint(OneFrame::ePoint &point);

    virtual void createVec2D() override;
    virtual void resetDataToDefault() override;
};

#endif // VIEWER_CLOG_PROCESSOR_H
