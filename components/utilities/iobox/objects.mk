include $(DBUILD_ROOT).dbuild/package.mk

obj-$(CONFIG_COMPONENTS_AW_IOBOX_LS) += ls.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_CAT) += cat.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_HEXDUMP) += hexdump.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_CP) += cp.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_MV) += mv.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_RM) += rm.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_UNLINK) += unlink.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_LINK) += link.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_RWCHECK) += rwcheck.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_RWSPEED) += rwspeed.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_VI) += vi/vi.o
obj-$(CONFIG_COMPONENTS_AW_IOBOX_DF) += df.o

obj-header += $(BASE)/components/thirdparty/console
obj-header += $(BASE)/components/thirdparty/ulog/syslog

$(eval $(call BuildPackage,iobox))
