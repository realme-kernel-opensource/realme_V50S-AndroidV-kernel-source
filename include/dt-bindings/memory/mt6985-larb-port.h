/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 MediaTek Inc.
 * Author: Yunfei Wang <yf.wang@mediatek.com>
 */

#ifndef _DTS_IOMMU_PORT_MT6985_H_
#define _DTS_IOMMU_PORT_MT6985_H_

#include <dt-bindings/memory/mtk-memory-port.h>

/* table id must be the same as mtk_iommu.h */
#define MM_TAB					(0)
#define APU_TAB					(1)

/* iova region definition */
#define NORMAL_DOM				(0)
#define VDEC_DOM				(1)
#define LK_RESV_DOM				(2)
#define CCU0_DOM				(3)
#define CCU1_DOM				(4)
#define VIDEO_UP_DOM0				(5)
#define VIDEO_UP_DOM1				(6)


#define APU_DATA_DOM				(0)
#define APU_SEC_DOM				(1)
#define APU_CODE_DOM				(2)

/* larb0 */
#define M4U_PORT_L0_DISP_OVL0_2L_HDR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 0, 0)
#define M4U_PORT_L0_DISP_OVL0_2L_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 0, 1)
#define M4U_PORT_L0_DISP_OVL1_2L_RDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 0, 2)
#define M4U_PORT_L0_DISP_OVL2_2L_HDR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 0, 3)
#define M4U_PORT_L0_DISP_OVL2_2L_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 0, 4)
#define M4U_PORT_L0_DISP_OVL3_2L_RDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 0, 5)
#define M4U_PORT_L0_DISP_WDMA0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 0, 6)
#define M4U_PORT_L0_DISP_UFBC_WDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 0, 7)
#define M4U_PORT_L0_DISP_FAKE0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 0, 8)

/* larb1 */
#define M4U_PORT_L1_DISP_OVL0_2L_RDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 1, 0)
#define M4U_PORT_L1_DISP_OVL1_2L_HDR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 1, 1)
#define M4U_PORT_L1_DISP_OVL1_2L_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 1, 2)
#define M4U_PORT_L1_DISP_OVL2_2L_RDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 1, 3)
#define M4U_PORT_L1_DISP_OVL3_2L_HDR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 1, 4)
#define M4U_PORT_L1_DISP_OVL3_2L_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 1, 5)
#define M4U_PORT_L1_DISP_WDMA2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 1, 6)
#define M4U_PORT_L1_DISP_FAKE1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 1, 7)

/* larb2 */
#define M4U_PORT_L2_MDP_RDMA0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 2, 0)
#define M4U_PORT_L2_MDP_WROT0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 2, 1)
#define M4U_PORT_L2_DISP_FAKE0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 2, 2)
#define M4U_PORT_L2_MDP_RDMA1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 2, 3)
#define M4U_PORT_L2_MDP_WROT1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 2, 4)
#define M4U_PORT_L2_MDP_RDMA2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 2, 5)
#define M4U_PORT_L2_MDP_RDMA3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 2, 6)
#define M4U_PORT_L2_MDP_WDMA2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 2, 7)
#define M4U_PORT_L2_MDP_WDMA3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 2, 8)

/* larb3 */
#define M4U_PORT_L3_MDP_RDMA0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 3, 0)
#define M4U_PORT_L3_MDP_WROT0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 3, 1)
#define M4U_PORT_L3_DISP_FAKE0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 3, 2)
#define M4U_PORT_L3_MDP_RDMA1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 3, 3)
#define M4U_PORT_L3_MDP_WROT1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 3, 4)
#define M4U_PORT_L3_MDP_RDMA2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 3, 5)
#define M4U_PORT_L3_MDP_RDMA3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 3, 6)
#define M4U_PORT_L3_MDP_WDMA2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 3, 7)
#define M4U_PORT_L3_MDP_WDMA3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 3, 8)

