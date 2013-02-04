#include "llibs.h"
#define __STDC_CONSTANT_MACROS 1
extern "C"{
	#include <libavformat/avformat.h>
	#include <libavutil/pixdesc.h>
	#include <libswscale/swscale.h>
	#include <libswresample/swresample.h>
}
#include <list>
#include <wx/filename.h>

// UTILITIES
#define DEMUXER "demuxer"
#define ISTREAM "istream"

struct VAStream;
struct VAFormat{
	AVFormatContext* format;
	std::list<VAStream*>* va_streams;
};
struct VAStream{
	AVStream *stream;
	VAFormat *va_format;
};

#define DECODE_TYPE(type) (type == AVMEDIA_TYPE_VIDEO || type == AVMEDIA_TYPE_AUDIO || type == AVMEDIA_TYPE_SUBTITLE)

#define SAFE_NAME(s) (s==NULL ? "unknown" : s)

inline double av_q2d_safe(AVRational a){
    return a.den == 0 ? 0 : av_q2d(a);
}

int gcd(int a, int b){
	// Swap bigger number to first number
	if(b > a)
		a ^= b ^= a ^= b;
	// Zero isn't a valid divisor
	if(b==0)
		return 1;
	// divisor = gcd ?
	int rest = a % b;
	if(rest == 0)
		return b;
	else
		return gcd(b, rest);
}

void lua_pushrational(lua_State *L, AVRational a){
	lua_createtable(L, 0, 2);
	lua_pushnumber(L, a.num); lua_setfield(L, -2, "num");
	lua_pushnumber(L, a.den); lua_setfield(L, -2, "den");
}

void lua_pushframe(lua_State *L, int64_t frame_i, AVPictureType frame_type, int width, int height, AVPicture *picture){
	// Create frame table
	lua_createtable(L, width * height * 3, 2);
	lua_pushnumber(L, frame_i); lua_setfield(L, -2, "i");
	char frame_type_str[2] = {av_get_picture_type_char(frame_type), '\0'};
	lua_pushstring(L, frame_type_str); lua_setfield(L, -2, "type");
	// Push RGB data
	unsigned long i = 0;
	uint8_t *row;
	for(unsigned int y = 0; y < height; y++){
		row = picture->data[0] + y * picture->linesize[0];
		for(unsigned int x = 0; x < width; x++){
			lua_pushnumber(L, *(row)); lua_rawseti(L, -2, ++i);	// r
			lua_pushnumber(L, *(row+1)); lua_rawseti(L, -2, ++i);	// g
			lua_pushnumber(L, *(row+2)); lua_rawseti(L, -2, ++i);	// b
			row += 3;
		}
	}
}

void lua_pushsamples(lua_State *L, uint64_t sample_i, int64_t channels, int nb_samples, uint8_t *data){
	// Create samples table
	unsigned int samples_n = channels * nb_samples;
	lua_createtable(L, samples_n, 1);
	lua_pushnumber(L, sample_i); lua_setfield(L, -2, "i");
	// Push s16 data
	int16_t *data16 = reinterpret_cast<int16_t*>(data);
	for(unsigned int sample_i = 0; sample_i < samples_n; sample_i++){
		lua_pushnumber(L, *data16++); lua_rawseti(L, -2, sample_i+1);
	}
}

