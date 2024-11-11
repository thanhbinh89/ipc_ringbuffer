/*
Author: Thanhbinh89
*/

#ifndef __VIDEO_FRAME_HH__
#define __VIDEO_FRAME_HH__

#include <string>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

template <std::size_t FRAME_MAX_SIZE>
class AVFrame {
public:
    struct Header {
        uint64_t frameTs; // Timestamp in microseconds
        size_t frameSize;
    } __attribute__((packed));

    Header header;
    uint8_t data[FRAME_MAX_SIZE];

    AVFrame() { reset(); }

    void reset() {
        std::memset(&header, 0, sizeof(header));
        std::memset(data, 0, sizeof(data));
    }

    void setFrameData(uint64_t timestamp , uint8_t *dataIn, size_t dataSize) {
        header.frameTs = timestamp;
        header.frameSize = dataSize;
        std::memcpy(data, dataIn, dataSize);
    }
};

template <std::size_t BYTES_PER_GOP>
class AVGop {
public:
    uint64_t gopTs; // Timestamp of GOP in microseconds
    uint16_t frameCnt;
    uint8_t data[BYTES_PER_GOP];

    AVGop() { reset(); }

    void reset() {
        frameCnt = 0;
        gopAppendIdx = 0;
        std::memset(data, 0, sizeof(data));
    }

    void startNewGop(uint64_t timestamp) {
        reset();
        gopTs = timestamp;
    }

    template <std::size_t FRAME_MAX_SIZE>
    void appendFrame(const AVFrame<FRAME_MAX_SIZE> &frame) {
        std::memcpy(data + gopAppendIdx, &frame.header, sizeof(frame.header));
        gopAppendIdx += sizeof(frame.header);
        std::memcpy(data + gopAppendIdx, frame.data, frame.header.frameSize);
        gopAppendIdx += frame.header.frameSize;
        frameCnt++;
    }

    bool isEmpty() {
        return frameCnt == 0;
    }

private:
    uint32_t gopAppendIdx;
};

#endif // __VIDEO_FRAME_HH__