/* larb4 */
#define M4U_PORT_L4_HW_VDEC_MC_C_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 0)
#define M4U_PORT_L4_HW_VDEC_UFO_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 1)
#define M4U_PORT_L4_HW_VDEC_PP_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 2)
#define M4U_PORT_L4_HW_VDEC_PRED_RD_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 3)
#define M4U_PORT_L4_HW_VDEC_PRED_WR_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 4)
#define M4U_PORT_L4_HW_VDEC_PPWRAP_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 5)
#define M4U_PORT_L4_HW_VDEC_TILE_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 6)
#define M4U_PORT_L4_HW_VDEC_VLD_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 7)
#define M4U_PORT_L4_HW_VDEC_VLD2_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 8)
#define M4U_PORT_L4_HW_VDEC_AVC_MV_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 9)
#define M4U_PORT_L4_HW_VDEC_UFO_EXT_C		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 4, 10)

/* larb5 */
#define M4U_PORT_L5_HW_VDEC_LAT0_VLD_EXT	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 5, 0)
#define M4U_PORT_L5_HW_VDEC_LAT0_VLD2_EXT	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 5, 1)
#define M4U_PORT_L5_HW_VDEC_LAT0_AVC_MV_EXT	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 5, 2)
#define M4U_PORT_L5_HW_VDEC_LAT0_PRED_RD_EXT	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 5, 3)
#define M4U_PORT_L5_HW_VDEC_LAT0_TILE_EXT	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 5, 4)
#define M4U_PORT_L5_HW_VDEC_LAT0_WDMA_EXT	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 5, 5)
#define M4U_PORT_L5_HW_VDEC_UFO_ENC_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 5, 6)
#define M4U_PORT_L5_HW_VDEC_UFO_ENC_EXT_C	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 5, 7)
#define M4U_PORT_L5_HW_VDEC_MC_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 5, 8)

/* larb6 */
#define M4U_PORT_L6_HW_MINI_MDP_R0_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 6, 0)
#define M4U_PORT_L6_HW_MINI_MDP_W0_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 6, 1)
#define M4U_PORT_L6_HW_MINI_MDP_R1_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 6, 2)
#define M4U_PORT_L6_HW_MINI_MDP_W1_EXT		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 6, 3)

/* larb7 */
#define M4U_PORT_L7_VENC_RCPU			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 0)
#define M4U_PORT_L7_VENC_REC			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 1)
#define M4U_PORT_L7_VENC_BSDMA			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 2)
#define M4U_PORT_L7_VENC_SV_COMV		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 3)
#define M4U_PORT_L7_VENC_RD_COMV		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 4)
#define M4U_PORT_L7_VENC_NBM_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 5)
#define M4U_PORT_L7_VENC_NBM_RDMA_LITE		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 6)
#define M4U_PORT_L7_JPGENC_Y_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 7)
#define M4U_PORT_L7_JPGENC_C_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 8)
#define M4U_PORT_L7_JPGENC_Q_TABLE		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 9)
#define M4U_PORT_L7_VENC_SUB_W_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 10)
#define M4U_PORT_L7_VENC_FCS_NBM_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 11)
#define M4U_PORT_L7_VENC_EC_WPP_BSDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 12)
#define M4U_PORT_L7_VENC_EC_WPP_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 13)
#define M4U_PORT_L7_VENC_DB_SYSRAM_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 14)
#define M4U_PORT_L7_VENC_DB_SYSRAM_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 15)
#define M4U_PORT_L7_JPGENC_BSDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 16)
#define M4U_PORT_L7_JPGDEC_WDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 17)
#define M4U_PORT_L7_JPGDEC_BSDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 18)
#define M4U_PORT_L7_VENC_NBM_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 19)
#define M4U_PORT_L7_VENC_NBM_WDMA_LITE		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 20)
#define M4U_PORT_L7_VENC_CUR_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 21)
#define M4U_PORT_L7_VENC_CUR_CHROMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 22)
#define M4U_PORT_L7_VENC_REF_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 23)
#define M4U_PORT_L7_VENC_REF_CHROMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 24)
#define M4U_PORT_L7_VENC_SUB_R_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 25)
#define M4U_PORT_L7_VENC_FCS_NBM_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 26)
#define M4U_PORT_L7_JPGDEC_WDMA_1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 27)
#define M4U_PORT_L7_JPGDEC_BSDMA_1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 28)
#define M4U_PORT_L7_JPGDEC_HUFF_OFFSET_1	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 29)
#define M4U_PORT_L7_JPGDEC_HUFF_OFFSET_0	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 7, 30)

