#include "utils.h"
#include "cjson/cJSON.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
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


typedef enum {
	JSON_PARSE_OK,
	JSON_PARSE_INVALID_CITY,
	JSON_PARSE_ERROR
} JSON_PARSE_RESULT;

struct MemoryStruct {
	char* data;
	size_t size;
};

static bool isEquals(const char* t_str1, const char* t_str2)
{
	int i = 0;
	while (t_str1[i] != '\0' || t_str2[i] != '\0') {
		if (tolower(t_str1[i]) != tolower(t_str2[i])) {
			return false;
		}
		++i;
	}
	return t_str1[i] == '\0' && t_str2[i] == '\0';
}


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


static JSON_PARSE_RESULT parseData(struct MemoryStruct* t_mem, const char* t_city, char* t_buf)
{
	JSON_PARSE_RESULT res = JSON_PARSE_ERROR;
	cJSON* json = cJSON_Parse(t_mem->data);
	if (json == NULL) {
		return JSON_PARSE_ERROR;
	}

	cJSON* nearest_area = cJSON_GetObjectItemCaseSensitive(json, "nearest_area");
	if (nearest_area == NULL || cJSON_IsInvalid(nearest_area)) {
		goto fault;
	}
	cJSON* nearest_area0 = cJSON_GetArrayItem(nearest_area, 0);
	if (nearest_area0 == NULL || cJSON_IsInvalid(nearest_area0)) {
		goto fault;
	}
	cJSON* area_name_array = cJSON_GetObjectItemCaseSensitive(nearest_area0, "areaName");
	if (area_name_array == NULL || cJSON_IsInvalid(area_name_array)) {
		goto fault;
	}
	cJSON* area_name_elem = NULL;
	bool correct_city = false;
	int counter = 0;
	cJSON_ArrayForEach(area_name_elem, area_name_array) {
		cJSON* city = cJSON_GetObjectItemCaseSensitive(area_name_elem, "value");
		if (city != NULL && cJSON_IsString(city)) {
			if (isEquals(city->valuestring, t_city)) {
				correct_city = true;
				break;
			}
			if (counter) {
				strcat(t_buf, ", ");
			}
			strcat(t_buf, city->valuestring);
		}
		else {
			goto fault;
		}
		++counter;
	}
	if (!correct_city) {
		res = JSON_PARSE_INVALID_CITY;
		goto fault;
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
	counter = 0;
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
	res = JSON_PARSE_OK;

fault:
	cJSON_Delete(json);
	return res;
}


bool printWeather(const char* t_region)
{
	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
		return false;
	}

	bool result = false;
	CURL* handle;
	CURLcode res;

	handle = curl_easy_init();
	if (handle == NULL) {
		return false;
	}

	struct MemoryStruct chunk;
	chunk.data = NULL;
	chunk.size = 0;
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
	long response_code;
	res = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response_code);
	if (res != CURLE_OK && response_code != 200) {
		printf("Failed to get data from server\n");
		goto fault;
	}

	char buf[1024] = "\0";
	switch (parseData(&chunk, t_region, buf)) {
		case JSON_PARSE_OK:
			printf("%s", buf);
			result = true;
			break;
		case JSON_PARSE_INVALID_CITY:
			printf("Invalid region entered. Maybe you mean: %s\n", buf);
			goto fault;
		case JSON_PARSE_ERROR:
		default:
			printf("failed to parse server response\n");
			goto fault;
	}

fault:
	free(chunk.data);
	curl_easy_cleanup(handle);
	curl_global_cleanup();
	return result;
}
