/*
 * Copyright (C) 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/atomic.h>
#include <linux/types.h>
#include "sc520csmipirawavatarb5_Sensor.h"

#define LEFT_SIZE (0x4000)
#define SC520CS_EEPROM_I2C_ADDR 0x6c
#define DEVICE_VERSION_SC520CS_AVATARB5 "sc520cs_avaterb5"
#define IMGSENSOR_SENSOR_IDX_SUB 1
#define MULTI_WRITE 1
#define LOG_TAG "sc520cs_mipi_raw"
#define LOG_INF(format, args...) \
	pr_info(LOG_TAG "[%s] " format, __func__, ##args)
#define LOG_ERR(format, ...) \
	pr_err(LOG_TAG "Line:%d FUNC:%s:"format, __LINE__, __func__, ## __VA_ARGS__)
#define LOG_INFO(format, ...) \
	pr_info(LOG_TAG "  %s:  "format, __func__, ## __VA_ARGS__)
#define LOG_DEBUG(format, ...) \
	pr_debug(LOG_TAG "  %s:  "format, __func__, ## __VA_ARGS__)
#define ERROR_I2C       1
#define ERROR_CHECKSUM  2
#define ERROR_READ_FLAG 3
#define OTP_THREAHOLD   3
#define MAX_EEPROM_BYTE 0x1FFF
#define CHECKSUM_FLAG_ADDR MAX_EEPROM_BYTE-1
#define READ_FLAG_ADDR MAX_EEPROM_BYTE-2
#define OTP_DATA_GOOD_FLAG 0x88
#define MAX_NAME_LENGTH 20
#define MAX_GROUP_NUM 8
#define MAX_GROUP_ADDR_NUM 3
#ifndef I2C_WR_FLAG
#define I2C_WR_FLAG     (0x1000)
#define I2C_MASK_FLAG   (0x00ff)
#endif
typedef struct
{
	int group_start_addr;
	int group_end_addr;
	int group_flag_addr;
	int group_checksum_addr;
}GROUP_ADDR_INFO;
typedef struct
{
	char group_name[MAX_NAME_LENGTH];
	GROUP_ADDR_INFO group_addr_info[MAX_GROUP_ADDR_NUM];
}GROUP_INFO;
typedef struct
{
	char module_name[MAX_NAME_LENGTH];
	int group_num;
	int group_addr_info_num;
	GROUP_INFO group_info[MAX_GROUP_NUM];
	int  (* readFunc) (u16 addr, u8 *data);
}OTP_MAP;

char g_otp_buf[3][MAX_EEPROM_BYTE] = {{0}, {0}, {0}};
static kal_uint8 otp_data[LEFT_SIZE] = {0};
static DEFINE_SPINLOCK(imgsensor_drv_lock);
static BYTE sc520cs_common_data[CAMERA_EEPPROM_COMDATA_LENGTH] = { 0 };
static kal_uint8 deviceInfo_register_value = 0x00;
extern void register_imgsensor_deviceinfo(char *name, char *version, u8 module_id);

static struct imgsensor_info_struct imgsensor_info = {
	.sensor_id = SC520CS_SENSOR_ID_AVATARB5,
	.checksum_value = 0x55e2a82f,
	.module_id = 0x07,
	.pre = {
		.pclk = 90000000,
		.linelength = 1500,
		.framelength = 2000,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 2592,
		.grabwindow_height = 1944,
		.mipi_data_lp2hs_settle_dc = 14,
		.max_framerate = 300,
		.mipi_pixel_rate = 179200000,//896Mbps*2
	},
	.cap = {
		.pclk = 90000000,
		.linelength = 1500,
		.framelength = 2000,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 2592,
		.grabwindow_height = 1944,
		.mipi_data_lp2hs_settle_dc = 14,
		.max_framerate = 300,
		.mipi_pixel_rate = 179200000,//896Mbps*2
	},
	.cap1 = {
		.pclk = 90000000,
		.linelength = 1500,
		.framelength = 2000,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 2592,
		.grabwindow_height = 1944,
		.mipi_data_lp2hs_settle_dc = 14,
		.max_framerate = 300,
		.mipi_pixel_rate = 179200000,//896Mbps*2
	},
	.normal_video = {
		.pclk = 90000000,
		.linelength = 1500,
		.framelength = 2000,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 2592,
		.grabwindow_height = 1944,
		.mipi_data_lp2hs_settle_dc = 14,
		.max_framerate = 300,
		.mipi_pixel_rate = 179200000,//896Mbps*2
	},
	.hs_video = {
		.pclk = 90000000,
		.linelength = 1500,
		.framelength = 2000,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 1296,
		.grabwindow_height = 972,
		.mipi_data_lp2hs_settle_dc = 14,
		.max_framerate = 300,
		.mipi_pixel_rate = 179200000,//896Mbps*2
	},
	.slim_video = {
		.pclk = 90000000,
		.linelength = 1500,
		.framelength = 2000,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 2592,
		.grabwindow_height = 1458,
		.mipi_data_lp2hs_settle_dc = 14,
		.max_framerate = 300,
		.mipi_pixel_rate = 179200000,//896Mbps*2
	},

	.custom1 = {
		.pclk = 90000000,
		.linelength = 1500,
		.framelength = 2000,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 2592,
		.grabwindow_height = 1944,
		.mipi_data_lp2hs_settle_dc = 14,
		.max_framerate = 300,
		.mipi_pixel_rate = 179200000,//896Mbps*2
	},
  	.exp_step = 2,
	.min_gain = BASEGAIN, // 1x gain
	.max_gain = 16 * BASEGAIN, // real again is 15.5x
	.min_gain_iso = 100,
	.gain_step = 1,
	.gain_type = 1,
	.margin = 5,            //sensor framelength & shutter margin
	.min_shutter = 2,
	.max_frame_length = 0x7FFF,
	.ae_shut_delay_frame = 0,
	.ae_sensor_gain_delay_frame = 0,
	.ae_ispGain_delay_frame = 2,
	.ihdr_support = 0,     // //1, support; 0,not support
	.ihdr_le_firstline = 0, // //1,le first ; 0, se first
	.sensor_mode_num = 6,	  //support sensor mode num

	.cap_delay_frame = 3,
	.pre_delay_frame = 2,
	.video_delay_frame = 3,
	.hs_video_delay_frame = 3,
	.slim_video_delay_frame = 3,
	.custom1_delay_frame = 3,
	.frame_time_delay_frame = 2,
	.isp_driving_current = ISP_DRIVING_6MA,
	.sensor_interface_type = SENSOR_INTERFACE_TYPE_MIPI,
	.mipi_sensor_type = MIPI_OPHY_NCSI2,
	.mipi_settle_delay_mode = MIPI_SETTLEDELAY_MANUAL,//0,MIPI_SETTLEDELAY_AUTO; 1,MIPI_SETTLEDELAY_MANUAL
	.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_B,
	.mclk = 24,
	.mipi_lane_num = SENSOR_MIPI_2_LANE,
	.i2c_addr_table = {0x20, 0x6c, 0xff},
	.i2c_speed = 400,
};

static struct imgsensor_struct imgsensor = {
	.mirror = IMAGE_NORMAL,
	.sensor_mode = IMGSENSOR_MODE_INIT,
	.shutter = 0x3d0,                   //current shutter
	.gain = 0x40,                      //current gain
	.dummy_pixel = 0,
	.dummy_line = 0,
	.current_fps = 300,//full size current fps : 24fps for PIP, 30fps for Normal or ZSD
	.autoflicker_en = KAL_FALSE,
	.test_pattern = KAL_FALSE,
	.current_scenario_id = MSDK_SCENARIO_ID_CAMERA_PREVIEW,
	.ihdr_en = 0,
	.i2c_write_id = 0x6c,
};

/* Sensor output window information */
static struct SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[6] = {
 { 2592, 1944,	  0,	0, 2592, 1944, 2592, 1944, 0000, 0000, 2592, 1944,    0, 0, 2592, 1944}, // Preview
 { 2592, 1944,	  0,	0, 2592, 1944, 2592, 1944, 0000, 0000, 2592, 1944,    0, 0, 2592, 1944}, // capture
 { 2592, 1944,	  0,	0, 2592, 1944, 2592, 1944, 0000, 0000, 2592, 1944,    0, 0, 2592, 1944}, // video
 { 2592, 1944,	  0,	0, 2592, 1944, 1296,  972, 0000, 0000, 1296,  972,    0, 0, 1296,  972}, //hight speed video
 { 2592, 1944,	  0,  243, 2592, 1458, 2592, 1458, 0000, 0000, 2592, 1458,    0, 0, 2592, 1458},// slim video
 { 2592, 1944,    0,    0, 2592, 1944, 2592, 1944, 0000, 0000, 2592, 1944,    0, 0, 2592, 1944},// custom1
};
#if MULTI_WRITE
#define I2C_BUFFER_LEN 765

static kal_uint16 sc520cs_table_write_cmos_sensor(
					kal_uint16 *para, kal_uint32 len)
{
	char puSendCmd[I2C_BUFFER_LEN];
	kal_uint32 tosend, IDX;
	kal_uint16 addr = 0, addr_last = 0, data;

	tosend = 0;
	IDX = 0;
	while (len > IDX) {
		addr = para[IDX];

		{
			puSendCmd[tosend++] = (char)(addr >> 8);
			puSendCmd[tosend++] = (char)(addr & 0xFF);
			data = para[IDX + 1];
			//puSendCmd[tosend++] = (char)(data >> 8);
			puSendCmd[tosend++] = (char)(data & 0xFF);
			IDX += 2;
			addr_last = addr;
		}

		if ((I2C_BUFFER_LEN - tosend) < 4 ||
			len == IDX ||
			addr != addr_last) {
			iBurstWriteReg_multi(puSendCmd, tosend,
				imgsensor.i2c_write_id,
				3, imgsensor_info.i2c_speed);

			tosend = 0;
		}
	}
	return 0;
}
#endif

static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte = 0;
	char pu_send_cmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };

	iReadRegI2C(pu_send_cmd, 2, (u8 *)&get_byte, 1, imgsensor.i2c_write_id);

	return get_byte;
}

static void write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
	char pu_send_cmd[4] = {(char)(addr >> 8),
		(char)(addr & 0xFF), (char)(para & 0xFF)};

	iWriteRegI2C(pu_send_cmd, 3, imgsensor.i2c_write_id);
}

static void write_cmos_sensor8(kal_uint32 addr, kal_uint32 para)
{
	char pu_send_cmd[4] = {(char)(addr >> 8),
		(char)(addr & 0xFF), (char)(para & 0xFF)};

	iWriteRegI2C(pu_send_cmd, 3, imgsensor.i2c_write_id);
}

