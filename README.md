# ffmpegCamera
## 编译

windows系统下使用camke-gui编译，注意要指定sdl路径。
目前是debug模式。

## 相关知识点

### 过程

AVFrame—编码—》AVPacket

### 名词

**PTS和DTS**

PTS（Presentation Time Stamp）属于AVFrame，用于表示什么时候显示给用户。

DTS(Decoding Time Stamp)属于AVPacket，表示解码时间。

如果视频中各帧是按顺序输入的话，那么解码时间和显示时间一致，但很多编码标准，编码顺序和输入顺序并不一致，所以两者不同。

**GOP**Group Of Picture，两个I帧之间形成的一组图片。常要设定gop_size的值，gop越大，压缩率越高，节约出来的空间也可以保存更多I帧，画质会变好。

常见压缩率：I——7， P——20，B——50

**mux**全称multiplex，混流，将多路流（音频、视频）封装到一个流文件里。

**demux**逆操作。

## 经历

花在SDL和ffmpeg和CMake适配上的时间太多了。。

## 现存问题

卡，sdl窗口太卡了