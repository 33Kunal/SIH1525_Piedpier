#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

int main() {
  // Open the input TS packet stream.
  AVFormatContext *fmt_ctx = NULL;
  if (avformat_open_input(&fmt_ctx, "input.ts", NULL, NULL) != 0) {
    fprintf(stderr, "Error opening input file.\n");
    return -1;
  }

  // Find the audio and video streams in the TS packet stream.
  int audio_stream_index = -1;
  int video_stream_index = -1;
  for (int i = 0; i < fmt_ctx->nb_streams; i++) {
    if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
      audio_stream_index = i;
    } else if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_index = i;
    }
  }

  // Create an AVFormatContext structure for the output MP4 file.
  AVFormatContext *out_fmt_ctx = NULL;
  if (avformat_alloc_output_context2(&out_fmt_ctx, NULL, "mp4", NULL) != 0) {
    fprintf(stderr, "Error allocating AVFormatContext.\n");
    return -1;
  }

  // Add an audio stream to the output MP4 file.
  AVStream *out_audio_stream = avformat_new_stream(out_fmt_ctx, NULL);
  if (out_audio_stream == NULL) {
    fprintf(stderr, "Error creating output audio stream.\n");
    return -1;
  }

  // Set the codec parameters for the output audio stream.
  out_audio_stream->codecpar->codec_id = fmt_ctx->streams[audio_stream_index]->codecpar->codec_id;
  out_audio_stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

  // Add a video stream to the output MP4 file.
  AVStream *out_video_stream = avformat_new_stream(out_fmt_ctx, NULL);
  if (out_video_stream == NULL) {
    fprintf(stderr, "Error creating output video stream.\n");
    return -1;
  }

  // Set the codec parameters for the output video stream.
  out_video_stream->codecpar->codec_id = fmt_ctx->streams[video_stream_index]->codecpar->codec_id;
  out_video_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;

  // Open the output MP4 file.
  if (avio_open(&out_fmt_ctx->pb, "output.mp4", AVIO_FLAG_WRITE) != 0) {
    fprintf(stderr, "Error opening output file.\n");
    return -1;
  }

  // Write the header of the output MP4 file.
  if (avformat_write_header(out_fmt_ctx, NULL) != 0) {
    fprintf(stderr, "Error writing header of output file.\n");
    return -1;
  }

  // Decode and write the audio and video frames to the output MP4 file.
  AVPacket pkt;
  int got_