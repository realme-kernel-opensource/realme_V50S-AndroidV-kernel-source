// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2016 MediaTek Inc.
 * Author: PC Chen <pc.chen@mediatek.com>
 */

#include <linux/module.h>
#include <linux/slab.h>

#include "mtk_vcodec_intr.h"
#include "vdec_vcu_if.h"
#include "vdec_drv_base.h"
#include "mtk_vcodec_dec.h"
#include "mtk_vcodec_drv.h"
#include "vdec_drv_if.h"


static void put_fb_to_free(struct vdec_inst *inst, struct vdec_fb *fb)
{
	struct ring_fb_list *list;

	if (fb != NULL) {
		list = &inst->vsi->list_free;
		if (list->count == DEC_MAX_FB_NUM) {
			mtk_vcodec_err(inst, "[FB] put fb free_list full");
			return;
		}

		mtk_vcodec_debug(inst,
						 "[FB] put fb into free_list @(%p, %llx)",
						 fb->fb_base[0].va,
						 (u64)fb->fb_base[1].dma_addr);

		list->fb_list[list->write_idx].vdec_fb_va = (u64)(fb->index + 1);
		list->write_idx = (list->write_idx == DEC_MAX_FB_NUM - 1U) ?
						  0U : list->write_idx + 1U;
		list->count++;
	}
}

static void get_pic_info(struct vdec_inst *inst,
						 struct vdec_pic_info *pic)
{
	if (inst == NULL)
		return;
	if (inst->vsi == NULL)
		return;

	memcpy(pic, &inst->vsi->pic, sizeof(struct vdec_pic_info));

	mtk_vcodec_debug(inst, "pic(%d, %d), buf(%d, %d), bitdepth = %d, fourcc = %d\n",
		pic->pic_w, pic->pic_h, pic->buf_w, pic->buf_h,
		pic->bitdepth, pic->fourcc);
	mtk_vcodec_debug(inst, "Y/C(%d, %d)", pic->fb_sz[0], pic->fb_sz[1]);
}

static void get_crop_info(struct vdec_inst *inst, struct v4l2_rect *r)
{
	if (inst == NULL)
		return;
	if (inst->vsi == NULL)
		return;

	r->left      = inst->vsi->crop.left;
	r->top       = inst->vsi->crop.top;
	r->width     = inst->vsi->crop.width;
	r->height    = inst->vsi->crop.height;
	mtk_vcodec_debug(inst, "l=%d, t=%d, w=%d, h=%d",
		r->left, r->top, r->width, r->height);
}

static void get_dpb_size(struct vdec_inst *inst, unsigned int *dpb_sz)
{
	if (inst == NULL)
		return;
	if (inst->vsi == NULL)
		return;

	*dpb_sz = inst->vsi->dec.dpb_sz + 1U;
	mtk_vcodec_debug(inst, "sz=%d", *dpb_sz);
}

static int vdec_init(struct mtk_vcodec_ctx *ctx, unsigned long *h_vdec)
{
	struct vdec_inst *inst = NULL;
	int err = 0;
	struct vcu_v4l2_callback_func cb;

	inst = kzalloc(sizeof(*inst), GFP_KERNEL);
	if (!inst)
		return -ENOMEM;
	if (!ctx) {
		err = -ENOMEM;
		goto error_free_inst;
	}

	inst->ctx = ctx;
	inst->vcu.id = IPI_VDEC_COMMON;
	inst->vcu.dev = VCU_FPTR(vcu_get_plat_device)(ctx->dev->plat_dev);
	if (inst->vcu.dev  == NULL) {
		mtk_vcodec_err(inst, "vcu device is not ready");
		goto error_free_inst;
	}

	inst->vcu.ctx = ctx;
	inst->vcu.handler = vcu_dec_ipi_handler;
	*h_vdec = (unsigned long)inst;

	mtk_vcodec_add_ctx_list(ctx);

	err = vcu_dec_init(&inst->vcu);
	if (err != 0) {
		mtk_vcodec_err(inst, "%s err=%d", __func__, err);
		goto error_free_inst;
	}

	memset(&cb, 0, sizeof(struct vcu_v4l2_callback_func));
	cb.gce_timeout_dump = mtk_vcodec_gce_timeout_dump;
	cb.vdec_realease_lock = vdec_check_release_lock;
	VCU_FPTR(vcu_set_v4l2_callback)(inst->vcu.dev, &cb);

	inst->vsi = (struct vdec_vsi *)inst->vcu.vsi;
	ctx->input_driven = inst->vsi->input_driven;
	ctx->ipi_blocked = &inst->vsi->ipi_blocked;
	*(ctx->ipi_blocked) = 0;
	ctx->align_mode = inst->vsi->align_mode;
	ctx->wait_align = &inst->vsi->wait_align;
	*(ctx->wait_align) = 0;
	atomic_set(&ctx->align_type, 0);
	ctx->src_cnt = &inst->vsi->src_cnt;
	ctx->dst_cnt = &inst->vsi->dst_cnt;

	mtk_vcodec_debug(inst, "Decoder Instance >> %p", inst);

	return 0;

error_free_inst:
	if (ctx)
		mtk_vcodec_del_ctx_list(ctx);
	kfree(inst);
	*h_vdec = (unsigned long)NULL;

	return err;
}

