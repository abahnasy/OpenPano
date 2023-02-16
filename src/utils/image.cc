#include "image.h"



template <typename T>
void _resize_bilinear(const Mat<T>& src, Mat<T>& dst) {
    // taken from OpenPano repo TODO: add reference

    // place holders for the anchor points
    std::vector<int> tabsx(dst.rows());
	std::vector<int> tabsy(dst.cols());
	std::vector<float> tabrx(dst.rows());
	std::vector<float> tabry(dst.cols());
    
    // get ratio of the new image to the old image
    const float fx = (float)(dst.rows()) / src.rows();
	const float fy = (float)(dst.cols()) / src.cols();
	const float ifx = 1.f / fx;
	const float ify = 1.f / fy;

    // get the anchor points for rows
    for (int dx = 0; dx < dst.rows(); dx++) {
        // tabsx[i] = (int)(i * ifx);
        // tabrx[i] = (i * ifx) - tabsx[i];
        float rx = (dx+0.5f) * ifx - 0.5f;
		int sx = floor(rx);
		rx -= sx;
		if (sx < 0) {
			sx = rx = 0;
		} else if (sx + 1 >= src.rows()) {
			sx = src.rows() - 2;
			rx = 1;
		}
		tabsx[dx] = sx;
		tabrx[dx] = rx;
    }
    // get the anchor points for cols
    for (int dy = 0; dy < dst.cols(); dy++) {
        // tabsy[i] = (int)(i * ify);
        // tabry[i] = (i * ify) - tabsy[i];
        float ry = (dy+0.5f) * ify - 0.5f;
        int sy = floor(ry);
        ry -= sy;
        if (sy < 0) {
            sy = ry = 0;
        } else if (sy + 1 >= src.cols()) {
            sy = src.cols() - 2;
            ry = 1;
        }
        tabsy[dy] = sy;
        tabry[dy] = ry;
    }

    // resize the image
    const int ch = src.channels();
	for (int dx = 0; dx < dst.rows(); ++dx) {
		const float *p0 = src.ptr(tabsx[dx]+0);
		const float *p1 = src.ptr(tabsx[dx]+1);
		float *pdst = dst.ptr(dx);
		float rx = tabrx[dx], irx = 1.0f - rx;
		for (int dy = 0; dy < dst.cols(); ++dy) {
			float *pcdst = pdst + dy*ch;
			const float *pc00 = p0 + (tabsy[dy]+0)*ch;
			const float *pc01 = p0 + (tabsy[dy]+1)*ch;
			const float *pc10 = p1 + (tabsy[dy]+0)*ch;
			const float *pc11 = p1 + (tabsy[dy]+1)*ch;
			float ry = tabry[dy], iry = 1.0f - ry;
			for (int c = 0; c < ch; ++c) {
				float res = rx * (pc11[c]*ry + pc10[c]*iry)
					+ irx * (pc01[c]*ry + pc00[c]*iry);
				pcdst[c] = res;
			}
		}
	}



}

void resize(const Mat<T>& src, Mat<T>& dst) {
    static_assert(src.rows() > 1 && src.cols() > 1, "src image is too small");
    static_assert(dst.rows() > 1 && dst.cols() > 1, "dst image is too small");
    static_assert(src.channels() == dst.channels(), "src and dst image must have the same number of channels");
    _resize_bilinear(src, dst);
    //TODO: add other resize methods, e.g., nearest neighbours, add configuration parameter for the resize method

}