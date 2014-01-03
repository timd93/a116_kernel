/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.c
 *
 * Project:
 * --------
 *   YUSU
 *
 * Description:
 * ------------
 *   Source code of Sensor driver
 *
 *
 * Author:
 * -------
 *   Jackie Su (MTK02380)
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 * 02 19 2012 koli.lin
 * [ALPS00237768] [A52][??]�Z�mcamera a5141 AF?�J�C
 * [Camera] Modify the isp delay frame to synchrnoize with sensor exposure time and sensor gain.
 *
 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/slab.h>


#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"

#include "a5141mipi_Sensor.h"
#include "a5141mipi_Camera_Sensor_para.h"
#include "a5141mipi_CameraCustomized.h"


#define CAPTURE_15FPS
#define A5141MIPI_DEBUG
#ifdef A5141MIPI_DEBUG
#define SENSORDB printk
#else
#define SENSORDB(x,...)
#endif

static DEFINE_SPINLOCK(a5141mipiraw_drv_lock);

#define AUTO_FLICKER_NO 10
kal_uint16 A5141_Frame_Length_preview = 0;


struct A5141MIPI_SENSOR_STRUCT A5141MIPI_sensor= 
{	
	.i2c_write_id = 0x6c,
	.i2c_read_id  = 0x6d,
	
#ifdef MIPI_INTERFACE
	.preview_vt_clk = 1040,
	//.capture_vt_clk = 1040,
	.capture_vt_clk = 1118,
#else
	.preview_vt_clk = 520,
	.capture_vt_clk = 520,
#endif
};

kal_uint16 A5141MIPI_dummy_pixels=0, A5141MIPI_dummy_lines=0;
kal_uint16 A5141MIPI_PV_dummy_pixels=0,A5141MIPI_PV_dummy_lines=0;

kal_uint16 A5141MIPI_exposure_lines=0x100;
kal_uint16 A5141MIPI_sensor_global_gain=BASEGAIN, A5141MIPI_sensor_gain_base=BASEGAIN;
kal_uint16 A5141MIPI_sensor_gain_array[2][5]={{0x0204,0x0208, 0x0206, 0x020C, 0x020A},{0x08,0x8, 0x8, 0x8, 0x8}};


MSDK_SENSOR_CONFIG_STRUCT A5141MIPISensorConfigData;
kal_uint32 A5141MIPI_FAC_SENSOR_REG;

/* FIXME: old factors and DIDNOT use now. s*/
SENSOR_REG_STRUCT A5141MIPISensorCCT[FACTORY_END_ADDR]=CAMERA_SENSOR_CCT_DEFAULT_VALUE;
SENSOR_REG_STRUCT A5141MIPISensorReg[ENGINEER_END]=CAMERA_SENSOR_REG_DEFAULT_VALUE;
/* FIXME: old factors and DIDNOT use now. e*/
MSDK_SCENARIO_ID_ENUM A5141_CurrentScenarioId = ACDK_SCENARIO_ID_CAMERA_PREVIEW;

typedef enum
{
  A5141MIPI_MODE_PREVIEW,  
  A5141MIPI_MODE_CAPTURE  
} A5141MIPI_MODE;
A5141MIPI_MODE g_iA5141MIPI_Mode = A5141MIPI_MODE_PREVIEW;

extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);

kal_uint16 A5141MIPI_read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte=0;
    char puSendCmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
	iReadRegI2C(puSendCmd , 2, (u8*)&get_byte,2,A5141MIPI_sensor.i2c_write_id);
    return ((get_byte<<8)&0xff00)|((get_byte>>8)&0x00ff);
}

void A5141MIPI_write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
	char puSendCmd[4] = {(char)(addr >> 8) , (char)(addr & 0xFF) ,(char)(para >> 8),(char)(para & 0xFF)};
	iWriteRegI2C(puSendCmd , 4,A5141MIPI_sensor.i2c_write_id);
}

kal_uint16 A5141MIPI_read_cmos_sensor_8(kal_uint32 addr)
{
	kal_uint16 get_byte=0;
    char puSendCmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
	iReadRegI2C(puSendCmd , 2, (u8*)&get_byte,1,A5141MIPI_sensor.i2c_write_id);
    return get_byte;
}

void A5141MIPI_write_cmos_sensor_8(kal_uint32 addr, kal_uint32 para)
{
	char puSendCmd[4] = {(char)(addr >> 8) , (char)(addr & 0xFF) ,(char)(para & 0xFF)};
	iWriteRegI2C(puSendCmd , 3,A5141MIPI_sensor.i2c_write_id);
}


/*******************************************************************************
* 
********************************************************************************/
static kal_uint16 A5141reg2gain(kal_uint16 reg_gain)
{
    kal_uint16 gain;
	kal_uint16 collumn_gain, asc1_gain, initial_gain;
	kal_uint16 collumn_gain_shift = 10, asc1_gain_shift = 8;
	kal_uint16 collumn_gain_value = 1, asc1_gain_value = 1;

    collumn_gain = (reg_gain & (0x03 << collumn_gain_shift)) >> collumn_gain_shift;
	asc1_gain = (reg_gain & (0x03 << asc1_gain_shift)) >> asc1_gain_shift;
	initial_gain = reg_gain & 0x7F;

	if (collumn_gain == 0) {
		collumn_gain_value = 1;
	} else if (collumn_gain == 1) {
		collumn_gain_value = 3;
	} else if (collumn_gain == 2) {
		collumn_gain_value = 2;
	} else if (collumn_gain == 3) {
		collumn_gain_value = 4;
	}

	if (asc1_gain == 0) {
		asc1_gain_value = 1;
	} else if (asc1_gain == 1) {
		asc1_gain_value = 0xFFFF;
	} else if (asc1_gain == 2) {
		asc1_gain_value = 2;
	} else {
		// not exist
		SENSORDB("error gain setting");
	}

	if ( asc1_gain_value == 0xFFFF) {
		gain = BASEGAIN * initial_gain * collumn_gain_value * 4 / (32 * 3);
	} else {
		gain = BASEGAIN * initial_gain * collumn_gain_value * asc1_gain_value / (32);
	}

    return gain;
}


/*******************************************************************************
* 
********************************************************************************/
static kal_uint16 A5141gain2reg(kal_uint16 gain)
{
    kal_uint16 reg_gain;
	kal_uint16 collumn_gain = 0, asc1_gain = 0, initial_gain = 0;
	kal_uint16 collumn_gain_shift = 10, asc1_gain_shift = 8;


	// a5141 min reg gain 0x112E , mtk gain 122, so here block litter than min gian value
	/*	
	if (gain < (4 * BASEGAIN) / 3) {
		collumn_gain = (0x00&0x03) << collumn_gain_shift;
		asc1_gain = (0x00&0x03) << asc1_gain_shift;
		initial_gain = (32 * gain / BASEGAIN) & 0x7F;
	*/
	if (gain <= 122) {
		collumn_gain = (0x00&0x03) << collumn_gain_shift;
		asc1_gain = (0x01&0x03) << asc1_gain_shift;
		initial_gain = 0x2E;
	} else if (gain < 2 * BASEGAIN) {
		collumn_gain = (0x00&0x03) << collumn_gain_shift;
		asc1_gain = (0x01&0x03) << asc1_gain_shift;
		initial_gain = (32 * gain * 3 / (BASEGAIN * 4)) & 0x7F;
	} else if (gain < (8 * BASEGAIN)/3 + 1) {
		collumn_gain = (0x02&0x03) << collumn_gain_shift;
		asc1_gain = (0x00&0x03) << asc1_gain_shift;
		initial_gain = (32 * gain / (BASEGAIN * 2)) & 0x7F;
	} else if (gain < 3 * BASEGAIN) {
		collumn_gain = (0x02&0x03) << collumn_gain_shift;
		asc1_gain = (0x01&0x03) << asc1_gain_shift;
		initial_gain = (32 * gain * 3 / (BASEGAIN * 2 * 4)) & 0x7F;
	} else if (gain < 4 * BASEGAIN) {
		collumn_gain = (0x01&0x03) << collumn_gain_shift;
		asc1_gain = (0x00&0x03) << asc1_gain_shift;
		initial_gain = (32 * gain / (BASEGAIN * 3)) & 0x7F;
	} else if (gain < (16 * BASEGAIN) / 3 + 1) {
		collumn_gain = (0x03&0x03) << collumn_gain_shift;
		asc1_gain = (0x00&0x03) << asc1_gain_shift;
		initial_gain = (32 * gain / (BASEGAIN * 4)) & 0x7F;
	} else if (gain < 8 * BASEGAIN) {
		collumn_gain = (0x03&0x03) << collumn_gain_shift;
		asc1_gain = (0x01&0x03) << asc1_gain_shift;
		initial_gain = (32 * gain * 3 / (BASEGAIN * 4 * 4)) & 0x7F;
	} else if (gain < 32 * BASEGAIN) {
		collumn_gain = (0x03&0x03) << collumn_gain_shift;
		asc1_gain = (0x02&0x03) << asc1_gain_shift;
		initial_gain = (32 * gain / (BASEGAIN * 4 * 2)) & 0x7F;
	} else {
		// not exist
		SENSORDB("error gain setting");
	}

	reg_gain = collumn_gain | asc1_gain | initial_gain;

    return reg_gain;
}

/*************************************************************************
* FUNCTION
*    read_A5141MIPI_gain
*
* DESCRIPTION
*    This function is to set global gain to sensor.
*
* PARAMETERS
*    None
*
* RETURNS
*    gain : sensor global gain(base: 0x40)
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint16 read_A5141MIPI_gain(void)
{
	
	/*

	kal_uint16 reg_gain=0, gain=0;
		 
	reg_gain = A5141MIPI_read_cmos_sensor(0x305E);
	gain = A5141reg2gain(reg_gain);	//change reg gain to mtk gain
	
	SENSORDB("read_A5141MIPI_gain: reg_gain =0x%x, gain = %d \n", reg_gain, gain);

	return gain;
	*/


	
	volatile signed char i;
    kal_uint16 temp_reg=0, sensor_gain=0,temp_reg_base=0;
    
	temp_reg_base=A5141MIPISensorCCT[SENSOR_BASEGAIN].Para;

	for(i=0;i<4;i++)
	{
		temp_reg=A5141MIPISensorCCT[PRE_GAIN_R_INDEX+i].Para;

		if(temp_reg>=0x08 && temp_reg<=0x78)  // 0x78 is 15 by 8 ,means max gain is 15 multiple
			A5141MIPI_sensor_gain_array[1][PRE_GAIN_R_INDEX+i]=((((temp_reg*BASEGAIN)/8)*temp_reg_base)/8); //change to MTK basegain
		else if(temp_reg>0x78)
		    SENSORDB("error gain setting");
	}

	sensor_gain=(temp_reg_base*BASEGAIN)/8;

	return sensor_gain;   //mtk gain unit
	
}

