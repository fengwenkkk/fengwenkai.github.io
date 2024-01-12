
#ifndef FFMPEG_API_H
#define FFMPEG_API_H

extern double ffmpeg_GetMediaFileLength(const char *szFile);
extern int ffmpeg_MixFile(const char *szAudioFile, const char *szVideoFile, const char *szMixFile);
extern int ffmpeg_ConvertVideoFile(const char *szInputFile, const char *szOutputFile);
extern int ffmpeg_ConvertAudioFile(const char *szAudioFile, const char *szOutFile);

#endif
