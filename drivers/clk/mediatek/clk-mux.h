/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Owen Chen <owen.chen@mediatek.com>
 */

#ifndef __DRV_CLK_MTK_MUX_H
#define __DRV_CLK_MTK_MUX_H

#include <linux/clk-provider.h>

struct mtk_clk_mux {
	struct clk_hw hw;
	struct regmap *regmap;
	struct regmap *hwv_regmap;
	const struct mtk_mux *data;
	spinlock_t *lock;
	unsigned int flags;
};

struct mtk_mux {
	int id;
	const char *name;
	const char * const *parent_names;
	unsigned int flags;

	u32 mux_ofs;
	u32 set_ofs;
	u32 clr_ofs;
	u32 upd_ofs;
	u32 hwv_set_ofs;
	u32 hwv_clr_ofs;
	u32 hwv_sta_ofs;

	u8 mux_shift;
	u8 mux_width;
	u8 gate_shift;
	u8 upd_shift;
	u8 ipi_shift;
	u8 qs_shift;
	u8 default_opp;

	const struct clk_ops *ops;

	signed char num_parents;
};

extern const struct clk_ops mtk_mux_ops;
extern const struct clk_ops mtk_mux_clr_set_ops;
extern const struct clk_ops mtk_mux_clr_set_upd_ops;
extern const struct clk_ops mtk_mux_gate_ops;
extern const struct clk_ops mtk_mux_gate_clr_set_upd_ops;
extern const struct clk_ops mtk_mux_gate_clr_set_upd_2_ops;
extern const struct clk_ops mtk_hwv_mux_ops;
extern const struct clk_ops mtk_ipi_mux_ops;

#define GATE_CLR_SET_UPD_FLAGS(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width,	\
			_gate, _upd_ofs, _upd, _flags, _ops) {		\
		.id = _id,						\
		.name = _name,						\
		.mux_ofs = _mux_ofs,					\
		.set_ofs = _mux_set_ofs,				\
		.clr_ofs = _mux_clr_ofs,				\
		.upd_ofs = _upd_ofs,					\
		.mux_shift = _shift,					\
		.mux_width = _width,					\
		.gate_shift = _gate,					\
		.upd_shift = _upd,					\
		.parent_names = _parents,				\
		.num_parents = ARRAY_SIZE(_parents),			\
		.flags = _flags,					\
		.ops = &_ops,						\
	}

#define MUX_GATE_CLR_SET_UPD_FLAGS_2(_id, _name, _parents, _mux_ofs,	\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width,	\
			_gate, _upd_ofs, _upd, _flags)			\
		GATE_CLR_SET_UPD_FLAGS(_id, _name, _parents, _mux_ofs,	\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width,	\
			_gate, _upd_ofs, _upd, _flags,			\
			mtk_mux_gate_clr_set_upd_2_ops)

#define MUX_GATE_CLR_SET_UPD_FLAGS(_id, _name, _parents, _mux_ofs,	\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width,	\
			_gate, _upd_ofs, _upd, _flags)			\
		GATE_CLR_SET_UPD_FLAGS(_id, _name, _parents, _mux_ofs,	\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width,	\
			_gate, _upd_ofs, _upd, _flags,			\
			mtk_mux_gate_clr_set_upd_ops)

#define MUX_GATE_CLR_SET_UPD(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width,	\
			_gate, _upd_ofs, _upd)				\
		MUX_GATE_CLR_SET_UPD_FLAGS(_id, _name, _parents,	\
			_mux_ofs, _mux_set_ofs, _mux_clr_ofs, _shift,	\
			_width, _gate, _upd_ofs, _upd,			\
			0)

#define MUX_CLR_SET_UPD_FLAGS(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width,	\
			_upd_ofs, _upd, _flags)				\
		GATE_CLR_SET_UPD_FLAGS(_id, _name, _parents, _mux_ofs,	\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width,	\
			0, _upd_ofs, _upd, _flags,			\
			mtk_mux_clr_set_upd_ops)

#define MUX_CLR_SET_UPD(_id, _name, _parents, _mux_ofs,			\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width,	\
			_upd_ofs, _upd)					\
		MUX_CLR_SET_UPD_FLAGS(_id, _name, _parents,		\
			_mux_ofs, _mux_set_ofs, _mux_clr_ofs, _shift,	\
			_width, _upd_ofs, _upd, 0)

#define MUX_CLR_SET(_id, _name, _parents, _mux_ofs,			\
			_mux_set_ofs, _mux_clr_ofs, _shift, _width) {	\
		.id = _id,						\
		.name = _name,						\
		.mux_ofs = _mux_ofs,					\
		.set_ofs = _mux_set_ofs,				\
		.clr_ofs = _mux_clr_ofs,				\
		.mux_shift = _shift,					\
		.mux_width = _width,					\
		.parent_names = _parents,				\
		.num_parents = ARRAY_SIZE(_parents),			\
		.flags = 0,				\
		.ops = &mtk_mux_clr_set_ops,				\
	}