static void vdec_deinit(unsigned long h_vdec)
{
	struct vdec_inst *inst = (struct vdec_inst *)h_vdec;

	mtk_vcodec_debug_enter(inst);

	vcu_dec_deinit(&inst->vcu);

	mtk_vcodec_del_ctx_list(inst->ctx);

	kfree(inst);
}

static int vdec_decode(unsigned long h_vdec, struct mtk_vcodec_mem *bs,
	struct vdec_fb *fb, unsigned int *src_chg)
{
	struct vdec_inst *inst = (struct vdec_inst *)h_vdec;
	struct vdec_vcu_inst *vcu = &inst->vcu;
	int ret = 0;
	unsigned int data[6];
	uint64_t fb_dma[VIDEO_MAX_PLANES] = { 0 };
	uint32_t num_planes;
	unsigned int i = 0;
	unsigned int bs_fourcc = inst->ctx->q_data[MTK_Q_DATA_SRC].fmt->fourcc;
	unsigned int fm_fourcc = inst->ctx->q_data[MTK_Q_DATA_DST].fmt->fourcc;
	unsigned int *errormap_info = &inst->ctx->errormap_info[0];

	num_planes = fb ? inst->vsi->dec.fb_num_planes : 0U;

	for (i = 0; i < num_planes; i++)
		fb_dma[i] = (u64)fb->fb_base[i].dma_addr;

	mtk_vcodec_debug(inst, "+ [%d] FB y_dma=%llx c_dma=%llx va=%p num_planes %d",
		inst->num_nalu, fb_dma[0], fb_dma[1], fb, num_planes);

	/* bs == NULL means reset decoder */
	if (bs == NULL) {
		if (fb == NULL)
			return vcu_dec_reset(vcu, VDEC_FLUSH); // flush (0)
		else if (fb->status == 0)
			return vcu_dec_reset(vcu, VDEC_DRAIN); // drain (1)
		else
			return vcu_dec_reset(vcu, VDEC_DRAIN_EOS); // drain & return EOS frame (2)
	}

	mtk_vcodec_debug(inst, "+ BS dma=0x%llx dmabuf=%p format=%c%c%c%c",
		(uint64_t)bs->dma_addr, bs->dmabuf, bs_fourcc & 0xFF,
		(bs_fourcc >> 8) & 0xFF, (bs_fourcc >> 16) & 0xFF,
		(bs_fourcc >> 24) & 0xFF);

	inst->vsi->dec.vdec_bs_va = (u64)(bs->index + 1);
	inst->vsi->dec.bs_dma = (uint64_t)bs->dma_addr;

	for (i = 0; i < num_planes; i++)
		inst->vsi->dec.fb_dma[i] = fb_dma[i];

