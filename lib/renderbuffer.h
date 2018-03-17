#ifndef ejoy2d_renderbuffer_h
#define ejoy2d_renderbuffer_h

#include <stdint.h>
#include "render.h"

#define MAX_COMMBINE 1024

struct vertex_pack {
	float vx;
	float vy;
	uint16_t tx;
	uint16_t ty;
};

struct vertex {
	struct vertex_pack vp;
	uint8_t rgba[4];
	uint8_t add[4];
};

struct quad {
	struct vertex p[4];
};

struct render_buffer {
	int object;
	int texid;
	RID vbid;
	struct quad vb[MAX_COMMBINE];
};

//got R, do no thing
void renderbuffer_initrender(struct render *R);
//����render_buffer����
void renderbuffer_init(struct render_buffer *rb);
//����buffer�����ύ��render��vertex buffer������ȥ�����this->vbid���ù���������render����һ���µ�vertex buffer
void renderbuffer_upload(struct render_buffer *rb);
//�ͷ�vbid����Ӧ��render�е�vertex buffer����
void renderbuffer_unload(struct render_buffer *rb);

//���buffer��vb������µ����ݣ��������ύ��render��ֻ�����ڸö����������
int renderbuffer_add(struct render_buffer *rb, const struct vertex_pack vb[4], uint32_t color, uint32_t additive);

//����û��ʲô�ط��ù�,��������object��ǣ�����˸�buffer������
static inline void renderbuffer_clear(struct render_buffer *rb) {
	rb->object = 0;
}

struct sprite;

/*
	// 0 : ok, 1 full, -1 error (type must be picture)
	���render_buffer����Ӷ�sprite�Ļ������ݣ����ջ����renderbuffer_add������Ҳ�ǲ����ύ���ݵ�render
*/
int renderbuffer_drawsprite(struct render_buffer *rb, struct sprite *s);

#endif
