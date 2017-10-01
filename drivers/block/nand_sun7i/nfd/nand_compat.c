#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/spinlock.h>
#include <linux/hdreg.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/mutex.h>
#include <mach/clock.h>
#include <mach/sunxi_sys_config.h>
#include <mach/sys_config.h>
#include <linux/dma-mapping.h>
#include <linux/types.h>
#include <mach/dma.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/cacheflush.h>
#include <linux/gpio.h>
#include "nand_lib.h"
#include "nand_blk.h"

static struct clk *ahb_nand_clk = NULL;
static struct clk *mod_nand_clk = NULL;

//static __u32 NAND_DMASingleMap(__u32 rw, __u32 buff_addr, __u32 len);
//static __u32 NAND_DMASingleUnmap(__u32 rw, __u32 buff_addr, __u32 len);

#define NAND_READ 0x5555
#define NAND_WRITE 0xAAAA

static DECLARE_WAIT_QUEUE_HEAD(DMA_wait);
dma_hdl_t dma_hdle = (dma_hdl_t)NULL;

int seq=0;
u32 nand_handle=0;
dma_cb_t done_cb;

static int nandrb_ready_flag = 1;

static DECLARE_WAIT_QUEUE_HEAD(NAND_RB_WAIT);

//#define RB_INT_MSG_ON
#ifdef  RB_INT_MSG_ON
#define dbg_rbint(fmt, args...) printk(fmt, ## args)
#else
#define dbg_rbint(fmt, ...)  ({})
#endif

//#define RB_INT_WRN_ON
#ifdef  RB_INT_WRN_ON
#define dbg_rbint_wrn(fmt, args...) printk(fmt, ## args)
#else
#define dbg_rbint_wrn(fmt, ...)  ({})
#endif

/*
*********************************************************************************************************
*                                               DMA TRANSFER END ISR
*
* Description: dma transfer end isr.
*
* Arguments  : none;
*
* Returns    : EPDK_TRUE/ EPDK_FALSE
*********************************************************************************************************
*/
//struct sw_dma_client nand_dma_client = {
//	.name="NAND_DMA",
//};

int NAND_ClkRequest(void)
{
    printk("[NAND] nand clk request start\n");
    ahb_nand_clk = clk_get(NULL,"ahb_nfc");
    if(!ahb_nand_clk) {
        return -1;
    }
    mod_nand_clk = clk_get(NULL,"nfc");
        if(!mod_nand_clk) {
        return -1;
    }
    printk("[NAND] nand clk request ok!\n");
    return 0;
}

void NAND_ClkRelease(void)
{
	clk_put(ahb_nand_clk);
	clk_put(mod_nand_clk);
}

int NAND_AHBEnable(void)
{
	return clk_enable(ahb_nand_clk);
}

int NAND_ClkEnable(void)
{
	return clk_enable(mod_nand_clk);
}

void NAND_AHBDisable(void)
{
	clk_disable(ahb_nand_clk);
}

void NAND_ClkDisable(void)
{
	clk_disable(mod_nand_clk);
}

int NAND_SetClk(__u32 nand_clk)
{
	return clk_set_rate(mod_nand_clk, nand_clk*2000000);
}

int NAND_GetClk(void)
{
	return (clk_get_rate(mod_nand_clk)/2000000);
}


int NAND_QueryDmaStat(void)
{
	return 0;
}

#if 0
__u32 NAND_DMASingleMap(__u32 rw, __u32 buff_addr, __u32 len) // FIXME
{
    __u32 mem_addr;

    if (rw == 1)
    {
	    mem_addr = (__u32)dma_map_single(NULL, (void *)buff_addr, len, DMA_TO_DEVICE);
	}
	else
    {
	    mem_addr = (__u32)dma_map_single(NULL, (void *)buff_addr, len, DMA_FROM_DEVICE);
	}

	return mem_addr;
}