	if (inst->vsi->input_driven == NON_INPUT_DRIVEN) {
		inst->vsi->dec.vdec_fb_va = (u64)0;
		inst->vsi->dec.index = 0xFF;
	}
	if (fb != NULL) {
		inst->vsi->dec.vdec_fb_va = (u64)(fb->index + 1);
		inst->vsi->dec.index = fb->index;
		if (fb->dma_general_buf != 0) {
			inst->vsi->general_buf_fd = fb->general_buf_fd;
			inst->vsi->general_buf_size = fb->dma_general_buf->size;
			inst->vsi->general_buf_dma = fb->dma_general_addr;
			mtk_vcodec_debug(inst, "dma_general_buf dma_buf=%p fd=%d dma=%llx size=%lu",
			    fb->dma_general_buf, inst->vsi->general_buf_fd,
			    inst->vsi->general_buf_dma,
			    fb->dma_general_buf->size);
		} else {
			fb->general_buf_fd = -1;
			inst->vsi->general_buf_fd = -1;
			inst->vsi->general_buf_size = 0;
			mtk_vcodec_debug(inst, "no general buf dmabuf");
		}

		if (fb->dma_meta_buf != 0) {
			inst->vsi->meta_buf_fd = fb->meta_buf_fd;
			inst->vsi->meta_buf_size = fb->dma_meta_buf->size;
			inst->vsi->meta_buf_dma = fb->dma_meta_addr;
			mtk_vcodec_debug(inst, "meta_buf_dma dma_buf=%p fd=%d dma=%llx size=%lu",
			    fb->dma_meta_buf, inst->vsi->meta_buf_fd,
			    inst->vsi->meta_buf_dma,
			    fb->dma_meta_buf->size);
		} else {
			fb->meta_buf_fd = -1;
			inst->vsi->meta_buf_fd = -1;
			inst->vsi->meta_buf_size = 0;
			mtk_vcodec_debug(inst, "no meta buf dmabuf");
		}
	}

	inst->vsi->dec.queued_frame_buf_count =
		inst->ctx->dec_params.queued_frame_buf_count;
	inst->vsi->dec.timestamp =
		inst->ctx->dec_params.timestamp;
	memcpy(&inst->vsi->hdr10plus_buf, bs->hdr10plus_buf, sizeof(struct hdr10plus_info));

	mtk_vcodec_debug(inst, "+ FB y_fd=%llx c_fd=%llx BS fd=%llx format=%c%c%c%c",
		inst->vsi->dec.fb_fd[0], inst->vsi->dec.fb_fd[1],
		inst->vsi->dec.bs_fd, fm_fourcc & 0xFF,
		(fm_fourcc >> 8) & 0xFF, (fm_fourcc >> 16) & 0xFF,
		(fm_fourcc >> 24) & 0xFF);

	data[0] = (unsigned int)bs->size;
	data[1] = (unsigned int)bs->length;
	data[2] = (unsigned int)bs->flags;
	data[3] =
		inst->ctx->dec_params.fixed_max_frame_size_width;
	data[4] =
		inst->ctx->dec_params.fixed_max_frame_size_height;
	data[5] =
		inst->ctx->dec_params.fixed_max_frame_buffer_mode;
	ret = vcu_dec_start(vcu, data, 6, bs, fb);

	*src_chg = inst->vsi->dec.vdec_changed_info;
	*(errormap_info + bs->index % VB2_MAX_FRAME) =
		inst->vsi->dec.error_map;

	if ((*src_chg & VDEC_NEED_SEQ_HEADER) != 0U)
		mtk_vcodec_err(inst, "- need first seq header -");
	else if ((*src_chg & VDEC_RES_CHANGE) != 0U)
		mtk_vcodec_debug(inst, "- resolution changed -");
	else if ((*src_chg & VDEC_HW_NOT_SUPPORT) != 0U)
		mtk_vcodec_err(inst, "- unsupported -");
	/*ack timeout means vpud has crashed*/
	if (ret == -EIO) {
		mtk_vcodec_err(inst, "- IPI msg ack timeout  -");
		*src_chg = *src_chg | VDEC_HW_NOT_SUPPORT;
	}

	if (ret < 0 || ((*src_chg & VDEC_HW_NOT_SUPPORT) != 0U)
		|| ((*src_chg & VDEC_NEED_SEQ_HEADER) != 0U))
		goto err_free_fb_out;

	inst->ctx->input_driven = inst->vsi->input_driven;
	inst->num_nalu++;
	return ret;

err_free_fb_out:
	put_fb_to_free(inst, fb);
	mtk_vcodec_debug(inst, "\n - NALU[%d] err=%d -\n", inst->num_nalu, ret);
	return ret;
}

static void vdec_get_bs(struct vdec_inst *inst,
						struct ring_bs_list *list,
						struct mtk_vcodec_mem **out_bs)
{
	u64 bs_index;
	unsigned long vdec_bs_va;
	struct mtk_vcodec_mem *bs;

get_bs:
	if (list->count == 0) {
		mtk_vcodec_debug(inst, "[BS] there is no bs");
		*out_bs = NULL;
		return;
	}

