#include <vector>

#include "common/mat.h"


namespace panorama {

// TODO: move to separate file
class Descriptor {
public:
    std::vector<float> coords;
    std::vector<float> descriptor;
};

class FeatureDetector {
protected:
    int sift_working_size;
    int num_octave;
    int num_scale;
    double scale_factor;
    double gauss_sigma;
    double gauss_window_factor;


public:
    FeatureDetector() = delete;
    FeatureDetector(int sift_working_size, int num_octave, int num_scale, double scale_factor, double gauss_sigma, double gauss_window_factor); 
    virtual ~FeatureDetector();
    vector<Descriptor> run(const Mat32f& img) const;
    virtual vector<Descriptor> _extract_features(const Mat32f& img) const = 0;
};


class SIFTDetector : public FeatureDetector {
public:
    SIFTDetector();
    ~SIFTDetector();
    std::vector<Descriptor> _extract_features(const Mat32f& img) const override;
    
};

class BRIEFDetector : public FeatureDetector {
public:
    BRIEFDetector();
    ~BRIEFDetector();
    
};

class SFIT {
public:
    SFIT();
    ~SFIT();
};

// TODO: split classes into different files
/*
Input:
    - image
    - number of octave
    - number of scale
    - scale factor
    - gaussian sigma
    - gaussian window factor

Output:
    - pyramid of images

*/

class SpaceScale {
public:
    SpaceScale(const Mat32f& img, int num_octave, int num_scale, double scale_factor, double gauss_sigma, double gauss_window_factor);
    ~SpaceScale();
    Mat32f* get_pyramid() const;
    int get_num_octave() const;
    int get_num_scale() const;
    double get_scale_factor() const;
    double get_gauss_sigma() const;
    double get_gauss_window_factor() const;
private:
    vector<GaussianPyramid> pyramids; // length = num_octave
    int num_octave;
    int num_scale;
    double scale_factor;
    double gauss_sigma;
    double gauss_window_factor;
};

class DOGSpace {
public:
    DOGSpace(const SpaceScale& ss);
    ~DOGSpace();
    Mat32f* get_pyramid() const;
    int get_num_octave() const;
    int get_num_scale() const;
    double get_scale_factor() const;
    double get_gauss_sigma() const;
    double get_gauss_window_factor() const;
};

class ExtremaDetector {
public:
    ExtremaDetector(const DOGSpace& ds);
    ~ExtremaDetector();
    std::vector<Descriptor> get_extrema() const;
};

class OrientationAssignment {
public:
    OrientationAssignment(const SpaceScale& ss, const DOGSpace& ds, const std::vector<Descriptor>& keypoints);
    ~OrientationAssignment();
    std::vector<Descriptor> get_keypoints() const;
};

class SIFT {
public:
    SIFT(const SpaceScale& ss, const std::vector<Descriptor>& keypoints);
    ~SIFT();
    std::vector<Descriptor> get_descriptors() const;
};


/*
* Given an image, build an octave of images with different blurred versions of the original image
*
*/

class GaussianPyramid {
public:
    GaussianPyramid(const Mat32f& img, int num_scale, double scale_factor, double gauss_sigma, double gauss_window_factor);
    ~GaussianPyramid();
    // Mat32f* get_pyramid() const;
    // int get_num_scale() const;
    cal_mag_and_ort(int idx);
private:
    std::vector<Mat32f> data; // length = num_scale
    std::vector<Mat32f> mag; // length = num_scale
    std::vector<Mat32f> ort; // length = num_scale, value in [0, 2*pi]
    int num_scale;
    int original_width, original_height;
};

class MultiScaleGaussianBlur {
public:
    MultiScaleGaussianBlur(int num_scale, double scale_factor, double gauss_sigma, double gauss_window_factor);
    ~MultiScaleGaussianBlur();
    // std::vector<Mat32f> get_blurred_images(const Mat32f& img) const;
    Mat32f run(const Mat32f& img, int scale_idx) const;
    
private:
    std::vector<GaussianBlur> data; // length = num_scale
    int num_scale;
    double scale_factor;
    double gauss_sigma;
    double gauss_window_factor;
};


/* Defined Here in .h file because of the template function `get_blurred()` 
 * change API to get the src and dst as arguments, think about how to handle the dims of the dst outside the class

*/

class GaussianBlur {
public:
    GaussianBlur(double sigma, double gauss_window_factor){
        // prepare kernel
        this->sigma = sigma;
        // TODO: decide on how to calculate kernel size ?!
        this->kernel_size = kw = ceil(0.3 * (sigma / 2 - 1) + 0.8) * gauss_window_factor; // taken from OpenPano
        // this->kernel_size = std::ceil(6 * sigma); // from https://github.com/dbarac/sift-cpp/blob/master/src/image.cpp
        // check this also: https://gist.github.com/bfraboni/946d9456b15cac3170514307cf032a27
        // odd size kernel
        if (kernel_size % 2) == 0:
            kernel_size += 1;
        kernel_buffer.reset(new float[kernel_size]);
        center {kernel_size / 2};
        kernel_ptr = kernel_buffer.get() + center; // pointer to the center of the kernel, so you can use -ve idxs
        kernel_ptr[0] = 1; // TODO: why?

        float exp_coeff = -1.0 / (2.0 * sigma * sigma);
        float wsum = 1.0;
        for (int i = 1; i <= center; ++i) {
            float w = std::exp(exp_coeff * i * i);
            kernel_ptr[i] = w;
            wsum += w*2;
        }
        float fac = 1.0 / wsum;
        kernel_ptr[0] = fac;
        for (int i = 1; i <= center; ++i) {
            kernel_ptr[-i] = kernel_ptr[i]*fac;
        }
        // TODO: review calculations and make sure it is a normalized kernel
    }
    ~GaussianBlur();
    
