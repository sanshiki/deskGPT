#include <opencv4/opencv2/opencv.hpp>
#include <iostream>

using namespace std;

unsigned char* image_src = new unsigned char[240 * 240 * 3];

void SetImageSrc(const cv::Mat &mat){
    cv::Mat temp;
    cv::resize(mat, temp, cv::Size(240, 240));
    cv::cvtColor(temp, temp, cv::COLOR_BGRA2RGB);
    std::memcpy(image_src, temp.data, 240 * 240 * 3);
}

// save image_src to binary file
void SaveImageBin(){
    FILE *fp = fopen("emoji.bin", "ab+");
    fwrite(image_src, 1, 240 * 240 * 3, fp);
    fclose(fp);
}

void VideoCap(string filename){
    cv::VideoCapture cap(filename);
    int t = 10;
    while(t--){
        cv::Mat frame;
        cap >> frame;
        SetImageSrc(frame);
        SaveImageBin();
    }
}

//read a .bin file, and show the image
void ReadImageBin(){
    FILE *fp = fopen("emoji.bin", "rb");
    unsigned char *image = new unsigned char[240 * 240 * 3];
    fread(image, 1, 240 * 240 * 3, fp);
    fclose(fp);
    cv::Mat mat(240, 240, CV_8UC3, image);
    cv::imshow("image", mat);
    cv::waitKey(0);
}

int main(){
    VideoCap("../1.mp4");
    VideoCap("../2.mp4");
    VideoCap("../3.mp4");
    ReadImageBin();
    return 0;
}