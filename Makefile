SHELL=/bin/bash

all:
	@for a in $$(ls); do \
		if [[ -d $$a && -f $$a/Makefile ]]; then \
			echo "Building $$a"; \
			$(MAKE) -C $$a; \
		fi; \
	done;
	@echo "Done!"

clean:
	@for a in $$(ls); do \
		if [[ -d $$a && -f $$a/Makefile ]]; then \
			echo "Building $$a"; \
			$(MAKE) -C $$a clean; \
		fi; \
	done;
	@echo "Done!"