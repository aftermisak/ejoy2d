#ifndef ejoy3d_render_h
#define ejoy3d_render_h

#include <stdint.h>

typedef unsigned int RID;

struct render;

struct render_init_args {
	int max_buffer;
	int max_layout;
	int max_target;
	int max_texture;
	int max_shader;
};

struct vertex_attrib {
	const char * name;//?
	int vbslot;
	int n;//?
	int size;
	int offset;
};

struct shader_init_args {
	const char * vs;
	const char * fs;
	int texture;
	const char **texture_uniform;//name arrays
};

enum RENDER_OBJ {
	INVALID = 0,
	VERTEXLAYOUT = 1,
	VERTEXBUFFER = 2,
	INDEXBUFFER = 3,
	TEXTURE = 4,
	TARGET = 5,
	SHADER = 6,
};

enum TEXTURE_TYPE {
	TEXTURE_2D = 0,
	TEXTURE_CUBE,
};

enum TEXTURE_FORMAT {
	TEXTURE_INVALID = 0,
	TEXTURE_RGBA8,
	TEXTURE_RGBA4,
	TEXTURE_RGB,
	TEXTURE_RGB565,
	TEXTURE_A8,
	TEXTURE_DEPTH,	// use for render target
	TEXTURE_PVR2,
	TEXTURE_PVR4,
	TEXTURE_ETC1,
};

enum BLEND_FORMAT {
	BLEND_DISABLE = 0,
	BLEND_ZERO,
	BLEND_ONE,
	BLEND_SRC_COLOR,
	BLEND_ONE_MINUS_SRC_COLOR,
	BLEND_SRC_ALPHA,
	BLEND_ONE_MINUS_SRC_ALPHA,
	BLEND_DST_ALPHA,
	BLEND_ONE_MINUS_DST_ALPHA,
	BLEND_DST_COLOR,
	BLEND_ONE_MINUS_DST_COLOR,
	BLEND_SRC_ALPHA_SATURATE,
};

enum DEPTH_FORMAT {
	DEPTH_DISABLE = 0,
	DEPTH_LESS_EQUAL,
	DEPTH_LESS,
	DEPTH_EQUAL,
	DEPTH_GREATER,
	DEPTH_GREATER_EQUAL,
	DEPTH_ALWAYS,
};

enum CLEAR_MASK {
	MASKC = 0x1,
	MASKD = 0x2,
	MASKS = 0x4,
};

enum UNIFORM_FORMAT {
	UNIFORM_INVALID = 0,
	UNIFORM_FLOAT1,
	UNIFORM_FLOAT2,
	UNIFORM_FLOAT3,
	UNIFORM_FLOAT4,
	UNIFORM_FLOAT33,
	UNIFORM_FLOAT44,
};

enum DRAW_MODE {
	DRAW_TRIANGLE = 0,
	DRAW_LINE,
};

enum CULL_MODE {
	CULL_DISABLE = 0,
	CULL_FRONT,
	CULL_BACK,
};
//get version of opengl es
int render_version(struct render *R);
//根据render初始化参数，得到render需要占用多少空间(其中最主要的是那五个array类型的buffer)
int render_size(struct render_init_args *args);
//创建并且初始化render，创建所有需要的buffer，并且获取opengl context的默认frame buffer
struct render * render_init(struct render_init_args *args, void * buffer, int sz);
void render_exit(struct render * R);

//将指定buffer预留插槽的RID更新为另一个引用
void render_set(struct render *R, enum RENDER_OBJ what, RID id, int slot);
/*
	释放指定buffer的指定资源，从render数据层和opengl层都会释放
	vertex buffer or index buffer
		glDeleteBuffers
	shader:
		glDeleteProgram
		glDeleteVertexArrays
	texture:
		glDeleteTextures
	target:
		glDeleteFramebuffers
*/
void render_release(struct render *R, enum RENDER_OBJ what, RID id);

/*
	n <= MAX_ATTRIB
	向render中注册n个vertex buffer偏移信息，返回attrib:array中分配一个新的RID
*/
RID render_register_vertexlayout(struct render *R, int n, struct vertex_attrib * attrib);