/* larb8 */
#define M4U_PORT_L8_VENC_RCPU			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 0)
#define M4U_PORT_L8_VENC_REC			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 1)
#define M4U_PORT_L8_VENC_BSDMA			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 2)
#define M4U_PORT_L8_VENC_SV_COMV		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 3)
#define M4U_PORT_L8_VENC_RD_COMV		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 4)
#define M4U_PORT_L8_VENC_NBM_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 5)
#define M4U_PORT_L8_VENC_NBM_RDMA_LITE		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 6)
#define M4U_PORT_L8_JPGENC_Y_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 7)
#define M4U_PORT_L8_JPGENC_C_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 8)
#define M4U_PORT_L8_JPGENC_Q_TABLE		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 9)
#define M4U_PORT_L8_VENC_SUB_W_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 10)
#define M4U_PORT_L8_VENC_FCS_NBM_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 11)
#define M4U_PORT_L8_VENC_EC_WPP_BSDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 12)
#define M4U_PORT_L8_VENC_EC_WPP_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 13)
#define M4U_PORT_L8_VENC_DB_SYSRAM_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 14)
#define M4U_PORT_L8_VENC_DB_SYSRAM_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 15)
#define M4U_PORT_L8_JPGENC_BSDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 16)
#define M4U_PORT_L8_JPGDEC_WDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 17)
#define M4U_PORT_L8_JPGDEC_BSDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 18)
#define M4U_PORT_L8_VENC_NBM_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 19)
#define M4U_PORT_L8_VENC_NBM_WDMA_LITE		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 20)
#define M4U_PORT_L8_VENC_CUR_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 21)
#define M4U_PORT_L8_VENC_CUR_CHROMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 22)
#define M4U_PORT_L8_VENC_REF_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 23)
#define M4U_PORT_L8_VENC_REF_CHROMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 24)
#define M4U_PORT_L8_VENC_SUB_R_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 25)
#define M4U_PORT_L8_VENC_FCS_NBM_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 26)
#define M4U_PORT_L8_JPGDEC_WDMA_1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 27)
#define M4U_PORT_L8_JPGDEC_BSDMA_1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 28)
#define M4U_PORT_L8_JPGDEC_HUFF_OFFSET_1	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 29)
#define M4U_PORT_L8_JPGDEC_HUFF_OFFSET_0	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 8, 30)

/* larb9 */
#define M4U_PORT_L9_IMGI_T1_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 11)
#define M4U_PORT_L9_IMGI_T1_N_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 12)
#define M4U_PORT_L9_IMGCI_T1_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 13)
#define M4U_PORT_L9_IMGCI_T1_N_B		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 14)
#define M4U_PORT_L9_SMTI_T1_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 15)
#define M4U_PORT_L9_YUVO_T1_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 16)
#define M4U_PORT_L9_YUVO_T1_N_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 17)
#define M4U_PORT_L9_YUVCO_T1_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 18)
#define M4U_PORT_L9_YUVO_T2_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 19)
#define M4U_PORT_L9_YUVO_T4_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 20)
#define M4U_PORT_L9_TNCSTO_T1_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 21)
#define M4U_PORT_L9_SMTO_T1_B			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 9, 25)

