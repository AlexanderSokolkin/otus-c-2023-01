#include "utils.h"
#include "cjson/cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>


ArgsSet parseArgs(int argc, char* argv[])
{
	switch (argc) {
		case 2:
			if (strcmp(argv[1], "--help") == 0) {
				return Help;
			}
			return Region;
		default:
			return InvalidArgs;
	}
}

void printHelp()
{
	const char* help = 	"displays in text format the current weather in the transferred region.\n"
						"\tinput format: weather [ *region* | *arguments* ]"
						"Available arguments:\n"
						"\t--help - print current help.";
	printf("%s\n", help);
}




struct MemoryStruct {
	char* data;
	size_t size;
};

static size_t writeMemoryCallback(void* t_contents, size_t t_size, size_t t_nmemb, void* t_userp)
{
	size_t realsize = t_size * t_nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)t_userp;

	char* ptr = (char*)realloc(mem->data, mem->size + realsize + 1);
	if (!ptr) {
		return 0;
	}

	mem->data = ptr;
	memcpy(&(mem->data[mem->size]), t_contents, realsize);
	mem->size += realsize;
	mem->data[mem->size] = 0;

	return realsize;
}

static bool parseData(struct MemoryStruct* t_mem, char* t_buf)
{
	bool res = false;
	cJSON* json = cJSON_Parse(t_mem->data);
	if (json == NULL) {
		return false;
	}
	cJSON* cc = cJSON_GetObjectItemCaseSensitive(json, "current_condition");
	if (cc == NULL || cJSON_IsInvalid(cc)) {
		goto fault;
	}
	cJSON* cc0 = cJSON_GetArrayItem(cc, 0);
	if (cc0 == NULL || cJSON_IsInvalid(cc0)) {
		goto fault;
	}


	char temp_C[16];
	cJSON* tmp_val = cJSON_GetObjectItemCaseSensitive(cc0, "temp_C");
	if (tmp_val != NULL && cJSON_IsString(tmp_val)) {
		strcpy(temp_C, tmp_val->valuestring);
	}
	else {
		goto fault;
	}

	char wind_speed[16];
	tmp_val = cJSON_GetObjectItemCaseSensitive(cc0, "windspeedKmph");
	if (tmp_val != NULL && cJSON_IsString(tmp_val)) {
		strcat(wind_speed, tmp_val->valuestring);
	}
	else {
		goto fault;
	}

	char wind_dir[8];
	tmp_val = cJSON_GetObjectItemCaseSensitive(cc0, "winddir16Point");
	if (tmp_val != NULL && cJSON_IsString(tmp_val)) {
		strcpy(wind_dir, tmp_val->valuestring);
	}
	else {
		goto fault;
	}

	char desc[256] = "";
	cJSON* weather_desc_array = cJSON_GetObjectItemCaseSensitive(cc0, "weatherDesc");
	if (weather_desc_array == NULL || cJSON_IsInvalid(weather_desc_array)) {
		goto fault;
	}
	cJSON* weather_desc_elem = NULL;
	int counter = 0;
	cJSON_ArrayForEach(weather_desc_elem, weather_desc_array) {
		tmp_val = cJSON_GetObjectItemCaseSensitive(weather_desc_elem, "value");
		if (tmp_val != NULL && cJSON_IsString(tmp_val)) {
			if (counter) {
				strcat(desc, ", ");
			}
			strcat(desc, tmp_val->valuestring);
		}
		else {
			goto fault;
		}
		++counter;
	}

	sprintf(t_buf, 	"%s"
					"\ntemperature - %s C"
					"\nwind direction - %s"
					"\nwind speed - %s km/h\n",
					desc, temp_C, wind_dir, wind_speed);
	res = true;

fault:
	cJSON_Delete(json);
	return res;
}


bool printWeather(const char* t_region)
{
	curl_global_init(CURL_GLOBAL_ALL);

	bool result = false;
	CURL* handle;
	CURLcode res;

	struct MemoryStruct chunk;
	chunk.data = NULL;
	chunk.size = 0;
	handle = curl_easy_init();
	char url[512];
	sprintf(url, "https://wttr.in/%s?format=j1", t_region);
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&chunk);
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "weather/curl-agent");
	res = curl_easy_perform(handle);
	if (res != CURLE_OK) {
		printf("failed to connect to the server - %s\n", curl_easy_strerror(res));
		goto fault;
	}

	char buf[1024];
	if (!parseData(&chunk, buf)) {
		printf("failed to parse server response\n");
		goto fault;
	}
	printf("%s", buf);
	result = true;

fault:
	free(chunk.data);
	curl_easy_cleanup(handle);
	curl_global_cleanup();
	return result;
}
