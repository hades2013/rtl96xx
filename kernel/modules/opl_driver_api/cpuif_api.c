/** THE FORMAT is FOR DOXYGEN to Generate the document automatically

  *  @defgroup CPUIF_API cpuif_api module
  *  @copyright opulan.com
  *  @author stephanxu@sina.com
  *  @version v0.1
  *  @date July-30-2007
  *  @histroy 
  *  \nauthor	   date		 description
  *  \n-------------------------------------------------------------------
     \nhux     July-30-2007    initial version:which supply the api to access the cpuif.
  *  @{
  */
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <opl_typedef.h>
#include <opl_cpuif.h>

#include "cpuif_api.h"

/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_CPUIF_DEVICE_NAME "/dev/opl_cpuif"

/** @}*/

/** @name type definition
 *  @{
 */
/*--------------------------type definition------------------------- */





/** @}*/

/** @name global variable and function imported
 *  @{
 */
/*-----------------global varible/function declaration--------------- */





/** @}*/
   
/** @name local function declaration
 *  @{
 */
/*-------------------local  function declaration---------------------- */





/** @}*/
 

/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  variable declaration and definition------------- */
int g_opl_cpuif_fd;




/** @}*/
 

/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */





/** @}*/


/** @defgroup cpuif_api function-description(cpuif_api.c)

 *  @{
 */
/*----------------------local  function definition--------------------- */
/** description: wait for dma0 interrupt, the process will sleep on it when the dma0 interrupt wake up it.
 *  @param void
 *  @return void

 *  @see 
 *  @deprecated 
 */
void wait_for_host_dma0_interrupt(void)
{
  int status = 0;
  status = ioctl(g_opl_cpuif_fd,WAIT_FOR_IPMUX_HOST_DMA0_INTERRUPT,NULL);
  if(status){
    printf("wait for ipmux host dma0 interrupt failed\n");
  }
}

/** description: read the data for dmas rx buffer.the data will loaded into pkt, and read len byte data.
 *  @param int dmas:which dmas rx buffer.
 *  @param int bd: bd table 's index
 *  @param void *pkt for loading data
 *  @param int len:how many bytes should be read
 *  @retval the len of data read.
 *  @retval -1 failed

 *  @see 
 *  @deprecated 
 */
int read_from_dma_rxbuf(int dmas,int bd,void *pkt,int len)
{
  int status ;
  dma_request_data_t req;
  req.dmas = dmas;
  req.bd = bd;
  req.len = len;
  memset(&req.buf,0,sizeof(req.buf));
  
  status = ioctl(g_opl_cpuif_fd,GET_IPMUX_DMA_RX_BUF_DATA,&req);
  if(!status){
    memcpy(pkt,req.buf,len);
    return len;
  }else{
    return -1;
  }
}
/** description: send the data to dmas rx buffer. 
 *  @param int dmas which dma should be send to.
 *  @param int bd bdtable's index.
 *  @param void *pkt the data payload here
 *  @param int len: how many byte should be sent
 *  @retval len bytes sent
 *  @retval -1 failed

 *  @see 
 *  @deprecated 
 */
int write_to_dma_txbuf(int dmas,int bd,void *pkt,int len)
{
  int status,i;
  dma_request_data_t req;
  req.dmas = dmas;
  req.bd = bd;
  req.len = len;
  memset(req.buf,0,sizeof(req.buf));
  
  for(i = 0;i<(len+3)/4;i++){
    ((u32*)req.buf)[i] = ((u32 *)pkt)[i];
  }
  status = ioctl(g_opl_cpuif_fd,SET_IPMUX_DMA_TX_BUF_DATA,&req);
  if(status){
    printf("send the data to host dma0 failed\n");
    return -1;
  }
  return len;
}

/** description: enable host dma0 interrupt.
 *  @param void
 *  @return void

 *  @see 
 *  @deprecated 
 */
void enable_host_dma0_intr(void)
{
  int status = 0;
  status = ioctl(g_opl_cpuif_fd,ENABLE_IPMUX_HOST_DMA0_INTERRUPT,NULL);
  if(status){
    printf("disable the host dma0 interrupt failed\n");
  }
}
/** description: disable host dma0 interrupt.
 *  @param void
 *  @return void

 *  @see 
 *  @deprecated 
 */
