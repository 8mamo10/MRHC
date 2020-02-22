#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
    if (argc != 3) {
        printf("usage: DisplayImage.out <from_path> <to_path> \n");
        return -1;
    }
    Mat image;
    //image = imread(argv[1], 1);
    image = imread(argv[1], 0);
    if (!image.data) {
        printf("No image data \n");
        return -1;
    }
    imwrite(argv[2], image);
    return 0;
}
