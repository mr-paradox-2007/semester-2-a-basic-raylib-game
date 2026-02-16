# ==============================================================================
# SET YOUR RAYLIB PATH HERE
# Example: /home/username/raylib
# ==============================================================================
RAYLIB_PATH = /home/ammar-anas/raylib

# Project Settings
PROJECT_NAME = DarkReflection
SRC = dark_reflection_refactored.cpp

# Libraries and Includes
INCLUDE_PATH = -I$(RAYLIB_PATH)/src
LIB_NATIVE = -L$(RAYLIB_PATH)/src -lraylib

# Web Specific (Requires Raylib to be compiled for Web first)
# Usually found in raylib/src or a custom web_release folder
LIB_WEB = $(RAYLIB_PATH)/web_release/libraylib_web.a

# Linux Build
linux:
	g++ $(SRC) -o $(PROJECT_NAME) $(INCLUDE_PATH) $(LIB_NATIVE) -lGL -lm -lpthread -ldl -lrt -lX11

# Windows Build (MinGW)
windows:
	g++ $(SRC) -o $(PROJECT_NAME).exe $(INCLUDE_PATH) $(LIB_NATIVE) -lopengl32 -lgdi32 -lwinmm

# Web Build (Emscripten)
web:
	emcc $(SRC) -o index.html -DPLATFORM_WEB $(INCLUDE_PATH) $(LIB_WEB) \
	-s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=128MB \
	-O3 --preload-file Assets

# Cleanup
clean:
	rm -f $(PROJECT_NAME) $(PROJECT_NAME).exe index.html index.js index.wasm index.data