static void set_dummy(void)
{
	LOG_INF("dummyline = %d, dummypixels = %d\n",
		imgsensor.dummy_line, imgsensor.dummy_pixel);
	write_cmos_sensor8(0x320e, imgsensor.frame_length >> 8);
	write_cmos_sensor8(0x320f, imgsensor.frame_length & 0xFF);
	write_cmos_sensor8(0x320c, imgsensor.line_length >> 8);
	write_cmos_sensor8(0x320d, imgsensor.line_length & 0xFF);


}	/*	set_dummy  */

static kal_uint32 return_sensor_id(void)
{
	kal_uint32 sensor_id = (read_cmos_sensor(0x3107) << 8) | read_cmos_sensor(0x3108);

	//return ((read_cmos_sensor(0x3107) << 8) | read_cmos_sensor(0x3108)); //0xee4b
	return sensor_id;
}


static void set_max_framerate(UINT16 framerate, kal_bool min_framelength_en)
{
	kal_uint32 frame_length = imgsensor.frame_length;

	frame_length = imgsensor.pclk / framerate * 10 / imgsensor.line_length;
	spin_lock(&imgsensor_drv_lock);
	imgsensor.frame_length = (frame_length > imgsensor.min_frame_length) ?
			frame_length : imgsensor.min_frame_length;
	imgsensor.dummy_line = imgsensor.frame_length -
		imgsensor.min_frame_length;

	if (imgsensor.frame_length > imgsensor_info.max_frame_length) {
		imgsensor.frame_length = imgsensor_info.max_frame_length;
		imgsensor.dummy_line = imgsensor.frame_length -
			imgsensor.min_frame_length;
	}
	if (min_framelength_en)
		imgsensor.min_frame_length = imgsensor.frame_length;

	spin_unlock(&imgsensor_drv_lock);
	set_dummy();
}	/*	set_max_framerate  */

static void write_shutter(kal_uint32 shutter)
{
	kal_uint16 realtime_fps = 0;

	spin_lock(&imgsensor_drv_lock);

	if (shutter > imgsensor.min_frame_length - imgsensor_info.margin)
		imgsensor.frame_length = shutter + imgsensor_info.margin;
	else
		imgsensor.frame_length = imgsensor.min_frame_length;
	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		imgsensor.frame_length = imgsensor_info.max_frame_length;
	spin_unlock(&imgsensor_drv_lock);

	shutter = (shutter < imgsensor_info.min_shutter) ?
		imgsensor_info.min_shutter : shutter;
	shutter = (shutter >
		(imgsensor_info.max_frame_length - imgsensor_info.margin)) ?
		(imgsensor_info.max_frame_length - imgsensor_info.margin) :
		shutter;
	if (imgsensor.autoflicker_en) {
		realtime_fps = imgsensor.pclk * 10 /
			(imgsensor.line_length * imgsensor.frame_length);
		if (realtime_fps >= 297 && realtime_fps <= 305)
			set_max_framerate(296, 0);
		else if (realtime_fps >= 147 && realtime_fps <= 150)
			set_max_framerate(146, 0);
		else{
		write_cmos_sensor8(0x320e, (imgsensor.frame_length >> 8) & 0xff);
		write_cmos_sensor8(0x320f, imgsensor.frame_length & 0xFF);
		}

	} else{
		// Extend frame length

		// ADD ODIN
		realtime_fps = imgsensor.pclk * 10 /
			(imgsensor.line_length * imgsensor.frame_length);
		if (realtime_fps > 300 && realtime_fps < 320)
			set_max_framerate(300, 0);
		// ADD END
		write_cmos_sensor8(0x320e, (imgsensor.frame_length >> 8) & 0xff);
		write_cmos_sensor8(0x320f, imgsensor.frame_length & 0xFF);
	}

	// Update Shutter
	shutter = shutter *2;
	write_cmos_sensor8(0x3e00, (shutter >> 12) & 0xFF);
	write_cmos_sensor8(0x3e01, (shutter >> 4)&0xFF);
	write_cmos_sensor8(0x3e02, (shutter<<4) & 0xF0);

	LOG_INF("shutter =%d, framelength =%d",
		shutter, imgsensor.frame_length);
}	/*	write_shutter  */

/*************************************************************************
 * FUNCTION
 *	set_shutter
 *
 * DESCRIPTION
 *	This function set e-shutter of sensor to change exposure time.
 *
 * PARAMETERS
 *	iShutter : exposured lines
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static void set_shutter(kal_uint32 shutter)
{
	unsigned long flags;

	LOG_INF("set_shutter");
	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);

	write_shutter(shutter);
}	/*	set_shutter */

/* SENSOR PRIVATE INFO FOR GAIN SETTING */
#define SC520CS_SENSOR_GAIN_BASE    (0x400)
#define SC520CS_SENSOR_GAIN_MAX              (160 * SC520CS_SENSOR_GAIN_BASE / 10)
#define SC520CS_SENSOR_GAIN_MAP_SIZE  5

static kal_uint16 gain2reg(const kal_uint16 gain)
{
	kal_uint16 reg_gain = gain << 4;

	if (reg_gain < SC520CS_SENSOR_GAIN_BASE)
		reg_gain = SC520CS_SENSOR_GAIN_BASE;
	else if (reg_gain > SC520CS_SENSOR_GAIN_MAX)
		reg_gain = SC520CS_SENSOR_GAIN_MAX;

	return (kal_uint16)reg_gain;
}

/*************************************************************************
 * FUNCTION
 *	set_gain
 *
 * DESCRIPTION
 *	This function is to set global gain to sensor.
 *
 * PARAMETERS
 *	iGain : sensor global gain(base: 0x40)
 *
 * RETURNS
 *	the actually gain set to sensor.
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static kal_uint16 set_gain(kal_uint16 gain)
{
	kal_uint16 reg_gain;
	kal_uint32 temp_gain;
	kal_int16 gain_index;
	kal_uint16 SC520CS_AGC_Param[SC520CS_SENSOR_GAIN_MAP_SIZE][2] = {
		{  1024,  0x00 },
		{  2048,  0x08 },
		{  4096,  0x09 },
		{  8192,  0x0B },
		{ 16384,  0x0f },
	};

    reg_gain = gain2reg(gain);

	for (gain_index = SC520CS_SENSOR_GAIN_MAP_SIZE - 1; gain_index > 0; gain_index--)
		if (reg_gain >= SC520CS_AGC_Param[gain_index][0])
			break;

	write_cmos_sensor(0x3e09, SC520CS_AGC_Param[gain_index][1]);
	temp_gain = reg_gain * SC520CS_SENSOR_GAIN_BASE / SC520CS_AGC_Param[gain_index][0];
	write_cmos_sensor(0x3e07, (temp_gain >> 3) & 0xff);

	LOG_INF("Exit! SC520CS_AGC_Param[gain_index][1] = 0x%x, temp_gain = 0x%x, gain = 0x%x, reg_gain = %d\n",
		SC520CS_AGC_Param[gain_index][1], temp_gain, gain, reg_gain);

	return reg_gain;
}/*set_gain  */

/*
static void set_mirror_flip(kal_uint8 image_mirror)
{
	LOG_INF("image_mirror = %d", image_mirror);

	switch (image_mirror) {
		case IMAGE_NORMAL:
			write_cmos_sensor8(0x3221,0x00);
			break;
		case IMAGE_H_MIRROR:
			write_cmos_sensor8(0x3221,0x06);
			break;
		case IMAGE_V_MIRROR:
			write_cmos_sensor8(0x3221,0x60);
			break;
		case IMAGE_HV_MIRROR:
			write_cmos_sensor8(0x3221,0x66);

		break;
	default:
		LOG_INF("Error image_mirror setting");
		break;
	}
}*/

/*************************************************************************
 * FUNCTION
 *	night_mode
 *
 * DESCRIPTION
 *	This function night mode of sensor.
 *
 * PARAMETERS
 *	bEnable: KAL_TRUE -> enable night mode, otherwise, disable night mode
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
#if 0
static void night_mode(kal_bool enable)
{
	/*No Need to implement this function*/
}	/*	night_mode	*/
#endif

static kal_uint16 avaterb5_sc520cs_init_setting[] = {
	0x0100,0x00,
	0x36e9,0x80,
	0x37f9,0x80,
	0x36ea,0x0e,
	0x36eb,0x0c,
	0x36ec,0x4a,
	0x36ed,0x18,
	0x36e9,0x44,
	0x37f9,0x23,
	0x301f,0x42,
	0x3205,0x27,
	0x3209,0x20,
	0x3211,0x04,
	0x3301,0x08,
	0x3306,0x60,
	0x3308,0x10,
	0x3309,0x80,
	0x330b,0xd0,
	0x3314,0x15,
	0x331f,0x71,
	0x3333,0x10,
	0x3334,0x40,
	0x3364,0x56,
	0x336c,0xcf,
	0x3390,0x09,
	0x3391,0x0f,
	0x3392,0x1f,
	0x3393,0x0e,
	0x3394,0x15,
	0x3395,0x19,
	0x33ad,0x1c,
	0x33af,0x3f,
	0x33b3,0x30,
	0x33b5,0x90,
	0x349f,0x1e,
	0x34a6,0x09,
	0x34a7,0x0b,
	0x34a8,0x20,
	0x34a9,0x20,
	0x34f8,0x0f,
	0x34f9,0x10,
	0x3630,0x88,
	0x3633,0x34,
	0x3635,0xc0,
	0x3637,0x52,
	0x3638,0xc1,
	0x363a,0x89,
	0x3670,0x4a,
	0x3674,0x48,
	0x3675,0x28,
	0x3676,0x18,
	0x367c,0x0f,
	0x367d,0x1f,
	0x3690,0x43,
	0x3691,0x43,
	0x3692,0x43,
	0x3698,0x89,
	0x3699,0x92,
	0x369a,0xa5,
	0x369b,0xca,
	0x369c,0x0b,
	0x369d,0x0f,
	0x36a2,0x09,
	0x36a3,0x0b,
	0x36a4,0x0f,
	0x36b1,0x38,
	0x36b2,0xc1,
	0x370f,0x01,
	0x3724,0xb1,
	0x3771,0x02,
	0x3772,0x03,
	0x3773,0x03,
	0x377a,0x0b,
	0x377b,0x0f,
	0x3901,0x04,
	0x3905,0x8d,
	0x391d,0x01,
	0x3926,0x23,
	0x3e00,0x00,
	0x3e01,0xf9,
	0x3e02,0x70,
	0x3e09,0x00,
	0x4401,0x13,
	0x4402,0x03,
	0x4403,0x0b,
	0x4404,0x21,
	0x4405,0x29,
	0x4407,0x0c,
	0x440c,0x34,
	0x440d,0x34,
	0x440e,0x29,
	0x440f,0x3f,
	0x4412,0x01,
	0x4424,0x01,
	0x442d,0x00,
	0x442e,0x00,
	0x4509,0x28,
	0x450d,0x19,
	0x451d,0x28,
	0x4526,0x05,
	0x5000,0x0e,
	0x5007,0x00,
	0x550e,0x00,
	0x550f,0x42,
	0x5780,0x66,
	0x5787,0x08,
	0x5788,0x04,
	0x5789,0x01,
	0x578d,0x40,
	0x5790,0x08,
	0x5791,0x04,
	0x5792,0x01,
	0x5799,0x46,
	0x57aa,0xeb,
	0x59f0,0x00,
	0x5ae0,0xfe,
	0x5ae1,0x40,
	0x5ae2,0x38,
	0x5ae3,0x30,
	0x5ae4,0x0c,
	0x5ae5,0x38,
	0x5ae6,0x30,
	0x5ae7,0x28,
	0x5ae8,0x3f,
	0x5ae9,0x34,
	0x5aea,0x2c,
	0x5aeb,0x3f,
	0x5aec,0x34,
	0x5aed,0x2c,
	0x3652,0x33,//MIPI test
	0x3654,0x01,//MIPI test
};


