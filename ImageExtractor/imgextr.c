/*
Image Extractor v1.0.8
~~~~~~~~~~~~~~~~~~~~~~

Image Extractor is a simple image carving tool written in C, to
extract images from raw data or disk image for digital forensics.

Image Extractor scans images by detecting header and trailer of
specified image type. Provides normal analysis and extraction.

Copyright (c) 2022 Group-14 of BIS30803. All rights reserved.
Version: 1.0.8
Last updated on 08/06/2022, 16:07:35 UTC
Author: Ching Hung Tan
Email: tanchinghung5098.1@gmail.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// Macros & constants definition
#define APP_NAME "Image Extractor"
#define APP_VERSION "1.0.8"
#define YEAR "2022"
#define UPDATE_DATE "June 8 2022"
#define UPDATE_TIME "16:07:35"
#define UPDATE_DATE_Y "2022"
#define UPDATE_DATE_M "06"
#define UPDATE_DATE_D "08"

#define BLOCK_SIZE 512
#define IMAGE_LIMIT 100


// Declarations and prototypes
void toUpperCase(char* str);
int inArray(char* array[], char* element);


int main(int argc, char* argv[]) {
	
	// check if command has no options/parameters
	
	if (argc == 1) {
		printf("%s [Version %s]\n", APP_NAME, APP_VERSION);
		printf("Copyright (c) %s Group-14 of BIS30803. All rights reserved.\n", YEAR);
		printf("\n");
		printf("Usage: imgextr [options]\n");
		printf("       imgextr {-i input_file} {-t type} [-a | -e output_folder]\n");
		printf("\n");
		printf("Use -h for help.\n");
		printf("\n");
		
	} else {
		int i = 2;
		int check = 0;
		int run = 0;
		char option[10] = "";
		
		// detect fixed commands
		strncpy(option, argv[1], sizeof(option));
		toUpperCase(option);
		
		if (!strcmp(option, "-V")) {
			printf("%s [Version %s] (tags/v%s, %s, %s UTC)\n", APP_NAME, APP_VERSION, APP_VERSION, UPDATE_DATE, UPDATE_TIME);
			printf("Copyright (c) %s Group-14 of BIS30803. All rights reserved.\n", YEAR);
			printf("Last updated on %s/%s/%s.\n", UPDATE_DATE_D, UPDATE_DATE_M, UPDATE_DATE_Y);
			printf("\n");
			printf("Group Members:\n1. Tan Ching Hung\n2. Muhammad Nor Aiman Bin Norddin\n3. Nur Hazwani Hanum Binti Samsudin\n");
			printf("\n");
			
		} else if (!strcmp(option, "-H")) {
			printf("Usage: imgextr [options]\n");
			printf("       imgextr {-i input_file} {-t image_type} [-a | -e output_folder]\n");
			printf("\n");
			printf("Example: imgextr -i disk_image.raw -t jpeg -a\n");
			printf("         imgextr -i disk_image.raw -t png -e \"output folder\"\n");
			printf("\n");
			printf("All options are case-insensitive.\n");
			printf("\n");
			printf("Print help / version / capabilities:\n");
			printf("-h\t\t\tshow help\n");
			printf("-v\t\t\tshow version\n");
			printf("-s\t\t\tshow available image types\n");
			printf("\n");
			printf("Main options (mandatory):\n");
			printf("-i input_file\t\tset input file\n");
			printf("-t image_type\t\tset image type (use -s to refer list of available image types)\n");
			printf("\n");
			printf("Main options (optional):\n");
			printf("-a\t\t\tperform image analysis (default)\n");
			printf("-e output_folder\tperform image extraction\n");
			printf("\n");
			
		} else if (!strcmp(option, "-S")) {
			printf("List of available image types:\n");
			printf("jpeg\t\t- Joint Photographic Experts Group file, including standard JPEG/JFIF, JPG/EXIF and JPG/SPIFF\n");
			printf("png\t\t- Portable Network Graphics file\n");
			printf("\n");
			
		} else {
			i--;
			check = 1;
		}
		
		int I = 0, T = 0, E = 0;
		char inputPath[1000] = "";
		char type[10] = "";
		char outputFolder[1000] = "";
		
		// parse dynamic command and read option-parameter pair
		if (check) {
			char* options[10] = {"-I", "-T", "-A", "-E"};
			char currOpt[10] = "";		// store current active option
			
			// iterate through arguments
			for (; i < argc; i++) {
				if (argv[i][0] == '-' && argv[i][1] != '\0') {
					strncpy(option, argv[i], sizeof(option));
					toUpperCase(option);
					int idx = inArray(options, option);
					
					if (idx != -1) {
						options[idx] = "";
						
						if (!strcmp(option, "-I")) {
							I = 1;
							strncpy(currOpt, "INPUT", sizeof(currOpt));
						} else if (!strcmp(option, "-T")) {
							T = 1;
							strncpy(currOpt, "TYPE", sizeof(currOpt));
						} else if (!strcmp(option, "-E")) {
							E = 1;
							strncpy(currOpt, "EXTRACT", sizeof(currOpt));
						}
					} else {
						strncpy(currOpt, "", sizeof(currOpt));
						printf("Unknown option '%s'.\n", argv[i]);
					}
				} else {
					if (!strcmp(currOpt, "INPUT")) {
						strncpy(inputPath, argv[i], sizeof(inputPath));
					} else if (!strcmp(currOpt, "TYPE")) {
						strncpy(type, argv[i], sizeof(type));
					} else if (!strcmp(currOpt, "EXTRACT")) {
						strncpy(outputFolder, argv[i], sizeof(outputFolder));
					}
					
					strncpy(currOpt, "", sizeof(currOpt));
				}
			}
			
			// parameter validation
			if (I) {
				if (strcmp(inputPath, "")) {
					if (T) {
						if (strcmp(type, "")) {
							char* types[10] = {"JPEG", "PNG"};
							toUpperCase(type);
							int idx = inArray(types, type);
							
							if (idx != -1) {
								run = 1;
								
								if (E) {
									if (!strcmp(outputFolder, "")) {
										run = 0;
										printf("Invalid parameter for option '-e'.\n\n");
									}
								}
							} else {
								printf("Invalid parameter for option '-t'.\n\n");
							}
						} else {
							printf("Invalid parameter for option '-t'.\n\n");
						}
					} else {
						printf("Option '-t' is required.\n\n");
					}
				} else {
					printf("Invalid parameter for option '-i'.\n\n");
				}
			} else {
				printf("Option '-i' is required.\n\n");
			}
		}
		
		// execution
		if (run) {
			// open file of inputPath (raw data) in binary read mode
			FILE* inputFile = fopen(inputPath, "rb");
			
			// check if inputFile is valid
			if (inputFile) {
				// create a buffer to store data block
				unsigned char* buffer = malloc(BLOCK_SIZE);
				
				// check if buffer is successfully created and allocated in memory
				if (buffer) {
					unsigned char header[16];
					int headerLength = 0;
					unsigned char trailer[16];
					int trailerLength = 0;
					char extension[5];
					
					// determine header based in image type
					if (!strcmp(type, "JPEG")) {
						header[0] = 0xFF;
						header[1] = 0xD8;
						header[2] = 0xFF;
						headerLength = 3;
						trailer[0] = 0xFF;
						trailer[1] = 0xD9;
						trailerLength = 2;
						strncpy(extension, "jpeg", sizeof(extension));
					} else if (!strcmp(type, "PNG")) {
						header[0] = 0x89;
						header[1] = 0x50;
						header[2] = 0x4E;
						header[3] = 0x47;
						header[4] = 0x0D;
						header[5] = 0x0A;
						header[6] = 0x1A;
						header[7] = 0x0A;
						headerLength = 8;
						trailer[0] = 0x49;
						trailer[1] = 0x45;
						trailer[2] = 0x4E;
						trailer[3] = 0x44;
						trailer[4] = 0xAE;
						trailer[5] = 0x42;
						trailer[6] = 0x60;
						trailer[7] = 0x82;
						trailerLength = 8;
						strncpy(extension, "png", sizeof(extension));
					}
					
					// create array of offsets of found headers
					int headers[IMAGE_LIMIT];
					int headerCount = 0;
					int imageCount = 0;
					int imageFound = 0;
					int position = ftell(inputFile);
					
					// create array of output files
					FILE* outputFiles[IMAGE_LIMIT];
					int fileIdx = -1;
					
					printf("Finding image...\n");
					
					// read inputFile into buffer and iterate through data blocks
					while (fread(buffer, 1, BLOCK_SIZE, inputFile)) {
						int i = 0, j = 0, k = 0, l = 0;
						
						printf("\rScanning block (0x%08X - 0x%08X)...", position, ftell(inputFile));
						
						// iterate through each bytes in buffer
						for (i = 0; i < ftell(inputFile) - position; i++) {
							if (E) {
								// write current byte into output file if there has any active file
								if (fileIdx != -1) {
									fputc(buffer[i], outputFiles[fileIdx]);
								}
							}
							
							// check if byte sequence matches the header
							if (buffer[i] == header[j]) {
								if (j == headerLength - 1) {
									if (headerCount < IMAGE_LIMIT) {
										headers[headerCount++] = position + i - headerLength + 1;
										
										if (E) {
											if (outputFolder[strlen(outputFolder) - 1] != '\\' && outputFolder[strlen(outputFolder) - 1] != '/') {
												strncat(outputFolder, "\\", sizeof(outputFolder) - strlen(outputFolder));
											}
											
											char outputPath[1000];
											strncpy(outputPath, outputFolder, sizeof(outputPath));
											char imageCountStr[10];
											itoa(++imageCount, imageCountStr, 10);
											strncat(outputPath, imageCountStr, sizeof(outputPath) - strlen(outputPath));
											strncat(outputPath, ".", sizeof(outputPath) - strlen(outputPath));
											strncat(outputPath, extension, sizeof(outputPath) - strlen(outputPath));
											
											// open file of outputPath in binary write mode
											outputFiles[++fileIdx] = fopen(outputPath, "wb");
											
											// check if output file is valid
											if (!outputFiles[fileIdx]) {
												printf("\rFailed to create output file. Incorrect path or denied access to file.\n\n");
												exit(1);
											}
											
											// write header into the output file
											for (l = 0; l < headerLength; l++) {
												fputc(header[l], outputFiles[fileIdx]);
											}
										}
									}
									
									j = 0;
								}
								
								j++;
							} else {
								j = 0;
							}
							
							// check if byte sequence matches the trailer
							if (buffer[i] == trailer[k]) {
								if (k == trailerLength - 1) {
									if (headerCount > 0) {
										if (E) {
											if (fileIdx != -1) {
												fclose(outputFiles[fileIdx--]);
											}
										}
										
										printf("\r\tSOF found at offset 0x%08X			\n", headers[--headerCount]);
										printf("\tEOF found at offset 0x%08X\n", position + i);
										printf("%d image(s) found.\n\n", ++imageFound);
										printf("Finding next image...\n");
									}
									
									k = 0;
								}
								
								k++;
							} else {
								k = 0;
							}
						}
						
						position = ftell(inputFile);
					}
					
					printf("\r");
					
					if (E) {
						printf("Extraction complete. %d image(s) found & extracted.\n\n", imageFound);
					} else {
						printf("Analysis complete. %d image(s) found.\n\n", imageFound);
					}
				} else {
					printf("Failed to create buffer.\n\n");
				}
				
				// free up the buffer and close the inputFile
				free(buffer);
				fclose(inputFile);
			} else {
				printf("Failed to load input file. File not found or denied access to file.\n\n");
			}
		}
	}
	
	return 0;
}


// to convert string to uppercase
void toUpperCase(char* str) {
	while (*str) {
		*str = toupper(*str);
		str++;
	}
}


// to check if a string exists in given array, then return the position if found, otherwise -1
int inArray(char* array[], char element[]) {
	int i = 0;
	
	while (array[i]) {
		if (!strcmp(array[i], element)) return i;
		i++;
	}
	
	return -1;
}
