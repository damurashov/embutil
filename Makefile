LIBRARY = build/LIBRARY_NAME_STUB

all: $(LIBRARY)

$(LIBRARY): build
	$(MAKE) -C build -j4

build:
	mkdir -p build && \
		cd build && \
		cmake ..

clean:
	rm -rf build
	rm -rf *txt.user
