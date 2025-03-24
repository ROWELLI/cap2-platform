#include <cstring>
#include <iostream>

using namespace std;

#define BGR_RPOS_CH(H, W, X, Y) (((Y)*(W)*3) + ((X) * 3) + 2)
#define BGR_GPOS_CH(H, W, X, Y) (((Y)*(W)*3) + ((X) * 3) + 1)
#define BGR_BPOS_CH(H, W, X, Y) (((Y)*(W)*3) + ((X) * 3))

#define RGB_UINT32_MASK 0xFF

#define RGB_RPOS_UINT32(H, W, X, Y) (((((Y)*(W)) + (X))>>16) & RGB_UINT32_MASK)
#define RGB_GPOS_UINT32(H, W, X, Y) (((((Y)*(W)) + (X)) >> 8) & RGB_UINT32_MASK)
#define RGB_BPOS_UINT32(H, W, X, Y) ((((Y)*(W)) + (X)) & RGB_UINT32_MASK)

/*
class ImageProcessor{
    public:
        //ratio will only accept the value as multiples of 30
        ImageProcessor(int height, int tpad, int sratio, int eratio, int* rows, int row_size){
            this->height = height;
            this->tpad = tpad;
            this->sratio = (sratio/30)*30;
            this->eratio = (eratio/30)*30;
            this->row_size = row_size;
            this->rows = rows;
            if(row_size > tpad)
                calc_rows = new int[row_size-tpad];
            calc_row();
        }
        int calc_row();
        int mask(unsigned char *image);
        int rotate();
        int toArray();
        int clear();
        unsigned char *image;

    private:
        int height;
        int tpad;
        int sratio;
        int eratio;
        int row_size;
        int* calc_rows;
        int* rows;
};
*/

#include "image_processor.h"

/**
 * calculate row for spherical display using end-ratio, start-ratio
 *
 * returns
 * 0 : success
 * 1 : null error
 * 2 : ratio error
 */
int ImageProcessor::calc_row(){
    int *p_rows = rows+tpad;
    int dratio = eratio-sratio;
    int sum = 0;
    if(!rows) return 1;
    if(!calc_rows) return 1;
    if(dratio < 30) return 2;
    for(int i = 0; i < calc_row_size; i++){
        calc_rows[i] = *(p_rows+i)*(dratio/360.0);
        sum+=calc_rows[i];
    }
    partial_buf_size = sum*3;
    partial_buf = new unsigned char[partial_buf_size];
    // for(int i = 0; i < calc_row_size; i++){
    //     cout << calc_rows[i] << ',';
    // }
    // cout << endl;
    return 0;
}

int ImageProcessor::mask(uint32_t *image) {
    if (!rows || !calc_rows || !partial_buf) return 1;

    int mask_height = std::max(1, height / calc_row_size);
    int idx = 0;

    for (int i = 0; i < calc_row_size; i++) {
        double mask_width = (calc_rows[i] > 0) ? (double)width / calc_rows[i] : 1.0;

        for (int j = 0; j < calc_rows[i]; j++) {
            int xpos = mask_width * j + mask_width/2;
            int ypos = i * mask_height + mask_height/2;

            if (idx + 3 < partial_buf_size) {  // Prevent buffer overflow
                partial_buf[i] = RGB_GPOS_UINT32(height, width, xpos, ypos);
                partial_buf[i+1] = RGB_RPOS_UINT32(height, width, xpos, ypos);
                partial_buf[i+2] = RGB_BPOS_UINT32(height, width, xpos, ypos);
                idx += 3;
            }
        }
    }
    return 0;
}