// FUNCTIONS
DEF_HEAD_1ARG(list_formats, 0)
	// Count formats
	unsigned int i = 0;
	AVInputFormat *iformat = NULL;
	while((iformat = av_iformat_next(iformat)) != NULL)
		i++;
	// Formats to Lua
	lua_createtable(L, i, 0);
	i = 0;
	iformat = NULL;
	while((iformat = av_iformat_next(iformat)) != NULL){
		lua_pushstring(L, iformat->long_name); lua_rawseti(L, -2, ++i);
	}
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(list_codecs, 0)
	// Count codecs
	unsigned int i = 0;
	AVCodec *codec = NULL;
	while((codec = av_codec_next(codec)) != NULL)
		i++;
	// Codecs to Lua
	lua_createtable(L, i, 0);
	i = 0;
	codec = NULL;
	while((codec = av_codec_next(codec)) != NULL){
		lua_pushstring(L, codec->long_name); lua_rawseti(L, -2, ++i);
	}
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(create_demuxer, 1)
	// Get filename
	wxString filename = wxFileName(wxString::FromUTF8(luaL_checkstring(L, 1))).GetShortPath();
	// Create demuxer
	AVFormatContext *format = NULL;
	if(avformat_open_input(&format, filename, NULL, NULL) < 0)
		luaL_error2(L, "invalid file");
	// Find stream information
	if(avformat_find_stream_info(format, NULL) < 0){
		avformat_close_input(&format);
		luaL_error2(L, "couldn't find stream information");
	}
	// Create userdata
	VAFormat *va_format = lua_createuserdata<VAFormat>(L, DEMUXER);
	va_format->format = format;
	va_format->va_streams = new std::list<VAStream*>;
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(delete_demuxer, 1)
	// Get demuxer
	VAFormat *va_format = reinterpret_cast<VAFormat*>(luaL_checkuserdata(L, 1, DEMUXER));
	// Close stream codecs
	for(std::list<VAStream*>::iterator iter = va_format->va_streams->begin(); iter != va_format->va_streams->end(); ++iter){
		if( (*iter)->stream->codec && DECODE_TYPE((*iter)->stream->codec->codec_type) )
			avcodec_close((*iter)->stream->codec);
		(*iter)->va_format = NULL;
	}
	delete va_format->va_streams;
	// Delete demuxer
	avformat_close_input(&va_format->format);
DEF_TAIL

DEF_HEAD_1ARG(demuxer_info, 1)
	// Get demuxer
	VAFormat *va_format = reinterpret_cast<VAFormat*>(luaL_checkuserdata(L, 1, DEMUXER));
	// Create demuxer information table
	lua_createtable(L, 0, 8);
	lua_createtable(L, av_dict_count(va_format->format->metadata), 0);
	AVDictionaryEntry *entry = NULL;
	for(unsigned int i = 1; (entry = av_dict_get(va_format->format->metadata, "", entry, AV_DICT_IGNORE_SUFFIX)); i++){
		lua_createtable(L, 0, 2);
		lua_pushstring(L, entry->key); lua_setfield(L, -2, "key");
		lua_pushstring(L, entry->value); lua_setfield(L, -2, "value");
		lua_rawseti(L, -2, i);
	}
	lua_setfield(L, -2, "metadata");
	lua_createtable(L, va_format->format->nb_chapters, 0);
	for(unsigned int chapter_i = 0; chapter_i < va_format->format->nb_chapters; chapter_i++){
		AVChapter *chapter = va_format->format->chapters[chapter_i];
		lua_createtable(L, 0, 4);
		lua_pushnumber(L, chapter->start * av_q2d_safe(chapter->time_base)); lua_setfield(L, -2, "start");
		lua_pushnumber(L, chapter->end * av_q2d_safe(chapter->time_base)); lua_setfield(L, -2, "end");
		AVDictionaryEntry *entry = av_dict_get(chapter->metadata, "", NULL, AV_DICT_IGNORE_SUFFIX);
		lua_pushstring(L, entry ? entry->key : ""); lua_setfield(L, -2, "key");
		lua_pushstring(L, entry ? entry->value : ""); lua_setfield(L, -2, "value");
		lua_rawseti(L, -2, chapter_i+1);
	}
	lua_setfield(L, -2, "chapters");
	lua_pushnumber(L, va_format->format->nb_streams); lua_setfield(L, -2, "streams");
	lua_pushstring(L, va_format->format->filename); lua_setfield(L, -2, "filename");
	lua_pushstring(L, va_format->format->iformat->long_name); lua_setfield(L, -2, "container");
	lua_pushnumber(L, va_format->format->duration / static_cast<double>(AV_TIME_BASE)); lua_setfield(L, -2, "duration");
	lua_pushnumber(L, va_format->format->bit_rate); lua_setfield(L, -2, "bitrate");
	lua_pushnumber(L, avio_size(va_format->format->pb)); lua_setfield(L, -2, "filesize");
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(demuxer_get_stream, 2)
	// Get demuxer & stream index
	VAFormat *va_format = reinterpret_cast<VAFormat*>(luaL_checkuserdata(L, 1, DEMUXER));
	int stream_index = luaL_checknumber(L, 2);
	// Get stream
	if(stream_index < 0 || stream_index >= va_format->format->nb_streams)
		luaL_error2(L, "invalid stream index");
	AVStream *stream = va_format->format->streams[stream_index];
	for(std::list<VAStream*>::iterator iter = va_format->va_streams->begin(); iter != va_format->va_streams->end(); ++iter)
		if( (*iter)->stream == stream )
			luaL_error2(L, "stream already in use (or waiting for gc)");
	// Open stream codec
	if(DECODE_TYPE(stream->codec->codec_type)){
		AVCodec *codec = avcodec_find_decoder(stream->codec->codec_id);
		if(codec == NULL)
			luaL_error2(L, "codec not found");
		if(avcodec_open2(stream->codec, codec, NULL) < 0)
			luaL_error2(L, "couldn't open codec");
	}
	// Create userdata
	VAStream *va_stream = lua_createuserdata<VAStream>(L, ISTREAM);
	va_stream->stream = stream;
	va_stream->va_format = va_format;
	va_format->va_streams->push_back(va_stream);
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(delete_stream, 1)
	// Get stream
	VAStream *va_stream = reinterpret_cast<VAStream*>(luaL_checkuserdata(L, 1, ISTREAM));
	// Close stream codec & remove from stream list
	if(va_stream->va_format != NULL){
		if(va_stream->stream->codec && DECODE_TYPE(va_stream->stream->codec->codec_type))
			avcodec_close(va_stream->stream->codec);
		va_stream->va_format->va_streams->remove(va_stream);
	}