/*******************************************************************************
* 
********************************************************************************/
void write_A5141MIPI_gain(kal_uint16 gain)
{
	
	
	
    kal_uint16 reg_gain;
  
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x01);		//parameter_hold
	if(gain >= BASEGAIN && gain <= 32*BASEGAIN)
	{
		//if (gain < 122)
		//	gain = 122;
		
		reg_gain = 8 * gain/BASEGAIN;        //change mtk gain base to aptina gain base
	    A5141MIPI_write_cmos_sensor(0x0204,reg_gain);
	    
	    SENSORDB("reg_gain =%d,gain = %d \n",reg_gain, gain);
	}
	else
	    SENSORDB("error gain setting");
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x00);		//parameter_hold
	

	/*
	kal_uint16 reg_gain, reg;
	
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x01);		//parameter_hold
	
	if(gain >= BASEGAIN && gain <= 32*BASEGAIN)
	{
		reg_gain = A5141gain2reg(gain);                //change mtk gain base to aptina gain base

		reg = A5141MIPI_read_cmos_sensor(0x305E);
		reg = (reg & 0xF000) | reg_gain;
		A5141MIPI_write_cmos_sensor(0x305E, reg);

		SENSORDB("reg =0x%x, gain = %d \n",reg, gain);
	}
	else
	    SENSORDB("error gain setting");

	A5141MIPI_write_cmos_sensor_8(0x0104, 0x00);		//parameter_hold
	*/
}


/*************************************************************************
* FUNCTION
* set_A5141MIPI_gain
*
* DESCRIPTION
* This function is to set global gain to sensor.
*
* PARAMETERS
* gain : sensor global gain(base: 0x40)
*
* RETURNS
* the actually gain set to sensor.
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint16 A5141MIPI_Set_gain(kal_uint16 gain)
{
	write_A5141MIPI_gain(gain);
}

/*******************************************************************************
* 
********************************************************************************/
void A5141MIPI_camera_para_to_sensor(void)
{
    kal_uint32    i;

	
    for(i=0; 0xFFFFFFFF!=A5141MIPISensorReg[i].Addr; i++)
    {
        A5141MIPI_write_cmos_sensor(A5141MIPISensorReg[i].Addr, A5141MIPISensorReg[i].Para);
    }
    for(i=ENGINEER_START_ADDR; 0xFFFFFFFF!=A5141MIPISensorReg[i].Addr; i++)
    {
        A5141MIPI_write_cmos_sensor(A5141MIPISensorReg[i].Addr, A5141MIPISensorReg[i].Para);
    }
    for(i=FACTORY_START_ADDR; i<FACTORY_END_ADDR; i++)
    {
        A5141MIPI_write_cmos_sensor(A5141MIPISensorCCT[i].Addr, A5141MIPISensorCCT[i].Para);
    }
}


/*************************************************************************
* FUNCTION
*    A5141MIPI_sensor_to_camera_para
*
* DESCRIPTION
*    // update camera_para from sensor register
*
* PARAMETERS
*    None
*
* RETURNS
*    gain : sensor global gain(base: 0x40)
*
* GLOBALS AFFECTED
*
*************************************************************************/
void A5141MIPI_sensor_to_camera_para(void)
{
    kal_uint32    i;
	
    for(i=0; 0xFFFFFFFF!=A5141MIPISensorReg[i].Addr; i++)
    {
        spin_lock(&a5141mipiraw_drv_lock);
        A5141MIPISensorReg[i].Para = A5141MIPI_read_cmos_sensor(A5141MIPISensorReg[i].Addr);
		spin_unlock(&a5141mipiraw_drv_lock);		
    }
    for(i=ENGINEER_START_ADDR; 0xFFFFFFFF!=A5141MIPISensorReg[i].Addr; i++)
    {	
    	spin_lock(&a5141mipiraw_drv_lock);
        A5141MIPISensorReg[i].Para = A5141MIPI_read_cmos_sensor(A5141MIPISensorReg[i].Addr);
		spin_unlock(&a5141mipiraw_drv_lock);
	}
}