    template <typename T>
    Mat<T> get_blurred(Mat<T>& img) const {
        static_assert(img.channels() == 1, "Only support single channel image");
        Mat<T> blurred_img(img.rows, img.cols, img.channels());
        const int h = img.height();
        const int w = img.width();

        std::vector<T> cur_line_mem(center*2 + std::max(h,w), 0)
        T* cur_line = cur_line_mem.data() + center; // TODO: change to unique ptr

        // over columns
        for (size_t j{0}, j < w; ++j) {
            const T* src = img.ptr(0, j);
            for(size_t i{0}; i < h; ++i) {
                cur_line[i] = *src;
                src += w; // get the next pixel in the column
            }

            // pad with border values
            T v0 = cur_line[0];
            for(size_t i{1}; i <= center; ++i) {
                cur_line[-i] = v0;
            }
            v0 = cur_line[h-1];
            for(size_t i{0}; i < center; ++i) {
                cur_line[h+i] = v0;
            }

            T* dst = blurred_img.ptr(0, j);
            for(size_t i{0}; i < h; ++i) {
                float sum = 0;
                for(size_t k{-1*center}; k <= center; ++k) {
                    sum += cur_line[i+k] * kernel_ptr[k];
                }
                *dst = sum;
                dst += w; // next pixel in the column
            }
        }

        // over rows
        for(size_t i{0}; i < h; ++i) {
            const T* dest = blurred_img.ptr(i); // second conv on the result of the first one
            // TOOD: change with std::copy()
            memcopy((void*)cur_line, (void*)dest, w*sizeof(T));
            // for(size_t j{0}; j < w; ++j) {
            //     cur_line[j] = *src;
            //     src += 1; // next pixel in the row
            // }

            // pad with border values
            T v0 = cur_line[0];
            for(size_t j{1}; j <= center; ++j) {
                cur_line[-j] = v0;
            }
            v0 = cur_line[w-1];
            for(size_t j{0}; j < center; ++j) {
                cur_line[w+j] = v0;
            }

            
            for(size_t j{0}; j < w; ++j) {
                float sum = 0;
                for(size_t k{-1*center}; k <= center; ++k) {
                    sum += cur_line[j+k] * kernel_ptr[k];
                }
                *dest = sum; // TODO: combine both lines -> *(dest++) = sum;
                dest += 1; // next pixel in the row
            }

            return blurred_img;
        }


    }
private:
    float sigma;
    int kernel_size;
    float* kernel_ptr; // TODO: why not unique ptr?
    std::unique_ptr<float, std::default_delete<float>> kernel_buffer;

    const int center;

};

class 









} // namespace panorama