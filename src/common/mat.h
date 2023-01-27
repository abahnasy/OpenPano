#include <iostream>
#include <memory>



template <typename T>
class Mat {
public:
    Mat() = default;
    Mat(int height, int width, int channels) : height(height), width(width), channels(channels),
    data(std::shared_ptr<T>(new T[height * width * channels], std::default_delete<T[]>())) {}
    ~Mat() = default;
    T& at(int row, int col, int channel) {
        static_assert(row >= 0 && row < height, "row index out of range");
        static_assert(col >= 0 && col < width, "col index out of range");
        static_assert(channel >= 0 && channel < channels, "channel index out of range");
        return data.get()[row * width * channels + col * channels + channel];
    }
    Mat<T> clone() {
        Mat<T> result(height, width, channels);
        // memcpy(result.ptr(0), this->ptr(0), sizeof(T) * m_rows * m_cols * m_channels);
        std::copy(data.get(), data.get() + height * width * channels, result.data.get());
        return result;
    }
    int rows() const { return height; }
    int cols() const { return width; }
    int height() const { return height; }
    int width() const { return width; }
    int channels() const { return channels; }
    int pixels() const { return height * width; }
    
        
private:
    int height;
    int width;
    int channels;
    std::shared_ptr<T> data;

};

using Mat32f = Mat<float>;
using Matuc = Mat<unsigned char>;