/*************************************************************************
* FUNCTION
*    A5141MIPI_get_sensor_group_count
*
* DESCRIPTION
*    //
*
* PARAMETERS
*    None
*
* RETURNS
*    gain : sensor global gain(base: 0x40)
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_int32  A5141MIPI_get_sensor_group_count(void)
{
    return GROUP_TOTAL_NUMS;
}

void A5141MIPI_get_sensor_group_info(kal_uint16 group_idx, kal_int8* group_name_ptr, kal_int32* item_count_ptr)
{
   	switch (group_idx)
   	{
        case PRE_GAIN:
            sprintf((char *)group_name_ptr, "CCT");
            *item_count_ptr = 5;
            break;
        case CMMCLK_CURRENT:
            sprintf((char *)group_name_ptr, "CMMCLK Current");
            *item_count_ptr = 1;
            break;
        case FRAME_RATE_LIMITATION:
            sprintf((char *)group_name_ptr, "Frame Rate Limitation");
            *item_count_ptr = 2;
            break;
        case REGISTER_EDITOR:
            sprintf((char *)group_name_ptr, "Register Editor");
            *item_count_ptr = 2;
            break;
        default:
            ASSERT(0);
	}
}

void A5141MIPI_get_sensor_item_info(kal_uint16 group_idx,kal_uint16 item_idx, MSDK_SENSOR_ITEM_INFO_STRUCT* info_ptr)
{
    kal_int16 temp_reg=0;
    kal_uint16 temp_gain=0, temp_addr=0, temp_para=0;
	
    switch (group_idx)
    {
        case PRE_GAIN:
           switch (item_idx)
          {
              case 0:
                sprintf((char *)info_ptr->ItemNamePtr,"Pregain-R");
                  temp_addr = PRE_GAIN_R_INDEX;
              break;
              case 1:
                sprintf((char *)info_ptr->ItemNamePtr,"Pregain-Gr");
                  temp_addr = PRE_GAIN_Gr_INDEX;
              break;
              case 2:
                sprintf((char *)info_ptr->ItemNamePtr,"Pregain-Gb");
                  temp_addr = PRE_GAIN_Gb_INDEX;
              break;
              case 3:
                sprintf((char *)info_ptr->ItemNamePtr,"Pregain-B");
                  temp_addr = PRE_GAIN_B_INDEX;
              break;
              case 4:
                 sprintf((char *)info_ptr->ItemNamePtr,"SENSOR_BASEGAIN");
                 temp_addr = SENSOR_BASEGAIN;
              break;
              default:
                 ASSERT(0);
          }

            temp_para=A5141MIPISensorCCT[temp_addr].Para;

		   if(temp_para>=0x08 && temp_para<=0x78)
				temp_gain=(temp_para*BASEGAIN)/8;
			else
				ASSERT(0);

            temp_gain=(temp_gain*1000)/BASEGAIN;

            info_ptr->ItemValue=temp_gain;
            info_ptr->IsTrueFalse=KAL_FALSE;
            info_ptr->IsReadOnly=KAL_FALSE;
            info_ptr->IsNeedRestart=KAL_FALSE;
            info_ptr->Min=1000;
            info_ptr->Max=15000;
            break;
        case CMMCLK_CURRENT:
            switch (item_idx)
            {
                case 0:
                    sprintf((char *)info_ptr->ItemNamePtr,"Drv Cur[2,4,6,8]mA");
                
                    //temp_reg=A5141MIPISensorReg[CMMCLK_CURRENT_INDEX].Para;
                    temp_reg = ISP_DRIVING_2MA;
                    if(temp_reg==ISP_DRIVING_2MA)
                    {
                        info_ptr->ItemValue=2;
                    }
                    else if(temp_reg==ISP_DRIVING_4MA)
                    {
                        info_ptr->ItemValue=4;
                    }
                    else if(temp_reg==ISP_DRIVING_6MA)
                    {
                        info_ptr->ItemValue=6;
                    }
                    else if(temp_reg==ISP_DRIVING_8MA)
                    {
                        info_ptr->ItemValue=8;
                    }
                
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_FALSE;
                    info_ptr->IsNeedRestart=KAL_TRUE;
                    info_ptr->Min=2;
                    info_ptr->Max=8;
                    break;
                default:
                    ASSERT(0);
            }
            break;
        case FRAME_RATE_LIMITATION:
            switch (item_idx)
            {
                case 0:
                    sprintf((char *)info_ptr->ItemNamePtr,"Max Exposure Lines");
                    info_ptr->ItemValue=    111;  //A5141MIPI_MAX_EXPOSURE_LINES;
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_TRUE;
                    info_ptr->IsNeedRestart=KAL_FALSE;
                    info_ptr->Min=0;
                    info_ptr->Max=0;
                    break;
                case 1:
                    sprintf((char *)info_ptr->ItemNamePtr,"Min Frame Rate");
                    info_ptr->ItemValue=12;
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_TRUE;
                    info_ptr->IsNeedRestart=KAL_FALSE;
                    info_ptr->Min=0;
                    info_ptr->Max=0;
                    break;
                default:
                    ASSERT(0);
            }
            break;
        case REGISTER_EDITOR:
            switch (item_idx)
            {
                case 0:
                    sprintf((char *)info_ptr->ItemNamePtr,"REG Addr.");
                    info_ptr->ItemValue=0;
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_FALSE;
                    info_ptr->IsNeedRestart=KAL_FALSE;
                    info_ptr->Min=0;
                    info_ptr->Max=0xFFFF;
                    break;
                case 1:
                    sprintf((char *)info_ptr->ItemNamePtr,"REG Value");
                    info_ptr->ItemValue=0;
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_FALSE;
                    info_ptr->IsNeedRestart=KAL_FALSE;
                    info_ptr->Min=0;
                    info_ptr->Max=0xFFFF;
                    break;
                default:
                ASSERT(0);
            }
            break;
        default:
            ASSERT(0);
    }
}



kal_bool A5141MIPI_set_sensor_item_info(kal_uint16 group_idx, kal_uint16 item_idx, kal_int32 ItemValue)
{
//   kal_int16 temp_reg;
   kal_uint16  temp_gain=0,temp_addr=0, temp_para=0;

   switch (group_idx)
    {
        case PRE_GAIN:
            switch (item_idx)
            {
              case 0:
                temp_addr = PRE_GAIN_R_INDEX;
              break;
              case 1:
                temp_addr = PRE_GAIN_Gr_INDEX;
              break;
              case 2:
                temp_addr = PRE_GAIN_Gb_INDEX;
              break;
              case 3:
                temp_addr = PRE_GAIN_B_INDEX;
              break;
              case 4:
                temp_addr = SENSOR_BASEGAIN;
              break;
              default:
                 ASSERT(0);
          }

			temp_gain=((ItemValue*BASEGAIN+500)/1000);			//+500:get closed integer value

		  if(temp_gain>=1*BASEGAIN && temp_gain<=15*BASEGAIN)
          {
             temp_para=(temp_gain*8+BASEGAIN/2)/BASEGAIN;
          }          
          else
			  ASSERT(0);
			spin_lock(&a5141mipiraw_drv_lock);
            A5141MIPISensorCCT[temp_addr].Para = temp_para;
			spin_unlock(&a5141mipiraw_drv_lock);

			A5141MIPI_write_cmos_sensor(A5141MIPISensorCCT[temp_addr].Addr,temp_para);

			spin_lock(&a5141mipiraw_drv_lock);
           	A5141MIPI_sensor_gain_base=read_A5141MIPI_gain();
			spin_unlock(&a5141mipiraw_drv_lock);

            break;
        case CMMCLK_CURRENT:
            switch (item_idx)
            {
                case 0:
                    //no need to apply this item for driving current
                    break;
                default:
                    ASSERT(0);
            }
            break;
        case FRAME_RATE_LIMITATION:
            ASSERT(0);
            break;
        case REGISTER_EDITOR:
            switch (item_idx)
            {
                case 0:
					spin_lock(&a5141mipiraw_drv_lock);
                    A5141MIPI_FAC_SENSOR_REG=ItemValue;
					spin_unlock(&a5141mipiraw_drv_lock);
                    break;
                case 1:
                    A5141MIPI_write_cmos_sensor(A5141MIPI_FAC_SENSOR_REG,ItemValue);
                    break;
                default:
                    ASSERT(0);
            }
            break;
        default:
            ASSERT(0);
    }
   
    return KAL_TRUE;
}


/*******************************************************************************
*
********************************************************************************/
static void A5141MIPI_Init_setting(void)
{
    kal_uint16 status = 0;
	
	SENSORDB( "A5141MIPI initial setting \n");
	
	A5141MIPI_write_cmos_sensor_8(0x0103, 0x01); 	//SOFTWARE_RESET (clears itself)
	mDELAY(5);		//Initialization Time
	
	//[Demo Initialization 1296 x 972 MCLK= 26MHz, PCLK=104MHz]
	//stop_streaming
	A5141MIPI_write_cmos_sensor_8(0x0100, 0x00);	// MODE_SELECT

	
	#ifdef MIPI_INTERFACE
		#ifdef RAW10
			A5141MIPI_write_cmos_sensor(0x301A, 0x0218);	//RESET_REGISTER enable mipi interface  bit[9] mask bad frame
			A5141MIPI_write_cmos_sensor(0x3064, 0xB800);	// SMIA_TEST
			A5141MIPI_write_cmos_sensor(0x31AE, 0x0202);	// two lane
			A5141MIPI_write_cmos_sensor(0x0112, 0x0A0A);	// 10bit raw output
		#else
			A5141MIPI_write_cmos_sensor(0x301A, 0x0218);	//RESET_REGISTER enable mipi interface  bit[9] mask bad frame
			A5141MIPI_write_cmos_sensor(0x3064, 0x0805);	// SMIA_TEST
			A5141MIPI_write_cmos_sensor(0x31AE, 0x0202);	// two lane
			A5141MIPI_write_cmos_sensor(0x0112, 0x0808);	// 8bit raw output
		#endif
	#else
	    do
	    {
			A5141MIPI_write_cmos_sensor(0x301A, 0x12C8);	//RESET_REGISTER enable parallel bit[9] mask bad frame
			mDELAY(2);	
			status = A5141MIPI_read_cmos_sensor(0x301A);    //polling status change 
			SENSORDB("A5141MIPI status = %x \n",status);
		}while(status != 0x12C8)   ;
		
		A5141MIPI_write_cmos_sensor(0x3064, 0x5840);	    // SMIA_TEST
		A5141MIPI_write_cmos_sensor(0x31AE, 0x0101);	    // SERIAL_FORMAT
	#endif

	
	//REV4_recommended_settings
	A5141MIPI_write_cmos_sensor(0x316A, 0x8400);	// DAC_FBIAS
	A5141MIPI_write_cmos_sensor(0x316C, 0x8400);	// DAC_TXLO
	A5141MIPI_write_cmos_sensor(0x316E, 0x8400);	// DAC_ECL
	A5141MIPI_write_cmos_sensor(0x3EFA, 0x1A1F);	// DAC_LD_ECL
	A5141MIPI_write_cmos_sensor(0x3ED2, 0xD965);	// DAC_LD_6_7
	A5141MIPI_write_cmos_sensor(0x3ED8, 0x7F1B);	// DAC_LD_12_13
	A5141MIPI_write_cmos_sensor(0x3EDA, 0x2F11);	// DAC_LD_14_15
	A5141MIPI_write_cmos_sensor(0x3EE2, 0x0060);	// DAC_LD_22_23
	A5141MIPI_write_cmos_sensor(0x3EF2, 0xD965);	// DAC_LP_6_7
	A5141MIPI_write_cmos_sensor(0x3EF8, 0x797F);	// DAC_LD_TXHI
	A5141MIPI_write_cmos_sensor(0x3EFC, 0x286F);	// DAC_LD_FBIAS
	A5141MIPI_write_cmos_sensor(0x3EFE, 0x2C01);	// DAC_LD_TXLO

	//REV1_pixel_timing
	A5141MIPI_write_cmos_sensor(0x3E00, 0x042F);	// DYNAMIC_SEQRAM_00
	A5141MIPI_write_cmos_sensor(0x3E02, 0xFFFF);	// DYNAMIC_SEQRAM_02
	A5141MIPI_write_cmos_sensor(0x3E04, 0xFFFF);	// DYNAMIC_SEQRAM_04
	A5141MIPI_write_cmos_sensor(0x3E06, 0xFFFF);	// DYNAMIC_SEQRAM_06
	A5141MIPI_write_cmos_sensor(0x3E08, 0x8071);	// DYNAMIC_SEQRAM_08
	A5141MIPI_write_cmos_sensor(0x3E0A, 0x7281);	// DYNAMIC_SEQRAM_0A
	A5141MIPI_write_cmos_sensor(0x3E0C, 0x4011);	// DYNAMIC_SEQRAM_0C
	A5141MIPI_write_cmos_sensor(0x3E0E, 0x8010);	// DYNAMIC_SEQRAM_0E
	A5141MIPI_write_cmos_sensor(0x3E10, 0x60A5);	// DYNAMIC_SEQRAM_10
	A5141MIPI_write_cmos_sensor(0x3E12, 0x4080);	// DYNAMIC_SEQRAM_12
	A5141MIPI_write_cmos_sensor(0x3E14, 0x4180);	// DYNAMIC_SEQRAM_14
	A5141MIPI_write_cmos_sensor(0x3E16, 0x0018);	// DYNAMIC_SEQRAM_16
	A5141MIPI_write_cmos_sensor(0x3E18, 0x46B7);	// DYNAMIC_SEQRAM_18
	A5141MIPI_write_cmos_sensor(0x3E1A, 0x4994);	// DYNAMIC_SEQRAM_1A
	A5141MIPI_write_cmos_sensor(0x3E1C, 0x4997);	// DYNAMIC_SEQRAM_1C
	A5141MIPI_write_cmos_sensor(0x3E1E, 0x4682);	// DYNAMIC_SEQRAM_1E
	A5141MIPI_write_cmos_sensor(0x3E20, 0x0018);	// DYNAMIC_SEQRAM_20
	A5141MIPI_write_cmos_sensor(0x3E22, 0x4241);	// DYNAMIC_SEQRAM_22
	A5141MIPI_write_cmos_sensor(0x3E24, 0x8000);	// DYNAMIC_SEQRAM_24
	A5141MIPI_write_cmos_sensor(0x3E26, 0x1880);	// DYNAMIC_SEQRAM_26
	A5141MIPI_write_cmos_sensor(0x3E28, 0x4785);	// DYNAMIC_SEQRAM_28
	A5141MIPI_write_cmos_sensor(0x3E2A, 0x4992);	// DYNAMIC_SEQRAM_2A
	A5141MIPI_write_cmos_sensor(0x3E2C, 0x4997);	// DYNAMIC_SEQRAM_2C
	A5141MIPI_write_cmos_sensor(0x3E2E, 0x4780);	// DYNAMIC_SEQRAM_2E
	A5141MIPI_write_cmos_sensor(0x3E30, 0x4D80);	// DYNAMIC_SEQRAM_30
	A5141MIPI_write_cmos_sensor(0x3E32, 0x100C);	// DYNAMIC_SEQRAM_32
	A5141MIPI_write_cmos_sensor(0x3E34, 0x8000);	// DYNAMIC_SEQRAM_34
	A5141MIPI_write_cmos_sensor(0x3E36, 0x184A);	// DYNAMIC_SEQRAM_36
	A5141MIPI_write_cmos_sensor(0x3E38, 0x8042);	// DYNAMIC_SEQRAM_38
	A5141MIPI_write_cmos_sensor(0x3E3A, 0x001A);	// DYNAMIC_SEQRAM_3A
	A5141MIPI_write_cmos_sensor(0x3E3C, 0x9610);	// DYNAMIC_SEQRAM_3C
	A5141MIPI_write_cmos_sensor(0x3E3E, 0x0C80);	// DYNAMIC_SEQRAM_3E
	A5141MIPI_write_cmos_sensor(0x3E40, 0x4DC6);	// DYNAMIC_SEQRAM_40
	A5141MIPI_write_cmos_sensor(0x3E42, 0x4A80);	// DYNAMIC_SEQRAM_42
	A5141MIPI_write_cmos_sensor(0x3E44, 0x0018);	// DYNAMIC_SEQRAM_44
	A5141MIPI_write_cmos_sensor(0x3E46, 0x8042);	// DYNAMIC_SEQRAM_46
	A5141MIPI_write_cmos_sensor(0x3E48, 0x8041);	// DYNAMIC_SEQRAM_48
	A5141MIPI_write_cmos_sensor(0x3E4A, 0x0018);	// DYNAMIC_SEQRAM_4A
	A5141MIPI_write_cmos_sensor(0x3E4C, 0x804B);	// DYNAMIC_SEQRAM_4C
	A5141MIPI_write_cmos_sensor(0x3E4E, 0xB74B);	// DYNAMIC_SEQRAM_4E
	A5141MIPI_write_cmos_sensor(0x3E50, 0x8010);	// DYNAMIC_SEQRAM_50
	A5141MIPI_write_cmos_sensor(0x3E52, 0x6056);	// DYNAMIC_SEQRAM_52
	A5141MIPI_write_cmos_sensor(0x3E54, 0x001C);	// DYNAMIC_SEQRAM_54
	A5141MIPI_write_cmos_sensor(0x3E56, 0x8211);	// DYNAMIC_SEQRAM_56
	A5141MIPI_write_cmos_sensor(0x3E58, 0x8056);	// DYNAMIC_SEQRAM_58
	A5141MIPI_write_cmos_sensor(0x3E5A, 0x827C);	// DYNAMIC_SEQRAM_5A
	A5141MIPI_write_cmos_sensor(0x3E5C, 0x0970);	// DYNAMIC_SEQRAM_5C
	A5141MIPI_write_cmos_sensor(0x3E5E, 0x8082);	// DYNAMIC_SEQRAM_5E
	A5141MIPI_write_cmos_sensor(0x3E60, 0x7281);	// DYNAMIC_SEQRAM_60
	A5141MIPI_write_cmos_sensor(0x3E62, 0x4C40);	// DYNAMIC_SEQRAM_62
	A5141MIPI_write_cmos_sensor(0x3E64, 0x8E4D);	// DYNAMIC_SEQRAM_64
	A5141MIPI_write_cmos_sensor(0x3E66, 0x8110);	// DYNAMIC_SEQRAM_66
	A5141MIPI_write_cmos_sensor(0x3E68, 0x0CAF);	// DYNAMIC_SEQRAM_68
	A5141MIPI_write_cmos_sensor(0x3E6A, 0x4D80);	// DYNAMIC_SEQRAM_6A
	A5141MIPI_write_cmos_sensor(0x3E6C, 0x100C);	// DYNAMIC_SEQRAM_6C
	A5141MIPI_write_cmos_sensor(0x3E6E, 0x8440);	// DYNAMIC_SEQRAM_6E
	A5141MIPI_write_cmos_sensor(0x3E70, 0x4C81);	// DYNAMIC_SEQRAM_70
	A5141MIPI_write_cmos_sensor(0x3E72, 0x7C5F);	// DYNAMIC_SEQRAM_72
	A5141MIPI_write_cmos_sensor(0x3E74, 0x7000);	// DYNAMIC_SEQRAM_74
	A5141MIPI_write_cmos_sensor(0x3E76, 0x0000);	// DYNAMIC_SEQRAM_76
	A5141MIPI_write_cmos_sensor(0x3E78, 0x0000);	// DYNAMIC_SEQRAM_78
	A5141MIPI_write_cmos_sensor(0x3E7A, 0x0000);	// DYNAMIC_SEQRAM_7A
	A5141MIPI_write_cmos_sensor(0x3E7C, 0x0000);	// DYNAMIC_SEQRAM_7C
	A5141MIPI_write_cmos_sensor(0x3E7E, 0x0000);	// DYNAMIC_SEQRAM_7E
	A5141MIPI_write_cmos_sensor(0x3E80, 0x0000);	// DYNAMIC_SEQRAM_80
	A5141MIPI_write_cmos_sensor(0x3E82, 0x0000);	// DYNAMIC_SEQRAM_82
	A5141MIPI_write_cmos_sensor(0x3E84, 0x0000);	// DYNAMIC_SEQRAM_84
	A5141MIPI_write_cmos_sensor(0x3E86, 0x0000);	// DYNAMIC_SEQRAM_86
	A5141MIPI_write_cmos_sensor(0x3E88, 0x0000);	// DYNAMIC_SEQRAM_88
	A5141MIPI_write_cmos_sensor(0x3E8A, 0x0000);	// DYNAMIC_SEQRAM_8A
	A5141MIPI_write_cmos_sensor(0x3E8C, 0x0000);	// DYNAMIC_SEQRAM_8C
	A5141MIPI_write_cmos_sensor(0x3E8E, 0x0000);	// DYNAMIC_SEQRAM_8E
	A5141MIPI_write_cmos_sensor(0x3E90, 0x0000);	// DYNAMIC_SEQRAM_90
	A5141MIPI_write_cmos_sensor(0x3E92, 0x0000);	// DYNAMIC_SEQRAM_92
	A5141MIPI_write_cmos_sensor(0x3E94, 0x0000);	// DYNAMIC_SEQRAM_94
	A5141MIPI_write_cmos_sensor(0x3E96, 0x0000);	// DYNAMIC_SEQRAM_96
	A5141MIPI_write_cmos_sensor(0x3E98, 0x0000);	// DYNAMIC_SEQRAM_98
	A5141MIPI_write_cmos_sensor(0x3E9A, 0x0000);	// DYNAMIC_SEQRAM_9A
	A5141MIPI_write_cmos_sensor(0x3E9C, 0x0000);	// DYNAMIC_SEQRAM_9C
	A5141MIPI_write_cmos_sensor(0x3E9E, 0x0000);	// DYNAMIC_SEQRAM_9E
	A5141MIPI_write_cmos_sensor(0x3EA0, 0x0000);	// DYNAMIC_SEQRAM_A0
	A5141MIPI_write_cmos_sensor(0x3EA2, 0x0000);	// DYNAMIC_SEQRAM_A2
	A5141MIPI_write_cmos_sensor(0x3EA4, 0x0000);	// DYNAMIC_SEQRAM_A4
	A5141MIPI_write_cmos_sensor(0x3EA6, 0x0000);	// DYNAMIC_SEQRAM_A6
	A5141MIPI_write_cmos_sensor(0x3EA8, 0x0000);	// DYNAMIC_SEQRAM_A8
	A5141MIPI_write_cmos_sensor(0x3EAA, 0x0000);	// DYNAMIC_SEQRAM_AA
	A5141MIPI_write_cmos_sensor(0x3EAC, 0x0000);	// DYNAMIC_SEQRAM_AC
	A5141MIPI_write_cmos_sensor(0x3EAE, 0x0000);	// DYNAMIC_SEQRAM_AE
	A5141MIPI_write_cmos_sensor(0x3EB0, 0x0000);	// DYNAMIC_SEQRAM_B0
	A5141MIPI_write_cmos_sensor(0x3EB2, 0x0000);	// DYNAMIC_SEQRAM_B2
	A5141MIPI_write_cmos_sensor(0x3EB4, 0x0000);	// DYNAMIC_SEQRAM_B4
	A5141MIPI_write_cmos_sensor(0x3EB6, 0x0000);	// DYNAMIC_SEQRAM_B6
	A5141MIPI_write_cmos_sensor(0x3EB8, 0x0000);	// DYNAMIC_SEQRAM_B8
	A5141MIPI_write_cmos_sensor(0x3EBA, 0x0000);	// DYNAMIC_SEQRAM_BA
	A5141MIPI_write_cmos_sensor(0x3EBC, 0x0000);	// DYNAMIC_SEQRAM_BC
	A5141MIPI_write_cmos_sensor(0x3EBE, 0x0000);	// DYNAMIC_SEQRAM_BE
	A5141MIPI_write_cmos_sensor(0x3EC0, 0x0000);	// DYNAMIC_SEQRAM_C0
	A5141MIPI_write_cmos_sensor(0x3EC2, 0x0000);	// DYNAMIC_SEQRAM_C2
	A5141MIPI_write_cmos_sensor(0x3EC4, 0x0000);	// DYNAMIC_SEQRAM_C4
	A5141MIPI_write_cmos_sensor(0x3EC6, 0x0000);	// DYNAMIC_SEQRAM_C6
	A5141MIPI_write_cmos_sensor(0x3EC8, 0x0000);	// DYNAMIC_SEQRAM_C8
	A5141MIPI_write_cmos_sensor(0x3ECA, 0x0000);	// DYNAMIC_SEQRAM_CA

	// dynamic power disable
	A5141MIPI_write_cmos_sensor(0x3170, 0x2150);	// ANALOG_CONTROL
	A5141MIPI_write_cmos_sensor(0x317A, 0x0150);	// ANALOG_CONTROL6
	A5141MIPI_write_cmos_sensor(0x3ECC, 0x2200);	// DAC_LD_0_1
	A5141MIPI_write_cmos_sensor(0x3174, 0x0000);	// ANALOG_CONTROL3
	A5141MIPI_write_cmos_sensor(0x3176, 0x0000);	// ANALOG_CONTROL4

	A5141MIPI_write_cmos_sensor(0x30BC, 0x0384);
	A5141MIPI_write_cmos_sensor(0x30C0, 0x1220);
	A5141MIPI_write_cmos_sensor(0x30D4, 0x9200);
	A5141MIPI_write_cmos_sensor(0x30B2, 0xC000);
	
	A5141MIPI_write_cmos_sensor(0x31B0, 0x00C4);
	A5141MIPI_write_cmos_sensor(0x31B2, 0x0064);
	A5141MIPI_write_cmos_sensor(0x31B4, 0x0E77);
	A5141MIPI_write_cmos_sensor(0x31B6, 0x0D24);
	A5141MIPI_write_cmos_sensor(0x31B8, 0x020E);
	A5141MIPI_write_cmos_sensor(0x31BA, 0x0710);
	A5141MIPI_write_cmos_sensor(0x31BC, 0x2A0D);
	A5141MIPI_write_cmos_sensor(0x31BE, 0xC007);

	//A5141MIPI_write_cmos_sensor(0x3ECE, 0x0000);	// DAC_LD_2_3
	//A5141MIPI_write_cmos_sensor(0x0400, 0x0000);	// SCALING_MODE disable scale
	//A5141MIPI_write_cmos_sensor(0x0404, 0x0010);	// SCALE_M
	
	A5141MIPI_write_cmos_sensor(0x305E, 0x112E);  	// global gain
	A5141MIPI_write_cmos_sensor(0x30F0, 0x8000);  	// enable AF


	//PLL MCLK = 26MHZ, PCLK = 104MHZ, VT = 104MHZ
	#ifdef RAW10
		A5141MIPI_write_cmos_sensor(0x0300, 0x05);	//vt_pix_clk_div = 5
		A5141MIPI_write_cmos_sensor(0x0302, 0x01);	//vt_sys_clk_div = 1
		A5141MIPI_write_cmos_sensor(0x0304, 0x02);	//pre_pll_clk_div = 2
		A5141MIPI_write_cmos_sensor(0x0306, 0x28);	//pll_multiplier    =  40
		A5141MIPI_write_cmos_sensor(0x0308, 0x0A);	//op_pix_clk_div =  10
		A5141MIPI_write_cmos_sensor(0x030A, 0x01);	//op_sys_clk_div = 1
	#else
		#ifdef MIPI_INTERFACE
			A5141MIPI_write_cmos_sensor(0x0300, 0x04);	//vt_pix_clk_div = 8
		#else
			A5141MIPI_write_cmos_sensor(0x0300, 0x08);	//vt_pix_clk_div = 8
		#endif
		A5141MIPI_write_cmos_sensor(0x0302, 0x01);	//vt_sys_clk_div = 1
		A5141MIPI_write_cmos_sensor(0x0304, 0x02);	//pre_pll_clk_div = 2
		A5141MIPI_write_cmos_sensor(0x0306, 0x20);	//pll_multiplier    =  32
		A5141MIPI_write_cmos_sensor(0x0308, 0x08);	//op_pix_clk_div =  8
		A5141MIPI_write_cmos_sensor(0x030A, 0x01);	//op_sys_clk_div = 1
	#endif

	//A5141MIPI_write_cmos_sensor(0x306E, 0xbc00);	// slew rate for color issue
	//A5141MIPI_write_cmos_sensor(0x3040, 0x04C3); 
	//A5141MIPI_write_cmos_sensor(0x3010, 0x0184); 	// FINE_CORRECTION
	//A5141MIPI_write_cmos_sensor(0x3012, 0x029C); 	// COARSE_INTEGRATION_TIME_
	//A5141MIPI_write_cmos_sensor(0x3014, 0x0908); 	// FINE_INTEGRATION_TIME_
	//A5141MIPI_write_cmos_sensor_8(0x0100, 0x01); 	// MODE_SELECT

	mDELAY(5);				// Allow PLL to lock
}   /*  A5141MIPI_Sensor_Init  */

