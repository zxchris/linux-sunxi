# AllWinner sun7i NAND driver 2.xx
obj-$(CONFIG_SUN7I_NAND)  += sun7i_nand.o

#sun7i_nand-y := nand_interface.o nfd/nand_osal_for_linux.o nfd/nand_ota_burn.o nfd/boot_head/boot0_op.o nfd/nand_base.o nfd/nand_blk.o nfd/nand_dev.o nfd/nand_class.o libnand.a
sun7i_nand-y := nand_interface.o nfd/nand_compat.o nfd/nand_ota_burn.o nfd/boot_head/boot0_op.o nfd/nand_base.o nfd/nand_blk.o nfd/nand_dev.o nfd/nand_class.o libnand.a

SUN7ICFLAGS = -Iarch/arm/mach-sunxi/include -Iarch/arm/mach-sun7i/include -Iinclude -Ilib/src/include -Ilib/osal -Infd -fno-unwind-tables -fno-asynchronous-unwind-tables
EXTRA_CFLAGS = $(SUN7ICFLAGS)
