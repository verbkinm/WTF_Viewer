#ifndef BATCH_PROCESSING_CUTTER_H
#define BATCH_PROCESSING_CUTTER_H

#include <QDir>
#include "../viewer_widget/frames/frames.h"

class Batch_Processing_Cutter
{
public:
    Batch_Processing_Cutter(const Frames &frames);

    void setData(int initial_empty_file, int work_space, int final_empty,
                 int number_of_packets,
                 int first_frames_of_a_packet, int good_frames, int last_frames_of_a_paket);

    void slice(const QDir &path);

private:
    size_t _initial_empty_file, _work_space, _final_empty_file,
        _number_of_packets,
        _first_frames_of_a_packet, _good_frames, _last_frames_of_a_paket;

    const Frames &_frames;

};

#endif // BATCH_PROCESSING_CUTTER_H