	bs_index = list->vdec_bs_va_list[list->read_idx];
	if (bs_index == 0 || bs_index > VB2_MAX_FRAME) {
		mtk_vcodec_err(inst, "free bs list read_idx %d bs_index %lld invalid !",
			list->read_idx, bs_index);
		list->read_idx = (list->read_idx == DEC_MAX_BS_NUM - 1U) ? 0U : list->read_idx + 1U;
		list->count--;
		if (list->count > 0)
			goto get_bs;
		else {
			*out_bs = NULL;
			return;
		}
	}
	vdec_bs_va = (unsigned long)inst->ctx->bs_list[bs_index];
	bs = (struct mtk_vcodec_mem *)vdec_bs_va;

	*out_bs = bs;
	mtk_vcodec_debug(inst, "[BS] get free bs %lld %lx", bs_index, vdec_bs_va);

	list->read_idx = (list->read_idx == DEC_MAX_BS_NUM - 1) ? 0 : list->read_idx + 1;
	list->count--;
}

static void vdec_get_fb(struct vdec_inst *inst,
	struct ring_fb_list *list,
	bool disp_list, struct vdec_fb **out_fb)
{
	u64 fb_index;
	unsigned long vdec_fb_va;
	struct vdec_fb *fb;

get_fb:
	if (list->count >= DEC_MAX_FB_NUM) {
		mtk_vcodec_err(inst, "list count %d invalid ! (write_idx %d, read_idx %d)",
			list->count, list->write_idx, list->read_idx);
		if (list->write_idx >= DEC_MAX_FB_NUM || list->read_idx >= DEC_MAX_FB_NUM)
			list->write_idx = list->read_idx = 0;
		if (list->write_idx >= list->read_idx)
			list->count = list->write_idx - list->read_idx;
		else
			list->count = list->write_idx + DEC_MAX_FB_NUM - list->read_idx;
	}
	if (list->count == 0) {
		mtk_vcodec_debug(inst, "[FB] there is no %s fb", disp_list ? "disp" : "free");
		*out_fb = NULL;
		return;
	}

	fb_index = (u64)list->fb_list[list->read_idx].vdec_fb_va;
	if (fb_index == 0 || fb_index > VB2_MAX_FRAME) {
		mtk_vcodec_err(inst, "%s fb list read_idx %d fb_index %lld invalid !",
			disp_list ? "disp" : "free", list->read_idx, fb_index);
		list->read_idx = (list->read_idx == DEC_MAX_FB_NUM - 1U) ? 0U : list->read_idx + 1U;
		list->count--;
		if (list->count > 0)
			goto get_fb;
		else {
			*out_fb = NULL;
			return;
		}
	}
	vdec_fb_va = (unsigned long)inst->ctx->fb_list[fb_index];
	fb = (struct vdec_fb *)vdec_fb_va;
	if (fb == NULL)
		return;
	fb->timestamp = list->fb_list[list->read_idx].timestamp;
#ifdef TV_INTEGRATION
	fb->field = list->fb_list[list->read_idx].field;
	fb->frame_type = list->fb_list[list->read_idx].frame_type;
#endif
	if (disp_list) {
		fb->status |= FB_ST_DISPLAY;
		if (list->fb_list[list->read_idx].reserved)
			fb->status |= FB_ST_NO_GENERATED;
	} else {
		fb->status |= FB_ST_FREE;
		if (list->fb_list[list->read_idx].reserved)
			fb->status |= FB_ST_EOS;
	}

	*out_fb = fb;
	mtk_vcodec_debug(inst, "[FB] get %s fb st=%x id=%d ts=%llu %lld %lx gbuf fd %d dma %p",
		disp_list ? "disp" : "free", fb->status, list->read_idx,
		list->fb_list[list->read_idx].timestamp,
		list->fb_list[list->read_idx].vdec_fb_va, vdec_fb_va,
		fb->general_buf_fd, fb->dma_general_buf);

	list->read_idx = (list->read_idx == DEC_MAX_FB_NUM - 1U) ? 0U : list->read_idx + 1U;
	list->count--;
}

static void get_supported_format(struct vdec_inst *inst,
	struct mtk_video_fmt *video_fmt)
{
	unsigned int i = 0;