static void sensor_init(void)
{
LOG_INF("[%s] sensor_init\n", __func__);
	write_cmos_sensor8(0x0103,0x01);
	mDELAY(10);

	sc520cs_table_write_cmos_sensor(avaterb5_sc520cs_init_setting,
		sizeof(avaterb5_sc520cs_init_setting) / sizeof(kal_uint16));

	if ((read_cmos_sensor(0x800B)&0xFF)==0)
    {
	 write_cmos_sensor8(0x550F,0x42);
    }else {
	 write_cmos_sensor8(0x550F,0x8a);
    }
LOG_INF("0X800B = 0x%x, 0X550f = 0x%x \n", read_cmos_sensor(0x800B),read_cmos_sensor(0x550F));

}

static kal_uint16 avaterb5_sc520cs_preview_setting[] = {
	0x0100,0x00,
	0x36e9,0x80,
	0x37f9,0x80,
	0x36ea,0x0e,
	0x36eb,0x0c,
	0x36ec,0x4a,
	0x36ed,0x18,
	0x36e9,0x44,
	0x37f9,0x23,
	0x301f,0x42,
	0x3205,0x27,
	0x3209,0x20,
	0x3211,0x04,
	0x3301,0x08,
	0x3306,0x60,
	0x3308,0x10,
	0x3309,0x80,
	0x330b,0xd0,
	0x3314,0x15,
	0x331f,0x71,
	0x3333,0x10,
	0x3334,0x40,
	0x3364,0x56,
	0x336c,0xcf,
	0x3390,0x09,
	0x3391,0x0f,
	0x3392,0x1f,
	0x3393,0x0e,
	0x3394,0x15,
	0x3395,0x19,
	0x33ad,0x1c,
	0x33af,0x3f,
	0x33b3,0x30,
	0x33b5,0x90,
	0x349f,0x1e,
	0x34a6,0x09,
	0x34a7,0x0b,
	0x34a8,0x20,
	0x34a9,0x20,
	0x34f8,0x0f,
	0x34f9,0x10,
	0x3630,0x88,
	0x3633,0x34,
	0x3635,0xc0,
	0x3637,0x52,
	0x3638,0xc1,
	0x363a,0x89,
	0x3670,0x4a,
	0x3674,0x48,
	0x3675,0x28,
	0x3676,0x18,
	0x367c,0x0f,
	0x367d,0x1f,
	0x3690,0x43,
	0x3691,0x43,
	0x3692,0x43,
	0x3698,0x89,
	0x3699,0x92,
	0x369a,0xa5,
	0x369b,0xca,
	0x369c,0x0b,
	0x369d,0x0f,
	0x36a2,0x09,
	0x36a3,0x0b,
	0x36a4,0x0f,
	0x36b1,0x38,
	0x36b2,0xc1,
	0x370f,0x01,
	0x3724,0xb1,
	0x3771,0x02,
	0x3772,0x03,
	0x3773,0x03,
	0x377a,0x0b,
	0x377b,0x0f,
	0x3901,0x04,
	0x3905,0x8d,
	0x391d,0x01,
	0x3926,0x23,
	0x3e00,0x00,
	0x3e01,0xf9,
	0x3e02,0x70,
	0x3e09,0x00,
	0x4401,0x13,
	0x4402,0x03,
	0x4403,0x0b,
	0x4404,0x21,
	0x4405,0x29,
	0x4407,0x0c,
	0x440c,0x34,
	0x440d,0x34,
	0x440e,0x29,
	0x440f,0x3f,
	0x4412,0x01,
	0x4424,0x01,
	0x442d,0x00,
	0x442e,0x00,
	0x4509,0x28,
	0x450d,0x19,
	0x451d,0x28,
	0x4526,0x05,
	0x5000,0x0e,
	0x5007,0x00,
	0x550e,0x00,
	0x550f,0x8a,
	0x5780,0x66,
	0x5787,0x08,
	0x5788,0x04,
	0x5789,0x01,
	0x578d,0x40,
	0x5790,0x08,
	0x5791,0x04,
	0x5792,0x01,
	0x5799,0x46,
	0x57aa,0xeb,
	0x59f0,0x00,
	0x5ae0,0xfe,
	0x5ae1,0x40,
	0x5ae2,0x38,
	0x5ae3,0x30,
	0x5ae4,0x0c,
	0x5ae5,0x38,
	0x5ae6,0x30,
	0x5ae7,0x28,
	0x5ae8,0x3f,
	0x5ae9,0x34,
	0x5aea,0x2c,
	0x5aeb,0x3f,
	0x5aec,0x34,
	0x5aed,0x2c,
	0x3652,0x33,//MIPI test
	0x3654,0x01,//MIPI test
};

static void preview_setting(void)
{
LOG_INF("[%s] preview_setting\n", __func__);
	write_cmos_sensor8(0x0103,0x01);
		mDELAY(10);

	sc520cs_table_write_cmos_sensor(avaterb5_sc520cs_preview_setting,
		sizeof(avaterb5_sc520cs_preview_setting) / sizeof(kal_uint16));

  if ((read_cmos_sensor(0x800B)&0xFF)==0)
    {
	 write_cmos_sensor8(0x550F,0x42);
    }else {
	 write_cmos_sensor8(0x550F,0x8a);
    }
LOG_INF("0X800B = 0x%x, 0X550f = 0x%x \n", read_cmos_sensor(0x800B),read_cmos_sensor(0x550F));
}

static kal_uint16 avaterb5_sc520cs_capture_setting[] = {
	0x0100,0x00,
	0x36e9,0x80,
	0x37f9,0x80,
	0x36ea,0x0e,
	0x36eb,0x0c,
	0x36ec,0x4a,
	0x36ed,0x18,
	0x36e9,0x44,
	0x37f9,0x23,
	0x301f,0x42,
	0x3205,0x27,
	0x3209,0x20,
	0x3211,0x04,
	0x3301,0x08,
	0x3306,0x60,
	0x3308,0x10,
	0x3309,0x80,
	0x330b,0xd0,
	0x3314,0x15,
	0x331f,0x71,
	0x3333,0x10,
	0x3334,0x40,
	0x3364,0x56,
	0x336c,0xcf,
	0x3390,0x09,
	0x3391,0x0f,
	0x3392,0x1f,
	0x3393,0x0e,
	0x3394,0x15,
	0x3395,0x19,
	0x33ad,0x1c,
	0x33af,0x3f,
	0x33b3,0x30,
	0x33b5,0x90,
	0x349f,0x1e,
	0x34a6,0x09,
	0x34a7,0x0b,
	0x34a8,0x20,
	0x34a9,0x20,
	0x34f8,0x0f,
	0x34f9,0x10,
	0x3630,0x88,
	0x3633,0x34,
	0x3635,0xc0,
	0x3637,0x52,
	0x3638,0xc1,
	0x363a,0x89,
	0x3670,0x4a,
	0x3674,0x48,
	0x3675,0x28,
	0x3676,0x18,
	0x367c,0x0f,
	0x367d,0x1f,
	0x3690,0x43,
	0x3691,0x43,
	0x3692,0x43,
	0x3698,0x89,
	0x3699,0x92,
	0x369a,0xa5,
	0x369b,0xca,
	0x369c,0x0b,
	0x369d,0x0f,
	0x36a2,0x09,
	0x36a3,0x0b,
	0x36a4,0x0f,
	0x36b1,0x38,
	0x36b2,0xc1,
	0x370f,0x01,
	0x3724,0xb1,
	0x3771,0x02,
	0x3772,0x03,
	0x3773,0x03,
	0x377a,0x0b,
	0x377b,0x0f,
	0x3901,0x04,
	0x3905,0x8d,
	0x391d,0x01,
	0x3926,0x23,
	0x3e00,0x00,
	0x3e01,0xf9,
	0x3e02,0x70,
	0x3e09,0x00,
	0x4401,0x13,
	0x4402,0x03,
	0x4403,0x0b,
	0x4404,0x21,
	0x4405,0x29,
	0x4407,0x0c,
	0x440c,0x34,
	0x440d,0x34,
	0x440e,0x29,
	0x440f,0x3f,
	0x4412,0x01,
	0x4424,0x01,
	0x442d,0x00,
	0x442e,0x00,
	0x4509,0x28,
	0x450d,0x19,
	0x451d,0x28,
	0x4526,0x05,
	0x5000,0x0e,
	0x5007,0x00,
	0x550e,0x00,
	0x550f,0x8a,
	0x5780,0x66,
	0x5787,0x08,
	0x5788,0x04,
	0x5789,0x01,
	0x578d,0x40,
	0x5790,0x08,
	0x5791,0x04,
	0x5792,0x01,
	0x5799,0x46,
	0x57aa,0xeb,
	0x59f0,0x00,
	0x5ae0,0xfe,
	0x5ae1,0x40,
	0x5ae2,0x38,
	0x5ae3,0x30,
	0x5ae4,0x0c,
	0x5ae5,0x38,
	0x5ae6,0x30,
	0x5ae7,0x28,
	0x5ae8,0x3f,
	0x5ae9,0x34,
	0x5aea,0x2c,
	0x5aeb,0x3f,
	0x5aec,0x34,
	0x5aed,0x2c,
	0x3652,0x33,//MIPI test
	0x3654,0x01,//MIPI test
};