/* larb10 */
#define M4U_PORT_L10_IMGI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 0)
#define M4U_PORT_L10_IMGCI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 1)
#define M4U_PORT_L10_IMGCI_D1_N			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 2)
#define M4U_PORT_L10_DMGI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 3)
#define M4U_PORT_L10_RECI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 4)
#define M4U_PORT_L10_TNRAIMI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 5)
#define M4U_PORT_L10_RECI_D3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 6)
#define M4U_PORT_L10_RECBI_D2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 7)
#define M4U_PORT_L10_TNRWI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 8)
#define M4U_PORT_L10_TNRCI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 9)
#define M4U_PORT_L10_SMTI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 10)
#define M4U_PORT_L10_SMTCI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 11)
#define M4U_PORT_L10_IMG4O_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 12)
#define M4U_PORT_L10_IMG4CO_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 13)
#define M4U_PORT_L10_TNRMO_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 14)
#define M4U_PORT_L10_SMTO_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 15)
#define M4U_PORT_L10_SMTCO_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 16)
#define M4U_PORT_L10_TNRSI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 17)
#define M4U_PORT_L10_TNRSO_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 10, 18)

/* larb11 */
#define M4U_PORT_L11_WPE_RDMA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 0)
#define M4U_PORT_L11_WPE_RDMA_1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 1)
#define M4U_PORT_L11_WPE_RDMA_4P_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 2)
#define M4U_PORT_L11_WPE_RDMA_4P_1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 3)
#define M4U_PORT_L11_PIMGI_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 4)
#define M4U_PORT_L11_PIMGBI_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 5)
#define M4U_PORT_L11_WPE_WDMA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 6)
#define M4U_PORT_L11_WPE_WDMA_4P_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 7)
#define M4U_PORT_L11_WROT_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 8)
#define M4U_PORT_L11_TCCSO_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 11, 9)

/* larb12 */
#define M4U_PORT_L12_FDVT_RDA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 0)
#define M4U_PORT_L12_FDVT_WRA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 1)
#define M4U_PORT_L12_ME_RDMA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 2)
#define M4U_PORT_L12_ME_WDMA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 3)
#define M4U_PORT_L12_MEMMG_RDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 4)
#define M4U_PORT_L12_MEMMG_WDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 5)
#define M4U_PORT_L12_ME_2ND_RDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 6)
#define M4U_PORT_L12_ME_2ND_WDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 7)
#define M4U_PORT_L12_MEMMG_2ND_RDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 8)
#define M4U_PORT_L12_MEMMG_2ND_WDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 12, 9)

/* larb13 */
#define M4U_PORT_L13_CQI_1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 13, 0)
#define M4U_PORT_L13_CAMSV_1_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 13, 1)
#define M4U_PORT_L13_FAKE_ENG			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 13, 2)

/* larb14 */
#define M4U_PORT_L14_CQI_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 14, 0)
#define M4U_PORT_L14_CAMSV_0_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 14, 1)

/* larb15 */
#define M4U_PORT_L15_VIPI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 0)
#define M4U_PORT_L15_VIPI_D1_N			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 1)
#define M4U_PORT_L15_TNCSTI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 2)
#define M4U_PORT_L15_TNCSTI_D4			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 3)
#define M4U_PORT_L15_EECSI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 4)
#define M4U_PORT_L15_SNRCSI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 5)
#define M4U_PORT_L15_CSMCSI_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 6)
#define M4U_PORT_L15_SMTI_D4			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 7)
#define M4U_PORT_L15_SMTI_D6			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 8)
#define M4U_PORT_L15_IMG3O_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 9)
#define M4U_PORT_L15_IMG3CO_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 10)
#define M4U_PORT_L15_IMG2O_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 11)
#define M4U_PORT_L15_TNCO_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 12)
#define M4U_PORT_L15_TNCSO_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 13)
#define M4U_PORT_L15_TNCSTO_D1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 14)
#define M4U_PORT_L15_SMTO_D4			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 15)
#define M4U_PORT_L15_SMTO_D6			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 15, 16)

/* larb16 */
#define M4U_PORT_L16_CQI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 0)
#define M4U_PORT_L16_RAWI_R2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 1)
#define M4U_PORT_L16_RAWI_R3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 2)
#define M4U_PORT_L16_RAWI_R5			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 3)
#define M4U_PORT_L16_IMGO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 4)
#define M4U_PORT_L16_BPCI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 5)
#define M4U_PORT_L16_LCSI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 6)
#define M4U_PORT_L16_UFEO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 7)
#define M4U_PORT_L16_LTMSO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 8)
#define M4U_PORT_L16_DRZB2NO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 9)
#define M4U_PORT_L16_AAO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 10)
#define M4U_PORT_L16_AFO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 16, 11)