	inst->vcu.ctx = inst->ctx;
	vcu_dec_query_cap(&inst->vcu, GET_PARAM_VDEC_CAP_SUPPORTED_FORMATS,
					  video_fmt);
	for (i = 0; i < MTK_MAX_DEC_CODECS_SUPPORT; i++) {
		if (video_fmt[i].fourcc != 0) {
			mtk_vcodec_debug(inst, "video_formats[%d] fourcc %d type %d num_planes %d\n",
				i, video_fmt[i].fourcc, video_fmt[i].type,
				video_fmt[i].num_planes);
		}
	}
}

#ifdef TV_INTEGRATION
static void get_frame_intervals(struct vdec_inst *inst,
	struct mtk_video_frame_frameintervals *f_ints)
{
	inst->vcu.ctx = inst->ctx;
	vcu_dec_query_cap(&inst->vcu, GET_PARAM_CAPABILITY_FRAMEINTERVALS,
					  f_ints);

	mtk_vcodec_debug(inst, "codec fourcc %d w %d h %d max %d/%d min %d/%d step %d/%d\n",
			 f_ints->fourcc, f_ints->width, f_ints->height,
			 f_ints->stepwise.max.numerator, f_ints->stepwise.max.denominator,
			 f_ints->stepwise.min.numerator, f_ints->stepwise.min.denominator,
			 f_ints->stepwise.step.numerator, f_ints->stepwise.step.denominator);
}
#endif

static void get_frame_sizes(struct vdec_inst *inst,
	struct mtk_codec_framesizes *codec_framesizes)
{
	unsigned int i = 0;

	inst->vcu.ctx = inst->ctx;
	vcu_dec_query_cap(&inst->vcu, GET_PARAM_VDEC_CAP_FRAME_SIZES,
					  codec_framesizes);
	for (i = 0; i < MTK_MAX_DEC_CODECS_SUPPORT; i++) {
		if (codec_framesizes[i].fourcc != 0) {
			mtk_vcodec_debug(inst,
				"codec_fs[%d] fourcc %d s %d %d %d %d %d %d P %d L %d\n",
				i, codec_framesizes[i].fourcc,
				codec_framesizes[i].stepwise.min_width,
				codec_framesizes[i].stepwise.max_width,
				codec_framesizes[i].stepwise.step_width,
				codec_framesizes[i].stepwise.min_height,
				codec_framesizes[i].stepwise.max_height,
				codec_framesizes[i].stepwise.step_height,
				codec_framesizes[i].profile,
				codec_framesizes[i].level);
		}
	}

}

static void get_color_desc(struct vdec_inst *inst,
	struct mtk_color_desc *color_desc)
{
	inst->vcu.ctx = inst->ctx;
	memcpy(color_desc, &inst->vsi->color_desc, sizeof(*color_desc));
}

static void get_aspect_ratio(struct vdec_inst *inst, unsigned int *aspect_ratio)
{
	if (inst->vsi == NULL)
		return;

	inst->vcu.ctx = inst->ctx;
	*aspect_ratio = inst->vsi->aspect_ratio;
}

static void get_supported_fix_buffers(struct vdec_inst *inst,
					unsigned int *supported)
{
	inst->vcu.ctx = inst->ctx;
	if (inst->vsi != NULL)
		*supported = inst->vsi->fix_buffers;
}

static void get_supported_fix_buffers_svp(struct vdec_inst *inst,
					unsigned int *supported)
{
	inst->vcu.ctx = inst->ctx;
	if (inst->vsi != NULL)
		*supported = inst->vsi->fix_buffers_svp;
}

static void get_interlacing(struct vdec_inst *inst,
			    unsigned int *interlacing)
{
	inst->vcu.ctx = inst->ctx;
	if (inst->vsi != NULL)
		*interlacing = inst->vsi->interlacing;
}

static void get_interlacing_fieldseq(struct vdec_inst *inst,
			    unsigned int *bottomFirst)
{
	inst->vcu.ctx = inst->ctx;
	if (inst->vsi != NULL)
		*bottomFirst = inst->vsi->interlacing_fieldseq;
}

static void get_codec_type(struct vdec_inst *inst,
			   unsigned int *codec_type)
{
	inst->vcu.ctx = inst->ctx;
	if (inst->vsi != NULL)
		*codec_type = inst->vsi->codec_type;
}

static void get_input_driven(struct vdec_inst *inst,
			   unsigned int *input_driven)
{
	inst->vcu.ctx = inst->ctx;
	if (inst->vsi != NULL)
		*input_driven = inst->vsi->input_driven;
}

