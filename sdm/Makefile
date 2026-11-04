export V ?= 0

EXAMPLE_LIST := $(subst /,,$(dir $(wildcard */Makefile)))

.PHONY: all
all:
	@for example in $(EXAMPLE_LIST); do \
		$(MAKE) -C $$example || exit 1; \
	done

.PHONY: clean
clean:
	@for example in $(EXAMPLE_LIST); do \
		$(MAKE) -C $$example clean || exit 1; \
	done
