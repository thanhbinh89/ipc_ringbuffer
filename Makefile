#		= /home/ubuntu/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-
CXX			= $(CROSS)g++
CC			= $(CROSS)gcc
STRIP		= $(CROSS)strip

all: clean  ut_put_video ut_get_video_frame ut_get_video_gop ut_put_audio

ut_put_video:
	$(CXX) -o ut_put_video ut_put_video.cpp -I ./

ut_get_video_frame:
	$(CXX) -o ut_get_video_frame ut_get_video_frame.cpp -I ./

ut_get_video_gop:
	$(CXX) -o ut_get_video_gop ut_get_video_gop.cpp -I ./

ut_put_audio:
	$(CXX) -o ut_put_audio ut_put_audio.cpp -I ./

clean:
	rm -rf ut_put_video ut_get_video_frame ut_get_video_gop ut_put_audio

strip:
	$(STRIP) ut_put_video ut_get_video_frame ut_get_video_gop ut_put_audio