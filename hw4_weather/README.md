# Программа для отображения погоды в переданном регионе/стране

Для сборки программы необходимо:  
1) Установить curl;  
2) Скачать и установить cJSON - https://github.com/DaveGamble/cJSON.git  
	Например с помощью cmake:  
		* git clone https://github.com/DaveGamble/cJSON.git  
		* cd cJSON  
		* mkdir build & cd build  
		* cmake .. -DENABLE_CJSON_UTILS=On -DENABLE_CJSON_TEST=Off -DBUILD_SHARED_AND_STATIC_LIBS=On -DCMAKE_INSTALL_PREFIX=/some/dir/for/cJSON_install  
		* make  
		* make install  

3a) Для сборки с помощью CMake необходимо передать параметр -DcJSON_DIR  
		* git clone https://github.com/AlexanderSokolkin/otus-c-2023-01.git  
		* cd hw4_weather  
		* mkdir build & cd build  
		* cmake .. -DcJSON_DIR=/dir/with/cJSON_install/lib/cmake  
		* make  

3б) Для сборки с помощью Makefile необходимо передать параметр cJSON_DIR  
		* git clone https://github.com/AlexanderSokolkin/otus-c-2023-01.git  
		* cd hw4_weather  
		* make .. cJSON_DIR=/dir/with/cJSON_install  
