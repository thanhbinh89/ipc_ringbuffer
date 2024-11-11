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

#define V720P_BUFFER_SECONDS            (26)
#define V720P_BUFFER_GOPS               (V720P_BUFFER_SECONDS / V720P_SECONDS_PER_GOP)

int main(void) {

    // Initialize shared memory ring buffers for frames and GOPs
    ShmRingBuffer<AVFrame<V720P_FRAME_MAX_SIZE>> v720pFrameBuff(V720P_FPS, false, "v720p_frame_buff");   

    for (int f = 0; f < V720P_BUFFER_SECONDS * V720P_FPS * 5; f++) {        
        try {
            AVFrame<V720P_FRAME_MAX_SIZE> v720pFrame = v720pFrameBuff.dump_front();
            printf("index: %02d, frameTs: %lu, frameSize: %lu, frameData: %s\n", 
                f, v720pFrame.header.frameTs, v720pFrame.header.frameSize, (const char *)v720pFrame.data);
        }
        catch(const std::exception& e) {
            printf("index: %02d, exception: %s\n", f, e.what());
        }

        // simulation frame wait
        usleep(1000000 / V720P_FPS);
    }
}