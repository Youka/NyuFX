#include "llibs.h"
#define __STDC_CONSTANT_MACROS 1
extern "C"{
	#include <libavformat/avformat.h>
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
	std::list<VAStream*>* streams;
};
struct VAStream{
	AVStream *stream;
	AVFormatContext* format;
	bool is_deleted;
};

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
	va_format->streams = new std::list<VAStream*>;
	return 1;
DEF_TAIL

#include <wx/msgdlg.h>
DEF_HEAD_1ARG(delete_demuxer, 1)
	// Get demuxer
	VAFormat *va_format = reinterpret_cast<VAFormat*>(luaL_checkuserdata(L, 1, DEMUXER));
	// Close stream codecs
	for(std::list<VAStream*>::iterator iter = va_format->streams->begin(); iter != va_format->streams->end(); ++iter){
		if( (*iter)->stream->codec )
			avcodec_close((*iter)->stream->codec);
		(*iter)->is_deleted = true;
	}
	delete va_format->streams;
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
	lua_pushnumber(L, va_format->format->nb_programs); lua_setfield(L, -2, "programs");
	lua_pushnumber(L, va_format->format->nb_chapters); lua_setfield(L, -2, "chapters");
	return 1;
DEF_TAIL

// REGISTER
inline void register_va_meta(lua_State *L){
	// Initialize libav
	av_register_all();
	// Meta methods
	luaL_newmetatable(L, DEMUXER);
	lua_pushcfunction(L, l_delete_demuxer); lua_setfield(L, -2, "__gc");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_demuxer_info); lua_setfield(L, -2, "getinfo");
	lua_pop(L, 1);
	luaL_newmetatable(L, MUXER);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
	luaL_newmetatable(L, ISTREAM);
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