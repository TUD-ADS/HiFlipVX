/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_reader.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*  Reads and write PGM files
*/

#include "vx_helper.h"

/*****************************************************************************/
/* Macros */
#define HI(num) (((num) & 0x0000FF00) << 8)
#define LO(num) ((num) & 0x000000FF)

/*****************************************************************************/
/* PGM data structure */
typedef struct PgmImage {
    vx_int32 row;
    vx_int32 col;
    vx_int32 max_gray;
    vx_uint8 *matrix;
} PgmData;

/*****************************************************************************/
/* Skip comments */
void SkipComments(FILE *fp) {
    vx_int32 ch;
    vx_char line[100];

    while ((ch = fgetc(fp)) != EOF && isspace(ch));
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        SkipComments(fp);
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
}

/*****************************************************************************/
/* Read image */
PgmData* ReadPgm(const char *file_name, PgmData*data) {
    FILE *pgm_file;
    vx_char version[3];
    vx_int32 i = 0, j = 0;
    vx_int32 lo = 0, hi = 0;
    vx_int32 dat = 0, count = 0;

    // Open file
    pgm_file = fopen(file_name, "rb");
    if (pgm_file == NULL) {
        perror("Cannot open file to read!");
        return nullptr;
    }

    // Check file type
    fgets(version, sizeof(version), pgm_file);
    if (strcmp(version, "P5")) {
        perror("Wrong file type!");
        return nullptr;
    }

    // Read column amount
    SkipComments(pgm_file);
    count = fscanf(pgm_file, "%d", &(data->col));
    if (feof(pgm_file) || ferror(pgm_file)) {
        perror("File read error!");
        return nullptr;
    }

    // Read row amount
    SkipComments(pgm_file);
    count = fscanf(pgm_file, "%d", &(data->row));
    if (feof(pgm_file) || ferror(pgm_file)) {
        perror("File read error!");
        return nullptr;
    }

    // Read max value
    SkipComments(pgm_file);
    count = fscanf(pgm_file, "%d", &(data->max_gray));
    if (feof(pgm_file) || ferror(pgm_file)) {
        perror("File read error!");
        return nullptr;
    }

    // Read image data
    fgetc(pgm_file);
    if (data->max_gray > 255) {
        for (i = 0; i < data->row; ++i) {
            for (j = 0; j < data->col; ++j) {
                hi = fgetc(pgm_file);
                lo = fgetc(pgm_file);
                data->matrix[i*(data->col) + j] = (hi << 8) + lo;
            }
        }
    } else {
        for (i = 0; i < data->row; ++i) {
            for (j = 0; j < data->col; ++j) {
                lo = fgetc(pgm_file);
                data->matrix[i*(data->col) + j] = lo;
            }
        }
    }

    // Close file
    fclose(pgm_file);
    return data;
}

/*****************************************************************************/
/* Write image */
void WritePgm(const char* filename, const PgmData* data) {
    FILE * pgm_file;
    vx_int32 i = 0, j = 0;
    vx_int32 hi = 0, lo = 0;

    // Open file
    pgm_file = fopen(filename, "wb");
    if (pgm_file == NULL) {
        perror("Cannot open file to write");
        return;
    }

    // Write file type
    fprintf(pgm_file, "P5 ");
    if (ferror(pgm_file)) {
        perror("Write file error!");
        return;
    }

    // Write column & row amount
    fprintf(pgm_file, "%d %d ", data->col, data->row);
    if (ferror(pgm_file)) {
        perror("Write file error!");
        return;
    }

    // Write max value
    fprintf(pgm_file, "%d ", data->max_gray);
    if (ferror(pgm_file)) {
        perror("Write file error!");
        return;
    }

    // Write image data
    if (data->max_gray > 255) {
        for (i = 0; i < data->row; ++i) {
            for (j = 0; j < data->col; ++j) {
                hi = HI(data->matrix[i*(data->col) + j]);
                lo = LO(data->matrix[i*(data->col) + j]);
                fputc(hi, pgm_file);
                fputc(lo, pgm_file);
            }

        }
    } else {
        for (i = 0; i < data->row; ++i) {
            for (j = 0; j < data->col; ++j) {
                lo = LO(data->matrix[i*(data->col) + j]);
                fputc(lo, pgm_file);
            }
        }
    }

    // Close file
    fclose(pgm_file);
}

/*****************************************************************************/
/* Copy the MetaData for the Pgm Image */
void CopyImageMetaData(struct PgmImage* src, struct PgmImage* dst) {
    dst->col = src->col;
    dst->row = src->row;
    dst->max_gray = src->max_gray;
}
