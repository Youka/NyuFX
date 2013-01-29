#include "llibs.h"
#define __STDC_CONSTANT_MACROS 1
extern "C"{
	#include <inttypes.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
}

// REGISTER
void luaopen_va(lua_State *L){
	av_register_all();
}