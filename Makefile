.PHONY: all native web clean

all: web

native:
	gcc main.c animation.c -o game -lraylib -lGL -lm -lpthread -ldl -lrt

web: web/game.html

web/game.html: main.c animation.c raylib/src/libraylib.web.a
	mkdir -p web
	emcc main.c animation.c -o web/game.html \
		raylib/src/libraylib.web.a \
		-Iraylib/src \
		-DPLATFORM_WEB \
		--preload-file assets \
		--shell-file shell.html \
		-s USE_GLFW=3 \
		-s ASYNCIFY \
		-s TOTAL_MEMORY=67108864 \
		-s FORCE_FILESYSTEM=1 \
		-s ASSERTIONS=0 \
		-s ALLOW_MEMORY_GROWTH=1 \
		-Os

clean:
	rm -rf web
	rm -f game
