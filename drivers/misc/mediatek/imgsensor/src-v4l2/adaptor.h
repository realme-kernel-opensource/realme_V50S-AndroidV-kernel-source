/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (c) 2019 MediaTek Inc. */

#ifndef __ADAPTOR_H__
#define __ADAPTOR_H__

#include <linux/i2c.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-fwnode.h>
#include <media/v4l2-subdev.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_graph.h>
#include <linux/gpio/consumer.h>
#include <linux/regulator/consumer.h>
#include <linux/pinctrl/consumer.h>

#include "adaptor-def.h"
#include "adaptor-subdrv.h"
#include "imgsensor-user.h"

/* frame-sync */
#include "frame_sync.h"

#define to_ctx(__sd) container_of(__sd, struct adaptor_ctx, sd)

#define adaptor_logd(_ctx, format, args...) do { \
	if ((_ctx) && unlikely(*((_ctx)->sensor_debug_flag))) { \
		dev_info((_ctx)->dev, "[%s][%s][%s] " format, \
			(_ctx)->sd.name, \
			(_ctx)->subdrv->name, __func__, ##args); \
	} \
} while (0)

#define adaptor_logi(_ctx, format, args...) do { \
	if (_ctx) { \
		dev_info((_ctx)->dev, "[%s][%s][%s] " format, \
			(_ctx)->sd.name, \
			(_ctx)->subdrv->name, __func__, ##args); \
	} \
} while (0)

struct adaptor_ctx;
static unsigned int sensor_debug;

union feature_para {
	u64 u64[4];
	u32 u32[8];
	u16 u16[16];
	u8 u8[32];
};

struct sensor_mode {
	u32 id;
	u32 llp;
	u32 fll;
	u32 width;
	u32 height;
	u32 mipi_pixel_rate;
	u32 cust_pixel_rate;
	u32 max_framerate;
	u32 pclk;
	u64 linetime_in_ns;
	u64 linetime_in_ns_readout;
	u64 fine_intg_line;
	struct mtk_csi_param csi_param;
	u8 esd_reset_by_user;
};

struct adaptor_hw_ops {
	int (*set)(struct adaptor_ctx *ctx, void *data, int val);
	int (*unset)(struct adaptor_ctx *ctx, void *data, int val);
	void *data;
};

struct adaptor_ctx {
	struct mutex mutex;
	struct i2c_client *i2c_client;
	struct device *dev;
	struct v4l2_subdev sd;
	struct media_pad pad;
	struct v4l2_fwnode_endpoint ep;

	/* V4L2 Controls */
	struct v4l2_ctrl_handler ctrls;
	struct v4l2_ctrl *pixel_rate;
	struct v4l2_ctrl *vblank;
	struct v4l2_ctrl *hblank;
	struct v4l2_ctrl *exposure;
	struct v4l2_ctrl *exposure_abs;
	struct v4l2_ctrl *hflip;
	struct v4l2_ctrl *vflip;
	struct v4l2_ctrl *pd_pixel_region;
	struct v4l2_ctrl *max_fps;
	struct v4l2_ctrl *test_pattern;

	/* custom v4l2 ctrls */
	struct v4l2_ctrl *anti_flicker;
	struct v4l2_ctrl *analogue_gain;
	struct v4l2_ctrl *awb_gain;
	struct v4l2_ctrl *shutter_gain_sync;
	struct v4l2_ctrl *ihdr_shutter_gain;
	struct v4l2_ctrl *dual_gain;
	struct v4l2_ctrl *hdr_shutter;
	struct v4l2_ctrl *shutter_frame_length;
	struct v4l2_ctrl *pdfocus_area;
	struct v4l2_ctrl *hdr_atr;
	struct v4l2_ctrl *hdr_tri_shutter;
	struct v4l2_ctrl *hdr_tri_gain;
	struct v4l2_ctrl *hdr_ae_ctrl;

	/* custom v4l2 ctrls - frame sync - */
	struct v4l2_ctrl *frame_sync;
	struct v4l2_ctrl *fsync_async_master;
	struct v4l2_ctrl *fsync_map_id;
	struct v4l2_ctrl *fsync_listen_target;

	/* hw handles */
	struct clk *clk[CLK_MAXCNT];
	struct regulator *regulator[REGULATOR_MAXCNT];
	struct pinctrl *pinctrl;
	struct pinctrl_state *state[STATE_MAXCNT];
	struct adaptor_hw_ops hw_ops[HW_ID_MAXCNT];

	/* sensor */
	struct subdrv_entry *subdrv;
	struct subdrv_ctx subctx;
	struct sensor_mode mode[MODE_MAXCNT];
	struct sensor_mode *cur_mode;
	struct sensor_mode *try_format_mode;
	int mode_cnt;
	MSDK_SENSOR_INFO_STRUCT sensor_info;
	MSDK_SENSOR_CONFIG_STRUCT sensor_cfg;
	int fmt_code;
	int idx; /* requireed by frame-sync modules */
	int dts_idx; /* dts idx (before reindex) */
	struct mtk_hdr_ae ae_memento;

	u32 seamless_scenarios[SENSOR_SCENARIO_ID_MAX];

	/* sensor property */
	u32 location;
	u32 rotation;

	/* frame-sync */
	struct FrameSync *fsync_mgr;
	unsigned int fsync_out_fl;

	/* flags */
	unsigned int is_streaming:1;
	unsigned int is_sensor_inited:1;
	unsigned int is_sensor_scenario_inited:1;
	unsigned int is_sensor_reset_stream_off:1;

	int open_refcnt;
	int power_refcnt;

	struct subdrv_pw_seq_entry *ctx_pw_seq;

	/* debug var */
	MSDK_SENSOR_REG_INFO_STRUCT sensorReg;

	unsigned int *sensor_debug_flag;
	unsigned int sof_cnt; /* from seninf vsync notify */
	int req_id; /* from mtk hdr ae ctrl */
#ifdef OPLUS_FEATURE_CAMERA_COMMON
	u64 shutter_for_timeout;
#else
	u32 shutter_for_timeout;
#endif
	u64 last_framelength;
	struct wakeup_source *sensor_ws;
	unsigned int aov_pm_ops_flag;	/* flag for aov pm ops */
	unsigned int aov_mclk_ulposc_flag;	/* flag for aov switch mclk to ulposc */
#ifdef OPLUS_FEATURE_CAMERA_COMMON
/*Added by rentianzhi@CamDrv, release the hw resource for Explorer AON driver, 20220124*/
	unsigned int support_explorer_aon_fl;//1:use explorer AON driver
	int aon_irq_gpio;
	int pid;
	int irq_cnt;
	struct work_struct aon_wq;
	struct mutex hw_mutex;
#endif
	unsigned int aov_i2c_bus_scl_switch_en;	/* flag for aov switch i2c bus scl */
	unsigned int aov_i2c_bus_sda_switch_en;	/* flag for aov switch i2c bus sda */
};

#endif
