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
//重置render_buffer对象
void renderbuffer_init(struct render_buffer *rb);
//将该buffer数据提交到render的vertex buffer缓存中去，如果this->vbid重置过，会请求render分配一个新的vertex buffer
void renderbuffer_upload(struct render_buffer *rb);
//释放vbid所对应的render中的vertex buffer对象
void renderbuffer_unload(struct render_buffer *rb);

//向该buffer的vb中添加新的数据，还不会提交到render，只保存在该对象的数据中
int renderbuffer_add(struct render_buffer *rb, const struct vertex_pack vb[4], uint32_t color, uint32_t additive);

//好像没有什么地方用过,就是重置object标记，清空了该buffer的数据
static inline void renderbuffer_clear(struct render_buffer *rb) {
	rb->object = 0;
}

struct sprite;

/*
	// 0 : ok, 1 full, -1 error (type must be picture)
	向该render_buffer中添加对sprite的绘制数据，最终会调用renderbuffer_add，所以也是不会提交数据到render
*/
int renderbuffer_drawsprite(struct render_buffer *rb, struct sprite *s);

#endif
