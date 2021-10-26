sudo apt install cmake libglfw3-dev libglfw3 libglew-dev libglew2.1 libfreetype-dev libfreetype6 libfreetype6-dev libglm-dev libglapi-mesa libgles-dev
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -S . -B bin
cmake --build bin --config Release --target tetris

echo "----ENJOY GAME IS YOURS----"