__u32 NAND_DMASingleUnmap(__u32 rw, __u32 buff_addr, __u32 len) // FIXME
{
	__u32 mem_addr = buff_addr;

	if (rw == 1)
	{
	    dma_unmap_single(NULL, (dma_addr_t)mem_addr, len, DMA_TO_DEVICE);
	}
	else
	{
	    dma_unmap_single(NULL, (dma_addr_t)mem_addr, len, DMA_FROM_DEVICE);
	}
}
#endif

void NAND_EnRbInt(void)
{
	//clear interrupt
	NFC_RbIntClearStatus();

	nandrb_ready_flag = 0;

	//enable interrupt
	NFC_RbIntEnable();

	dbg_rbint("rb int en\n");
}


void NAND_ClearRbInt(void)
{

	//disable interrupt
	NFC_RbIntDisable();;

	dbg_rbint("rb int clear\n");

	//clear interrupt
	NFC_RbIntClearStatus();

	//check rb int status
	if(NFC_RbIntGetStatus())
	{
		dbg_rbint_wrn("nand  clear rb int status error in int clear \n");
	}

	nandrb_ready_flag = 0;
}

void NAND_RbInterrupt(void)
{

	dbg_rbint("rb int occor! \n");
	if(!NFC_RbIntGetStatus())
	{
		dbg_rbint_wrn("nand rb int late \n");
	}

    NAND_ClearRbInt();

    nandrb_ready_flag = 1;
	wake_up( &NAND_RB_WAIT );

}


__s32 NAND_WaitRbReady(void)
{
	__u32 rb;

	NAND_EnRbInt();

	//wait_event(NAND_RB_WAIT, nandrb_ready_flag);
	dbg_rbint("rb wait \n");

	if(nandrb_ready_flag)
	{
		dbg_rbint("fast rb int\n");
		NAND_ClearRbInt();
		return 0;
	}

	rb=  NFC_GetRbSelect();
	if(!rb)
	{
		if(NFC_GetRbStatus(rb))
		{
			dbg_rbint("rb %u fast ready \n", rb);
			NAND_ClearRbInt();
			return 0;
		}
	}
	else
	{
		if(NFC_GetRbStatus(rb))
		{
			dbg_rbint("rb %u fast ready \n", rb);
			NAND_ClearRbInt();
			return 0;
		}
	}



	if(wait_event_timeout(NAND_RB_WAIT, nandrb_ready_flag, 1*HZ)==0)
	{
		dbg_rbint_wrn("nand wait rb int time out\n");
		NAND_ClearRbInt();

	}
	else
	{
		dbg_rbint("nand wait rb ready ok\n");
	}

	return 0;

}



void NAND_PIORequest(void)
{
    nand_handle = gpio_request_ex("nand_para",NULL);
}

void NAND_PIORelease(void)
{
}

void NAND_Memset(void* pAddr, unsigned char value, unsigned int len)
{
    memset(pAddr, value, len);
}

void NAND_Memcpy(void* pAddr_dst, void* pAddr_src, unsigned int len)
{
    memcpy(pAddr_dst, pAddr_src, len);
}

void* NAND_Malloc(unsigned int Size)
{
    return kmalloc(Size, GFP_KERNEL);
}

void NAND_Free(void *pAddr, unsigned int Size)
{
    kfree(pAddr);
}

int NAND_Print(const char * fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);

	return r;
}

void *NAND_IORemap(unsigned int base_addr, unsigned int size)
{
    return (void *)0xf1c03000;
}

__u32 NAND_GetIOBaseAddr(void)
{
	return 0xf1c03000;
}


int NAND_get_storagetype(void)
{
    //script_item_value_type_e script_ret;
    script_item_u storage_type;

    storage_type.val = 0;
    //script_ret = script_get_item("target","storage_type", &storage_type);
    //if(script_ret!=SCIRPT_ITEM_VALUE_TYPE_INT)
    //{
    //       printk("nand init fetch storage_type failed\n");
    //       storage_type.val=0;
    //       return storage_type.val;
    //}

    return storage_type.val;
}