#define MUX_HWV(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _hwv_sta_ofs,	\
			_hwv_set_ofs, _hwv_clr_ofs, _shift, _width,	\
			_gate, _upd_ofs, _upd) {		\
		.id = _id,						\
		.name = _name,						\
		.mux_ofs = _mux_ofs,					\
		.set_ofs = _mux_set_ofs,				\
		.clr_ofs = _mux_clr_ofs,				\
		.hwv_sta_ofs = _hwv_sta_ofs,				\
		.hwv_set_ofs = _hwv_set_ofs,				\
		.hwv_clr_ofs = _hwv_clr_ofs,				\
		.upd_ofs = _upd_ofs,					\
		.mux_shift = _shift,					\
		.mux_width = _width,					\
		.gate_shift = _gate,					\
		.upd_shift = _upd,					\
		.parent_names = _parents,				\
		.num_parents = ARRAY_SIZE(_parents),			\
		.flags =  CLK_USE_HW_VOTER,	\
		.ops = &mtk_hwv_mux_ops,				\
	}

#define MUX_HWV_FLAGS(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _hwv_sta_ofs,	\
			_hwv_set_ofs, _hwv_clr_ofs, _shift, _width,	\
			_gate, _upd_ofs, _upd, _flags) {		\
		.id = _id,						\
		.name = _name,						\
		.mux_ofs = _mux_ofs,					\
		.set_ofs = _mux_set_ofs,				\
		.clr_ofs = _mux_clr_ofs,				\
		.hwv_sta_ofs = _hwv_sta_ofs,				\
		.hwv_set_ofs = _hwv_set_ofs,				\
		.hwv_clr_ofs = _hwv_clr_ofs,				\
		.upd_ofs = _upd_ofs,					\
		.mux_shift = _shift,					\
		.mux_width = _width,					\
		.gate_shift = _gate,					\
		.upd_shift = _upd,					\
		.parent_names = _parents,				\
		.num_parents = ARRAY_SIZE(_parents),			\
		.flags = CLK_USE_HW_VOTER | _flags,	\
		.ops = &mtk_hwv_mux_ops,				\
	}

#define MUX_IPI_CTL(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _hwv_sta_ofs,	\
			_hwv_set_ofs, _hwv_clr_ofs, _ipi_shift,		\
			_shift, _width, _gate, _upd_ofs, _upd,		\
			_qs_shift, _default_opp, _flags) {		\
		.id = _id,						\
		.name = _name,						\
		.mux_ofs = _mux_ofs,					\
		.set_ofs = _mux_set_ofs,				\
		.clr_ofs = _mux_clr_ofs,				\
		.hwv_sta_ofs = _hwv_sta_ofs,				\
		.hwv_set_ofs = _hwv_set_ofs,				\
		.hwv_clr_ofs = _hwv_clr_ofs,				\
		.upd_ofs = _upd_ofs,					\
		.mux_shift = _shift,					\
		.mux_width = _width,					\
		.gate_shift = _gate,					\
		.upd_shift = _upd,					\
		.ipi_shift = _ipi_shift,				\
		.qs_shift = _qs_shift,                                \
		.default_opp = _default_opp,				\
		.parent_names = _parents,				\
		.num_parents = ARRAY_SIZE(_parents),			\
		.flags =  CLK_USE_HW_VOTER | _flags,	\
		.ops = &mtk_ipi_mux_ops,				\
	}

#define MUX_IPI_FLAGS(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _hwv_sta_ofs,	\
			_hwv_set_ofs, _hwv_clr_ofs, _ipi_shift,		\
			_shift, _width, _gate, _upd_ofs, _upd,		\
			_qs_shift, _default_opp, _flags)		\
		MUX_IPI_CTL(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _hwv_sta_ofs,	\
			_hwv_set_ofs, _hwv_clr_ofs, _ipi_shift,		\
			_shift, _width, _gate, _upd_ofs, _upd,		\
			_qs_shift, _default_opp, _flags)

#define MUX_IPI(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _hwv_sta_ofs,	\
			_hwv_set_ofs, _hwv_clr_ofs, _ipi_shift,		\
			_shift, _width, _gate, _upd_ofs, _upd)		\
		MUX_IPI_CTL(_id, _name, _parents, _mux_ofs,		\
			_mux_set_ofs, _mux_clr_ofs, _hwv_sta_ofs,	\
			_hwv_set_ofs, _hwv_clr_ofs, _ipi_shift,		\
			_shift, _width, _gate, _upd_ofs, _upd,		\
			0, 0, 0)

int mtk_clk_register_muxes(const struct mtk_mux *muxes,
			   int num, struct device_node *node,
			   spinlock_t *lock,
			   struct clk_onecell_data *clk_data);

#endif /* __DRV_CLK_MTK_MUX_H */