void disable_host_dma0_intr(void )
{
  int status = 0;
  status = ioctl(g_opl_cpuif_fd,DISABLE_IPMUX_HOST_DMA0_INTERRUPT,NULL);
  if(status){
    printf("disable the host dma0 interrupt failed\n");
  }
}
/** description: return the rx physical address for dmas base addr.in fact it is base address.
 *  @param int dmas:which dma
 *  @return physical address,which is uncacheable.

 *  @see 
 *  @deprecated 
 */
void *malloc_dma_rxbuf(int dmas,int size)
{
  int status = 0;
  dma_request_phys_addr_t req;
  req.dmas = dmas;
  req.dmas = size;
  status = ioctl(g_opl_cpuif_fd,GET_IPMUX_DMA_RX_PHYS_ADDR,&req);
  if(status){
    printf("get the dma %d rx physical address failed \n",dmas);
    return 0;
  }
  return (void *)req.phys_addr;
}
/** description: return the tx buffer physical address for dmas.in fact, in ipmux-e,it is not used.
 *  @param int dmas
 *  @return physcial address: uncacheable in mips platform

 *  @see 
 *  @deprecated 
 */
void *malloc_dma_txbuf(int dmas,int size)
{
  int status = 0;
  dma_request_phys_addr_t req;
  req.dmas = dmas;
  req.size = size;
  status = ioctl(g_opl_cpuif_fd,GET_IPMUX_DMA_TX_PHYS_ADDR,&req);
  if(status){
    printf("get the dma %d Tx physical address failed \n",dmas);
    return 0;
  }
  return (void *)req.phys_addr;
}

void set_dma_rxbuf_addr(int dmas,u32 addr)
{
  int status = 0;
  dma_request_phys_addr_t req;
  req.dmas = dmas;
  req.phys_addr = addr;
  status = ioctl(g_opl_cpuif_fd,SET_IPMUX_DMA_RX_PHYS_ADDR,&req);
  if(status){
    printf("get the dma %d Tx physical address failed \n",dmas);
    return ;
  }
}
void set_dma_txbuf_addr(int dmas,u32 addr)
{
  int status = 0;
  dma_request_phys_addr_t req;
  req.dmas = dmas;  
  req.phys_addr = addr;
  status = ioctl(g_opl_cpuif_fd,SET_IPMUX_DMA_TX_PHYS_ADDR,&req);
  if(status){
    printf("get the dma %d Tx physical address failed \n",dmas);
    return ;
  }
}

void free_dma_rxbuf(int dmas)
{
  int status = 0;
  dma_request_phys_addr_t req;
  req.dmas = dmas;
  status = ioctl(g_opl_cpuif_fd,FREE_IPMUX_DMA_RX_PHYS_ADDR,&req);
  if(status){
    printf("get the dma %d Tx physical address failed \n",dmas);
    return ;
  }
}
void free_dma_txbuf(int dmas)
{
  int status = 0;
  dma_request_phys_addr_t req;
  req.dmas = dmas;  
  status = ioctl(g_opl_cpuif_fd,FREE_IPMUX_DMA_TX_PHYS_ADDR,&req);
  if(status){
    printf("get the dma %d Tx physical address failed \n",dmas);
    return ;
  }
}
/** description:it is used to open the "/dev/cpuif",it is call by lib init,it is visible for user.  
 *  @param void
 *  @retval 0 success
 *  @retval -1 failed

 *  @see 
 *  @deprecated 
 */
int open_cpuif(void)
{
  g_opl_cpuif_fd = open(OPL_CPUIF_DEVICE_NAME, O_RDWR|O_SYNC );
  if(g_opl_cpuif_fd < 0){
    printf("open the %s device failed\n",OPL_CPUIF_DEVICE_NAME);
    return -1;
  }
  return 0;
}
/** description: it used to close the "/dev/cpuif" it is call by lib init,it is invisible for user.
 *  @param void
 *  @retval 0 success
 *  @retval !0 failed

 *  @see 
 *  @deprecated 
 */
int close_cpuif(void)
{
  int status = 0;
  if(g_opl_cpuif_fd > 0){
    status =close(g_opl_cpuif_fd);
  }
  return status;
}
/** @}*/
/** @}*/
