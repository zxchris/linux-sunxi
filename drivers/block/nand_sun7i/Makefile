# AllWinner sun7i NAND driver 2.13
obj-$(CONFIG_SUN7I_NAND)  += sun7i_nand.o
sun7i_nand-y := nand_interface.o  \
				nfd/nand_dma.o \
				nfd/nand_compat.o \
                nfd/nand_blk.o \
			    lib/src/format/nand_format.o lib/src/logic/bad_manage.o \
    		    lib/src/logic/logic_ctl.o lib/src/logic/mapping.o \
			    lib/src/logic/mbr.o lib/src/logic/mapping_base.o lib/src/logic/merge.o \
    		    lib/src/logic/read_reclaim.o lib/src/logic/wear_levelling.o lib/src/logic/logic_cache.o \
    		    lib/src/scan/nand_scan.o lib/src/scan/nand_id.o\
    		    lib/src/physic/nand_phy.o lib/src/physic/nand_simple_r.o lib/src/physic/nand_simple_w.o \
    		    lib/src/nfc/nfc_r.o lib/src/nfc/nfc_w.o
SUN7ICFLAGS = -Iarch/arm/mach-sunxi/include -Iarch/arm/mach-sun7i/include -Iinclude -Ilib/src/include -Ilib/osal -Infd -fno-unwind-tables -fno-asynchronous-unwind-tables
EXTRA_CFLAGS = $(SUN7ICFLAGS)