kal_uint16 A5141MIPI_PowerOn(void)
{
	const kal_uint8 i2c_addr[] = {A5141MIPI_WRITE_ID_1, A5141MIPI_WRITE_ID_2, 0x6C, 0x6E}; 
	kal_uint16 sensor_id = 0xffff;
	kal_uint16 i;
	
	for(i = 0; i < sizeof(i2c_addr) / sizeof(i2c_addr[0]); i++)
	{
		spin_lock(&a5141mipiraw_drv_lock);
		A5141MIPI_sensor.i2c_write_id = i2c_addr[i];
		spin_unlock(&a5141mipiraw_drv_lock);
		SENSORDB( "i2c address is %x \n", A5141MIPI_sensor.i2c_write_id);
		
		sensor_id = A5141MIPI_read_cmos_sensor(0x0000);
		if(sensor_id == A5141MIPI_SENSOR_ID)
			break;
	}
	
	return sensor_id;
}
/*************************************************************************
* FUNCTION
*   A5141MIPIOpen
*
* DESCRIPTION
*   This function initialize the registers of CMOS sensor
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/

UINT32 A5141MIPIOpen(void)
{
	kal_uint16 sensor_id = 0;
	
	sensor_id = A5141MIPI_PowerOn() ;
	
	SENSORDB("A5141MIPIOpen sensor_id is %x \n", sensor_id);
    if (sensor_id != A5141MIPI_SENSOR_ID)
        return ERROR_SENSOR_CONNECT_FAIL;

    A5141MIPI_Init_setting();

	spin_lock(&a5141mipiraw_drv_lock);
    A5141MIPI_sensor_gain_base = read_A5141MIPI_gain();
    g_iA5141MIPI_Mode = A5141MIPI_MODE_PREVIEW;
	spin_unlock(&a5141mipiraw_drv_lock);
	
    return ERROR_NONE;
}


/*************************************************************************
* FUNCTION
*   A5141MIPIGetSensorID
*
* DESCRIPTION
*   This function get the sensor ID 
*
* PARAMETERS
*   *sensorID : return the sensor ID 
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 A5141MIPIGetSensorID(UINT32 *sensorID) 
{
	*sensorID  = A5141MIPI_PowerOn() ;
		
	SENSORDB("A5141MIPIGetSensorID sensor_id is %x\n", *sensorID );
 
    if (*sensorID != A5141MIPI_SENSOR_ID) {
        *sensorID = 0xFFFFFFFF; 
        return ERROR_SENSOR_CONNECT_FAIL;
    }
	
    return ERROR_NONE;
}


/*************************************************************************
* FUNCTION
*   A5141MIPI_SetShutter
*
* DESCRIPTION
*   This function set e-shutter of A5141MIPI to change exposure time.
*
* PARAMETERS
*   shutter : exposured lines
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void A5141MIPI_SetShutter(kal_uint16 iShutter)
{
	unsigned long flags;

	SENSORDB("--*--[%s]: iShutter: %u\n", __FUNCTION__, iShutter);

	spin_lock_irqsave(&a5141mipiraw_drv_lock, flags);
	if(A5141MIPI_exposure_lines == iShutter){
		spin_unlock_irqrestore(&a5141mipiraw_drv_lock, flags);
		return;
	}
	A5141MIPI_exposure_lines=iShutter;
	spin_unlock_irqrestore(&a5141mipiraw_drv_lock, flags);
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x01); 	// GROUPED_PARAMETER_HOLD
	A5141MIPI_write_cmos_sensor(0x0202, iShutter);  /* course_integration_time */
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x00); 	// GROUPED_PARAMETER_HOLD
}