/* larb17 */
#define M4U_PORT_L17_YUVO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 17, 0)
#define M4U_PORT_L17_YUVO_R3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 17, 1)
#define M4U_PORT_L17_YUVO_R2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 17, 2)
#define M4U_PORT_L17_YUVO_R5			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 17, 3)
#define M4U_PORT_L17_RGBWI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 17, 4)
#define M4U_PORT_L17_TCYSO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 17, 5)
#define M4U_PORT_L17_DRZ4NO_R3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 17, 6)

/* larb18 */
#define M4U_PORT_L18_IMGADL_0_IPUI_E1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 18, 0)
#define M4U_PORT_L18_IMGADL_0_IPUI_E2		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 18, 1)

/* larb19 */
#define M4U_PORT_L19_CCUI			MTK_M4U_PORT_ID(MM_TAB, CCU0_DOM, 19, 0)
#define M4U_PORT_L19_CCUO			MTK_M4U_PORT_ID(MM_TAB, CCU0_DOM, 19, 1)
#define M4U_PORT_L19_CCUI2			MTK_M4U_PORT_ID(MM_TAB, CCU1_DOM, 19, 2)
#define M4U_PORT_L19_CCUO2			MTK_M4U_PORT_ID(MM_TAB, CCU1_DOM, 19, 3)
#define M4U_PORT_L19_DVS_RDMA			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 19, 4)
#define M4U_PORT_L19_DVS_WDMA			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 19, 5)
#define M4U_PORT_L19_DVP_RDMA			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 19, 6)
#define M4U_PORT_L19_DVP_WDMA			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 19, 7)

/* larb20 */
#define M4U_PORT_L20_DISP_OVL0_2L_HDR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 20, 0)
#define M4U_PORT_L20_DISP_OVL0_2L_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 20, 1)
#define M4U_PORT_L20_DISP_OVL1_2L_RDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 20, 2)
#define M4U_PORT_L20_DISP_OVL2_2L_HDR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 20, 3)
#define M4U_PORT_L20_DISP_OVL2_2L_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 20, 4)
#define M4U_PORT_L20_DISP_OVL3_2L_RDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 20, 5)
#define M4U_PORT_L20_DISP_WDMA0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 20, 6)
#define M4U_PORT_L20_DISP_UFBC_WDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 20, 7)
#define M4U_PORT_L20_DISP_FAKE0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 20, 8)

/* larb21 */
#define M4U_PORT_L21_DISP_OVL0_2L_RDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 21, 0)
#define M4U_PORT_L21_DISP_OVL1_2L_HDR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 21, 1)
#define M4U_PORT_L21_DISP_OVL1_2L_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 21, 2)
#define M4U_PORT_L21_DISP_OVL2_2L_RDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 21, 3)
#define M4U_PORT_L21_DISP_OVL3_2L_HDR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 21, 4)
#define M4U_PORT_L21_DISP_OVL3_2L_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 21, 5)
#define M4U_PORT_L21_DISP_WDMA2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 21, 6)
#define M4U_PORT_L21_DISP_FAKE1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 21, 7)

/* larb22 */
#define M4U_PORT_L22_WPE_RDMA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 0)
#define M4U_PORT_L22_WPE_RDMA_1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 1)
#define M4U_PORT_L22_WPE_RDMA_4P_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 2)
#define M4U_PORT_L22_WPE_RDMA_4P_1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 3)
#define M4U_PORT_L22_PIMGI_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 4)
#define M4U_PORT_L22_PIMGBI_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 5)
#define M4U_PORT_L22_WPE_WDMA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 6)
#define M4U_PORT_L22_WPE_WDMA_4P_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 7)
#define M4U_PORT_L22_WROT_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 8)
#define M4U_PORT_L22_TCCSO_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 22, 9)

