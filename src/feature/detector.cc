
#include <cmath>
#include "feature/detector.h"
#include "utils/image.h"


const double EPS = 1e-6;
namespace {

// TODO: move to utils
// fast approximation to atan2.
// atan2(a, b) = fast_atan(a, b), given max(abs(a),abs(b)) > EPS
// http://math.stackexchange.com/questions/1098487/atan2-faster-approximation
// save cal_mag_ort() 40% time
// from OpenPano
float fast_atan(float y, float x) {
	float absx = fabs(x), absy = fabs(y);
	float m = max(absx, absy);

	// undefined behavior in atan2.
	// but here we can safely ignore by setting ort=0
	if (m < EPS) return -M_PI;
	float a = min(absx, absy) / m;
	float s = a * a;
	float r = ((-0.0464964749 * s + 0.15931422) * s - 0.327622764) * s * a + a;
	if (absy > absx)
		r = M_PI_2 - r;
	if (x < 0) r = M_PI - r;
	if (y < 0) r = -r;
	return r;
}
}

namespace panorama {

std::vector<Descriptor> FeatureDetector::run(const Mat32f& img) const {
    return _extract_features(img);
}

FeatureDetector::FeatureDetector(int sift_working_size, int num_octave, int num_scale, double scale_factor, double gauss_sigma, double gauss_window_factor) : sift_working_size(sift_working_size), num_octave(num_octave), num_scale(num_scale), scale_factor(scale_factor), gauss_sigma(gauss_sigma), gauss_window_factor(gauss_window_factor) {}

std::vector<Descriptor> SIFTDetector::_extract_features(const Mat32f& img) const  override{
    // why this equation?
    float ratio =  this->sift_working_size * 2.0f / (img.width() + img.height());
    Mat32f scaled_img (img.height() * ratio, img.width() * ratio, img.channels());
    resize(img, scaled_img); // move the original image to the scaled image with the new dimension
    // scale space
    SpaceScale ss(scaled_img, this->num_octave, this->num_scale, this->scale_factor);
    // difference of gaussian (DoG)
    DOGSpace ds(ss);
    // local extrema
    ExtremaDetector ex_detc(ds);
    auto keypoints = ex_detc.get_extrema();
    // orientation assignment
    OrientationAssignment orientation_assignment(ss,ds,keypoints); // why taking all of these?
    // descriptor
    keypoints = orientation_assignment.get_keypoints();

    SIFT sift(ss, keypoints);
    auto descriptors = sift.get_descriptors();
    return descriptors;
}

// TODO: split classes into different files

/* GaussuanPyramid Implementatinos */
GaussianPyramid::GaussianPyramid(const Mat32f& img, int num_scale, double scale_factor, double gauss_sigma, double gauss_window_factor) {
    // init with octave size
    this->num_scale = num_scale; 
    this->scale_factor = scale_factor;
    this->gauss_sigma = gauss_sigma;
    this->gauss_window_factor = gauss_window_factor;
    data(num_scale); //TODO: use this-> for better readiblity
    mag(num_scale); // TODO: user .reserve() to secure memory and avoid resizing
    ort(num_scale);
    original_width = img.width(); // TODO: replace with curly braces initialization
    original_height = img.height();

    if (img.channels() == 3) {
        Mat32f gray_img(img.height(), img.width(), 1);
        rgb2gray(img, gray_img);
        data[0] = gray_img;
    } else {
        data[0] = img.clone();
    }

    MultiScaleGaussianBlur gaussian_blurer(num_scale, scale_factor, gauss_sigma, gauss_window_factor);
    for (size_t i = 1; i < num_scale; ++i) {
        data[i] = gaussian_blurer.run(data[0], i);
        calc_mag_and_ort(i);
    }

}



void GaussianPyramid::calc_mag_and_ort(int i) {
    const Mat32f& orig = data[i];
    int w = orig.width();
    int h = orig.height();
    mag[i] = Mat32f(h, w, 1);
    ort[i] = Mat32f(h, w, 1);

    for (size_t y = 0; y < h - 1; ++y) {
        float* mag_row = mag[i].ptr(y);
        float* ort_row = ort[i].ptr(y);
        const float* orig_row = orig.ptr(y);
        const float* orig_row_prev = orig.ptr(y - 1);
        const float* orig_row_next = orig.ptr(y + 1);
        mag_row[0] = 0;
        ort_row[0] = M_PI;
        for (size_t x = 1; x < w - 1; ++x) {
            if (y >= 1 && y < h - 1) {
                float dx = orig_row[x + 1] - orig_row[x - 1];
                float dy = orig_row_next[x] - orig_row_prev[x];
                
                mag_row[x] = std::sqrt(dx * dx + dy * dy);
                // mag_row[x] = hypotf(dx, dy);
                // ort_row[x] = std::atan2(dy, dx);
                ort_row[x] = fast_atan(dy, dx) + M_PI;
            } else {
                mag_row[x] = 0;
                ort_row[x] = M_PI;
            }
            // float dx = orig(y, x + 1) - orig(y, x - 1);
            // float dy = orig(y + 1, x) - orig(y - 1, x);
            // mag[i](y, x) = std::sqrt(dx * dx + dy * dy);
            // ort[i](y, x) = std::atan2(dy, dx);
        }
    }

    mag_row[w-1] = 0;
    ort_row[w-1] = M_PI;


}

MultiScaleGaussianBlur::MultiScaleGaussianBlur(int num_scale, double scale_factor, double gauss_sigma, double gauss_window_factor) {
    
    this->num_scale(num_scale);
    this->scale_factor(scale_factor);
    this->gauss_sigma(gauss_sigma);
    this->gauss_window_factor(gauss_window_factor);

    double temp_gauss_sigma = gauss_sigma;
    for(size_t i = 0, i < num_scale - 1, ++i) {
        // REF: https://yasenh.github.io/post/cpp-diary-1-emplace_back/
        data.emplace_back(
            temp_gauss_sigma, gauss_window_factor
        );
        temp_gauss_sigma *= scale_factor;

    }    
}

// std::vector<Mat32f> MultiScaleGaussianBlur::get_blurred_images(const Mat32f& img) const {
//     std::vector<Mat32f> blurred_images;
//     for (size_t i = 0; i < num_scale; ++i) {
//         Mat32f blurred_img(img.height(), img.width(), img.channels());
//         data[i].run(img, blurred_img);
//         blurred_images.push_back(blurred_img);
//     }
//     return blurred_images;

Mat32f run(const Mat32f& img, int scale_idx) const {
    // Mat32f blurred_img(img.height(), img.width(), img.channels());
    // return data[scale_idx - 1].run(img, blurred_img);
    // return blurred_img;

    return data[scale_idx - 1].get_blurred(img); // TODO: fix, take src and dst images, don't return concrete objects from executer classes / functions
}



/* SpaceScale Implementations */
SpaceScale::SpaceScale(const Mat32f& img, int num_octave, int num_scale, double scale_factor) {
    this->num_octave = num_octave;
    this->num_scale = num_scale;
    this->scale_factor = scale_factor;
    data.resize(num_octave);
    for (size_t i = 0; i < num_octave; ++i) {
        data[i] = GaussianPyramid(img, num_scale, scale_factor);
        // downsample
        if (i < num_octave - 1) {
            img = downsample(img);
        }
    }

} // namespace panorama