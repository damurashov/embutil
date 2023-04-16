all: build
	cd build ; \
	cmake .. ;\
	cmake --build .

build:
	mkdir build

run: all
	$(MAKE) -C build -j$(shell nproc)
	./build/snippet_lib $(RUN_ARGS)

.PHONY: all

clean:
	rm -rf build
	rm -rf *txt.user