/* larb23 */
#define M4U_PORT_L23_WPE_RDMA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 0)
#define M4U_PORT_L23_WPE_RDMA_1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 1)
#define M4U_PORT_L23_WPE_RDMA_4P_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 2)
#define M4U_PORT_L23_WPE_RDMA_4P_1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 3)
#define M4U_PORT_L23_PIMGI_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 4)
#define M4U_PORT_L23_PIMGBI_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 5)
#define M4U_PORT_L23_WPE_WDMA_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 6)
#define M4U_PORT_L23_WPE_WDMA_4P_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 7)
#define M4U_PORT_L23_WROT_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 8)
#define M4U_PORT_L23_TCCSO_P1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 23, 9)

/* larb24--dummy */

/* larb25 */
#define M4U_PORT_L25_MRAW0_CQI_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 0)
#define M4U_PORT_L25_MRAW0_IMGBO_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 1)
#define M4U_PORT_L25_MRAW2_CQI_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 2)
#define M4U_PORT_L25_MRAW2_IMGBO_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 3)
#define M4U_PORT_L25_PDAI_A_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 4)
#define M4U_PORT_L25_PDAI_A_1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 5)
#define M4U_PORT_L25_PDAI_A_2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 6)
#define M4U_PORT_L25_PDAI_A_3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 7)
#define M4U_PORT_L25_PDAI_A_4			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 8)
#define M4U_PORT_L25_PDAO_A_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 9)
#define M4U_PORT_L25_MRAW0_IMGO_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 10)
#define M4U_PORT_L25_MRAW2_IMGO_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 25, 11)

/* larb26 */
#define M4U_PORT_L26_MRAW1_CQI_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 0)
#define M4U_PORT_L26_MRAW1_IMGBO_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 1)
#define M4U_PORT_L26_MRAW3_CQI_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 2)
#define M4U_PORT_L26_MRAW3_IMGBO_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 3)
#define M4U_PORT_L26_PDAI_B_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 4)
#define M4U_PORT_L26_PDAI_B_1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 5)
#define M4U_PORT_L26_PDAI_B_2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 6)
#define M4U_PORT_L26_PDAI_B_3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 7)
#define M4U_PORT_L26_PDAI_B_4			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 8)
#define M4U_PORT_L26_PDAO_B_0			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 9)
#define M4U_PORT_L26_MRAW1_IMGO_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 10)
#define M4U_PORT_L26_MRAW3_IMGO_M1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 26, 11)

/* larb27 */
#define M4U_PORT_L27_IPUI			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 27, 0)
#define M4U_PORT_L27_IPUI_2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 27, 1)
#define M4U_PORT_L27_IPU3O			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 27, 2)
#define M4U_PORT_L27_CQI_U1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 27, 3)
#define M4U_PORT_L27_IMGO_U1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 27, 4)
#define M4U_PORT_L27_YUVO_U1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 27, 5)

/* larb28 */
#define M4U_PORT_L28_IMGI_T1_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 0)
#define M4U_PORT_L28_IMGI_T1_N_A		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 1)
#define M4U_PORT_L28_IMGCI_T1_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 2)
#define M4U_PORT_L28_IMGCI_T1_N_A		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 3)
#define M4U_PORT_L28_SMTI_T1_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 4)
#define M4U_PORT_L28_SMTI_T4_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 5)
#define M4U_PORT_L28_TNCSTI_T1_A		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 6)
#define M4U_PORT_L28_TNCSTI_T4_A		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 7)
#define M4U_PORT_L28_LTMSTI_T1_A		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 8)
#define M4U_PORT_L28_YUVO_T1_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 9)
#define M4U_PORT_L28_YUVO_T1_N_A		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 10)
#define M4U_PORT_L28_YUVCO_T1_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 11)
#define M4U_PORT_L28_YUVO_T2_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 12)
#define M4U_PORT_L28_YUVO_T4_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 13)
#define M4U_PORT_L28_TNCSTO_T1_A		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 14)
#define M4U_PORT_L28_TNCSO_T1_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 15)
#define M4U_PORT_L28_SMTO_T1_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 16)
#define M4U_PORT_L28_SMTO_T4_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 17)
#define M4U_PORT_L28_LTMSO_T1_A			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 28, 18)