static void capture_setting(kal_uint16 currefps)
{
LOG_INF("[%s] capture_setting\n", __func__);
    write_cmos_sensor8(0x0103,0x01);
		mDELAY(10);
	sc520cs_table_write_cmos_sensor(avaterb5_sc520cs_capture_setting,
		sizeof(avaterb5_sc520cs_capture_setting) / sizeof(kal_uint16));
	if ((read_cmos_sensor(0x800B)&0xFF)==0)
    {
	 write_cmos_sensor8(0x550F,0x42);
    }else {
	 write_cmos_sensor8(0x550F,0x8a);
    }
LOG_INF("0X800B = 0x%x, 0X550f = 0x%x \n", read_cmos_sensor(0x800B),read_cmos_sensor(0x550F));
}

static void normal_video_setting(void)
{
  sensor_init();
}

static kal_uint16 avaterb5_sc520cs_hs_video_setting[] = {
	0x36e9,0x80,
	0x37f9,0x80,
	0x36ea,0x0e,
	0x36eb,0x0c,
	0x36ec,0x4a,
	0x36ed,0x18,
	0x36e9,0x44,
	0x37f9,0x23,
	0x301f,0x90,
	0x3205,0x27,
	0x3208,0x05,
	0x3209,0x10,
	0x320a,0x03,
	0x320b,0xcc,
	0x3211,0x02,
	0x3213,0x04,
	0x3215,0x31,
	0x3220,0x81,
	0x3301,0x08,
	0x3306,0x60,
	0x3308,0x10,
	0x3309,0x80,
	0x330b,0xd0,
	0x3314,0x15,
	0x331f,0x71,
	0x3333,0x10,
	0x3334,0x40,
	0x3364,0x56,
	0x336c,0xcf,
	0x3390,0x09,
	0x3391,0x0f,
	0x3392,0x1f,
	0x3393,0x0e,
	0x3394,0x15,
	0x3395,0x19,
	0x33ad,0x1c,
	0x33af,0x3f,
	0x33b3,0x30,
	0x33b5,0x90,
	0x349f,0x1e,
	0x34a6,0x09,
	0x34a7,0x0b,
	0x34a8,0x20,
	0x34a9,0x20,
	0x34f8,0x0f,
	0x34f9,0x10,
	0x3630,0x88,
	0x3633,0x34,
	0x3635,0xc0,
	0x3637,0x52,
	0x3638,0xc1,
	0x363a,0x89,
	0x3670,0x4a,
	0x3674,0x48,
	0x3675,0x28,
	0x3676,0x18,
	0x367c,0x0f,
	0x367d,0x1f,
	0x3690,0x43,
	0x3691,0x43,
	0x3692,0x43,
	0x3698,0x89,
	0x3699,0x92,
	0x369a,0xa5,
	0x369b,0xca,
	0x369c,0x0b,
	0x369d,0x0f,
	0x36a2,0x09,
	0x36a3,0x0b,
	0x36a4,0x0f,
	0x36b1,0x38,
	0x36b2,0xc1,
	0x370f,0x01,
	0x3724,0xb1,
	0x3771,0x02,
	0x3772,0x03,
	0x3773,0x03,
	0x377a,0x0b,
	0x377b,0x0f,
	0x3901,0x04,
	0x3905,0x8d,
	0x391d,0x01,
	0x3926,0x23,
	0x3e00,0x00,
	0x3e01,0xf9,
	0x3e02,0x70,
	0x3e09,0x00,
	0x4401,0x13,
	0x4402,0x03,
	0x4403,0x0b,
	0x4404,0x21,
	0x4405,0x29,
	0x4407,0x0c,
	0x440c,0x34,
	0x440d,0x34,
	0x440e,0x29,
	0x440f,0x3f,
	0x4412,0x01,
	0x4424,0x01,
	0x442d,0x00,
	0x442e,0x00,
	0x4509,0x28,
	0x450d,0x19,
	0x451d,0x28,
	0x4526,0x05,
	0x5000,0x4e,
	0x5007,0x00,
	0x550e,0x00,
	0x550f,0x42,
	0x5780,0x66,
	0x5787,0x08,
	0x5788,0x04,
	0x5789,0x01,
	0x578d,0x40,
	0x5790,0x08,
	0x5791,0x04,
	0x5792,0x01,
	0x5799,0x46,
	0x57aa,0xeb,
	0x5901,0x04,
	0x59f0,0x00,
	0x5ae0,0xfe,
	0x5ae1,0x40,
	0x5ae2,0x38,
	0x5ae3,0x30,
	0x5ae4,0x0c,
	0x5ae5,0x38,
	0x5ae6,0x30,
	0x5ae7,0x28,
	0x5ae8,0x3f,
	0x5ae9,0x34,
	0x5aea,0x2c,
	0x5aeb,0x3f,
	0x5aec,0x34,
	0x5aed,0x2c,
	0x3652,0x33,//MIPI test
	0x3654,0x01,//MIPI test
};

static void hs_video_setting(void)
{
	LOG_INF("[%s] hs_video_setting\n", __func__);
    write_cmos_sensor8(0x0103,0x01);
		mDELAY(10);
	sc520cs_table_write_cmos_sensor(avaterb5_sc520cs_hs_video_setting,
		sizeof(avaterb5_sc520cs_hs_video_setting) / sizeof(kal_uint16));
	if ((read_cmos_sensor(0x800B)&0xFF)==0)
    {
	 write_cmos_sensor8(0x550F,0x42);
    }else {
	 write_cmos_sensor8(0x550F,0x8a);
    }
LOG_INF("0X800B = 0x%x, 0X550f = 0x%x \n", read_cmos_sensor(0x800B),read_cmos_sensor(0x550F));
}

static kal_uint16 avaterb5_sc520cs_slim_video_setting[] = {
	0x36e9,0x80,
	0x37f9,0x80,
	0x36ea,0x0e,
	0x36eb,0x0c,
	0x36ec,0x4a,
	0x36ed,0x18,
	0x36e9,0x44,
	0x37f9,0x23,
	0x301f,0x8f,
	0x3200,0x00,
	0x3201,0x00,
	0x3202,0x00,
	0x3203,0xf3,
	0x3204,0x0a,
	0x3205,0x27,
	0x3206,0x06,
	0x3207,0xb4,
	0x3208,0x0a,
	0x3209,0x20,
	0x320a,0x05,
	0x320b,0xb2,
	0x3210,0x00,
	0x3211,0x04,
	0x3212,0x00,
	0x3213,0x08,
	0x3301,0x08,
	0x3306,0x60,
	0x3308,0x10,
	0x3309,0x80,
	0x330b,0xd0,
	0x3314,0x15,
	0x331f,0x71,
	0x3333,0x10,
	0x3334,0x40,
	0x3364,0x56,
	0x336c,0xcf,
	0x3390,0x09,
	0x3391,0x0f,
	0x3392,0x1f,
	0x3393,0x0e,
	0x3394,0x15,
	0x3395,0x19,
	0x33ad,0x1c,
	0x33af,0x3f,
	0x33b3,0x30,
	0x33b5,0x90,
	0x349f,0x1e,
	0x34a6,0x09,
	0x34a7,0x0b,
	0x34a8,0x20,
	0x34a9,0x20,
	0x34f8,0x0f,
	0x34f9,0x10,
	0x3630,0x88,
	0x3633,0x34,
	0x3635,0xc0,
	0x3637,0x52,
	0x3638,0xc1,
	0x363a,0x89,
	0x3670,0x4a,
	0x3674,0x48,
	0x3675,0x28,
	0x3676,0x18,
	0x367c,0x0f,
	0x367d,0x1f,
	0x3690,0x43,
	0x3691,0x43,
	0x3692,0x43,
	0x3698,0x89,
	0x3699,0x92,
	0x369a,0xa5,
	0x369b,0xca,
	0x369c,0x0b,
	0x369d,0x0f,
	0x36a2,0x09,
	0x36a3,0x0b,
	0x36a4,0x0f,
	0x36b1,0x38,
	0x36b2,0xc1,
	0x370f,0x01,
	0x3724,0xb1,
	0x3771,0x02,
	0x3772,0x03,
	0x3773,0x03,
	0x377a,0x0b,
	0x377b,0x0f,
	0x3901,0x04,
	0x3905,0x8d,
	0x391d,0x01,
	0x3926,0x23,
	0x3e00,0x00,
	0x3e01,0xf9,
	0x3e02,0x70,
	0x3e09,0x00,
	0x4401,0x13,
	0x4402,0x03,
	0x4403,0x0b,
	0x4404,0x21,
	0x4405,0x29,
	0x4407,0x0c,
	0x440c,0x34,
	0x440d,0x34,
	0x440e,0x29,
	0x440f,0x3f,
	0x4412,0x01,
	0x4424,0x01,
	0x442d,0x00,
	0x442e,0x00,
	0x4509,0x28,
	0x450d,0x19,
	0x451d,0x28,
	0x4526,0x05,
	0x5000,0x0e,
	0x5007,0x00,
	0x550e,0x00,
	0x550f,0x8a,
	0x5780,0x66,
	0x5787,0x08,
	0x5788,0x04,
	0x5789,0x01,
	0x578d,0x40,
	0x5790,0x08,
	0x5791,0x04,
	0x5792,0x01,
	0x5799,0x46,
	0x57aa,0xeb,
	0x59f0,0x00,
	0x5ae0,0xfe,
	0x5ae1,0x40,
	0x5ae2,0x38,
	0x5ae3,0x30,
	0x5ae4,0x0c,
	0x5ae5,0x38,
	0x5ae6,0x30,
	0x5ae7,0x28,
	0x5ae8,0x3f,
	0x5ae9,0x34,
	0x5aea,0x2c,
	0x5aeb,0x3f,
	0x5aec,0x34,
	0x5aed,0x2c,
	0x3652,0x33,//MIPI test
	0x3654,0x01,//MIPI test
};

static void slim_video_setting(void)
{
	LOG_INF("[%s] slim_video_setting\n", __func__);
    write_cmos_sensor8(0x0103,0x01);
	mDELAY(10);
	sc520cs_table_write_cmos_sensor(avaterb5_sc520cs_slim_video_setting,
		sizeof(avaterb5_sc520cs_slim_video_setting) / sizeof(kal_uint16));
	if ((read_cmos_sensor(0x800B)&0xFF)==0)
    {
	 write_cmos_sensor8(0x550F,0x42);
    }else {
	 write_cmos_sensor8(0x550F,0x8a);
	 LOG_INF("0X800B = 0x%x, 0X550f = 0x%x \n", read_cmos_sensor(0x800B),read_cmos_sensor(0x550F));
    }
}

static void custom1_setting(void)
{
  sensor_init();
}
/* This sensor have two groups otp data in diffrent page in sensor */

int sc520cs_read_data(u16 addr, u8 *data);