/*************************************************************************
* FUNCTION
*   A5141MIPI_read_shutter
*
* DESCRIPTION
*   This function to  Get exposure time.
*
* PARAMETERS
*   None
*
* RETURNS
*   shutter : exposured lines
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT16 A5141MIPI_read_shutter(void)
{
    kal_uint16 ishutter;
	
	ishutter = A5141MIPI_read_cmos_sensor(0x0202); /* course_integration_time */
	
	return ishutter;
}

/*************************************************************************
* FUNCTION
*   A5141MIPI_night_mode
*
* DESCRIPTION
*   This function night mode of A5141MIPI.
*
* PARAMETERS
*   none
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void A5141MIPI_NightMode(kal_bool bEnable)
{
    // frame rate will be control by AE table
    
}



/*************************************************************************
* FUNCTION
*   A5141MIPIClose
*
* DESCRIPTION
*   This function is to turn off sensor module power.
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 A5141MIPIClose(void)
{
    return ERROR_NONE;
}	/* A5141MIPIClose() */

void A5141MIPI_Set_Mirror_Flip(kal_uint8 image_mirror)
{
	switch (image_mirror)
	{
		case IMAGE_NORMAL:
			A5141MIPI_write_cmos_sensor_8(0x0101,0x00);
		break;
		case IMAGE_H_MIRROR:
			A5141MIPI_write_cmos_sensor_8(0x0101,0x01);
		break;
		case IMAGE_V_MIRROR:
			A5141MIPI_write_cmos_sensor_8(0x0101,0x02);
		break;
		case IMAGE_HV_MIRROR:
			A5141MIPI_write_cmos_sensor_8(0x0101,0x03);
		break;
	}
}


static void A5141MIPI_preview_setting(void)
{
	//A5141MIPI_write_cmos_sensor_8(0x0100, 0x0); 	// MODE_SELECT
	kal_uint16 temp;
	//1296 x 972  Timing settings 30fps
	#ifdef MIPI_INTERFACE
		#ifdef RAW10
			//A5141MIPI_write_cmos_sensor(0x301A, 0x0018);	// enable mipi interface
			A5141MIPI_write_cmos_sensor(0x3064, 0xB800);	// SMIA_TEST
			A5141MIPI_write_cmos_sensor(0x31AE, 0x0202);	// two lane 201 tow 202
			A5141MIPI_write_cmos_sensor(0x0112, 0x0A0A);	// 10bit raw output
		#else
			//A5141MIPI_write_cmos_sensor(0x301A, 0x0018);	// enable mipi interface
			A5141MIPI_write_cmos_sensor(0x3064, 0x0805);	// SMIA_TEST
			A5141MIPI_write_cmos_sensor(0x31AE, 0x0202);	// two lane
			A5141MIPI_write_cmos_sensor(0x0112, 0x0808);	// 8bit raw output
		#endif
	#endif

	//PLL MCLK=26MHZ, PCLK = 104MHZ, VT = 104MHZ
	#ifdef RAW10
		A5141MIPI_write_cmos_sensor(0x0300, 0x05);	//vt_pix_clk_div = 5
		A5141MIPI_write_cmos_sensor(0x0302, 0x01);	//vt_sys_clk_div = 1
		A5141MIPI_write_cmos_sensor(0x0304, 0x02);	//pre_pll_clk_div = 2
		A5141MIPI_write_cmos_sensor(0x0306, 0x28);	//pll_multiplier    =  40
		A5141MIPI_write_cmos_sensor(0x0308, 0x0A);	//op_pix_clk_div =  10
		A5141MIPI_write_cmos_sensor(0x030A, 0x01);	//op_sys_clk_div = 1
	#else	
		#ifdef MIPI_INTERFACE
			A5141MIPI_write_cmos_sensor(0x0300, 0x04);	//vt_pix_clk_div = 4
		#else
			A5141MIPI_write_cmos_sensor(0x0300, 0x08);	//vt_pix_clk_div = 8
		#endif
		A5141MIPI_write_cmos_sensor(0x0302, 0x01);	//vt_sys_clk_div = 1
		A5141MIPI_write_cmos_sensor(0x0304, 0x02);	//pre_pll_clk_div = 2
		A5141MIPI_write_cmos_sensor(0x0306, 0x20);	//pll_multiplier    =  32
		A5141MIPI_write_cmos_sensor(0x0308, 0x08);	//op_pix_clk_div =  8
		A5141MIPI_write_cmos_sensor(0x030A, 0x01);	//op_sys_clk_div = 1
	#endif
	
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x01); 	// GROUPED_PARAMETER_HOLD
	A5141MIPI_write_cmos_sensor(0x0344, 0x0008); 	// X_ADDR_START   =  8
	A5141MIPI_write_cmos_sensor(0x0346, 0x0008); 	// Y_ADDR_START   =  8
	A5141MIPI_write_cmos_sensor(0x0348, 0x0A25); 	// X_ADDR_END      = 2597
	A5141MIPI_write_cmos_sensor(0x034A, 0x079D); 	// Y_ADDR_END       =  1949
	temp = A5141MIPI_read_cmos_sensor(0x3040);
	temp = temp & 0xF000;
	temp = temp | 0x04C3 ;
	A5141MIPI_write_cmos_sensor(0x3040, temp);	// READ_MODE  10 011 000011 xy binning enable xodd=3, yodd=3
	A5141MIPI_write_cmos_sensor(0x034C, 0x0510); 	// X_OUTPUT_SIZE    = 1296
	A5141MIPI_write_cmos_sensor(0x034E, 0x03CC); 	// Y_OUTPUT_SIZE    =  972

	A5141MIPI_write_cmos_sensor(0x300C, 0x0C4C); 	// LINE_LENGTH  3151
	A5141MIPI_write_cmos_sensor(0x300A, 0x0415); 	// FRAME_LINEs  1100
	
	A5141MIPI_write_cmos_sensor(0x3012, 0x0414); 	// coarse_integration_time
	A5141MIPI_write_cmos_sensor(0x3014, 0x0908); 	// fine_integration_time
	A5141MIPI_write_cmos_sensor(0x3010, 0x0184); 	// fine_correction
	
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x00); 	// GROUPED_PARAMETER_HOLD

	//start_streaming
	A5141MIPI_write_cmos_sensor_8(0x0100, 0x01); 	// MODE_SELECT

	spin_lock(&a5141mipiraw_drv_lock);
	#ifdef MIPI_INTERFACE
		A5141MIPI_sensor.preview_vt_clk = 1040;
	#else
		A5141MIPI_sensor.preview_vt_clk = 520;
	#endif
	spin_unlock(&a5141mipiraw_drv_lock);

	mDELAY(50);	
}

