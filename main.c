#include <unistd.h>
#include <stdio.h>
#include <linux/limits.h>
#include <locale.h>
#include <wchar.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define LIGHTNESS_THRESHOLD 127

void print_braille(unsigned char *image, int width, int height, int channels);

int main(int argc, char *argv[]) {
    // get file name
    if(argc != 2) {
        printf("Usage: [filename]");
        return 1;
    }

    char *filename = argv[1];

    setlocale(LC_CTYPE, "");

    // get current working path
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 1;
    }
    strcat(cwd, "/");
    char *file_path = strcat(cwd, filename);



    // get image from path
    int width, height, channels;

    unsigned char *image = stbi_load(file_path, &width, &height, &channels, 0);
    if (!image) {
        fprintf(stderr, "Error loading image: %s\n", stbi_failure_reason());
        return 1;
    }
    printf("Image: %s | %dx%d, channels: %d\n",filename, width, height, channels);


    print_braille(image, width, height, channels);
    return 0;
}

unsigned char get_bit(int cell_y, int cell_x) {
    unsigned char byte = 0b00000001;
    // 1 4
    // 2 5
    // 3 6
    // 7 8

    if(cell_x==0) {
        switch(cell_y) {
            case 1:
                byte = byte<<1;
                break;
            case 2:
                byte = byte<<2;
                break;
            case 3:
                byte = byte<<6;
                break;
            default:
                break;
        }
    }
    if(cell_x==1) {
        switch(cell_y) {
            case 0:
                byte = byte<<3;
                break;
            case 1:
                byte = byte<<4;
                break;
            case 2:
                byte = byte<<5;
                break;
            case 3:
                byte = byte<<7;
                break;
            default:
                break;
        }
    }
    return byte;
}

void print_braille(unsigned char *image, int width, int height, int channels) {

    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 2) {
            unsigned int byte_val = 0;

            // Iterate over the 2x4 cell
            for (int cell_y = 0; cell_y < 4; cell_y++) {
                for (int cell_x = 0; cell_x < 2; cell_x++) {
                    int pixel_y = y + cell_y;
                    int pixel_x = x + cell_x;

                    if (pixel_y < height && pixel_x < width) {
                        unsigned char *pixel = image + ((pixel_y * width) + pixel_x) * channels;

                        int avg = (pixel[0] + pixel[1] + pixel[2]) / 3;

                        // double avg = pixel[0] * 0.2125 + pixel[1] * 0.7154 + pixel[2] * 0.0721;

                        if (avg < LIGHTNESS_THRESHOLD) {
                            // 1 4
                            // 2 5
                            // 3 6
                            // 7 8
                            byte_val += get_bit(cell_y, cell_x);
                        }
                    }
                }
            }

            wchar_t braille_char_start = 0x2800;

            setlocale(LC_CTYPE, "");
            printf("%lc", braille_char_start + byte_val);

        }
        printf("\n");
    }
}