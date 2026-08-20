#ifndef PREPROCESSING_H
#define PREPROCESSING_H

unsigned char *resize_bilinear(
    const unsigned char *src,
    int src_width,
    int src_height,
    int dst_width,
    int dst_height
);

int create_lr_hr_pair(
    const char *input_path,
    const char *hr_path,
    const char *lr_path,
    int hr_size,
    int scale
);

#endif