static void A5141MIPI_capture_setting(void)
{
	//[2592 X 1944]
	kal_uint16 temp;
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x01); //Grouped Parameter Hold = 0x1
	//stop_streaming
	//A5141MIPI_write_cmos_sensor_8(0x0100, 0x00); 	// MODE_SELECT stop streaming
	#ifdef MIPI_INTERFACE
		#ifdef RAW10
			//A5141MIPI_write_cmos_sensor(0x301A, 0x0018);	// enable mipi interface
			A5141MIPI_write_cmos_sensor(0x3064, 0xB800);	// SMIA_TEST
			A5141MIPI_write_cmos_sensor(0x31AE, 0x0202);	// two lane 201 tow 202
			A5141MIPI_write_cmos_sensor(0x0112, 0x0A0A);	// 10bit raw output
		#else
			//A5141MIPI_write_cmos_sensor(0x301A, 0x0018);	// enable mipi interface
			A5141MIPI_write_cmos_sensor(0x3064, 0x0805);	// SMIA_TEST
			A5141MIPI_write_cmos_sensor(0x31AE, 0x0202);	// two lane
			A5141MIPI_write_cmos_sensor(0x0112, 0x0808);	// 8bit raw output
		#endif
	#endif

	//PLL MCLK=26MHZ, PCLK = 111.8MHZ, VT = 111.8MHZ
	#ifdef RAW10
		A5141MIPI_write_cmos_sensor(0x0300, 0x05);	//vt_pix_clk_div = 5
		A5141MIPI_write_cmos_sensor(0x0302, 0x01);	//vt_sys_clk_div = 1
		A5141MIPI_write_cmos_sensor(0x0304, 0x04);	//pre_pll_clk_div = 4
		A5141MIPI_write_cmos_sensor(0x0306, 0x56);	//pll_multiplier    =  86
		A5141MIPI_write_cmos_sensor(0x0308, 0x0A);	//op_pix_clk_div =  10
		A5141MIPI_write_cmos_sensor(0x030A, 0x01);	//op_sys_clk_div = 1
	#else		
		#ifdef MIPI_INTERFACE
			A5141MIPI_write_cmos_sensor(0x0300, 0x04);	//vt_pix_clk_div = 8
		#else
			A5141MIPI_write_cmos_sensor(0x0300, 0x08);	//vt_pix_clk_div = 8
		#endif
		A5141MIPI_write_cmos_sensor(0x0302, 0x01);	//vt_sys_clk_div = 1
		A5141MIPI_write_cmos_sensor(0x0304, 0x02);	//pre_pll_clk_div = 2
		A5141MIPI_write_cmos_sensor(0x0306, 0x20);	//pll_multiplier    =  32
		A5141MIPI_write_cmos_sensor(0x0308, 0x08);	//op_pix_clk_div =  8
		A5141MIPI_write_cmos_sensor(0x030A, 0x01);	//op_sys_clk_div = 1
	#endif


	A5141MIPI_write_cmos_sensor(0x0344, 0x0008);	//X_ADDR_START   = 8
	A5141MIPI_write_cmos_sensor(0x0346, 0x0008);	//Y_ADDR_START    = 8
	A5141MIPI_write_cmos_sensor(0x0348, 0x0A27);	//X_ADDR_END =  2599
	A5141MIPI_write_cmos_sensor(0x034A, 0x079F);	//Y_ADDR_END = 1951

	temp = A5141MIPI_read_cmos_sensor(0x3040);
	temp = temp & 0xF000 ;
	temp = temp | 0x0041;
	A5141MIPI_write_cmos_sensor(0x3040, temp);	//Read Mode = 0x41   1 000001 binning disable

	A5141MIPI_write_cmos_sensor(0x034C, 0x0A20);	//X_OUTPUT_SIZE= 2592
	A5141MIPI_write_cmos_sensor(0x034E, 0x0798);	//Y_OUTPUT_SIZE = 1944
	A5141MIPI_write_cmos_sensor(0x300A, 0x07E5);	//Frame Lines = 0x7E5  2021
	A5141MIPI_write_cmos_sensor(0x300C, 0x0E6E);	//Line Length = 0xE6E  3694
	A5141MIPI_write_cmos_sensor(0x3010, 0x00A0);	//Fine Correction = 0xA0
	//A5141MIPI_write_cmos_sensor(0x3012, 0x07E4);	//Coarse integration Time = 0x7E4
	A5141MIPI_write_cmos_sensor(0x3014, 0x0C8C);	//Fine Integration Time = 0xC8C

	A5141MIPI_write_cmos_sensor_8(0x0104, 0x00); //Grouped Parameter Hold = 0x0
	
	A5141MIPI_write_cmos_sensor_8(0x0100, 0x01);	//mode_select (Open streaming)

	spin_lock(&a5141mipiraw_drv_lock);
	#ifdef MIPI_INTERFACE
		A5141MIPI_sensor.capture_vt_clk = 1118;
	#else
		A5141MIPI_sensor.capture_vt_clk = 520;
	#endif
	spin_unlock(&a5141mipiraw_drv_lock);

	mDELAY(10);
}


/*************************************************************************
* FUNCTION
*   A5141MIPI_SetDummy
*
* DESCRIPTION
*   This function initialize the registers of CMOS sensor
*
* PARAMETERS
*   mode  ture : preview mode
*             false : capture mode
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/

static void A5141MIPI_SetDummy(kal_bool mode,const kal_uint16 iDummyPixels, const kal_uint16 iDummyLines)
{
    kal_uint16 Line_length_pclk, Frame_length_lines;
    
	if(mode == KAL_TRUE) //preview
	{
		Line_length_pclk   = A5141MIPI_PV_PERIOD_PIXEL_NUMS + iDummyPixels;
		Frame_length_lines = A5141MIPI_PV_PERIOD_LINE_NUMS  + iDummyLines;
	}
	else   //capture
	{
		Line_length_pclk   = A5141MIPI_FULL_PERIOD_PIXEL_NUMS + iDummyPixels;
		Frame_length_lines = A5141MIPI_FULL_PERIOD_LINE_NUMS  + iDummyLines;
	}
	
		spin_lock(&a5141mipiraw_drv_lock);
		A5141_Frame_Length_preview = Frame_length_lines;
		spin_unlock(&a5141mipiraw_drv_lock);
    SENSORDB("Enter A5141MIPI_SetDummy Frame_length_lines=%d, Line_length_pclk=%d\n",Frame_length_lines,Line_length_pclk);

	A5141MIPI_write_cmos_sensor_8(0x0104, 0x01);// GROUPED_PARAMETER_HOLD
	A5141MIPI_write_cmos_sensor(0x0340, Frame_length_lines);
	A5141MIPI_write_cmos_sensor(0x0342, Line_length_pclk);
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x00); //Grouped Parameter Hold = 0x00
	
}   /*  A5141MIPI_SetDummy */


/*************************************************************************
* FUNCTION
*   A5141MIPIPreview
*
* DESCRIPTION
*   This function start the sensor preview.
*
* PARAMETERS
*   *image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 A5141MIPIPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                                                MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	kal_uint32 temp;
		
	//sensor_config_data->SensorImageMirror = IMAGE_NORMAL;
	A5141MIPI_Set_Mirror_Flip(sensor_config_data->SensorImageMirror);

	A5141MIPI_preview_setting();

	spin_lock(&a5141mipiraw_drv_lock);
    A5141MIPI_PV_dummy_pixels = 0;
	A5141MIPI_PV_dummy_lines  = 0;
	spin_unlock(&a5141mipiraw_drv_lock);
	A5141MIPI_SetDummy(KAL_TRUE, A5141MIPI_PV_dummy_pixels, A5141MIPI_PV_dummy_lines);

    image_window->GrabStartX= A5141MIPI_PV_START_X;
    image_window->GrabStartY= A5141MIPI_PV_START_Y;
    image_window->ExposureWindowWidth= A5141MIPI_IMAGE_SENSOR_PV_WIDTH;
    image_window->ExposureWindowHeight= A5141MIPI_IMAGE_SENSOR_PV_HEIGHT;

	spin_lock(&a5141mipiraw_drv_lock);
	g_iA5141MIPI_Mode = A5141MIPI_MODE_PREVIEW;
	temp = A5141MIPI_exposure_lines;
	spin_unlock(&a5141mipiraw_drv_lock);
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x01); 	// GROUPED_PARAMETER_HOLD
	A5141MIPI_write_cmos_sensor(0x0202, temp); /* course_integration_time */
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x00); 	// GROUPED_PARAMETER_HOLD

	memcpy(&A5141MIPISensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

    return ERROR_NONE;
}	/* A5141MIPIPreview() */



