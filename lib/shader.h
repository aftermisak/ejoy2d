#ifndef EJOY_2D_SHADER_H
#define EJOY_2D_SHADER_H

#include "renderbuffer.h"
#include "render.h"

#include <stdint.h>
#include <lua.h>

#define PROGRAM_DEFAULT -1
#define PROGRAM_PICTURE 0
#define PROGRAM_RENDERBUFFER 1
#define PROGRAM_TEXT 2
#define PROGRAM_TEXT_EDGE 3
#define PROGRAM_GUI_TEXT 4
#define PROGRAM_GUI_EDGE 5

struct material;

/*
	��ʼ��ȫ��render
	��������ʼ��ȫ��render_state
	render_setblend
	render_buffer_create
	render_register_vertexlayout
	render_set
*/
void shader_init();
/*
	�����µ�program
	render_shader_create
	render_shader_bind
*/
void shader_load(int prog, const char *fs, const char *vs, int texture, const char ** texture_uniform_name);
/*
	�ͷ�������Դ
*/
void shader_unload();

//rs_commit�ᵼ����������

//�����µ�blend func���ı����ݵ���rs_commit
void shader_blend(int m1,int m2);
void shader_defaultblend();
//��texture����channel������ֵ���ı����ݵ���rs_commit
void shader_texture(int id, int channel);
//��Ӹ���vertex��Ϣ�����ҵ���rs_commit
void shader_draw(const struct vertex_pack vb[4],uint32_t color,uint32_t additive);
//��shader_draw���
void shader_drawpolygon(int n, const struct vertex_pack *vb, uint32_t color, uint32_t additive);
/*
	��ָ��program������һ���µĲ�����Ϣ�������current_program���ᵼ��rs_commit
	��current_program����Ϊָ��program�����Ҹ���uniform��Ϣ
	render_shader_bind
	apply_uniform
	material_apply
*/
void shader_program(int n, struct material *);
//-> rs_commit()
void shader_flush();
// render_clear( ...MASKC ) 
void shader_clear(unsigned long argb);
//render_version
int shader_version();
//render_enablescissor
void shader_scissortest(int enable);

int ejoy2d_shader(lua_State *L);

/*
	rs_commit do first

*/
void shader_drawbuffer(struct render_buffer * rb, float x, float y, float s);

int shader_adduniform(int prog, const char * name, enum UNIFORM_FORMAT t);

void shader_setuniform(int prog, int index, enum UNIFORM_FORMAT t, float *v);
//��ȡ����format��uniform�ڴ��С
int shader_uniformsize(enum UNIFORM_FORMAT t);

// these api may deprecated later
void shader_reset();
void shader_mask(float x, float y);
void reset_drawcall_count();
int drawcall_count();

#endif
