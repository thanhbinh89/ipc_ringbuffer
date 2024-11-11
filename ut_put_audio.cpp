#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <stdexcept>
#include <ctime>
#include <sys/time.h>

#include "shmringbuffer.hh"
#include "av_frame.hh"

#define ALAW_FRAME_MAX_SIZE            (2 * 1024)
#define ALAW_BYTE_PER_SECOND           (8000)
#define ALAW_SPS                       (64)
#define ALAW_GOP                       (64)
#define ALAW_SECONDS_PER_GOP           (ALAW_GOP / ALAW_SPS)
#define ALAW_BYTES_PER_GOP             (ALAW_SECONDS_PER_GOP * ALAW_BYTE_PER_SECOND)

#define ALAW_BUFFER_SECONDS            (30)
#define ALAW_BUFFER_GOPS               (ALAW_BUFFER_SECONDS / ALAW_SECONDS_PER_GOP)

uint64_t currentTimeInMicroSeconds() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return uint64_t(time.tv_sec) * 1000 * 1000 + time.tv_usec;
}

int main(int argc, char *argv[]) {

    // Initialize shared memory ring buffers for frames and GOPs
    ShmRingBuffer<AVFrame<ALAW_FRAME_MAX_SIZE>> alawFrameBuff(ALAW_SPS, true, "alaw_frame_buff");
    ShmRingBuffer<AVGop<ALAW_BYTES_PER_GOP>> alawGopBuff(ALAW_BUFFER_GOPS, true, "alaw_gop_buff");

    // Create frame and GOP objects for simulation
    AVFrame<ALAW_FRAME_MAX_SIZE> alawFrame;
    AVGop<ALAW_BYTES_PER_GOP> alawGop;
    

    for (int f = 0; f < ALAW_BUFFER_SECONDS * ALAW_SPS * 5; f++) {
        // Simulate frame emission
        uint64_t timestamp = currentTimeInMicroSeconds();
        uint8_t data[128] = {0};
        size_t dataLen = sprintf((char *)data, "data-test-%d: %lu", f, timestamp);

        // Clone frame
        alawFrame.setFrameData(timestamp, (uint8_t *)data, dataLen);
        printf("index: %02d, frameTs: %lu, frameSize: %lu\n", 
                f, alawFrame.header.frameTs, alawFrame.header.frameSize);

        // Puth to frame buffer
        alawFrameBuff.push_back(alawFrame); // Push frame to frame buffer

        // Check new gop
        if (f % ALAW_SPS == 0) { // Start new GOP if video stream have an I-frame
            // Check gop has any frame
            if (!alawGop.isEmpty()) {
                // Put to gop buffer
                alawGopBuff.push_back(alawGop); // Push previous GOP to buffer
                printf("index: %02d, gopTs: %lu, frameCnt: %d\n", 
                    f, alawGop.gopTs, alawGop.frameCnt);
            }
            // Reuse gop
            alawGop.startNewGop(timestamp);
        }
        // Append frame to gop
        alawGop.appendFrame(alawFrame);
        
        // simulation frame wait
        usleep(1000000 / ALAW_SPS);
    }
}