/*******************************************************************************
*
********************************************************************************/
UINT32 A5141MIPICapture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                                                MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    kal_uint32 shutter = A5141MIPI_exposure_lines;

	if ((image_window->ImageTargetWidth<= A5141MIPI_IMAGE_SENSOR_PV_WIDTH) &&
		  (image_window->ImageTargetHeight<= A5141MIPI_IMAGE_SENSOR_PV_HEIGHT))
	{	
		SENSORDB("Enter A5141MIPICapture small size  \n ");

		spin_lock(&a5141mipiraw_drv_lock);
		A5141MIPI_sensor.capture_vt_clk = A5141MIPI_sensor.preview_vt_clk;
		spin_unlock(&a5141mipiraw_drv_lock);

		spin_lock(&a5141mipiraw_drv_lock);
		A5141MIPI_dummy_pixels = 0;
		A5141MIPI_dummy_lines  = 0;
		spin_unlock(&a5141mipiraw_drv_lock);
		A5141MIPI_SetDummy(KAL_TRUE, A5141MIPI_dummy_pixels, A5141MIPI_dummy_lines);
		
		shutter = shutter * (A5141MIPI_PV_PERIOD_PIXEL_NUMS + A5141MIPI_PV_dummy_pixels)/(A5141MIPI_PV_PERIOD_PIXEL_NUMS +A5141MIPI_dummy_pixels);
		shutter = shutter * A5141MIPI_sensor.capture_vt_clk / A5141MIPI_sensor.preview_vt_clk;
		
		image_window->GrabStartX           = A5141MIPI_PV_START_X;
        image_window->GrabStartY           = A5141MIPI_PV_START_Y;
        image_window->ExposureWindowWidth  = A5141MIPI_IMAGE_SENSOR_PV_WIDTH;
        image_window->ExposureWindowHeight = A5141MIPI_IMAGE_SENSOR_PV_HEIGHT;
	}
	else  //5M
	{
		spin_lock(&a5141mipiraw_drv_lock);
		if(g_iA5141MIPI_Mode == A5141MIPI_MODE_CAPTURE) {
			spin_unlock(&a5141mipiraw_drv_lock);
	       	return;
		}
		g_iA5141MIPI_Mode = A5141MIPI_MODE_CAPTURE;
		spin_unlock(&a5141mipiraw_drv_lock);
		
		SENSORDB("Enter A5141MIPICapture full size  \n ");

		A5141MIPI_capture_setting();

		spin_lock(&a5141mipiraw_drv_lock);
		A5141MIPI_dummy_pixels = 0;
		A5141MIPI_dummy_lines  = 0;
		spin_unlock(&a5141mipiraw_drv_lock);
		A5141MIPI_SetDummy(KAL_FALSE, A5141MIPI_dummy_pixels, A5141MIPI_dummy_lines);

		SENSORDB("preview shutter =%d \n",shutter);

		shutter = shutter * (A5141MIPI_PV_PERIOD_PIXEL_NUMS + A5141MIPI_PV_dummy_pixels)/(A5141MIPI_FULL_PERIOD_PIXEL_NUMS +A5141MIPI_dummy_pixels);
		shutter = shutter * A5141MIPI_sensor.capture_vt_clk / A5141MIPI_sensor.preview_vt_clk;

		SENSORDB("capture  shutter =%d , gain = %d\n",shutter, read_A5141MIPI_gain());

		image_window->GrabStartX           = A5141MIPI_FULL_START_X;
        image_window->GrabStartY           = A5141MIPI_FULL_START_Y;
        image_window->ExposureWindowWidth  = A5141MIPI_IMAGE_SENSOR_FULL_WIDTH;
        image_window->ExposureWindowHeight = A5141MIPI_IMAGE_SENSOR_FULL_HEIGHT;
	}

	if(shutter < 1)
	    shutter = 1;
	
    A5141MIPI_write_cmos_sensor_8(0x0104, 0x01); 	// GROUPED_PARAMETER_HOLD
	A5141MIPI_write_cmos_sensor(0x0202, shutter); 	/* coarse_integration_time */
	A5141MIPI_write_cmos_sensor_8(0x0104, 0x00); 	// GROUPED_PARAMETER_HOLD
	
	memcpy(&A5141MIPISensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	
    return ERROR_NONE;
}	/* A5141MIPICapture() */

UINT32 A5141MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
    pSensorResolution->SensorFullWidth     =  A5141MIPI_IMAGE_SENSOR_FULL_WIDTH;
    pSensorResolution->SensorFullHeight    =  A5141MIPI_IMAGE_SENSOR_FULL_HEIGHT;
    pSensorResolution->SensorPreviewWidth  =  A5141MIPI_IMAGE_SENSOR_PV_WIDTH;
    pSensorResolution->SensorPreviewHeight =  A5141MIPI_IMAGE_SENSOR_PV_HEIGHT;

    return ERROR_NONE;
}   /* A5141MIPIGetResolution() */

UINT32 A5141MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
                                                MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
                                                MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{

	switch(ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
			pSensorInfo->SensorPreviewResolutionX=A5141MIPI_IMAGE_SENSOR_FULL_WIDTH;
			pSensorInfo->SensorPreviewResolutionY=A5141MIPI_IMAGE_SENSOR_FULL_HEIGHT;
			pSensorInfo->SensorCameraPreviewFrameRate=15;
		break;

		default:
        	pSensorInfo->SensorPreviewResolutionX=A5141MIPI_IMAGE_SENSOR_PV_WIDTH;
       		pSensorInfo->SensorPreviewResolutionY=A5141MIPI_IMAGE_SENSOR_PV_HEIGHT;
			pSensorInfo->SensorCameraPreviewFrameRate=30;
		break;
	}
    pSensorInfo->SensorFullResolutionX    =  A5141MIPI_IMAGE_SENSOR_FULL_WIDTH;
    pSensorInfo->SensorFullResolutionY    =  A5141MIPI_IMAGE_SENSOR_FULL_HEIGHT;

    pSensorInfo->SensorVideoFrameRate=30;
    pSensorInfo->SensorStillCaptureFrameRate=15;
    pSensorInfo->SensorWebCamCaptureFrameRate=15;
    pSensorInfo->SensorResetActiveHigh=FALSE;
    pSensorInfo->SensorResetDelayCount=5;
    pSensorInfo->SensorOutputDataFormat     = SENSOR_OUTPUT_FORMAT_RAW_B;
    pSensorInfo->SensorClockPolarity        = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorHsyncPolarity        = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorVsyncPolarity        = SENSOR_CLOCK_POLARITY_HIGH;
    pSensorInfo->SensorInterruptDelayLines  = 1;
    
	#ifdef MIPI_INTERFACE
   		pSensorInfo->SensroInterfaceType        = SENSOR_INTERFACE_TYPE_MIPI;
   	#else
   		pSensorInfo->SensroInterfaceType		= SENSOR_INTERFACE_TYPE_PARALLEL;
   	#endif
    pSensorInfo->SensorDriver3D = 0;   // the sensor driver is 2D

    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxWidth=CAM_SIZE_2M_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxHeight=CAM_SIZE_2M_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].ISOSupported=TRUE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].BinningEnable=FALSE;

    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxWidth=CAM_SIZE_2M_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxHeight=CAM_SIZE_2M_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].ISOSupported=TRUE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].BinningEnable=FALSE;

    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxWidth=CAM_SIZE_2M_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxHeight=CAM_SIZE_2M_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].ISOSupported=FALSE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].BinningEnable=FALSE;

    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxWidth=CAM_SIZE_05M_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxHeight=CAM_SIZE_1M_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].ISOSupported=FALSE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].BinningEnable=TRUE;

    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxWidth=CAM_SIZE_05M_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxHeight=CAM_SIZE_05M_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].ISOSupported=FALSE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].BinningEnable=TRUE;

    pSensorInfo->CaptureDelayFrame = 1; 
    pSensorInfo->PreviewDelayFrame = 2; 
    pSensorInfo->VideoDelayFrame = 5; 
    pSensorInfo->SensorMasterClockSwitch = 0; 
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_6MA;      
    pSensorInfo->AEShutDelayFrame = 0;		    /* The frame of setting shutter default 0 for TG int */
    pSensorInfo->AESensorGainDelayFrame = 1;    /* The frame of setting sensor gain */
    pSensorInfo->AEISPGainDelayFrame = 2;	
	   
    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
            pSensorInfo->SensorClockFreq=26;
            pSensorInfo->SensorClockDividCount=	3;
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2;
            pSensorInfo->SensorPixelClockCount= 3;
            pSensorInfo->SensorDataLatchCount= 2;
            pSensorInfo->SensorGrabStartX = A5141MIPI_PV_START_X; 
            pSensorInfo->SensorGrabStartY = A5141MIPI_PV_START_Y;    
            
			#ifdef MIPI_INTERFACE
	            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE;			
	            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
		        pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
		        pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	            pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
	            pSensorInfo->SensorHightSampling = 0;   // 0 is default 1x 
	            pSensorInfo->SensorPacketECCOrder = 1;
	        #endif
            break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
            pSensorInfo->SensorClockFreq=26;
            pSensorInfo->SensorClockDividCount=	3;
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2;
            pSensorInfo->SensorPixelClockCount= 3;
            pSensorInfo->SensorDataLatchCount= 2;
            pSensorInfo->SensorGrabStartX = A5141MIPI_FULL_START_X; 
            pSensorInfo->SensorGrabStartY = A5141MIPI_FULL_START_X;   
            
			#ifdef MIPI_INTERFACE
	            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE;			
	            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
	            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
	            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0; 
	            pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
	            pSensorInfo->SensorHightSampling = 0;   // 0 is default 1x
	            pSensorInfo->SensorPacketECCOrder = 1;
	        #endif
            break;
        default:
            pSensorInfo->SensorClockFreq=26;
            pSensorInfo->SensorClockDividCount=	3;
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2;
            pSensorInfo->SensorPixelClockCount= 3;
            pSensorInfo->SensorDataLatchCount= 2;
            pSensorInfo->SensorGrabStartX = A5141MIPI_PV_START_X; 
            pSensorInfo->SensorGrabStartY = A5141MIPI_PV_START_X;             
            break;
    }

   // A5141MIPIPixelClockDivider=pSensorInfo->SensorPixelClockCount;
    memcpy(pSensorConfigData, &A5141MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

    return ERROR_NONE;
}   /* A5141MIPIGetInfo() */


UINT32 A5141MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
                                                MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	spin_lock(&a5141mipiraw_drv_lock);
	A5141_CurrentScenarioId = ScenarioId;
	spin_unlock(&a5141mipiraw_drv_lock);
    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
            A5141MIPIPreview(pImageWindow, pSensorConfigData);
            break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
            A5141MIPICapture(pImageWindow, pSensorConfigData);
            break;
        default:
            return ERROR_INVALID_SCENARIO_ID;
           
    }
	
    return TRUE;
} /* A5141MIPIControl() */


UINT32 A5141MIPISetVideoMode(UINT16 u2FrameRate)
{
	kal_uint16 MAX_Frame_length =0;

	SENSORDB("A5141MIPISetVideoMode u2FrameRate =%d",u2FrameRate);

	if(u2FrameRate >30 || u2FrameRate <5)
	    SENSORDB("Error frame rate seting");

    if (A5141MIPI_MODE_PREVIEW == g_iA5141MIPI_Mode)
    {
	MAX_Frame_length = A5141MIPI_sensor.preview_vt_clk*100000/(A5141MIPI_PV_PERIOD_PIXEL_NUMS+A5141MIPI_PV_dummy_pixels)/u2FrameRate;
	if(MAX_Frame_length < A5141MIPI_PV_PERIOD_LINE_NUMS )
		MAX_Frame_length = A5141MIPI_PV_PERIOD_LINE_NUMS;

	spin_lock(&a5141mipiraw_drv_lock);
	A5141MIPI_PV_dummy_lines = MAX_Frame_length - A5141MIPI_PV_PERIOD_LINE_NUMS  ;
	spin_unlock(&a5141mipiraw_drv_lock);

	A5141MIPI_SetDummy(KAL_TRUE, A5141MIPI_PV_dummy_pixels, A5141MIPI_PV_dummy_lines);
    }
    else if (A5141MIPI_MODE_CAPTURE == g_iA5141MIPI_Mode)
    {
	    MAX_Frame_length = A5141MIPI_sensor.capture_vt_clk*100000/(A5141MIPI_FULL_PERIOD_PIXEL_NUMS + A5141MIPI_dummy_pixels)/u2FrameRate;
	    if(MAX_Frame_length < A5141MIPI_FULL_PERIOD_LINE_NUMS )
		    MAX_Frame_length = A5141MIPI_FULL_PERIOD_LINE_NUMS;

	    spin_lock(&a5141mipiraw_drv_lock);
	    A5141MIPI_dummy_lines = MAX_Frame_length - A5141MIPI_FULL_PERIOD_LINE_NUMS  ;
	    spin_unlock(&a5141mipiraw_drv_lock);

	    A5141MIPI_SetDummy(KAL_FALSE, A5141MIPI_dummy_pixels, A5141MIPI_dummy_lines);
    }
    
    return KAL_TRUE;
}