/* larb29 */
#define M4U_PORT_L29_CQI_2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 29, 0)
#define M4U_PORT_L29_CQI_3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 29, 1)
#define M4U_PORT_L29_CQI_4			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 29, 2)
#define M4U_PORT_L29_CQI_5			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 29, 3)
#define M4U_PORT_L29_CAMSV_2_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 29, 4)
#define M4U_PORT_L29_CAMSV_3_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 29, 5)
#define M4U_PORT_L29_CAMSV_4_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 29, 6)
#define M4U_PORT_L29_CAMSV_5_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 29, 7)

/* larb30 */
#define M4U_PORT_L30_CQI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 0)
#define M4U_PORT_L30_RAWI_R2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 1)
#define M4U_PORT_L30_RAWI_R3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 2)
#define M4U_PORT_L30_RAWI_R5			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 3)
#define M4U_PORT_L30_IMGO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 4)
#define M4U_PORT_L30_BPCI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 5)
#define M4U_PORT_L30_LCSI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 6)
#define M4U_PORT_L30_UFEO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 7)
#define M4U_PORT_L30_LTMSO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 8)
#define M4U_PORT_L30_DRZB2NO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 9)
#define M4U_PORT_L30_AAO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 10)
#define M4U_PORT_L30_AFO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 30, 11)

/* larb31 */
#define M4U_PORT_L31_CQI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 0)
#define M4U_PORT_L31_RAWI_R2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 1)
#define M4U_PORT_L31_RAWI_R3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 2)
#define M4U_PORT_L31_RAWI_R5			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 3)
#define M4U_PORT_L31_IMGO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 4)
#define M4U_PORT_L31_BPCI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 5)
#define M4U_PORT_L31_LCSI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 6)
#define M4U_PORT_L31_UFEO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 7)
#define M4U_PORT_L31_LTMSO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 8)
#define M4U_PORT_L31_DRZB2NO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 9)
#define M4U_PORT_L31_AAO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 10)
#define M4U_PORT_L31_AFO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 31, 11)

/* larb32 */
#define M4U_PORT_L32_DISP_POSTMASK0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 32, 0)
#define M4U_PORT_L32_DISP_MDP_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 32, 1)
#define M4U_PORT_L32_DISP_ODDMR0_DMRR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 32, 2)
#define M4U_PORT_L32_DISP_ODDMR0_ODR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 32, 3)
#define M4U_PORT_L32_DISP_ODDMR0_ODW		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 32, 4)
#define M4U_PORT_L32_DISP_WDMA1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 32, 5)
#define M4U_PORT_L32_DISP_UFBC_WDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 32, 6)

/* larb33 */
#define M4U_PORT_L33_DISP_POSTMASK0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 33, 0)
#define M4U_PORT_L33_DISP_MDP_RDMA0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 33, 1)
#define M4U_PORT_L33_DISP_ODDMR0_DMRR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 33, 2)
#define M4U_PORT_L33_DISP_ODDMR0_ODR		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 33, 3)
#define M4U_PORT_L33_DISP_ODDMR0_ODW		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 33, 4)
#define M4U_PORT_L33_DISP_WDMA1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 33, 5)
#define M4U_PORT_L33_DISP_UFBC_WDMA1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 33, 6)

/* larb34 */
#define M4U_PORT_L34_YUVO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 34, 0)
#define M4U_PORT_L34_YUVO_R3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 34, 1)
#define M4U_PORT_L34_YUVO_R2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 34, 2)
#define M4U_PORT_L34_YUVO_R5			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 34, 3)
#define M4U_PORT_L34_RGBWI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 34, 4)
#define M4U_PORT_L34_TCYSO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 34, 5)
#define M4U_PORT_L34_DRZ4NO_R3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 34, 6)

