#include "ardrone/ardrone.h"

std::vector<cv::Rect> detect(cv::Mat &image) {
    cv::HOGDescriptor hog;
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

    std::vector<cv::Rect> found, found_filtered;
    hog.detectMultiScale(image, found, 0, cv::Size(8,8), cv::Size(32,32), 1.05, 2);

    for(size_t i = 0; i < found.size(); i++) {
        cv::Rect r = found[i];
        int j;
        for (j = 0; j < found.size(); j++)
            if (j != i && (r & found[j]) == r)
                break;
        if (j == found.size())
            found_filtered.push_back(r);
    }

    return found_filtered;
}

void drawRectangles(std::vector<cv::Rect> &humans, cv::Mat &image) {
    for (cv::Rect r : humans) {
        r.x += cvRound(r.width*0.1);
        r.width = cvRound(r.width*0.8);
        r.y += cvRound(r.height*0.07);
        r.height = cvRound(r.height*0.8);
        rectangle(image, r.tl(), r.br(), cv::Scalar(0,255,0), 3);
    }
}

class Movement {
    public:
        double x, y, z, r;
        Movement(double x, double y, double z, double r) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->r = r;
        }
};

const Movement STOP(0, 0, 0, 0);
const Movement FORWARD(1, 0, 0, 0);
const Movement BACKWARD(-1, 0, 0, 0);
const Movement LEFT(0, 1, 0, 0);
const Movement RIGHT(0, -1, 0, 0);
const Movement UP(0, 0, 1, 0);
const Movement DOWN(0, 0, -1, 0);

const Movement ROTATE(0, 0, 0, 0.33);

// --------------------------------------------------------------------------
// main(Number of arguments, Argument values)
// Description  : This is the entry point of the program.
// Return value : SUCCESS:0  ERROR:-1
// --------------------------------------------------------------------------
int main(int argc, char *argv[])
{

    std::vector<Movement> movements;
    movements.push_back(STOP);
    movements.push_back(STOP);

    movements.push_back(UP);

    movements.push_back(STOP);
    movements.push_back(STOP);

    for (int i = 0; i < 4; i++) {
        movements.push_back(FORWARD);
        movements.push_back(STOP);
        movements.push_back(ROTATE);
        movements.push_back(STOP);
    }


    movements.push_back(STOP);

    // AR.Drone class
    ARDrone ardrone;

    // Initialize
    if (!ardrone.open()) {
        std::cout << "Failed to initialize." << std::endl;
        return -1;
    }

    // Battery
    std::cout << "Battery = " << ardrone.getBatteryPercentage() << "[%]" << std::endl;

    // Instructions
    std::cout << "***************************************" << std::endl;
    std::cout << "*       CV Drone sample program       *" << std::endl;
    std::cout << "*           - How to play -           *" << std::endl;
    std::cout << "***************************************" << std::endl;
    std::cout << "*                                     *" << std::endl;
    std::cout << "* - Controls -                        *" << std::endl;
    std::cout << "*    'Space' -- Takeoff/Landing       *" << std::endl;
    std::cout << "*    'Up'    -- Move forward          *" << std::endl;
    std::cout << "*    'Down'  -- Move backward         *" << std::endl;
    std::cout << "*    'Left'  -- Turn left             *" << std::endl;
    std::cout << "*    'Right' -- Turn right            *" << std::endl;
    std::cout << "*    'Q'     -- Move upward           *" << std::endl;
    std::cout << "*    'A'     -- Move downward         *" << std::endl;
    std::cout << "*                                     *" << std::endl;
    std::cout << "* - Others -                          *" << std::endl;
    std::cout << "*    'C'     -- Change camera         *" << std::endl;
    std::cout << "*    'Esc'   -- Exit                  *" << std::endl;
    std::cout << "*                                     *" << std::endl;
    std::cout << "***************************************" << std::endl;


    bool intruders = false;

    int accumulator = 0;
    for (int counter = 0; ; counter++) {

        // Key input
        int key = cv::waitKey(33);
        if (key == 0x1b) break;

        cv::Mat image = ardrone.getImage();
        std::vector<cv::Rect> humans = detect(image);
        drawRectangles(humans, image);

        accumulator += humans.size();
        if (counter % 30 == 0) {
            if (counter / 30 >= movements.size())
                break;
            auto currentMovement = movements[counter / 30];
            ardrone.move3D(currentMovement.x, currentMovement.y, currentMovement.z, currentMovement.r);
//            ardrone.move3D(1, 0, 0, 0);
    //        sleep(1);

            if (accumulator / 15)
                intruders = true;
            accumulator = 0;
        }

        std::cout << counter << " " << accumulator <<  std::endl;



        // Take off / Landing 
        if (key == ' ') {
            if (ardrone.onGround()) ardrone.takeoff();
            else                    ardrone.landing();
            sleep(3);
        }

        // Move
        double vx = 0.0, vy = 0.0, vz = 0.0, vr = 0.0;
        if (key == 'i' || key == CV_VK_UP)    vx =  1.0;
        if (key == 'k' || key == CV_VK_DOWN)  vx = -1.0;
        if (key == 'u' || key == CV_VK_LEFT)  vr =  1.0;
        if (key == 'o' || key == CV_VK_RIGHT) vr = -1.0;
        if (key == 'j') vy =  1.0;
        if (key == 'l') vy = -1.0;
        if (key == 'q') vz =  1.0;
        if (key == 'a') vz = -1.0;

//        ardrone.move3D(vx, vy, vz, vr);

        // Change camera
        static int mode = 0;
        if (key == 'c') ardrone.setCamera(++mode % 4);

        // Display the image
        cv::imshow("camera", image);

    }

    // See you
    ardrone.close();

    return 0;
}
