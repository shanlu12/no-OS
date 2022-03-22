SRCS += $(PROJECT)/src/$(EXAMPLE_PROJECT)/$(PLATFORM)/main.c

ifeq (examples/iio_example,$(strip $(EXAMPLE_PROJECT)))
TINYIIOD=y
endif

ifeq (y,$(strip $(TINYIIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/accel/adxl355/iio_adxl355.h
SRCS += $(DRIVERS)/accel/adxl355/iio_adxl355.c

INCS += $(INCLUDE)/no_os_list.h \
		$(PLATFORM_DRIVERS)/uart_extra.h
endif
