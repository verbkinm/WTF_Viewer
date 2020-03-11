#ifndef VIEWER_CLOG_PROCESSOR_H
#define VIEWER_CLOG_PROCESSOR_H

#include "../../frames/frames.h"
#include "viewer_processor.h"

class Viewer_Clog_Processor : public Viewer_Processor
{
public:
    const static int CLOG_SIZE = 256;

    Viewer_Clog_Processor();

    virtual QImage getImage() override;
    virtual QImage getRedrawnImage() override;

    std::pair<float, float> getClusterRange() const;
    std::vector<size_t> getClustersLengthVector() const;

    std::pair<float, float> getTotsRange() const;
    std::vector<float> getVectorOfLengthsOfTots() const;

    std::pair<float, float> getTotsSumRange() const;
    std::vector<float> getVectorOfSumOfTots() const;
    Frames const &getFrames() const;

    void setFilter(const Filter_Clog &filter);
    virtual void clear() override;

private:
    Frames _frames;

    void modifyPointAccordingFilter(size_t frameNumber, size_t clusterNumber);
    void modifyPointAccordingPixMode(OneFrame::ePoint *point);
    void modifyPoint(size_t frameNumber, size_t clusterNumber);
    void generalCalibrationSettingsForEPoint(OneFrame::ePoint *point);

    void setMarkersGeneralOrTot();

    virtual bool createVec2D() override;
    virtual void resetDataToDefault() override;
};

#endif // VIEWER_CLOG_PROCESSOR_H
