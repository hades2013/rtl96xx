/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  version.h
*
* DESCRIPTION: 
*	
*
* Date Created: May 14, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/h/version.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VERSION_H_
#define __VERSION_H_
 
#define MAJOR_VERSION 3
#define MINOR_VERSION 2
#define BUILD_NUMBER 1

#if defined(ONU_1PORT_V1)
#define BUILD_FLAG "F13"
#elif defined(ONU_4PORT_V1_AR8228)
#define BUILD_FLAG "F23"
#elif defined(ONU_1PORT_V1E_FPGA)
#define BUILD_FLAG "ONU-V1E-FPGA-1"
#elif defined(ONU_4PORT_V1E_FPGA)
#define BUILD_FLAG "ONU-V1E-FPGA-4"
#elif defined(ONU_1PORT_V1E_RF100_GNA)
#define BUILD_FLAG "RF100-GNA"
#elif defined(ONU_4PORT_V1E_RF400_GNA)
#define BUILD_FLAG "RF400-GNA"
#elif defined(ONU_1PORT_V1E_F13)
#define BUILD_FLAG "F13-BG"
#elif defined(ONU_1PORT_V1E_C13)
#define BUILD_FLAG "C13-BG"
#elif defined(ONU_4PORT_V1E_F23)
#define BUILD_FLAG "F23-BG"
#elif defined(ONU_1PORT_V1E_RF100_GNA_ATE)
#define BUILD_FLAG "RF100-GNA-ATE"
#else
#define BUILD_FLAG "ONU-UNKN-VER"
#endif

 
#endif /* #ifndef __VERSION_H_ */
 

