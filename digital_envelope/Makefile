TA_DEV_KIT_DIR ?= $(OPTEE_OS)/out/$(PLATFORM_FLAVOR)/export-user_ta

TA_SRC_C += ta/digital_envelope_ta.c
TA_CFLAGS += -I$(TA_DEV_KIT_DIR)/include

include $(TA_DEV_KIT_DIR)/mk/ta.mk

host:
	$(MAKE) -C host

clean:
	$(MAKE) -C host clean
	$(MAKE) -C ta clean