/* larb35 */
#define M4U_PORT_L35_YUVO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 35, 0)
#define M4U_PORT_L35_YUVO_R3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 35, 1)
#define M4U_PORT_L35_YUVO_R2			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 35, 2)
#define M4U_PORT_L35_YUVO_R5			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 35, 3)
#define M4U_PORT_L35_RGBWI_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 35, 4)
#define M4U_PORT_L35_TCYSO_R1			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 35, 5)
#define M4U_PORT_L35_DRZ4NO_R3			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 35, 6)

/* larb36--dummy */

/* larb37 */
#define M4U_PORT_L37_VENC_RCPU			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 0)
#define M4U_PORT_L37_VENC_REC			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 1)
#define M4U_PORT_L37_VENC_BSDMA			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 2)
#define M4U_PORT_L37_VENC_SV_COMV		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 3)
#define M4U_PORT_L37_VENC_RD_COMV		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 4)
#define M4U_PORT_L37_VENC_NBM_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 5)
#define M4U_PORT_L37_VENC_NBM_RDMA_LITE		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 6)
#define M4U_PORT_L37_JPGENC_Y_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 7)
#define M4U_PORT_L37_JPGENC_C_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 8)
#define M4U_PORT_L37_JPGENC_Q_TABLE		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 9)
#define M4U_PORT_L37_VENC_SUB_W_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 10)
#define M4U_PORT_L37_VENC_FCS_NBM_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 11)
#define M4U_PORT_L37_VENC_EC_WPP_BSDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 12)
#define M4U_PORT_L37_VENC_EC_WPP_RDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 13)
#define M4U_PORT_L37_VENC_DB_SYSRAM_WDMA	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 14)
#define M4U_PORT_L37_VENC_DB_SYSRAM_RDMA	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 15)
#define M4U_PORT_L37_JPGENC_BSDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 16)
#define M4U_PORT_L37_JPGDEC_WDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 17)
#define M4U_PORT_L37_JPGDEC_BSDMA_0		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 18)
#define M4U_PORT_L37_VENC_NBM_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 19)
#define M4U_PORT_L37_VENC_NBM_WDMA_LITE		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 20)
#define M4U_PORT_L37_VENC_CUR_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 21)
#define M4U_PORT_L37_VENC_CUR_CHROMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 22)
#define M4U_PORT_L37_VENC_REF_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 23)
#define M4U_PORT_L37_VENC_REF_CHROMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 24)
#define M4U_PORT_L37_VENC_SUB_R_LUMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 25)
#define M4U_PORT_L37_VENC_FCS_NBM_WDMA		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 26)
#define M4U_PORT_L37_JPGDEC_WDMA_1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 27)
#define M4U_PORT_L37_JPGDEC_BSDMA_1		MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 28)
#define M4U_PORT_L37_JPGDEC_HUFF_OFFSET_1	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 29)
#define M4U_PORT_L37_JPGDEC_HUFF_OFFSET_0	MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 37, 30)

/* fake larb38 */
#define M4U_PORT_L38_CCU0			MTK_M4U_PORT_ID(MM_TAB, CCU0_DOM, 38, 0)
#define M4U_PORT_L38_CCU1			MTK_M4U_PORT_ID(MM_TAB, CCU1_DOM, 38, 1)

/* fake larb39 */
#define M4U_PORT_L39_VIDEO_UP0			MTK_M4U_PORT_ID(MM_TAB, VIDEO_UP_DOM0, 39, 0)
#define M4U_PORT_L39_VIDEO_UP1			MTK_M4U_PORT_ID(MM_TAB, VIDEO_UP_DOM1, 39, 0)
#define M4U_PORT_L39_GCE_DM			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 39, 1)
#define M4U_PORT_L39_GCE_MM			MTK_M4U_PORT_ID(MM_TAB, NORMAL_DOM, 39, 2)

/* fake larb40 reserved */
#define M4U_PORT_L40_APU_DATA			MTK_M4U_PORT_ID(APU_TAB, APU_DATA_DOM, 40, 0)
#define M4U_PORT_L40_APU_CODE			MTK_M4U_PORT_ID(APU_TAB, APU_CODE_DOM, 40, 1)

#endif /* _DTS_IOMMU_PORT_MT6985_H_ */
