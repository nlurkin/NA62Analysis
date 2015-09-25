

build:
	@cmake -H. -Bbuild ${ARGS}
	@make -C build

clean:
	rm -rf build
	rm -rf lib
	rm -rf obj
	rm scripts/*.pyc

.PHONY: build clean