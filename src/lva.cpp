#include "llibs.h"
#define __STDC_CONSTANT_MACROS 1
extern "C"{
	#include <libavformat/avformat.h>
	#include <libavutil/pixdesc.h>
	#include <libswscale/swscale.h>
}
#include <list>
#include <wx/filename.h>

// UTILITIES
#define DEMUXER "demuxer"
#define MUXER "muxer"
#define ISTREAM "istream"
#define OSTREAM "ostream"

struct VAStream;
struct VAFormat{
	AVFormatContext* format;
	std::list<VAStream*>* va_streams;
};
struct VAStream{
	AVStream *stream;
	VAFormat *va_format;
};

inline double av_q2d_safe(AVRational a){
    return a.den == 0 ? 0 : av_q2d(a);
}

void lua_pushrational(lua_State *L, AVRational a){
	lua_createtable(L, 0, 2);
	lua_pushnumber(L, a.num); lua_setfield(L, -2, "num");
	lua_pushnumber(L, a.den); lua_setfield(L, -2, "den");
}

#define SAFE_NAME(s) s==NULL ? "unknown" : s

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

// FUNCTIONS
DEF_HEAD_1ARG(create_demuxer, 1)
	// Get filename
	wxString filename = wxFileName(wxString::FromUTF8(luaL_checkstring(L, 1))).GetShortPath();
	// Create demuxer
	AVFormatContext *format = NULL;
	if(avformat_open_input(&format, filename, NULL, NULL) < 0)
		luaL_error2(L, wxString::Format("invalid file: \"%s\"", filename));
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
		if( (*iter)->stream->codec )
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
	lua_pushnumber(L, va_format->format->nb_streams); lua_setfield(L, -2, "streams");
	lua_pushstring(L, va_format->format->filename); lua_setfield(L, -2, "filename");
	lua_pushstring(L, va_format->format->iformat->long_name); lua_setfield(L, -2, "container");
	lua_pushnumber(L, va_format->format->start_time / static_cast<double>(AV_TIME_BASE)); lua_setfield(L, -2, "start_time");
	lua_pushnumber(L, va_format->format->duration / static_cast<double>(AV_TIME_BASE)); lua_setfield(L, -2, "duration");
	lua_pushnumber(L, va_format->format->bit_rate); lua_setfield(L, -2, "bitrate");
	lua_pushnumber(L, va_format->format->nb_chapters); lua_setfield(L, -2, "chapters");
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
			luaL_error2(L, "stream already in use");
	// Open stream codec
	AVCodec *codec = avcodec_find_decoder(stream->codec->codec_id);
	if(codec == NULL)
		luaL_error2(L, wxString::Format("codec not found: %s", stream->codec->codec_name));
	if(avcodec_open2(stream->codec, codec, NULL) < 0)
		luaL_error2(L, "couldn't open codec");
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
		if(va_stream->stream->codec)
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
	lua_createtable(L, 0, 2);
	lua_pushnumber(L, stream->index); lua_setfield(L, -2, "index");
	lua_pushstring(L, SAFE_NAME(av_get_media_type_string(stream->codec->codec_type))); lua_setfield(L, -2, "type");
	switch(stream->codec->codec_type){
		case AVMEDIA_TYPE_VIDEO:
			lua_pushstring(L, stream->codec->codec->long_name); lua_setfield(L, -2, "codec");
			lua_pushnumber(L, stream->codec->bit_rate); lua_setfield(L, -2, "bitrate");
			lua_pushstring(L, SAFE_NAME(av_get_profile_name(stream->codec->codec, stream->codec->profile))); lua_setfield(L, -2, "profile");
			lua_pushnumber(L, stream->codec->level); lua_setfield(L, -2, "level");
			lua_pushrational(L, stream->r_frame_rate); lua_setfield(L, -2, "framerate");
			lua_pushrational(L, stream->time_base); lua_setfield(L, -2, "time_base");
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
			lua_pushnumber(L, stream->nb_frames / static_cast<double>(stream->codec->sample_rate)); lua_setfield(L, -2, "duration");
			lua_pushnumber(L, stream->codec->bits_per_coded_sample); lua_setfield(L, -2, "bits_per_sample");
			break;
	}
	return 1;
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
	lua_pop(L, 1);
	luaL_newmetatable(L, MUXER);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
	luaL_newmetatable(L, OSTREAM);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
}

inline void register_va_lib(lua_State *L){
	const luaL_Reg va[] = {
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