DEF_TAIL

DEF_HEAD_1ARG(stream_info, 1)
	// Get stream
	VAStream *va_stream = reinterpret_cast<VAStream*>(luaL_checkuserdata(L, 1, ISTREAM));
	if(va_stream->va_format == NULL)
		luaL_error2(L, "stream invalid (demuxer already deleted)");
	AVStream *stream = va_stream->stream;
	// Create stream information table
	lua_createtable(L, 0, 4);
	lua_pushnumber(L, stream->index); lua_setfield(L, -2, "index");
	lua_pushstring(L, SAFE_NAME(av_get_media_type_string(stream->codec->codec_type))); lua_setfield(L, -2, "type");
	switch(stream->codec->codec_type){
		case AVMEDIA_TYPE_VIDEO:
			lua_pushstring(L, stream->codec->codec->long_name); lua_setfield(L, -2, "codec");
			lua_pushnumber(L, stream->codec->bit_rate); lua_setfield(L, -2, "bitrate");
			lua_pushstring(L, SAFE_NAME(av_get_profile_name(stream->codec->codec, stream->codec->profile))); lua_setfield(L, -2, "profile");
			lua_pushnumber(L, stream->codec->level); lua_setfield(L, -2, "level");
			lua_pushrational(L, stream->r_frame_rate); lua_setfield(L, -2, "framerate");
			lua_pushnumber(L, stream->start_time * av_q2d_safe(stream->time_base)); lua_setfield(L, -2, "start_time");
			lua_pushnumber(L, stream->duration * av_q2d_safe(stream->time_base)); lua_setfield(L, -2, "duration");
			lua_pushnumber(L, stream->nb_frames); lua_setfield(L, -2, "frames");
			lua_pushnumber(L, stream->codec->width); lua_setfield(L, -2, "width");
			lua_pushnumber(L, stream->codec->height); lua_setfield(L, -2, "height");
			{
				int div = gcd(stream->codec->width, stream->codec->height);
				AVRational aspect_ratio = {stream->codec->width / div, stream->codec->height / div};
				lua_pushrational(L, aspect_ratio); lua_setfield(L, -2, "display_aspect_ratio");
			}
			lua_pushstring(L, SAFE_NAME(av_get_pix_fmt_name(stream->codec->pix_fmt))); lua_setfield(L, -2, "pixel_format");
			lua_pushnumber(L, stream->codec->has_b_frames); lua_setfield(L, -2, "b_frames");
			lua_pushnumber(L, stream->codec->refs); lua_setfield(L, -2, "reference_frames");
			break;
		case AVMEDIA_TYPE_AUDIO:
			lua_pushstring(L, stream->codec->codec->long_name); lua_setfield(L, -2, "codec");
			lua_pushnumber(L, stream->codec->bit_rate); lua_setfield(L, -2, "bitrate");
			lua_pushstring(L, SAFE_NAME(av_get_profile_name(stream->codec->codec, stream->codec->profile))); lua_setfield(L, -2, "profile");
			lua_pushnumber(L, stream->codec->level); lua_setfield(L, -2, "level");
			lua_pushnumber(L, stream->nb_frames); lua_setfield(L, -2, "samples");
			lua_pushnumber(L, stream->codec->sample_rate); lua_setfield(L, -2, "sample_rate");
			lua_pushnumber(L, stream->codec->channels); lua_setfield(L, -2, "channels");
			lua_pushstring(L, SAFE_NAME(av_get_sample_fmt_name(stream->codec->sample_fmt))); lua_setfield(L, -2, "sample_format");
			lua_pushnumber(L, stream->codec->sample_rate > 0 ? stream->nb_frames / static_cast<double>(stream->codec->sample_rate) : 0); lua_setfield(L, -2, "duration");
			{
				char buf[64] = {0};
				av_get_channel_layout_string(buf, sizeof(buf), stream->codec->channels, stream->codec->channel_layout);
				lua_pushstring(L, buf); lua_setfield(L, -2, "channel_layout");
			}
			lua_pushnumber(L, stream->codec->bits_per_coded_sample); lua_setfield(L, -2, "bits_per_sample");
			break;
		case AVMEDIA_TYPE_SUBTITLE:
			lua_pushstring(L, stream->codec->codec->long_name); lua_setfield(L, -2, "codec");
			lua_pushstring(L, stream->codec->subtitle_header_size > 0 ? reinterpret_cast<char*>(stream->codec->subtitle_header) : ""); lua_setfield(L, -2, "header");
			break;
	}
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(stream_get_frames, 2)
	// Get stream & callback function
	VAStream *va_stream = reinterpret_cast<VAStream*>(luaL_checkuserdata(L, 1, ISTREAM));
	if(va_stream->va_format == NULL)
		luaL_error2(L, "stream invalid (demuxer already deleted)");
	luaL_argcheck(L, lua_isfunction(L,2), 2, "function expected");
	// Demuxer pointers
	AVFormatContext *format = va_stream->va_format->format;
	AVStream *stream = va_stream->stream;
	// Set demuxer read pointer to stream start
	if(av_seek_frame(format, stream->index, 0, AVSEEK_FLAG_BACKWARD) < 0 &&
		av_seek_frame(format, stream->index, 0, 0) < 0)
		luaL_error2(L, "rewinding stream failed");
	// Decode stream
	switch(stream->codec->codec_type){
		case AVMEDIA_TYPE_VIDEO:{
				// Allocate image buffers & converter
				AVFrame *frame = avcodec_alloc_frame();
				AVPicture picture;
				SwsContext *sws_ctx = NULL;
				if(stream->codec->pix_fmt != AV_PIX_FMT_RGB24){
					avpicture_alloc(&picture, AV_PIX_FMT_RGB24, stream->codec->width, stream->codec->height);
					sws_ctx = sws_getContext(stream->codec->width, stream->codec->height, stream->codec->pix_fmt,
													stream->codec->width, stream->codec->height, AV_PIX_FMT_RGB24,
													SWS_FAST_BILINEAR, NULL, NULL, NULL);
				}
				// Stream data
				AVPacket packet, ref_packet;
				int decoded_bytes, got_frame;
				// Read stream
				while(av_read_frame(format, &packet) == 0){
					if(packet.stream_index == stream->index){
						// Unpack packet
						ref_packet = packet;
						do{
							// Decode frame
							if((decoded_bytes = avcodec_decode_video2(stream->codec, frame, &got_frame, &ref_packet)) < 0){
								av_free_packet(&packet);
								if(sws_ctx != NULL){
									sws_freeContext(sws_ctx);
									avpicture_free(&picture);
								}
								avcodec_free_frame(&frame);
								avcodec_flush_buffers(stream->codec);
								luaL_error2(L, "couldn't decode frame");
							}
							ref_packet.size -= decoded_bytes; ref_packet.data += decoded_bytes;
							// Found frame
							if(got_frame != 0){
								// Send frame to Lua
								lua_pushvalue(L, 2);
								if(sws_ctx != NULL){
									sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, picture.data, picture.linesize);
									lua_pushframe(L, frame->coded_picture_number, frame->pict_type, frame->width, frame->height, &picture);
								}else
									lua_pushframe(L, frame->coded_picture_number, frame->pict_type, frame->width, frame->height, reinterpret_cast<AVPicture*>(frame));
								if(lua_pcall(L, 1, 0, 0)){
									av_free_packet(&packet);
									if(sws_ctx != NULL){
										sws_freeContext(sws_ctx);
										avpicture_free(&picture);
									}
									avcodec_free_frame(&frame);
									avcodec_flush_buffers(stream->codec);
									luaL_error2(L, lua_tostring(L,-1));
								}
								lua_gc(L, LUA_GCCOLLECT, 0);
							}
						}while(ref_packet.size > 0);
					}
					av_free_packet(&packet);
				}
				// Flush cached frames
				packet.size = 0; packet.data = NULL;
				do{
					// Decode frame
					if(avcodec_decode_video2(stream->codec, frame, &got_frame, &packet) < 0){
						if(sws_ctx != NULL){
							sws_freeContext(sws_ctx);
							avpicture_free(&picture);
						}
						avcodec_free_frame(&frame);
						avcodec_flush_buffers(stream->codec);
						luaL_error2(L, "couldn't decode frame");
					}
					// Found frame
					if(got_frame != 0){
						// Send frame to Lua
						lua_pushvalue(L, 2);
						if(sws_ctx != NULL){
							sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, picture.data, picture.linesize);
							lua_pushframe(L, frame->coded_picture_number, frame->pict_type, frame->width, frame->height, &picture);
						}else
							lua_pushframe(L, frame->coded_picture_number, frame->pict_type, frame->width, frame->height, reinterpret_cast<AVPicture*>(frame));
						if(lua_pcall(L, 1, 0, 0)){
							if(sws_ctx != NULL){
								sws_freeContext(sws_ctx);
								avpicture_free(&picture);
							}
							avcodec_free_frame(&frame);
							avcodec_flush_buffers(stream->codec);
							luaL_error2(L, lua_tostring(L,-1));
						}
						lua_gc(L, LUA_GCCOLLECT, 0);
					}
				}while(got_frame != 0);
				// Free image buffers & converter
				if(sws_ctx != NULL){
					sws_freeContext(sws_ctx);
					avpicture_free(&picture);
				}
				avcodec_free_frame(&frame);
			}
			break;
		case AVMEDIA_TYPE_AUDIO:{
				// Allocate samples buffer & converter
				AVFrame *frame = avcodec_alloc_frame();
				SwrContext *swr_ctx = NULL;
				if(stream->codec->sample_fmt != AV_SAMPLE_FMT_S16){
					swr_ctx = swr_alloc_set_opts(NULL,
												stream->codec->channel_layout, AV_SAMPLE_FMT_S16, stream->codec->sample_rate,
												stream->codec->channel_layout, stream->codec->sample_fmt, stream->codec->sample_rate,
												0, NULL);
					if(swr_init(swr_ctx) < 0){
						avcodec_free_frame(&frame);
						luaL_error2(L, "couldn't initialize sample converter");
					}
				}
				// Stream data
				AVPacket packet, ref_packet;
				int decoded_bytes, got_frame;
				uint64_t sample_i = 0;
				// Read stream
				while(av_read_frame(format, &packet) == 0){
					if(packet.stream_index == stream->index){
						// Unpack packet
						ref_packet = packet;
						do{
							// Decode samples
							if((decoded_bytes = avcodec_decode_audio4(stream->codec, frame, &got_frame, &ref_packet)) < 0){
								av_free_packet(&packet);
								if(swr_ctx != NULL)
									swr_free(&swr_ctx);
								avcodec_free_frame(&frame);
								avcodec_flush_buffers(stream->codec);
								luaL_error2(L, "couldn't decode samples");
							}
							ref_packet.size -= decoded_bytes; ref_packet.data += decoded_bytes;
							// Found samples
							if(got_frame != 0){
								// Send samples to Lua
								lua_pushvalue(L, 2);
								if(swr_ctx != NULL){
									uint8_t *s16_data;
									av_samples_alloc(&s16_data, NULL, frame->channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
									swr_convert(swr_ctx, &s16_data, frame->nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
									lua_pushsamples(L, sample_i, frame->channels, frame->nb_samples, s16_data);
									av_free(s16_data);
								}else
									lua_pushsamples(L, sample_i, frame->channels, frame->nb_samples, frame->data[0]);
								if(lua_pcall(L, 1, 0, 0)){
									av_free_packet(&packet);
									if(swr_ctx != NULL)
										swr_free(&swr_ctx);
									avcodec_free_frame(&frame);
									avcodec_flush_buffers(stream->codec);
									luaL_error2(L, lua_tostring(L,-1));
								}
								lua_gc(L, LUA_GCCOLLECT, 0);
								sample_i += frame->nb_samples;
							}
						}while(ref_packet.size > 0);
					}
					av_free_packet(&packet);
				}
				// Flush cached samples
				packet.size = 0; packet.data = NULL;
				do{
					// Decode samples
					if(avcodec_decode_audio4(stream->codec, frame, &got_frame, &packet) < 0){
						if(swr_ctx != NULL)
							swr_free(&swr_ctx);
						avcodec_free_frame(&frame);
						avcodec_flush_buffers(stream->codec);
						luaL_error2(L, "couldn't decode samples");
					}
					// Found samples
					if(got_frame != 0){
						// Send samples to Lua
						lua_pushvalue(L, 2);
						if(swr_ctx != NULL){
							uint8_t *s16_data;
							av_samples_alloc(&s16_data, NULL, frame->channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
							swr_convert(swr_ctx, &s16_data, frame->nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
							lua_pushsamples(L, sample_i, frame->channels, frame->nb_samples, s16_data);
							av_free(s16_data);
						}else
							lua_pushsamples(L, sample_i, frame->channels, frame->nb_samples, frame->data[0]);
						if(lua_pcall(L, 1, 0, 0)){
							if(swr_ctx != NULL)
								swr_free(&swr_ctx);
							avcodec_free_frame(&frame);
							avcodec_flush_buffers(stream->codec);
							luaL_error2(L, lua_tostring(L,-1));
						}
						lua_gc(L, LUA_GCCOLLECT, 0);
						sample_i += frame->nb_samples;
					}
				}while(got_frame != 0);
				// Free samples buffer
				if(swr_ctx != NULL)
					swr_free(&swr_ctx);
				avcodec_free_frame(&frame);
			}
			break;
		case AVMEDIA_TYPE_SUBTITLE:{
				// Just SSA/ASS supported
				if(stream->codec->codec_id != AV_CODEC_ID_SSA)
					luaL_error2(L, "not supported subtitle format (SSA/ASS only)");
				// Subtitle buffer
				AVSubtitle subtitle;
				// Stream data
				AVPacket packet, ref_packet;
				int decoded_bytes, got_sub;
				// Read stream
				while(av_read_frame(format, &packet) == 0){
					if(packet.stream_index == stream->index){
						// Unpack packet
						ref_packet = packet;
						do{
							// Decode subtitle
							if((decoded_bytes = avcodec_decode_subtitle2(stream->codec, &subtitle, &got_sub, &ref_packet)) < 0){
								av_free_packet(&packet);
								avcodec_flush_buffers(stream->codec);
								luaL_error2(L, "couldn't decode subtitle");
							}
							ref_packet.size -= decoded_bytes; ref_packet.data += decoded_bytes;
							// Found subtitle
							if(got_sub != 0){
								// Send subtitle to Lua
								for(unsigned int i = 0; i < subtitle.num_rects; i++){
									lua_pushvalue(L, 2);
									lua_pushstring(L, subtitle.rects[i]->ass);
									if(lua_pcall(L, 1, 0, 0)){
										av_free_packet(&packet);
										avsubtitle_free(&subtitle);
										avcodec_flush_buffers(stream->codec);
										luaL_error2(L, lua_tostring(L,-1));
									}
									lua_gc(L, LUA_GCCOLLECT, 0);
								}
								avsubtitle_free(&subtitle);
							}
						}while(ref_packet.size > 0);
					}
					av_free_packet(&packet);
				}
				// Flush cached subtitles
				packet.size = 0; packet.data = NULL;
				do{
					// Decode subtitle
					if(avcodec_decode_subtitle2(stream->codec, &subtitle, &got_sub, &packet) < 0){
						avcodec_flush_buffers(stream->codec);
						luaL_error2(L, "couldn't decode subtitle");
					}
					// Found subtitle
					if(got_sub != 0){
						// Send subtitle to Lua
						for(unsigned int i = 0; i < subtitle.num_rects; i++){
							lua_pushvalue(L, 2);
							lua_pushstring(L, subtitle.rects[i]->ass);
							if(lua_pcall(L, 1, 0, 0)){
								avsubtitle_free(&subtitle);
								avcodec_flush_buffers(stream->codec);
								luaL_error2(L, lua_tostring(L,-1));
							}
							lua_gc(L, LUA_GCCOLLECT, 0);
						}
						avsubtitle_free(&subtitle);
					}
				}while(got_sub != 0);
			}
			break;
		default:{
				// Stream data
				AVPacket packet;
				// Read stream
				while(av_read_frame(format, &packet) == 0){
					if(packet.stream_index == stream->index){
						// Send packet to Lua
						lua_pushvalue(L, 2);
						char *packet_str = (char*)av_malloc(packet.size+1);
						memcpy(packet_str, packet.data, packet.size); packet_str[packet.size] = '\0';
						lua_pushstring(L, packet_str);
						av_free(packet_str);
						if(lua_pcall(L, 1, 0, 0)){
							av_free_packet(&packet);
							luaL_error2(L, lua_tostring(L,-1));
						}
						lua_gc(L, LUA_GCCOLLECT, 0);
					}
					av_free_packet(&packet);
				}
			}
			break;
	}
DEF_TAIL

// REGISTER
inline void register_va_meta(lua_State *L){
	// Initialize libav
	av_register_all();
	// Meta methods
	luaL_newmetatable(L, DEMUXER);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_delete_demuxer); lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, l_demuxer_info); lua_setfield(L, -2, "get_info");
	lua_pushcfunction(L, l_demuxer_get_stream); lua_setfield(L, -2, "get_stream");
	lua_pop(L, 1);
	luaL_newmetatable(L, ISTREAM);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_delete_stream); lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, l_stream_info); lua_setfield(L, -2, "get_info");
	lua_pushcfunction(L, l_stream_get_frames); lua_setfield(L, -2, "get_frames");
	lua_pop(L, 1);
}

inline void register_va_lib(lua_State *L){
	const luaL_Reg va[] = {
		{"formats", l_list_formats},
		{"codecs", l_list_codecs},
		{"create_demuxer", l_create_demuxer},
		{0, 0}
	};
	luaL_register(L, "va", va);
	lua_pop(L, 1);
}

void luaopen_va(lua_State *L){
	register_va_meta(L);
	register_va_lib(L);
}