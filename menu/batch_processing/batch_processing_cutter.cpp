#include <fstream>
#include <QMessageBox>
#include <QApplication>
#include <QFileInfo>

#include "batch_processing_cutter.h"

Batch_Processing_Cutter::Batch_Processing_Cutter(const Frames &frames) :
    _initial_empty_file(0), _work_space(0), _final_empty_file(0),
    _number_of_packets(0),
    _first_frames_of_a_packet(0), _good_frames(0), _last_frames_of_a_paket(0),
    _frames(frames)
{

}

void Batch_Processing_Cutter::setData(int initial_empty_file, int work_space, int final_empty,
                                      int number_of_packets,
                                      int first_frames_of_a_packet, int good_frames, int last_frames_of_a_paket)
{
    _initial_empty_file = initial_empty_file;
    _work_space = work_space;
    _final_empty_file = final_empty;
    _number_of_packets = number_of_packets;
    _first_frames_of_a_packet = first_frames_of_a_packet;
    _good_frames = good_frames;
    _last_frames_of_a_paket = last_frames_of_a_paket;

}

void Batch_Processing_Cutter::slice(const QDir &path)
{   
    int good = 0, failed = 0;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    size_t frames_in_a_packet = _first_frames_of_a_packet + _good_frames + _last_frames_of_a_paket;

    for (size_t number_of_packet = 0; number_of_packet < _number_of_packets; ++number_of_packet)
    {
        QFileInfo fi(path, QString::number(number_of_packet) + ".clog");
        QFile fileOut(fi.filePath());
        fileOut.open(QIODevice::WriteOnly);
        if(!fileOut.isOpen())
        {
            failed++;
            continue;
        }

        size_t start_packet = _initial_empty_file + number_of_packet * frames_in_a_packet;
        size_t end_packet = start_packet + frames_in_a_packet;

        for(size_t frame_number = start_packet + _first_frames_of_a_packet; frame_number < end_packet - _last_frames_of_a_paket; ++frame_number)
        {
            auto ptr = _frames.getFrame(frame_number);
            if(ptr)
                fileOut.write(ptr->toString().c_str());
        }

        good++;
        fileOut.close();
    }
    QApplication::restoreOverrideCursor();

    std::string text = "written " + std::to_string(good) + " files,\n"
                       "error write " + std::to_string(failed) + " files.";

    QMessageBox::information(nullptr, "Information", text.c_str());
}