static void get_align_mode(struct vdec_inst *inst,
			   unsigned int *align_mode)
{
	inst->vcu.ctx = inst->ctx;
	if (inst->vsi != NULL)
		*align_mode = inst->vsi->align_mode;
}

#ifdef TV_INTEGRATION
static void get_frame_interval(struct vdec_inst *inst,
			       struct v4l2_fract *time_per_frame)
{
	inst->vcu.ctx = inst->ctx;
	if (inst->vsi != NULL)
		memcpy(time_per_frame, &inst->vsi->time_per_frame, sizeof(struct v4l2_fract));
}
#endif

static int vdec_get_param(unsigned long h_vdec,
	enum vdec_get_param_type type, void *out)
{
	struct vdec_inst *inst = (struct vdec_inst *)h_vdec;
	int ret = 0;

	if (inst == NULL)
		return -EINVAL;

	switch (type) {
	case GET_PARAM_FREE_BITSTREAM_BUFFER:
		if (inst->vsi == NULL)
			return -EINVAL;
		vdec_get_bs(inst, &inst->vsi->list_free_bs, out);
		break;

	case GET_PARAM_DISP_FRAME_BUFFER:
	{
		struct vdec_fb *pfb;

		if (inst->vsi == NULL)
			return -EINVAL;
		vdec_get_fb(inst, &inst->vsi->list_disp, true, out);

		pfb = *((struct vdec_fb **)out);
		break;
	}

	case GET_PARAM_FREE_FRAME_BUFFER:
	{
		struct vdec_fb *pfb;

		if (inst->vsi == NULL)
			return -EINVAL;
		vdec_get_fb(inst, &inst->vsi->list_free, false, out);

		pfb = *((struct vdec_fb **)out);
		break;
	}

	case GET_PARAM_PIC_INFO:
		get_pic_info(inst, out);
		break;

	case GET_PARAM_DPB_SIZE:
		get_dpb_size(inst, out);
		break;

	case GET_PARAM_CROP_INFO:
		get_crop_info(inst, out);
		break;

	case GET_PARAM_VDEC_CAP_SUPPORTED_FORMATS:
		get_supported_format(inst, out);
		break;

	case GET_PARAM_VDEC_CAP_FRAME_SIZES:
		get_frame_sizes(inst, out);
		break;

	case GET_PARAM_COLOR_DESC:
		if (inst->vsi == NULL)
			return -EINVAL;
		get_color_desc(inst, out);
		break;

	case GET_PARAM_ASPECT_RATIO:
		get_aspect_ratio(inst, out);
		break;

	case GET_PARAM_PLATFORM_SUPPORTED_FIX_BUFFERS:
		get_supported_fix_buffers(inst, out);
		break;

	case GET_PARAM_PLATFORM_SUPPORTED_FIX_BUFFERS_SVP:
		get_supported_fix_buffers_svp(inst, out);
		break;

	case GET_PARAM_INTERLACING:
		get_interlacing(inst, out);
		break;

	case GET_PARAM_CODEC_TYPE:
		get_codec_type(inst, out);
		break;

	case GET_PARAM_INPUT_DRIVEN:
		get_input_driven(inst, out);
		break;

	case GET_PARAM_ALIGN_MODE:
		get_align_mode(inst, out);
		break;

	case GET_PARAM_INTERLACING_FIELD_SEQ:
		get_interlacing_fieldseq(inst, out);
		break;
#ifdef TV_INTEGRATION
	case GET_PARAM_FRAME_INTERVAL:
		get_frame_interval(inst, out);
		break;
	case GET_PARAM_CAPABILITY_FRAMEINTERVALS:
		get_frame_intervals(inst, out);
		break;
#endif

	case GET_PARAM_VDEC_VCU_VPUD_LOG:
		VCU_FPTR(vcu_get_log)(out, LOG_PROPERTY_SIZE);
		break;

	default:
		mtk_vcodec_err(inst, "invalid get parameter type=%d", type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int vdec_set_param(unsigned long h_vdec,
	enum vdec_set_param_type type, void *in)
{
	struct vdec_inst *inst = (struct vdec_inst *)h_vdec;
	uint64_t size;
	int ret = 0;

	if (inst == NULL)
		return -EINVAL;

	switch (type) {
	case SET_PARAM_FRAME_BUFFER:
		vcu_dec_set_frame_buffer(&inst->vcu, in);
		break;
	case SET_PARAM_FRAME_SIZE:
		vcu_dec_set_param(&inst->vcu, (unsigned int)type, in, 2U);
		break;
	case SET_PARAM_SET_FIXED_MAX_OUTPUT_BUFFER:
		vcu_dec_set_param(&inst->vcu, (unsigned int)type, in, 3U);
		break;
	case SET_PARAM_DECODE_MODE:
	case SET_PARAM_NAL_SIZE_LENGTH:
	case SET_PARAM_WAIT_KEY_FRAME:
	case SET_PARAM_DECODE_ERROR_HANDLE_MODE:
	case SET_PARAM_OPERATING_RATE:
	case SET_PARAM_TOTAL_FRAME_BUFQ_COUNT:
	case SET_PARAM_TOTAL_BITSTREAM_BUFQ_COUNT:
	case SET_PARAM_SET_DV:
	case SET_PARAM_NO_REORDER:
		vcu_dec_set_param(&inst->vcu, (unsigned int)type, in, 1U);
		break;
	case SET_PARAM_PUT_FB:
		vcu_dec_set_param(&inst->vcu, (unsigned int)type, in, 0);
	case SET_PARAM_UFO_MODE:
		break;
	case SET_PARAM_CRC_PATH:
		if (inst->vsi == NULL)
			return -EINVAL;
		size = strlen((char *) *(uintptr_t *)in);
		memcpy(inst->vsi->crc_path, (void *) *(uintptr_t *)in, size);
		break;
	case SET_PARAM_GOLDEN_PATH:
		if (inst->vsi == NULL)
			return -EINVAL;
		size = strlen((char *) *(uintptr_t *)in);
		memcpy(inst->vsi->golden_path, (void *) *(uintptr_t *)in, size);
		break;
	case SET_PARAM_FB_NUM_PLANES:
		if (inst->vsi == NULL)
			return -EINVAL;
		inst->vsi->dec.fb_num_planes = *(unsigned int *)in;
		break;
	case SET_PARAM_VDEC_PROPERTY:
		mtk_vcodec_err(inst, "VCU not support SET_PARAM_VDEC_PROPERTY\n");
		break;
	case SET_PARAM_CROP_INFO:
		if (inst->vsi == NULL)
			return -EINVAL;
		memcpy(&inst->vsi->crop, (struct v4l2_rect *)in, sizeof(struct v4l2_rect));
		break;
	case SET_PARAM_HDR10_INFO: {
		struct v4l2_vdec_hdr10_info *hdr10_info = in;

		if (inst->vsi == NULL)
			return -EINVAL;

		memcpy(&inst->vsi->hdr10_info, hdr10_info, sizeof(struct v4l2_vdec_hdr10_info));
		inst->vsi->hdr10_info_valid = true;
		break;
	}
	case SET_PARAM_TRICK_MODE:
		if (inst->vsi == NULL)
			return -EINVAL;
		inst->vsi->trick_mode = *(unsigned int *)in;
		break;
	case SET_PARAM_VDEC_VCU_VPUD_LOG:
		ret = VCU_FPTR(vcu_set_log)((char *) in);
		break;
	default:
		mtk_vcodec_err(inst, "invalid set parameter type=%d\n", type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int vdec_flush(unsigned long h_vdec, struct vdec_fb *fb,
	enum vdec_flush_type type)
{
	struct vdec_inst *inst = (struct vdec_inst *)h_vdec;
	struct vdec_vcu_inst *vcu = &inst->vcu;
	int ret;

	mtk_vcodec_debug(inst, "+ flush with type %d", type);

	inst->vsi->flush_type = type;
	if (fb == NULL)
		ret = vcu_dec_reset(vcu, VDEC_FLUSH); // flush (0)
	else if (fb->status == 0)
		ret = vcu_dec_reset(vcu, VDEC_DRAIN); // drain (1)
	else
		ret = vcu_dec_reset(vcu, VDEC_DRAIN_EOS); // drain & return EOS frame (2)


	mtk_vcodec_debug(inst, "+ flush ret %d", ret);
	return ret;
}

static struct vdec_common_if vdec_if = {
	vdec_init,
	vdec_decode,
	vdec_get_param,
	vdec_set_param,
	vdec_deinit,
	vdec_flush,
};

const struct vdec_common_if *get_dec_vcu_if(void);

const struct vdec_common_if *get_dec_vcu_if(void)
{
	return &vdec_if;
}