int ImageProcessor::mask(unsigned char *image) {
    if (!rows || !calc_rows || !partial_buf) return 1;

    int mask_height = std::max(1, height / calc_row_size);
    int idx = 0;

    for (int i = 0; i < calc_row_size; i++) {
        double mask_width = (calc_rows[i] > 0) ? (double)width / calc_rows[i] : 1.0;

        for (int j = 0; j < calc_rows[i]; j++) {
            int xpos = mask_width * j + mask_width/2;
            int ypos = i * mask_height + mask_height/2;



/*
            for (int xmask = 0; xmask < (int)mask_width; xmask++) {
                for (int ymask = 0; ymask < mask_height; ymask++) {
                    int xpos = std::min(width - 1, (int)std::round(mask_width * j + xmask));
                    int ypos = std::min(height - 1, i * mask_height + ymask);

                    rsum += image[GET_RPOS(height, width, xpos, ypos)];
                    gsum += image[GET_GPOS(height, width, xpos, ypos)];
                    bsum += image[GET_BPOS(height, width, xpos, ypos)];
                }
            }
*/         
            if (idx + 3 < partial_buf_size) {  // Prevent buffer overflow
                partial_buf[idx] = image[BGR_GPOS_CH(height, width, xpos, ypos)];
                partial_buf[idx + 1] = image[BGR_RPOS_CH(height, width, xpos, ypos)];
                partial_buf[idx + 2] = image[BGR_BPOS_CH(height, width, xpos, ypos)];
                idx += 3;
            }
        }
    }
    
    return 0;
}
/*
int ImageProcessor::mask(unsigned char *image) {
    if (!rows || !calc_rows || !partial_buf) return 1;

    int mask_height = std::max(1, height / calc_row_size);
    int idx = 0;

    for (int i = 0; i < calc_row_size; i++) {
        double mask_width = (calc_rows[i] > 0) ? (double)width / calc_rows[i] : 1.0;

        for (int j = 0; j < calc_rows[i]; j++) {
            unsigned long long rsum = 0, gsum = 0, bsum = 0;
            int mask_size = std::max(1, (mask_height * (int)mask_width));

            for (int xmask = 0; xmask < (int)mask_width; xmask++) {
                for (int ymask = 0; ymask < mask_height; ymask++) {
                    int xpos = std::min(width - 1, (int)std::round(mask_width * j + xmask));
                    int ypos = std::min(height - 1, i * mask_height + ymask);

                    rsum += image[GET_RPOS(height, width, xpos, ypos)];
                    gsum += image[GET_GPOS(height, width, xpos, ypos)];
                    bsum += image[GET_BPOS(height, width, xpos, ypos)];
                }
            }

            if (idx + 3 < partial_buf_size) {  // Prevent buffer overflow
                partial_buf[idx] = (unsigned char)(gsum / mask_size);
                partial_buf[idx + 1] = (unsigned char)(rsum / mask_size);
                partial_buf[idx + 2] = (unsigned char)(bsum / mask_size);
                idx += 3;
            }
        }
    }
    
    cout << "image: ";
    for(int i = 0; i<21; i++){
        cout << (int)image[i] << ", ";
    }
    cout << endl;

    cout << "buf: ";
    for(int i = 0; i<21; i++){
        cout << (int)partial_buf[i] << ", ";
    }
    cout << endl;
    
    return 0;
}
*/
int ImageProcessor::rotate(){
    unsigned char * p_partial_buf = partial_buf;
    long long idx = 0;
    int h = 0;
    if(!processed_image) return 1;
/*
    for(int i = 0; i < partial_buf_size; i+=3){
        cout << "(" << (unsigned int)partial_buf[i] << " " << (unsigned int)partial_buf[i+1] << " " << (unsigned int)partial_buf[i+2] << ")";
    }
  
  */
    //cout << endl;
    //cout << processed_image_size << " " << sratio << endl;
    memset(processed_image, 0, processed_image_size);
    while(idx < processed_image_size){
        if(h >= tpad+calc_row_size){
            break;
        }
        if(h < tpad){
            idx+=rows[h]*3;
            h++;
            continue;
        }
        int lpad = ((int)((sratio/360.0)*rows[h]))*3;
        int row_ch= rows[h]*3;
        int calc_row_ch = calc_rows[h-tpad]*3;
        memcpy(processed_image+(idx+lpad), p_partial_buf, min(row_ch-lpad, calc_row_ch));
        //cout << lpad << "," << row_ch << "," << calc_row_ch << endl;
        if(row_ch-lpad < calc_row_ch){
            memcpy(processed_image+idx, p_partial_buf+(row_ch-lpad+1), calc_row_ch-(row_ch-lpad));
        }

/*
        if(h == 15){
            for(int i = 0; i < row_ch; i++){
                cout << (int)(processed_image+idx)[i] << " ";
            }
            cout << endl;
        }
*/
        p_partial_buf+=calc_row_ch;
        idx+=row_ch;
        h++;
    }
    return 0;
}