OTP_MAP sc520cs_otp_map_avatar5 = {
		.module_name = "SC520CS_AVATAR5",
		.group_num = 3,
		.group_addr_info_num = 2,
		.readFunc = sc520cs_read_data,
		.group_info = {
						{"awb",
							{
								{0x80B7, 0x80BE, 0x80B6, 0x80BF},
								{0x888E, 0x8895, 0x80B6, 0x8896},
							},
						},
						{"lsc",
							{
								{0x80C0, 0x8871, 0x80B6, 0x8872},
								{0x8897, 0x8FE2, 0x80B6, 0x8FE3},
							},
						},
						{"sn",
							{
								{0x8873, 0x888C, 0x80B6, 0x888D},
								{0x8FE4, 0x8FFD, 0x80B6, 0x8FFE},
							},
						},
			},
};

static int read_reg16_data8(u16 addr, u8 *data)
{
    char pusendcmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };
    return iReadRegI2C(pusendcmd, 2, (u8 *)data, 1, SC520CS_EEPROM_I2C_ADDR);
}

static int write_reg16_data8(u16 addr, u8 data)
{
	char pu_send_cmd[4] = {(char)(addr >> 8), (char)(addr & 0xFF),
                           (char)(data & 0xFF)};
	return iWriteRegI2C(pu_send_cmd, 3, SC520CS_EEPROM_I2C_ADDR);
}

int sc520cs_read_data(u16 addr, u8 *data)
{
	return read_reg16_data8(addr, data);
}

static int sc520cs_read_table(kal_uint16 addr, kal_uint8 *table,
                              kal_uint32 size)
{
    char pusendcmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };
    return iReadRegI2C(pusendcmd, 2, (u8 *)table, size,
                       SC520CS_EEPROM_I2C_ADDR);
}

void sc520cs_sensor_otp_init(int section, kal_uint8 threshold)
{
	/* 0x4412, 0xxx, read 0x8000-0x87FF data  0x4412,0x01
	 * read 0x8800-0x8FFF, 0x4412,0x03
	 * 0x4407, 0xxx, read 0x8000-0x87FF       0x4407,0x0c
	 * read 0x8800-0x8FFF, 0x4407,0x00
	*/
	write_reg16_data8(0x36b0, 0x4c);
	write_reg16_data8(0x36b1, threshold);	/* set threshold */
	write_reg16_data8(0x36b2, 0xc1);
	if (section == 1) {
		/* High byte start address for OTP load Group1 */
		write_reg16_data8(0x4408, (0x8000 >> 8)&0xff);
		/* Low byte start address for OTP load  Group1 */
		write_reg16_data8(0x4409, (0x8000)&0xff);
		/* High byte end address for OTP load Group1 */
		write_reg16_data8(0x440a, (0x87FF >> 8)&0xff);
		/* Low byte end address for OTP load Group1 */
		write_reg16_data8(0x440b, (0x87FF)&0xff);
		write_reg16_data8(0x4401, 0x13);
		write_reg16_data8(0x4412, 0x01);
		write_reg16_data8(0x4407, 0x0c);
		/* 0x4400, 0x11 manual load */
		write_reg16_data8(0x4400, 0x11);
		LOG_INFO("load section 1 \n");
	} else if (section == 2) {
		/* High byte start address for OTP load Group1 */
		write_reg16_data8(0x4408, (0x8800 >> 8)&0xff);
		/* Low byte start address for OTP load  Group1 */
		write_reg16_data8(0x4409, (0x8800)&0xff);
		/* High byte end address for OTP load Group1 */
		write_reg16_data8(0x440a, (0x8FFF >> 8)&0xff);
		/* Low byte end address for OTP load Group1 */
		write_reg16_data8(0x440b, (0x8FFF)&0xff);
		write_reg16_data8(0x4401, 0x13);
		write_reg16_data8(0x4412, 0x03);
		write_reg16_data8(0x4407, 0x00);
		 /* 0x4400, 0x11 manual load */
		write_reg16_data8(0x4400, 0x11);
		LOG_INFO("load section 2 \n");
	} else {
		LOG_INFO("section input error \n");
	}
    /* wait page load finished */
	mdelay(30);
}

static int parse_otp_map_data_sc520cs(OTP_MAP * map, char * data, int temp)
{
	int i = 0;
	int addr = 0, size = 0, curr_addr = 0, read_size = 0;
	int ret = 0;
	char group_flag = 0;
	int checksum = -1;
	int group_layer = 0;  /*0:group1, 1:group2*/

	LOG_INFO("module: %s ......", map->module_name);

	ret = map->readFunc(0x80B6, &group_flag);
	LOG_INFO("parse_otp_map_data_sc520cs 0x%x", ret);
	if (ret < 0) {
		LOG_ERR("read group flag error addr 0x%04x", 0x80B6);
		return -ERROR_I2C;
	}
	LOG_INFO("readFunc: flag_addr = 0x%04x, value = 0x%02x\n", 0x80B6,
             group_flag);
	if ((group_flag & 0x3) == 0x01) {
		group_layer = 0;
		sc520cs_sensor_otp_init(1, temp);
		LOG_INFO("group 1 selected\n");
	} else if ((group_flag & 0xc) == 0x04) {
		group_layer = 1;
        sc520cs_sensor_otp_init(2, temp);
		LOG_INFO("group 2 selected\n");
	} else {
		LOG_ERR("group selected error!!!\n");
		return -ERROR_READ_FLAG;
	}

	for (i = 0; i < map->group_num; i++) {
		checksum = 0;
		size = 0;
		LOG_INFO("%d groupinfo: %s, start_addr 0x%0x(0x%0x)", group_layer,
                 map->group_info[i].group_name, curr_addr, curr_addr);
		if (!strcmp(map->group_info[i].group_name, "lsc") &&
            (group_layer == 0)) {
			/* There is need cross-page stitching to read, go here */
			addr =
               map->group_info[i].group_addr_info[group_layer].group_start_addr;
			read_size = 0x87FF -
            map->group_info[i].group_addr_info[group_layer].group_start_addr+1;
			/*read lsc group 2 part 2 data*/
			ret = sc520cs_read_table(addr, data, read_size);
			LOG_INFO("Get SC520CS OTP Data of: %s, addr: 0x%04x, data: 0x%04x",
                     map->group_info[i].group_name, addr, data);
			if (ret < 0) {
				LOG_ERR("read data error");
			}
			for (addr =
            map->group_info[i].group_addr_info[group_layer].group_start_addr;
            addr <=0x87FF; addr++) {
				checksum += *data;
				curr_addr++;
				size++;
				data++;
				}
            /*load page 2 for lsc part one data */
			sc520cs_sensor_otp_init(2, temp);
			addr = 0x8866;
			read_size =
            map->group_info[i].group_addr_info[group_layer].group_end_addr -
            0x8866 + 1;
			/*read lsc group 2 part 1 data*/
			ret = sc520cs_read_table(addr, data, read_size);
			LOG_INFO("Get SC520CS OTP Data of: %s, addr: 0x%04x, data: 0x%04x",
                     map->group_info[i].group_name, addr, data);
			if (ret < 0) {
				LOG_ERR("read data error");
			}
			for ( addr = 0x8866; addr <=
                 map->group_info[i].group_addr_info[group_layer].group_end_addr;
                 addr++) {
				checksum += *data;
				curr_addr++;
				size++;
				data++;
				}

		} else {    /*There is no cross-page stitching to read, go here*/
			addr =
            map->group_info[i].group_addr_info[group_layer].group_start_addr;
			read_size =
            map->group_info[i].group_addr_info[group_layer].group_end_addr -
            map->group_info[i].group_addr_info[group_layer].group_start_addr+1;
			/*read lsc group 2 part 1 data*/
			ret = sc520cs_read_table(addr, data, read_size);
			LOG_INFO("Get SC520CS OTP Data of: %s, addr: 0x%04x, data: 0x%04x",
                     map->group_info[i].group_name, addr, data);
			if (ret < 0) {
				LOG_ERR("read data error");
			}
			for ( addr =
            map->group_info[i].group_addr_info[group_layer].group_start_addr;
            addr <=
            map->group_info[i].group_addr_info[group_layer].group_end_addr;
            addr++) {
				checksum += *data;
				curr_addr++;
				size++;
				data++;
				}
		}

		/* add flag to let the format compatible with other sensors,
         * 0x01 means valid. */
		*data = 0x01;
		data++;
		checksum = checksum % 0xFF;
		ret = map->readFunc(
            map->group_info[i].group_addr_info[group_layer].group_checksum_addr,
            data);
		if (checksum == *data) {
			curr_addr++;
			size++;
			data++;
			LOG_INFO("groupinfo: %s, checksum OK c(%04d) r(%04d), "
                     "viraddr: 0x%04x(%04d)", map->group_info[i].group_name,
                     checksum, *data, *data);
		} else {
			LOG_ERR("groupinfo: %s, checksum ERROR ret=%d, checksum=%04d "
                    "data=%04d", map->group_info[i].group_name, ret, checksum,
                    *data);
			return -ERROR_CHECKSUM;
		}
		LOG_INFO("groupinfo: %s, end_addr 0x%04x(%04d) size 0x%04x(%04d)",
				 map->group_info[i].group_name, curr_addr-1, curr_addr-1, size,
                 size);
	}
	return ret;
}

static unsigned int sc520cs_read_region(unsigned int addr, unsigned char *data,
                                        unsigned int size)
{
	int ret = 0;
	int i;
	int otpThreshold[3] = {0x38, 0x28, 0x48}; /* the Threshold for sc520cs*/
	int tempotpthreshold =  0;
	if (g_otp_buf[IMGSENSOR_SENSOR_IDX_SUB][READ_FLAG_ADDR]) {
		LOG_INFO("read otp data from g_otp_buf:addr 0x%x, size %d", addr, size);
		memcpy((void *)data, &g_otp_buf[IMGSENSOR_SENSOR_IDX_SUB][addr], size);
		return size;
	}

	for (i = 0; i < OTP_THREAHOLD; i++) {
		tempotpthreshold = otpThreshold[i];
		LOG_INFO("Get SC520CS OTP data used Threshold: 0x%x", tempotpthreshold);
        /*we need reload the data page before read anther page data.*/
		sc520cs_sensor_otp_init(1, tempotpthreshold);
		ret = parse_otp_map_data_sc520cs(&sc520cs_otp_map_avatar5,
            &g_otp_buf[IMGSENSOR_SENSOR_IDX_SUB][0], tempotpthreshold);
		if(!ret)
			break;
	}
	if (!ret) {
		g_otp_buf[IMGSENSOR_SENSOR_IDX_SUB][CHECKSUM_FLAG_ADDR] =
            OTP_DATA_GOOD_FLAG;
		g_otp_buf[IMGSENSOR_SENSOR_IDX_SUB][READ_FLAG_ADDR] = 1;
	}

	if (NULL != data) {
		memcpy((void *)data, &g_otp_buf[IMGSENSOR_SENSOR_IDX_SUB][addr], size);
	}
	return ret;
}

