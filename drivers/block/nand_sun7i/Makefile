obj-$(CONFIG_SUNXI_NAND)  += sunxi_nand.o
sunxi_nand-y := nand_interface.o  \
                nfd/nand_blk.o nfd/nand_osal_for_linux.o libnand.a \
			    lib/src/format/nand_format.o lib/src/logic/bad_manage.o \
    		    lib/src/logic/logic_ctl.o lib/src/logic/mapping.o \
			    lib/src/logic/mbr.o lib/src/logic/mapping_base.o lib/src/logic/merge.o \
    		    lib/src/logic/read_reclaim.o lib/src/logic/wear_levelling.o lib/src/logic/logic_cache.o \
    		    lib/src/scan/nand_scan.o lib/src/scan/nand_id.o\
    		    lib/src/physic/nand_phy.o lib/src/physic/nand_simple_r.o lib/src/physic/nand_simple_w.o \
    		    lib/src/nfc/nfc_r.o lib/src/nfc/nfc_w.o

# $(OBJ) : $(LIBPATH)/../include/nand_include.h $(LIBPATH)/osal/nand_osal.h