UINT32 A5141MIPISetAutoFlickerMode(kal_bool bEnable, UINT16 u2FrameRate)
{
    SENSORDB("[A5141MIPISetAutoFlickerMode] frame rate(10base) = %d %d\n", bEnable, u2FrameRate);
    if(bEnable) 
	{   
		// enable auto flicker   
		A5141MIPI_write_cmos_sensor_8(0x0104, 1);       
		A5141MIPI_write_cmos_sensor(0x0340, A5141_Frame_Length_preview +AUTO_FLICKER_NO);
		A5141MIPI_write_cmos_sensor_8(0x0104, 0);        	
    } else 
    {
    	// disable auto flicker
		A5141MIPI_write_cmos_sensor_8(0x0104, 1);        
		A5141MIPI_write_cmos_sensor(0x0340, A5141_Frame_Length_preview);
		A5141MIPI_write_cmos_sensor_8(0x0104, 0);         	    
    }
    return TRUE;
}


UINT32 A5141MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
                                                                UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{    
    UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
    UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
    UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
    UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
    UINT32 SensorRegNumber;
    UINT32 i;
    PNVRAM_SENSOR_DATA_STRUCT pSensorDefaultData=(PNVRAM_SENSOR_DATA_STRUCT) pFeaturePara;
    MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
    MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;
    MSDK_SENSOR_GROUP_INFO_STRUCT *pSensorGroupInfo=(MSDK_SENSOR_GROUP_INFO_STRUCT *) pFeaturePara;
    MSDK_SENSOR_ITEM_INFO_STRUCT *pSensorItemInfo=(MSDK_SENSOR_ITEM_INFO_STRUCT *) pFeaturePara;
    MSDK_SENSOR_ENG_INFO_STRUCT	*pSensorEngInfo=(MSDK_SENSOR_ENG_INFO_STRUCT *) pFeaturePara;

    switch (FeatureId)
    {
        case SENSOR_FEATURE_GET_RESOLUTION:
            *pFeatureReturnPara16++=A5141MIPI_IMAGE_SENSOR_FULL_WIDTH;
            *pFeatureReturnPara16=A5141MIPI_IMAGE_SENSOR_FULL_HEIGHT;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_PERIOD:
			switch(A5141_CurrentScenarioId)
			{
				case MSDK_SCENARIO_ID_CAMERA_ZSD:
        		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	            	*pFeatureReturnPara16++= A5141MIPI_FULL_PERIOD_PIXEL_NUMS + A5141MIPI_dummy_pixels;//A5141MIPI_PV_PERIOD_PIXEL_NUMS+A5141MIPI_dummy_pixels;
	            	*pFeatureReturnPara16=A5141MIPI_FULL_PERIOD_LINE_NUMS+A5141MIPI_dummy_lines;
	           		*pFeatureParaLen=4;
				     break;

				default:
					 *pFeatureReturnPara16++= A5141MIPI_PV_PERIOD_PIXEL_NUMS + A5141MIPI_PV_dummy_pixels;//A5141MIPI_PV_PERIOD_PIXEL_NUMS+A5141MIPI_dummy_pixels;
	            	*pFeatureReturnPara16=A5141MIPI_PV_PERIOD_LINE_NUMS+A5141MIPI_PV_dummy_lines;
	           		*pFeatureParaLen=4;
				     break;
			}
            break;
        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
			switch(A5141_CurrentScenarioId)
			{
				case MSDK_SCENARIO_ID_CAMERA_ZSD:
        		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	            	*pFeatureReturnPara32 = A5141MIPI_sensor.capture_vt_clk * 100000;
	           		*pFeatureParaLen=4;
					break;
				default:
					*pFeatureReturnPara32 = A5141MIPI_sensor.preview_vt_clk * 100000;
	           		*pFeatureParaLen=4;
					break;
			}
            break;
        case SENSOR_FEATURE_SET_ESHUTTER:
            A5141MIPI_SetShutter(*pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_NIGHTMODE:
            A5141MIPI_NightMode((BOOL) *pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_GAIN:
            A5141MIPI_Set_gain((UINT16) *pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_FLASHLIGHT:
            break;
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
           // A5141MIPI_isp_master_clock=*pFeatureData32;
            break;
        case SENSOR_FEATURE_SET_REGISTER:
            A5141MIPI_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
            break;
        case SENSOR_FEATURE_GET_REGISTER:
            pSensorRegData->RegData = A5141MIPI_read_cmos_sensor(pSensorRegData->RegAddr);
            break;
        case SENSOR_FEATURE_SET_CCT_REGISTER:
            SensorRegNumber=FACTORY_END_ADDR;	
            for (i=0;i<SensorRegNumber;i++)
            {
                spin_lock(&a5141mipiraw_drv_lock);
                A5141MIPISensorCCT[i].Addr=*pFeatureData32++;
                A5141MIPISensorCCT[i].Para=*pFeatureData32++;
				spin_unlock(&a5141mipiraw_drv_lock);
            }
            break;
        case SENSOR_FEATURE_GET_CCT_REGISTER:
            SensorRegNumber=FACTORY_END_ADDR;
            if (*pFeatureParaLen<(SensorRegNumber*sizeof(SENSOR_REG_STRUCT)+4))
                return FALSE;
            *pFeatureData32++=SensorRegNumber;
			
            for (i=0;i<SensorRegNumber;i++)
            {
                spin_lock(&a5141mipiraw_drv_lock);
                *pFeatureData32++=A5141MIPISensorCCT[i].Addr;
                *pFeatureData32++=A5141MIPISensorCCT[i].Para;
				spin_unlock(&a5141mipiraw_drv_lock);
            }
			
            break;
        case SENSOR_FEATURE_SET_ENG_REGISTER:
            SensorRegNumber=ENGINEER_END;
            for (i=0;i<SensorRegNumber;i++)
            {
            	spin_lock(&a5141mipiraw_drv_lock);
                A5141MIPISensorReg[i].Addr=*pFeatureData32++;
                A5141MIPISensorReg[i].Para=*pFeatureData32++;
				spin_unlock(&a5141mipiraw_drv_lock);
            }
            break;
        case SENSOR_FEATURE_GET_ENG_REGISTER:
            SensorRegNumber=ENGINEER_END;
            if (*pFeatureParaLen<(SensorRegNumber*sizeof(SENSOR_REG_STRUCT)+4))
                return FALSE;
            *pFeatureData32++=SensorRegNumber;
            for (i=0;i<SensorRegNumber;i++)
            {
            	spin_lock(&a5141mipiraw_drv_lock);
                *pFeatureData32++=A5141MIPISensorReg[i].Addr;
                *pFeatureData32++=A5141MIPISensorReg[i].Para;
				spin_unlock(&a5141mipiraw_drv_lock);
            }
            break;
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
            if (*pFeatureParaLen>=sizeof(NVRAM_SENSOR_DATA_STRUCT))
            {
                pSensorDefaultData->Version=NVRAM_CAMERA_SENSOR_FILE_VERSION;
                pSensorDefaultData->SensorId=A5141MIPI_SENSOR_ID;
                memcpy(pSensorDefaultData->SensorEngReg, A5141MIPISensorReg, sizeof(SENSOR_REG_STRUCT)*ENGINEER_END);
                memcpy(pSensorDefaultData->SensorCCTReg, A5141MIPISensorCCT, sizeof(SENSOR_REG_STRUCT)*FACTORY_END_ADDR);
            }
            else
                return FALSE;
            *pFeatureParaLen=sizeof(NVRAM_SENSOR_DATA_STRUCT);
            break;
        case SENSOR_FEATURE_GET_CONFIG_PARA:
            memcpy(pSensorConfigData, &A5141MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
            *pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
            break;
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
            A5141MIPI_camera_para_to_sensor();
            break;

        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
            A5141MIPI_sensor_to_camera_para();
            break;
        case SENSOR_FEATURE_GET_GROUP_COUNT:
            *pFeatureReturnPara32++=A5141MIPI_get_sensor_group_count();
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_GROUP_INFO:
            A5141MIPI_get_sensor_group_info(pSensorGroupInfo->GroupIdx, pSensorGroupInfo->GroupNamePtr, &pSensorGroupInfo->ItemCount);
            *pFeatureParaLen=sizeof(MSDK_SENSOR_GROUP_INFO_STRUCT);
            break;
        case SENSOR_FEATURE_GET_ITEM_INFO:
            A5141MIPI_get_sensor_item_info(pSensorItemInfo->GroupIdx,pSensorItemInfo->ItemIdx, pSensorItemInfo);
            *pFeatureParaLen=sizeof(MSDK_SENSOR_ITEM_INFO_STRUCT);
            break;

        case SENSOR_FEATURE_SET_ITEM_INFO:
            A5141MIPI_set_sensor_item_info(pSensorItemInfo->GroupIdx, pSensorItemInfo->ItemIdx, pSensorItemInfo->ItemValue);
            *pFeatureParaLen=sizeof(MSDK_SENSOR_ITEM_INFO_STRUCT);
            break;

        case SENSOR_FEATURE_GET_ENG_INFO:
            pSensorEngInfo->SensorId = 221;
            pSensorEngInfo->SensorType = CMOS_SENSOR;
            pSensorEngInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_RAW_B;
            *pFeatureParaLen=sizeof(MSDK_SENSOR_ENG_INFO_STRUCT);
            break;
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
            // get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
            // if EEPROM does not exist in camera module.
            *pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_SET_VIDEO_MODE:
            A5141MIPISetVideoMode(*pFeatureData16);
            break;
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            A5141MIPIGetSensorID(pFeatureReturnPara32); 
            break; 
		case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
			A5141MIPISetAutoFlickerMode((BOOL) *pFeatureData16, *(pFeatureData16 + 1));
   			break; 
        default:
            break;
    }
	
    return ERROR_NONE;
}	/* A5141MIPIFeatureControl() */


SENSOR_FUNCTION_STRUCT	SensorFuncA5141MIPI=
{
    A5141MIPIOpen,
    A5141MIPIGetInfo,
    A5141MIPIGetResolution,
    A5141MIPIFeatureControl,
    A5141MIPIControl,
    A5141MIPIClose
};

UINT32 A5141_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
    /* To Do : Check Sensor status here */
    if (pfFunc!=NULL)
        *pfFunc=&SensorFuncA5141MIPI;

    return ERROR_NONE;
}   /* SensorInit() */
