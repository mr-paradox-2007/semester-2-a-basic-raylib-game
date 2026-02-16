# ==============================================================================
# SET YOUR RAYLIB PATH HERE
# Example: /home/username/raylib
# ==============================================================================
RAYLIB_PATH = /home/ammar-anas/raylib

# Project Settings
PROJECT_NAME = DarkReflection
SRC = dark_reflection_web_optimized.cpp

# Libraries and Includes
INCLUDE_PATH = -I$(RAYLIB_PATH)/src
LIB_NATIVE = -L$(RAYLIB_PATH)/src -lraylib

# Web Specific
LIB_WEB = $(RAYLIB_PATH)/web_release/libraylib_web.a

# Compiler flags
CFLAGS = -Wall -std=c++11 -D_DEFAULT_SOURCE -Wno-missing-braces

# Linux Build
linux:
	g++ $(SRC) -o $(PROJECT_NAME) $(INCLUDE_PATH) $(LIB_NATIVE) $(CFLAGS) -lGL -lm -lpthread -ldl -lrt -lX11

# Windows Build (MinGW)
windows:
	g++ $(SRC) -o $(PROJECT_NAME).exe $(INCLUDE_PATH) $(LIB_NATIVE) $(CFLAGS) -lopengl32 -lgdi32 -lwinmm

# Web Build (Emscripten) - Optimized for web deployment
web:
	emcc $(SRC) -o index.html -DPLATFORM_WEB $(INCLUDE_PATH) $(LIB_WEB) \
	-s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=256MB \
	-s ASYNCIFY -O2 --preload-file Assets \
	--shell-file shell.html

# Web Build - Debug version (for troubleshooting)
web-debug:
	emcc $(SRC) -o index.html -DPLATFORM_WEB $(INCLUDE_PATH) $(LIB_WEB) \
	-s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=256MB \
	-s ASYNCIFY -s ASSERTIONS=1 -g --preload-file Assets \
	--shell-file shell.html

# Run with local server (requires Python 3)
serve:
	python3 -m http.server 8000

# Cleanup
clean:
	rm -f $(PROJECT_NAME) $(PROJECT_NAME).exe index.html index.js index.wasm index.data

# Help
help:
	@echo "Dark Reflection Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  make linux       - Build for Linux"
	@echo "  make windows     - Build for Windows (using MinGW)"
	@echo "  make web         - Build for Web (requires Emscripten)"
	@echo "  make web-debug   - Build for Web with debugging enabled"
	@echo "  make serve       - Start local web server on port 8000"
	@echo "  make clean       - Remove all build files"
	@echo "  make help        - Show this help message"
	@echo ""
	@echo "For web build, ensure:"
	@echo "  1. Emscripten SDK is installed and activated"
	@echo "  2. Raylib is compiled for web (in web_release folder)"
	@echo "  3. Assets folder is in the current directory"

.PHONY: linux windows web web-debug serve clean help