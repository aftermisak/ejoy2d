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
//����render��ʼ���������õ�render��Ҫռ�ö��ٿռ�(��������Ҫ���������array���͵�buffer)
int render_size(struct render_init_args *args);
//�������ҳ�ʼ��render������������Ҫ��buffer�����һ�ȡopengl context��Ĭ��frame buffer
struct render * render_init(struct render_init_args *args, void * buffer, int sz);
void render_exit(struct render * R);

//��ָ��bufferԤ����۵�RID����Ϊ��һ������
void render_set(struct render *R, enum RENDER_OBJ what, RID id, int slot);
/*
	�ͷ�ָ��buffer��ָ����Դ����render���ݲ��opengl�㶼���ͷ�
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
	��render��ע��n��vertex bufferƫ����Ϣ������attrib:array�з���һ���µ�RID
*/
RID render_register_vertexlayout(struct render *R, int n, struct vertex_attrib * attrib);


/*
	// what should be VERTEXBUFFER or INDEXBUFFER
	����vertex buffer ����index buffer
	�����ø���data,n,stride��buffer��ֵ��
	render���opengl�㶼�ᴴ����Ӧ��Ϣ������render�㲻���buffer���ݣ�buffer����ֱ�Ӵ���opengl��
	glGenBuffers  ����
	glBindBuffer  ��
	glBufferData  ��ֵ
*/
RID render_buffer_create(struct render *R, enum RENDER_OBJ what, const void *data, int n, int stride);
/*
	����ĳ��buffer��opengl�����ݺ�render�����Ϣ
	glBindBuffer
	glBufferData
*/
void render_buffer_update(struct render *R, RID id, const void * data, int n);


/* 
	�����µ�texture���ҷ�����RID
	��������ͺͿ�ߡ�mipmap����Ϣ��������ڴ���������Ϣ���ұ���
	opengl ��:  glGenTextures, opengl��Ҳֻ�Ǵ�����
*/
RID render_texture_create(struct render *R, int width, int height, enum TEXTURE_FORMAT format, enum TEXTURE_TYPE type, int mipmap);
/*
	����ָ��texture����
	glActiveTexture
	glBindTexture
	glTexParameteri
	glPixelStorei
	glCompressedTexImage2D or glTexImage2D
*/
void render_texture_update(struct render *R, RID id, int width, int height, const void *pixels, int slice, int miplevel);

/*
	// subupdate only support slice 0, miplevel 0
	����ָ����Χ( x, y, w, h )texture����
	glActiveTexture
	glBindTexture
	glPixelStorei
	glCompressedTexSubImage2D or glTexSubImage2D
*/
void render_texture_subupdate(struct render *R, RID id, const void *pixels, int x, int y, int w, int h);

/*
	�����µ�frame buffer���Ұ��´�����texture����frame buffer�����������render target��RID
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
	��ȡrender target(rt)���󶨵�texture��RID
*/

RID render_target_texture(struct render *R, RID rt);

/*
	����shader��
	����������ɫ��
	�����������õ�attrib_layout��Ϣ�� ͨ�� glBindAttribLocation ����Ӧ��������vertex buffer����Ӧλ��
	��ȡ��uniform��Ϣ���ұ���
	����һ����shaderʹ�õ�vertex array object
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
	������Ӧ��gl program 
	����������ʹ�õ� texture uniform
	glUseProgram
	glUniform1i
*/
void render_shader_bind(struct render *R, RID id);
/*
	��ȡ��ǰ�����gl program ����Ϊ�������ֵ�uniform��opengl�е�location
	glGetUniformLocation
*/
int render_shader_locuniform(struct render *R, const char * name);
/*
	���µ�ǰ�����gl program�и���location��uniformֵ
	glUniform[n][t] ȡ������format����
	like glUniform1f UNIFORM_FLOAT4 UNIFORM_FLOAT33 ...��
*/
void render_shader_setuniform(struct render *R, int loc, enum UNIFORM_FORMAT format, const float *v);

//����viewport�� ���Ǹ��� glViewport �ļ򵥷�װ
void render_setviewport(struct render *R, int x, int y, int width, int height );
//������Ⱦ�����������Ǹ��� glScissor �ļ򵥷�װ
void render_setscissor(struct render *R, int x, int y, int width, int height );



void render_setblend(struct render *R, enum BLEND_FORMAT src, enum BLEND_FORMAT dst);
void render_setdepth(struct render *R, enum DEPTH_FORMAT d);
void render_setcull(struct render *R, enum CULL_MODE c);
void render_enabledepthmask(struct render *R, int enable);
void render_enablescissor(struct render *R, int enable);

//reset render state disable blend, scissor test, depth test, face cull. �������û�Ĭ�ϵ�frame buffer
void render_state_reset(struct render *R);

/*
	�ύ��ǰ������Ⱦ״̬Ȼ��glClear
	render_state_commit
	glClear 
*/
void render_clear(struct render *R, enum CLEAR_MASK mask, unsigned long argb);
/*
	�ύ��ǰ������Ⱦ״̬������Ⱦ
	render_state_commit
	glDrawElements
*/
void render_draw(struct render *R, enum DRAW_MODE mode, int fromidx, int ni);

#endif