/*
	// what should be VERTEXBUFFER or INDEXBUFFER
	创建vertex buffer 或者index buffer
	并且用给定data,n,stride给buffer赋值，
	render层和opengl层都会创建相应信息，其中render层不会存buffer数据，buffer数据直接传给opengl层
	glGenBuffers  创建
	glBindBuffer  绑定
	glBufferData  赋值
*/
RID render_buffer_create(struct render *R, enum RENDER_OBJ what, const void *data, int n, int stride);
/*
	更新某个buffer的opengl层数据和render层的信息
	glBindBuffer
	glBufferData
*/
void render_buffer_update(struct render *R, RID id, const void * data, int n);


/* 
	创建新的texture并且返回其RID
	会根据类型和宽高、mipmap等信息，计算出内存三公埌信息并且保存
	opengl 层:  glGenTextures, opengl层也只是创建出
*/
RID render_texture_create(struct render *R, int width, int height, enum TEXTURE_FORMAT format, enum TEXTURE_TYPE type, int mipmap);
/*
	更新指定texture数据
	glActiveTexture
	glBindTexture
	glTexParameteri
	glPixelStorei
	glCompressedTexImage2D or glTexImage2D
*/
void render_texture_update(struct render *R, RID id, int width, int height, const void *pixels, int slice, int miplevel);

/*
	// subupdate only support slice 0, miplevel 0
	更新指定范围( x, y, w, h )texture数据
	glActiveTexture
	glBindTexture
	glPixelStorei
	glCompressedTexSubImage2D or glTexSubImage2D
*/
void render_texture_subupdate(struct render *R, RID id, const void *pixels, int x, int y, int w, int h);

/*
	创建新的frame buffer并且绑定新创建的texture用以frame buffer的输出，返回render target的RID
	render_texture_create
	render_texture_update
	glGenFramebuffers
	glBindFramebuffer
	glFramebufferTexture2D
	glCheckFramebufferStatus
		if error happen -> glDeleteFramebuffers
	glBindFramebuffer
*/
RID render_target_create(struct render *R, int width, int height, enum TEXTURE_FORMAT format);
/*
	// render_release TARGET would not release the texture attachment
	获取render target(rt)所绑定的texture的RID
*/

RID render_target_texture(struct render *R, RID rt);

/*
	创建shader，
	编译链接着色器
	更具事先设置的attrib_layout信息， 通过 glBindAttribLocation 绑定相应属性名到vertex buffer的相应位置
	获取其uniform信息并且保存
	创建一个该shader使用的vertex array object
	glCreateProgram
		vs and fs:
		glCreateShader
		glCompileShader
		if error happen glDeleteShader
	glAttachShader
	glBindAttribLocation
	if error happen glDeleteProgram
*/
RID render_shader_create(struct render *R, struct shader_init_args *args);
/*
	激活相应的gl program 
	并且设置其使用的 texture uniform
	glUseProgram
	glUniform1i
*/
void render_shader_bind(struct render *R, RID id);
/*
	获取当前激活的gl program 名称为给定名字的uniform在opengl中的location
	glGetUniformLocation
*/
int render_shader_locuniform(struct render *R, const char * name);
/*
	更新当前激活的gl program中给定location的uniform值
	glUniform[n][t] 取决于其format类型
	like glUniform1f UNIFORM_FLOAT4 UNIFORM_FLOAT33 ...等
*/
void render_shader_setuniform(struct render *R, int loc, enum UNIFORM_FORMAT format, const float *v);

//设置viewport， 这是个对 glViewport 的简单封装
void render_setviewport(struct render *R, int x, int y, int width, int height );
//设置渲染剪切区域，这是个对 glScissor 的简单封装
void render_setscissor(struct render *R, int x, int y, int width, int height );



void render_setblend(struct render *R, enum BLEND_FORMAT src, enum BLEND_FORMAT dst);
void render_setdepth(struct render *R, enum DEPTH_FORMAT d);
void render_setcull(struct render *R, enum CULL_MODE c);
void render_enabledepthmask(struct render *R, int enable);
void render_enablescissor(struct render *R, int enable);

//reset render state disable blend, scissor test, depth test, face cull. 并且设置回默认的frame buffer
void render_state_reset(struct render *R);

/*
	提交当前各种渲染状态然后glClear
	render_state_commit
	glClear 
*/
void render_clear(struct render *R, enum CLEAR_MASK mask, unsigned long argb);
/*
	提交当前各种渲染状态并且渲染
	render_state_commit
	glDrawElements
*/
void render_draw(struct render *R, enum DRAW_MODE mode, int fromidx, int ni);

#endif
