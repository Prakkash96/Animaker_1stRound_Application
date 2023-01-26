#include <iostream>
#include <windows.h>
#include <thread>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>

using namespace std;
using namespace cv;

int flag = 1;
HANDLE hEvent;

bool stringCompare(string str1, string str2)
{
    int i = 0;
    int len1 = int(str1.size());
    int len2 = int(str2.size());
    if (len1 != len2)
        return false;
    while (i < len1)
    {
        if ((str1[i] != str2[i]) && ((str1[i] >= 'a' && str1[i] <= 'z' && str1[i] - 32 != str2[i]) || (str1[i] >= 'A' && str1[i] <= 'Z' && str1[i] + 32 != str2[i])))
            return false;
        i++;
    }
    return true;
}

bool checkForValidInput(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Please enter any one of the following commands to proceed. \n1.Start \n2.Stop \n";
        return false;
    }
    if (argc > 2)
    {
        cout << "You have entered more than 1 command, Please enter only one from the following commands to proceed. \n1.Start \n2.Stop \n";
        return false;
    }
    if (argc == 2 && !stringCompare(argv[1], "Start") && !stringCompare(argv[1], "Stop"))
    {
        cout << "Please enter a valid command. \n1.Start (or) 2.Stop \n";
        return false;
    }

    return true;
}

void captureVideo()
{
    cv::VideoCapture video(0);
    if (!video.isOpened())
    {
        cout << "Please check your camera.\n";
        return;
    }
    cv::Mat frame;
	int frameWidth = int(video.get(cv::CAP_PROP_FRAME_WIDTH));
	int frameHeigth = int(video.get(cv::CAP_PROP_FRAME_HEIGHT));
    cv::VideoWriter output("output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, cv::Size(frameWidth, frameHeigth));
    while (flag)
    {
        bool success = video.read(frame);
        if (!success)
        {
            cout << "Error while processing your video\n";
            return;
        }
        cv::imshow("Video feed", frame);
        output.write(frame);
        cv::waitKey(1);
        if (WaitForSingleObject(hEvent, 0) != WAIT_TIMEOUT)
            flag = 0;
    }
    output.release();
	video.release();
	cv::destroyAllWindows();
}

int main(int argc, char **argv)
{
    if (!checkForValidInput(argc, argv))
        return 0;
    hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("MyEvent"));
    int event_status = GetLastError();
    if (event_status != 0 && event_status != ERROR_ALREADY_EXISTS)
    {
        cout << "Event initialization failed, Error code is .... " << event_status << "\n";
        return 0;
    }
    if (event_status == ERROR_ALREADY_EXISTS)
    {
        if (!stringCompare(argv[1], "Stop"))
        {
            cout << "Previous instance of this application is still running in the background, Please Stop the previous instance before Starting a new one.\n";
            return 0;
        }
        SetEvent(hEvent);
    }
    else
    {
        if (!stringCompare(argv[1], "start"))
            cout << "Closing the application... \n";
        else
        {
        thread th1(captureVideo);
        th1.join();
        }
    }
    CloseHandle(hEvent);
    return 0;
}
