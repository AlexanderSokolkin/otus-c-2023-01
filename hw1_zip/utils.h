#pragma once

#include <stddef.h>
#include <stdio.h>


void 	printHelp();
size_t	fileSize(FILE* t_file);
size_t 	jpegSize(FILE* t_file);
int 	isJpegZip(FILE* t_file, size_t t_jpeg_size);
void	printZipFiles(FILE* t_file, size_t t_jpeg_size);

