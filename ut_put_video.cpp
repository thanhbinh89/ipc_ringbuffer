#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <stdexcept>
#include <ctime>
#include <sys/time.h>

#include "shmringbuffer.hh"
#include "av_frame.hh"

#define V1080P_FRAME_MAX_SIZE           (300 * 1024)
#define V1080P_BIT_PER_SECOND           (3 * 1024 * 1024 * 3 / 2) // 3Mbps + 50 %
#define V1080P_BYTE_PER_SECOND          (V1080P_BIT_PER_SECOND / 8)
#define V1080P_FPS                      (13)
#define V1080P_GOP                      (13)
#define V1080P_SECONDS_PER_GOP          (V1080P_GOP / V1080P_FPS)
#define V1080P_BYTES_PER_GOP            (V1080P_SECONDS_PER_GOP * V1080P_BYTE_PER_SECOND)

#define V1080P_BUFFER_SECONDS           (26)
#define V1080P_BUFFER_GOPS              (V1080P_BUFFER_SECONDS / V1080P_SECONDS_PER_GOP)

#define V720P_FRAME_MAX_SIZE            (300 * 1024)
#define V720P_BIT_PER_SECOND            (512 * 1024 * 3 / 2) // 512kbps + 50 %
#define V720P_BYTE_PER_SECOND           (V720P_BIT_PER_SECOND / 8)
#define V720P_FPS                       (13)
#define V720P_GOP                       (13)
#define V720P_SECONDS_PER_GOP           (V720P_GOP / V720P_FPS)
#define V720P_BYTES_PER_GOP             (V720P_SECONDS_PER_GOP * V720P_BYTE_PER_SECOND)

#define V720P_BUFFER_SECONDS            (30)
#define V720P_BUFFER_GOPS               (V720P_BUFFER_SECONDS / V720P_SECONDS_PER_GOP)

uint64_t currentTimeInMicroSeconds() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return uint64_t(time.tv_sec) * 1000 * 1000 + time.tv_usec;
}

int main(int argc, char *argv[]) {

    // Initialize shared memory ring buffers for frames and GOPs
    ShmRingBuffer<AVFrame<V720P_FRAME_MAX_SIZE>> v720pFrameBuff(V720P_FPS, true, "v720p_frame_buff");
    ShmRingBuffer<AVGop<V720P_BYTES_PER_GOP>> v720pGopBuff(V720P_BUFFER_GOPS, true, "v720p_gop_buff");

    // Create frame and GOP objects for simulation
    AVFrame<V720P_FRAME_MAX_SIZE> v720pFrame;
    AVGop<V720P_BYTES_PER_GOP> v720pGop;
    

    for (int f = 0; f < V720P_BUFFER_SECONDS * V720P_FPS * 5; f++) {
        // Simulate frame emission
        uint64_t timestamp = currentTimeInMicroSeconds();
        uint8_t data[128] = {0};
        size_t dataLen = sprintf((char *)data, "data-test-%d: %lu", f, timestamp);

        // Clone frame
        v720pFrame.setFrameData(timestamp, (uint8_t *)data, dataLen);
        printf("index: %02d, frameTs: %lu, frameSize: %lu\n", 
                f, v720pFrame.header.frameTs, v720pFrame.header.frameSize);

        // Puth to frame buffer
        v720pFrameBuff.push_back(v720pFrame); // Push frame to frame buffer

        // Check new gop
        if (f % V720P_FPS == 0) { // Start new GOP if it's an I-frame
            // Check gop has any frame
            if (!v720pGop.isEmpty()) {
                // Put to gop buffer
                v720pGopBuff.push_back(v720pGop); // Push previous GOP to buffer
                printf("index: %02d, gopTs: %lu, frameCnt: %d\n", 
                    f, v720pGop.gopTs, v720pGop.frameCnt);
            }
            // Reuse gop
            v720pGop.startNewGop(timestamp);
        }
        // Append frame to gop
        v720pGop.appendFrame(v720pFrame);
        
        // simulation frame wait
        usleep(1000000 / V720P_FPS);
    }
}