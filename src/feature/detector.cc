
#include "feature/detector.h"
#include "utils/image.h"

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
GaussianPyramid::GaussianPyramid(const Mat32f& img, int num_scale) {
    // init with octave size
    this->num_scale = num_scale; 
    data(num_scale); //TODO: use this-> for better readiblity
    mag(num_scale);
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

std::vector<Mat32f> MultiScaleGaussianBlur::get_blurred_images(const Mat32f& img) const {
    std::vector<Mat32f> blurred_images;
    for (size_t i = 0; i < num_scale; ++i) {
        Mat32f blurred_img(img.height(), img.width(), img.channels());
        data[i].run(img, blurred_img);
        blurred_images.push_back(blurred_img);
    }
    return blurred_images;

Mat32f get_blurred_image(const Mat32f& img, int scale_idx) const {
    // Mat32f blurred_img(img.height(), img.width(), img.channels());
    // return data[scale_idx - 1].run(img, blurred_img);
    // return blurred_img;

    return data[scale_idx - 1].get_blurred(img); // TODO: fix, take src and dst images, don't return concrete objects from executer classes / functions
}

} // namespace panorama