static void read_sc520cs_module_data()
{
    // length   : 64
    // QR       : [8 ~ 33]
    // vcm      : [40 ~ 41]
    // lensid   : [44 ~ 45]
    // marcro   : [48 ~ 49]
    // inf      : [52 ~ 53]

    kal_uint16 idx = 0;
    kal_uint16 sn_length = 26;
	// offset from otp data address 0.
    kal_uint32 sn_starAddr = 0x758;

    memset(sc520cs_common_data, 0, sizeof(sc520cs_common_data));
	// QR
	sc520cs_read_region (sn_starAddr, &sc520cs_common_data[8], sn_length);
    // Vcm ID, 0xFF means not available.
    sc520cs_common_data[40] = 0xFF;
    // Lens ID, 0xFF means not available.
    sc520cs_common_data[44] = 0xFF;
    // Macro, 0xFF means not available.
    sc520cs_common_data[48] = 0xFF;
    // Inf, 0xFF means not available.
    sc520cs_common_data[52] = 0xFF;

    for (idx = 0; idx < CAMERA_EEPPROM_COMDATA_LENGTH; idx = idx + 4)
    {
        pr_info("sc520cs common cam data : %02x %02x %02x %02x\n",
           sc520cs_common_data[idx],
           sc520cs_common_data[idx + 1],
           sc520cs_common_data[idx + 2],
           sc520cs_common_data[idx + 3]);
    }
}

char sub_camera_info[20] = {0};
EXPORT_SYMBOL(sub_camera_info);
static kal_uint32 get_imgsensor_id(UINT32 *sensor_id)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
	LOG_INF("[get_imgsensor_id] ");
	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
			spin_lock(&imgsensor_drv_lock);
			imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
			spin_unlock(&imgsensor_drv_lock);
			do {
				*sensor_id =return_sensor_id();
				sprintf(sub_camera_info, "sc520cs");

				if (*sensor_id == imgsensor_info.sensor_id) {
					LOG_INF("i2c write id  : 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,*sensor_id);

					if(deviceInfo_register_value == 0x00) {
						register_imgsensor_deviceinfo("Cam_f", DEVICE_VERSION_SC520CS_AVATARB5, imgsensor_info.module_id);
						read_sc520cs_module_data ();
						deviceInfo_register_value = 0x01;
					}

					if (otp_data[0] == 0) {
						sc520cs_read_region(0, &otp_data[0], MAX_EEPROM_BYTE);
					} else {
						pr_info("otp data has already read");
					}
					return ERROR_NONE;
				}
				LOG_INF("get_imgsensor_id Read sensor id fail, i2c write id: 0x%x,sensor id: 0x%x\n", imgsensor.i2c_write_id,*sensor_id);
				retry--;
			} while(retry > 0);
			i++;
			retry = 2;
	}
	if (*sensor_id != imgsensor_info.sensor_id) {
		*sensor_id = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	return ERROR_NONE;
}

/*************************************************************************
 * FUNCTION
 *	open
 *
 * DESCRIPTION
 *	This function initialize the registers of CMOS sensor
 *
 * PARAMETERS
 *	None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static kal_uint32 open(void)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
	kal_uint16 sensor_id = 0;
	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
			sensor_id = return_sensor_id();
			if (sensor_id == imgsensor_info.sensor_id) {
				LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n",
					imgsensor.i2c_write_id, sensor_id);
				break;
			}

			retry--;
		} while (retry > 0);
		i++;
		if (sensor_id == imgsensor_info.sensor_id)
			break;
		retry = 2;
	}
	if (imgsensor_info.sensor_id != sensor_id) {
		LOG_INF("open sensor id fail: 0x%x\n", sensor_id);

		return ERROR_SENSOR_CONNECT_FAIL;
	}
	/* initail sequence write in  */
	sensor_init();

	spin_lock(&imgsensor_drv_lock);
	imgsensor.autoflicker_en = KAL_FALSE;
	imgsensor.sensor_mode = IMGSENSOR_MODE_INIT;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.dummy_pixel = 0;
	imgsensor.dummy_line = 0;
	imgsensor.ihdr_en = 0;
	imgsensor.test_pattern = KAL_FALSE;
	imgsensor.current_fps = imgsensor_info.pre.max_framerate;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}	/*	open  */
static kal_uint32 close(void)
{
	return ERROR_NONE;
}	/*	close  */


/*************************************************************************
 * FUNCTION
 * preview
 *
 * DESCRIPTION
 *	This function start the sensor preview.
 *
 * PARAMETERS
 *	*image_window : address pointer of pixel numbers in one period of HSYNC
 *  *sensor_config_data : address pointer of line numbers in one period of VSYNC
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static kal_uint32 preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E");
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_PREVIEW;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.current_fps = imgsensor_info.pre.max_framerate;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	preview_setting();
	return ERROR_NONE;
}	/*	preview   */

/*************************************************************************
 * FUNCTION
 *	capture
 *
 * DESCRIPTION
 *	This function setup the CMOS sensor in capture MY_OUTPUT mode
 *
 * PARAMETERS
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static kal_uint32 capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CAPTURE;

	if (imgsensor.current_fps == imgsensor_info.cap.max_framerate)	{
		imgsensor.pclk = imgsensor_info.cap.pclk;
		imgsensor.line_length = imgsensor_info.cap.linelength;
		imgsensor.frame_length = imgsensor_info.cap.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap.framelength;
		imgsensor.autoflicker_en = KAL_FALSE;
	} else {
	 //PIP capture: 24fps for less than 13M, 20fps for 16M,15fps for 20M
		imgsensor.pclk = imgsensor_info.cap1.pclk;
		imgsensor.line_length = imgsensor_info.cap1.linelength;
		imgsensor.frame_length = imgsensor_info.cap1.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap1.framelength;
		imgsensor.autoflicker_en = KAL_FALSE;
	}

	spin_unlock(&imgsensor_drv_lock);
	LOG_INF("Caputre fps:%d\n", imgsensor.current_fps);
	capture_setting(imgsensor.current_fps);

	return ERROR_NONE;

}	/* capture() */
static kal_uint32 normal_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_VIDEO;
	imgsensor.pclk = imgsensor_info.normal_video.pclk;
	imgsensor.line_length = imgsensor_info.normal_video.linelength;
	imgsensor.frame_length = imgsensor_info.normal_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.normal_video.framelength;
	imgsensor.current_fps = 300;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	normal_video_setting();
	return ERROR_NONE;
}	/*	normal_video   */

static kal_uint32 hs_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_HIGH_SPEED_VIDEO;
	imgsensor.pclk = imgsensor_info.hs_video.pclk;
	//imgsensor.video_mode = KAL_TRUE;
	imgsensor.line_length = imgsensor_info.hs_video.linelength;
	imgsensor.frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	hs_video_setting();

	return ERROR_NONE;
}    /*    hs_video   */

static kal_uint32 slim_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
		      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_SLIM_VIDEO;
	imgsensor.pclk = imgsensor_info.slim_video.pclk;
	imgsensor.line_length = imgsensor_info.slim_video.linelength;
	imgsensor.frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	slim_video_setting();

	return ERROR_NONE;
}    /*    slim_video     */

static kal_uint32 custom1(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
		      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM1;
	imgsensor.pclk = imgsensor_info.custom1.pclk;
	imgsensor.line_length = imgsensor_info.custom1.linelength;
	imgsensor.frame_length = imgsensor_info.custom1.framelength;
	imgsensor.min_frame_length = imgsensor_info.custom1.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	custom1_setting();

	return ERROR_NONE;
}    /*    custom1     */

static kal_uint32 get_resolution(
		MSDK_SENSOR_RESOLUTION_INFO_STRUCT * sensor_resolution)
{
	sensor_resolution->SensorFullWidth =
		imgsensor_info.cap.grabwindow_width;
	sensor_resolution->SensorFullHeight =
		imgsensor_info.cap.grabwindow_height;

	sensor_resolution->SensorPreviewWidth =
		imgsensor_info.pre.grabwindow_width;
	sensor_resolution->SensorPreviewHeight =
		imgsensor_info.pre.grabwindow_height;

	sensor_resolution->SensorVideoWidth =
		imgsensor_info.normal_video.grabwindow_width;
	sensor_resolution->SensorVideoHeight =
		imgsensor_info.normal_video.grabwindow_height;


	sensor_resolution->SensorHighSpeedVideoWidth =
		imgsensor_info.hs_video.grabwindow_width;
	sensor_resolution->SensorHighSpeedVideoHeight =
		imgsensor_info.hs_video.grabwindow_height;

	sensor_resolution->SensorSlimVideoWidth =
		imgsensor_info.slim_video.grabwindow_width;
	sensor_resolution->SensorSlimVideoHeight =
		imgsensor_info.slim_video.grabwindow_height;
		sensor_resolution->SensorCustom1Width =
		imgsensor_info.custom1.grabwindow_width;
	sensor_resolution->SensorCustom1Height =
		imgsensor_info.custom1.grabwindow_height;

	return ERROR_NONE;
}    /*    get_resolution    */


