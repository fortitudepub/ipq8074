#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libnl
# ZDY: bump to 3.3.0 (minimal requirement for the missing interface) for vpp lcp_interface
# using openwrt 18.06 for check patch needed.
PKG_VERSION:=3.3.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/thom311/libnl/releases/download/libnl3_3_0/libnl-3.3.0.tar.gz
PKG_MD5SUM:=ab3ef137cad95bdda5ff0ffa5175dfa5
PKG_LICENSE:=LGPL-2.1

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/libnl
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=netlink socket library
  DEPENDS:=+libpthread
  URL:=http://people.suug.ch/~tgr/libnl/
endef

define Package/libnl/description
 This package contains a library for applications dealing with netlink sockets
endef

TARGET_CFLAGS += -ffunction-sections $(FPIC)
TARGET_LDFLAGS += -pie

define Build/Compile
	$(call Build/Compile/Default)
	make -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
	mkdir -p $(1)/usr/include/libnl
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/libnl/
endef

define Package/libnl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl-route-3.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl-genl-3.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl-nf-3.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl-3.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libnl))
