#include "ardrone/ardrone.h"

// --------------------------------------------------------------------------
// main(Number of arguments, Argument values)
// Description  : This is the entry point of the program.
// Return value : SUCCESS:0  ERROR:-1
// --------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // AR.Drone class
    ARDrone ardrone;

    // Initialize
    if (!ardrone.open()) {
        std::cout << "Failed to initialize." << std::endl;
        return -1;
    }

    // Initialize detector
    cv::HOGDescriptor hog;
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

    // Main loop
    while (1) {
        // Key input
        int key = cv::waitKey(1);
        if (key == 0x1b) break;

        // Get an image
        cv::Mat image = ardrone.getImage();

        // Detect
        std::vector<cv::Rect> found;
        hog.detectMultiScale(image, found, 0, cv::Size(4, 4), cv::Size(0, 0), 1.5, 2.0);

        // Show bounding rect
        std::vector<cv::Rect>::const_iterator it;
        for (it = found.begin(); it != found.end(); ++it) {
            cv::Rect r = *it;
            cv::rectangle(image, r.tl(), r.br(), cv::Scalar(255, 0, 0), 2);
        }

        // Display the image
        cv::imshow("hog", image); 
    }

    // See you
    ardrone.close();

    return 0;
}