static kal_uint32 get_info(enum MSDK_SCENARIO_ID_ENUM scenario_id,
			MSDK_SENSOR_INFO_STRUCT *sensor_info,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("scenario_id = %d\n", scenario_id);

	sensor_info->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorInterruptDelayLines = 4; /* not use */
	sensor_info->SensorResetActiveHigh = FALSE; /* not use */
	sensor_info->SensorResetDelayCount = 5; /* not use */

	sensor_info->SensroInterfaceType =
	imgsensor_info.sensor_interface_type;
	sensor_info->MIPIsensorType = imgsensor_info.mipi_sensor_type;
	sensor_info->SettleDelayMode = imgsensor_info.mipi_settle_delay_mode;
	sensor_info->SensorOutputDataFormat =
		imgsensor_info.sensor_output_dataformat;

	sensor_info->CaptureDelayFrame = imgsensor_info.cap_delay_frame;
	sensor_info->PreviewDelayFrame = imgsensor_info.pre_delay_frame;
	sensor_info->VideoDelayFrame =
		imgsensor_info.video_delay_frame;
	sensor_info->HighSpeedVideoDelayFrame =
		imgsensor_info.hs_video_delay_frame;
	sensor_info->SlimVideoDelayFrame =
		imgsensor_info.slim_video_delay_frame;
	sensor_info->Custom1DelayFrame = imgsensor_info.custom1_delay_frame;

	sensor_info->SensorMasterClockSwitch = 0; /* not use */
	sensor_info->SensorDrivingCurrent =
		imgsensor_info.isp_driving_current;
/* The frame of setting shutter default 0 for TG int */
	sensor_info->AEShutDelayFrame =
		imgsensor_info.ae_shut_delay_frame;
/* The frame of setting sensor gain */
	sensor_info->AESensorGainDelayFrame =
		imgsensor_info.ae_sensor_gain_delay_frame;
	sensor_info->AEISPGainDelayFrame =
		imgsensor_info.ae_ispGain_delay_frame;
	sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
	sensor_info->IHDR_LE_FirstLine =
		imgsensor_info.ihdr_le_firstline;
	sensor_info->SensorModeNum =
		imgsensor_info.sensor_mode_num;

	sensor_info->SensorMIPILaneNumber =
		imgsensor_info.mipi_lane_num;
	sensor_info->SensorClockFreq = imgsensor_info.mclk;
	sensor_info->SensorClockDividCount = 3; /* not use */
	sensor_info->SensorClockRisingCount = 0;
	sensor_info->SensorClockFallingCount = 2; /* not use */
	sensor_info->SensorPixelClockCount = 3; /* not use */
	sensor_info->SensorDataLatchCount = 2; /* not use */

	sensor_info->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->SensorWidthSampling = 0;  // 0 is default 1x
	sensor_info->SensorHightSampling = 0;    // 0 is default 1x
	sensor_info->SensorPacketECCOrder = 1;

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
	    sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
				imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
	break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	    sensor_info->SensorGrabStartX = imgsensor_info.cap.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.cap.starty;

	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.cap.mipi_data_lp2hs_settle_dc;
	break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
	    sensor_info->SensorGrabStartX = imgsensor_info.normal_video.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.normal_video.starty;

	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.normal_video.mipi_data_lp2hs_settle_dc;
	break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
	    sensor_info->SensorGrabStartX = imgsensor_info.hs_video.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.hs_video.starty;
	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.hs_video.mipi_data_lp2hs_settle_dc;
	break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
	    sensor_info->SensorGrabStartX = imgsensor_info.slim_video.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.slim_video.starty;
	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.slim_video.mipi_data_lp2hs_settle_dc;
	break;
	case MSDK_SCENARIO_ID_CUSTOM1:
		sensor_info->SensorGrabStartX = imgsensor_info.custom1.startx;
		sensor_info->SensorGrabStartY = imgsensor_info.custom1.starty;
		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.custom1.mipi_data_lp2hs_settle_dc;
	break;
	default:
	    sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
	break;
	}

	return ERROR_NONE;
}    /*    get_info  */


static kal_uint32 control(enum MSDK_SCENARIO_ID_ENUM scenario_id,
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("scenario_id = %d\n", scenario_id);
	spin_lock(&imgsensor_drv_lock);
	imgsensor.current_scenario_id = scenario_id;
	spin_unlock(&imgsensor_drv_lock);
	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		LOG_INF("preview\n");
		preview(image_window, sensor_config_data);
		break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	case MSDK_SCENARIO_ID_CAMERA_ZSD:
		capture(image_window, sensor_config_data);
		break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		normal_video(image_window, sensor_config_data);
		break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
	hs_video(image_window, sensor_config_data);
		break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
	    slim_video(image_window, sensor_config_data);
		break;
	case MSDK_SCENARIO_ID_CUSTOM1:
		custom1(image_window, sensor_config_data);
	    break;
	default:
		preview(image_window, sensor_config_data);
		return ERROR_INVALID_SCENARIO_ID;
	}
	return ERROR_NONE;
}	/* control() */



static kal_uint32 set_video_mode(UINT16 framerate)
{
	LOG_INF("framerate = %d ", framerate);
	// SetVideoMode Function should fix framerate
	if (framerate == 0)
		// Dynamic frame rate
		return ERROR_NONE;
	spin_lock(&imgsensor_drv_lock);

	if ((framerate == 30) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 296;
	else if ((framerate == 15) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 146;
	else
		imgsensor.current_fps = 10 * framerate;
	spin_unlock(&imgsensor_drv_lock);
	set_max_framerate(imgsensor.current_fps, 1);
	set_dummy();
	return ERROR_NONE;
}


static kal_uint32 set_auto_flicker_mode(kal_bool enable,
			UINT16 framerate)
{
	LOG_INF("enable = %d, framerate = %d ", enable, framerate);
	spin_lock(&imgsensor_drv_lock);
	if (enable)
		imgsensor.autoflicker_en = KAL_TRUE;
	else //Cancel Auto flick
		imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}


static kal_uint32 set_max_framerate_by_scenario(
			enum MSDK_SCENARIO_ID_ENUM scenario_id,
			MUINT32 framerate)
{
	kal_uint32 frame_length;

	LOG_INF("scenario_id = %d, framerate = %d\n",
				scenario_id, framerate);

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
	    frame_length = imgsensor_info.pre.pclk / framerate * 10 /
			imgsensor_info.pre.linelength;
	    spin_lock(&imgsensor_drv_lock);
	    imgsensor.dummy_line = (frame_length >
			imgsensor_info.pre.framelength) ?
			(frame_length - imgsensor_info.pre.framelength) : 0;
	    imgsensor.frame_length = imgsensor_info.pre.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		if (framerate == 0)
			return ERROR_NONE;
	    frame_length = imgsensor_info.normal_video.pclk /
			framerate * 10 / imgsensor_info.normal_video.linelength;
	    spin_lock(&imgsensor_drv_lock);
	    imgsensor.dummy_line = (frame_length >
			imgsensor_info.normal_video.framelength) ?
		(frame_length - imgsensor_info.normal_video.framelength) : 0;
	    imgsensor.frame_length = imgsensor_info.normal_video.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		if (imgsensor.current_fps ==
				imgsensor_info.cap1.max_framerate) {
		frame_length = imgsensor_info.cap1.pclk / framerate * 10 /
				imgsensor_info.cap1.linelength;
		spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line = (frame_length >
			imgsensor_info.cap1.framelength) ?
			(frame_length - imgsensor_info.cap1.framelength) : 0;
		imgsensor.frame_length = imgsensor_info.cap1.framelength +
				imgsensor.dummy_line;
		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		} else {
			if (imgsensor.current_fps !=
				imgsensor_info.cap.max_framerate)
			LOG_INF("fps %d fps not support,use cap: %d fps!\n",
			framerate, imgsensor_info.cap.max_framerate/10);
			frame_length = imgsensor_info.cap.pclk /
				framerate * 10 / imgsensor_info.cap.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length >
				imgsensor_info.cap.framelength) ?
			(frame_length - imgsensor_info.cap.framelength) : 0;
			imgsensor.frame_length =
				imgsensor_info.cap.framelength +
				imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
		}
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
	    frame_length = imgsensor_info.hs_video.pclk /
			framerate * 10 / imgsensor_info.hs_video.linelength;
	    spin_lock(&imgsensor_drv_lock);
	    imgsensor.dummy_line = (frame_length >
			imgsensor_info.hs_video.framelength) ? (frame_length -
			imgsensor_info.hs_video.framelength) : 0;
	    imgsensor.frame_length = imgsensor_info.hs_video.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
	    frame_length = imgsensor_info.slim_video.pclk /
			framerate * 10 / imgsensor_info.slim_video.linelength;
	    spin_lock(&imgsensor_drv_lock);
	    imgsensor.dummy_line = (frame_length >
			imgsensor_info.slim_video.framelength) ? (frame_length -
			imgsensor_info.slim_video.framelength) : 0;
	    imgsensor.frame_length =
			imgsensor_info.slim_video.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_CUSTOM1:
		 frame_length = imgsensor_info.custom1.pclk /
			framerate * 10 / imgsensor_info.custom1.linelength;
	    spin_lock(&imgsensor_drv_lock);
	    imgsensor.dummy_line = (frame_length >
			imgsensor_info.custom1.framelength) ? (frame_length -
			imgsensor_info.custom1.framelength) : 0;
	    imgsensor.frame_length =
			imgsensor_info.custom1.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	default:  //coding with  preview scenario by default
	    frame_length = imgsensor_info.pre.pclk / framerate * 10 /
						imgsensor_info.pre.linelength;
	    spin_lock(&imgsensor_drv_lock);
	    imgsensor.dummy_line = (frame_length >
			imgsensor_info.pre.framelength) ?
			(frame_length - imgsensor_info.pre.framelength) : 0;
	    imgsensor.frame_length = imgsensor_info.pre.framelength +
				imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	    LOG_INF("error scenario_id = %d, we use preview scenario\n",
				scenario_id);
	break;
	}
	return ERROR_NONE;
}


static kal_uint32 get_default_framerate_by_scenario(
				enum MSDK_SCENARIO_ID_ENUM scenario_id,
				MUINT32 *framerate)
{
	LOG_INF("scenario_id = %d\n", scenario_id);

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
	    *framerate = imgsensor_info.pre.max_framerate;
	break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
	    *framerate = imgsensor_info.normal_video.max_framerate;
	break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	    *framerate = imgsensor_info.cap.max_framerate;
	break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
	    *framerate = imgsensor_info.hs_video.max_framerate;
	break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
	    *framerate = imgsensor_info.slim_video.max_framerate;
	break;
	case MSDK_SCENARIO_ID_CUSTOM1:
		*framerate = imgsensor_info.custom1.max_framerate;
	break;
	default:
	break;
	}

	return ERROR_NONE;
}

static kal_uint32 set_test_pattern_mode(kal_bool enable)
{
	LOG_INF("set_test_pattern_mode enable: %d", enable);

	if (enable) {
		write_cmos_sensor8(0x3337,0x30);
		write_cmos_sensor8(0x391d,0xc8);
		write_cmos_sensor8(0x3e06,0x00);
		write_cmos_sensor8(0x3e07,0x80);
		write_cmos_sensor8(0x3e09,0x00);
	} else {
		write_cmos_sensor8(0x3337,0x00);
		write_cmos_sensor8(0x391d,0xc9);

	}
	spin_lock(&imgsensor_drv_lock);
	imgsensor.test_pattern = enable;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}

static kal_uint32 streaming_control(kal_bool enable)
{
	LOG_INF("streaming_enable(0=Sw Standby,1=streaming): %d\n", enable);

	if (enable) {
		write_cmos_sensor8(0x0100, 0x01); // stream on
	} else {
		write_cmos_sensor8(0x0100, 0x00); // stream off
	}

	mdelay(10);
	return ERROR_NONE;
}


static kal_uint32 feature_control(
			MSDK_SENSOR_FEATURE_ENUM feature_id,
			UINT8 *feature_para, UINT32 *feature_para_len)
{
	UINT16 *feature_return_para_16 = (UINT16 *) feature_para;
	UINT16 *feature_data_16 = (UINT16 *) feature_para;
	UINT32 *feature_return_para_32 = (UINT32 *) feature_para;
	UINT32 *feature_data_32 = (UINT32 *) feature_para;
	INT32 *feature_return_para_i32 = (INT32 *) feature_para;
	unsigned long long *feature_data =
		(unsigned long long *) feature_para;

	struct SENSOR_WINSIZE_INFO_STRUCT *wininfo;
	MSDK_SENSOR_REG_INFO_STRUCT *sensor_reg_data =
		(MSDK_SENSOR_REG_INFO_STRUCT *) feature_para;

	LOG_INF("feature_id = %d\n", feature_id);
	switch (feature_id) {
	case SENSOR_FEATURE_GET_PERIOD:
	    *feature_return_para_16++ = imgsensor.line_length;
	    *feature_return_para_16 = imgsensor.frame_length;
	    *feature_para_len = 4;
	break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
	    *feature_return_para_32 = imgsensor.pclk;
	    *feature_para_len = 4;
	break;
	case SENSOR_FEATURE_GET_GAIN_RANGE_BY_SCENARIO:
		*(feature_data + 1) = imgsensor_info.min_gain;
		*(feature_data + 2) = imgsensor_info.max_gain;
		break;
	case SENSOR_FEATURE_GET_BASE_GAIN_ISO_AND_STEP:
		*(feature_data + 0) = imgsensor_info.min_gain_iso;
		*(feature_data + 1) = imgsensor_info.gain_step;
		*(feature_data + 2) = imgsensor_info.gain_type;
		break;
	case SENSOR_FEATURE_GET_MIN_SHUTTER_BY_SCENARIO:
		*(feature_data + 1) = imgsensor_info.min_shutter;
		*(feature_data + 2) = imgsensor_info.exp_step;
		break;
	case SENSOR_FEATURE_GET_BINNING_TYPE:
		switch (*(feature_data + 1)) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*feature_return_para_32 = 1; /*BINNING_NONE*/
			break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			if (*(feature_data + 2))/* HDR on */
				*feature_return_para_32 = 1;/*BINNING_NONE*/
			else
				*feature_return_para_32 = 2;/*BINNING_AVERAGED*/
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
		default:
			*feature_return_para_32 = 2; /*BINNING_AVERAGED*/
			break;
		}
		LOG_INF("SENSOR_FEATURE_GET_BINNING_TYPE AE_binning_type:%d\n",
			*feature_return_para_32);

		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_GET_EEPROM_COMDATA:
		memcpy(feature_return_para_32, sc520cs_common_data, CAMERA_EEPPROM_COMDATA_LENGTH);
		*feature_para_len = CAMERA_EEPPROM_COMDATA_LENGTH;
		break;
	case SENSOR_FEATURE_GET_FRAME_CTRL_INFO_BY_SCENARIO:
		/*
		* 1, if driver support new sw frame sync
		* set_shutter_frame_length() support third para auto_extend_en
		*/
		*(feature_data + 1) = 1;
		/* margin info by scenario */
		*(feature_data + 2) = imgsensor_info.margin;
		break;
	case SENSOR_FEATURE_SET_ESHUTTER:
	    set_shutter(*feature_data);
	break;
	case SENSOR_FEATURE_SET_NIGHTMODE:
	    /*night_mode((BOOL) * feature_data);*/
	break;
	case SENSOR_FEATURE_SET_GAIN:
	    set_gain((UINT16) *feature_data);
	break;
	case SENSOR_FEATURE_SET_FLASHLIGHT:
	break;
	case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
	break;
	case SENSOR_FEATURE_SET_REGISTER:
	    write_cmos_sensor(sensor_reg_data->RegAddr,
						sensor_reg_data->RegData);
	break;
	case SENSOR_FEATURE_GET_REGISTER:
	    sensor_reg_data->RegData =
				read_cmos_sensor(sensor_reg_data->RegAddr);
	break;
	case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
	    *feature_return_para_32 = LENS_DRIVER_ID_DO_NOT_CARE;
	    *feature_para_len = 4;
	break;
	case SENSOR_FEATURE_SET_VIDEO_MODE:
	    set_video_mode(*feature_data);
	break;
	case SENSOR_FEATURE_CHECK_SENSOR_ID:
	    get_imgsensor_id(feature_return_para_32);
	break;
	case SENSOR_FEATURE_GET_SENSOR_OTP_ALL:
		memcpy(feature_return_para_32, (UINT32 *)otp_data, sizeof(otp_data));
	break;
	case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
	    set_auto_flicker_mode((BOOL)*feature_data_16,
			*(feature_data_16+1));
	break;
	case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
	    set_max_framerate_by_scenario(
			(enum MSDK_SCENARIO_ID_ENUM)*feature_data,
			*(feature_data+1));
	break;
	case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
	    get_default_framerate_by_scenario(
			(enum MSDK_SCENARIO_ID_ENUM)*(feature_data),
			(MUINT32 *)(uintptr_t)(*(feature_data+1)));
	break;
	case SENSOR_FEATURE_SET_TEST_PATTERN:
	    set_test_pattern_mode((BOOL)*feature_data);
	break;
	case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:
	    *feature_return_para_32 = imgsensor_info.checksum_value;
	    *feature_para_len = 4;
	break;
	case SENSOR_FEATURE_SET_FRAMERATE:
	    LOG_INF("current fps :%d\n", (UINT32)*feature_data);
	    spin_lock(&imgsensor_drv_lock);
	    imgsensor.current_fps = *feature_data;
	    spin_unlock(&imgsensor_drv_lock);
	break;

	case SENSOR_FEATURE_SET_HDR:
	    LOG_INF("ihdr enable :%d\n", (BOOL)*feature_data);
	    spin_lock(&imgsensor_drv_lock);
	    imgsensor.ihdr_en = (BOOL)*feature_data;
	    spin_unlock(&imgsensor_drv_lock);
	break;
	case SENSOR_FEATURE_GET_CROP_INFO:
	    LOG_INF("SENSOR_FEATURE_GET_CROP_INFO scenarioId:%d\n",
				(UINT32)*feature_data);

	    wininfo = (struct SENSOR_WINSIZE_INFO_STRUCT *)
			(uintptr_t)(*(feature_data+1));

		switch (*feature_data_32) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[1],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[2],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[3],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[4],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_CUSTOM1:
			memcpy((void *)wininfo,
			(void *)&imgsensor_winsize_info[5],
			sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[0],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
		break;
		}
	break;
	case SENSOR_FEATURE_SET_IHDR_SHUTTER_GAIN:
	    LOG_INF("SENSOR_SET_SENSOR_IHDR LE=%d, SE=%d, Gain=%d\n",
			(UINT16)*feature_data, (UINT16)*(feature_data+1),
			(UINT16)*(feature_data+2));
	#if 0
	    ihdr_write_shutter_gain((UINT16)*feature_data,
			(UINT16)*(feature_data+1), (UINT16)*(feature_data+2));
	#endif
	break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ_BY_SCENARIO:
		switch (*feature_data) {
    		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
    			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
    				= imgsensor_info.cap.pclk;
    			break;
    		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
    			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
    				= imgsensor_info.normal_video.pclk;
    			break;
    		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
    			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
    				= imgsensor_info.hs_video.pclk;
    			break;
    		case MSDK_SCENARIO_ID_SLIM_VIDEO:
    			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
    				= imgsensor_info.slim_video.pclk;
    			break;
    		case MSDK_SCENARIO_ID_CUSTOM1:
    			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
    				= imgsensor_info.custom1.pclk;
    			break;
    		case MSDK_SCENARIO_ID_CUSTOM2:
    			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
    				= imgsensor_info.custom2.pclk;
    			break;
    		case MSDK_SCENARIO_ID_CUSTOM3:
    			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
    				= imgsensor_info.custom3.pclk;
    			break;
    		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
    		default:
    			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
    				= imgsensor_info.pre.pclk;
    			break;
		}
		break;
	case SENSOR_FEATURE_GET_PERIOD_BY_SCENARIO:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.cap.framelength << 16)
				+ imgsensor_info.cap.linelength;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.normal_video.framelength << 16)
				+ imgsensor_info.normal_video.linelength;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.hs_video.framelength << 16)
				+ imgsensor_info.hs_video.linelength;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.slim_video.framelength << 16)
				+ imgsensor_info.slim_video.linelength;
			break;
		case MSDK_SCENARIO_ID_CUSTOM1:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.custom1.framelength << 16)
				+ imgsensor_info.custom1.linelength;
			break;
		case MSDK_SCENARIO_ID_CUSTOM2:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.custom2.framelength << 16)
				+ imgsensor_info.custom2.linelength;
			break;
		case MSDK_SCENARIO_ID_CUSTOM3:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.custom3.framelength << 16)
				+ imgsensor_info.custom3.linelength;
			break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.pre.framelength << 16)
				+ imgsensor_info.pre.linelength;
			break;
		}
		break;
	case SENSOR_FEATURE_GET_MIPI_PIXEL_RATE:
	{
		kal_uint32 rate;

		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			rate = imgsensor_info.cap.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			rate = imgsensor_info.normal_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			rate = imgsensor_info.hs_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			rate = imgsensor_info.slim_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_CUSTOM1:
			rate = imgsensor_info.custom1.mipi_pixel_rate;
		break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			rate = imgsensor_info.pre.mipi_pixel_rate;
			break;
		}
		LOG_INF("sc500sc SENSOR_FEATURE_GET_MIPI_PIXEL_RATE");
		*(MUINT32 *) (uintptr_t) (*(feature_data + 1)) = rate;
	}
    	break;
	case SENSOR_FEATURE_GET_TEMPERATURE_VALUE:
		*feature_return_para_i32 = 0;
		*feature_para_len = 4;
	break;
	case SENSOR_FEATURE_SET_STREAMING_SUSPEND:
		streaming_control(KAL_FALSE);
	break;
	case SENSOR_FEATURE_SET_STREAMING_RESUME:
		if (*feature_data != 0)
			set_shutter(*feature_data);
		streaming_control(KAL_TRUE);
	break;
	default:
	break;
	}

	return ERROR_NONE;
}    /*    feature_control()  */

static struct SENSOR_FUNCTION_STRUCT sensor_func = {
	open,
	get_info,
	get_resolution,
	feature_control,
	control,
	close
};

UINT32 SC520CS_MIPI_RAW_AVATARB5_SensorInit(struct SENSOR_FUNCTION_STRUCT **pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc != NULL)
		*pfFunc =  &sensor_func;
	return ERROR_NONE;
}	/*	SC520CS_MIPI_RAW_SensorInit	*/

