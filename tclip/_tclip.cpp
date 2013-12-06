#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <iostream>
#include <map>
#include <math.h>
#include <time.h>


int detectFace(cv::Mat image, std::string face_cascade_name){
    cv::Mat image_gray;
    cv::CascadeClassifier face_cascade;
    std::vector<cv::Rect> faces;
    int face_size;
    int Y;

    if (!face_cascade.load(face_cascade_name))
    {
        printf("[error] can not load classifier file\n");
        return -1;
    }

    cv::cvtColor(image, image_gray, CV_BGR2GRAY);
    cv::equalizeHist(image_gray, image_gray);

    face_cascade.detectMultiScale(image_gray, faces, 1.1, 2,
                                  0 | CV_HAAR_SCALE_IMAGE,
                                  cv::Size(30, 30));
    face_size = faces.size();

    if (face_size > 0)
    {
        Y = faces[face_size -1].y - faces[face_size -1].height / 2;
        if (Y > image.size().height / 2)
            return -1;
        else
            return Y < 0 ? 0 : Y;
    }
    else
    {
        return -1;
    }
}

int detectCharacter(cv::Mat image)
{
    std::map<int, int> section_num;
    std::vector<cv::KeyPoint> keypoints;
    cv::Ptr<cv::FeatureDetector> detector;

    int start_x = 0;
    int end_x = 0;
    int section_index = 0;
    int total = 0;
    int avg = 0;
    int con_num = 4;
    int flag = 0;
    int counter = 0;
    int Y = 0;

    int slice_total = 10;
    int slice_num = section_num.size() / slice_total;
    int slice_counter = 0;

    cv::initModule_nonfree();
    detector = cv::FeatureDetector::create("SURF");

    if (detector.empty())
    {
        std::cout << "Can not create detector or descriptor exstractor "
                     "or descriptor matcher of given types" << std::endl;
        return -1;
    }

    start_x = 0;
    end_x = image.size().width;

    detector->detect(image, keypoints);

    for (std::vector<cv::KeyPoint>::iterator i = keypoints.begin();
         i != keypoints.end();
         i++)
    {
        if (i->pt.x > start_x && i->pt.x < end_x)
        {
            section_index = (int)ceil(i->pt.y / 10);
            section_num[section_index] = section_num[section_index] + 1;
            total = total + 1;
        }
    }
    avg = total / section_num.size();

    for (int m = 0; m < slice_total; m++)
    {
        for (int n = m * slice_num; n < (m+1) * slice_num; n++)
        {
            if ( section_num[n] >= avg )
            {
                slice_counter++;
                break;
            }
        }
    }
    if (slice_counter >= slice_total)
    {
        return -1;
    }

    for (std::map<int, int>::iterator i = section_num.begin();
         i != section_num.end();
         i++)
    {
        if (i->second >= avg && flag == 0)
            counter++;
        else
            counter = 0;

        if (counter >= con_num && flag == 0)
        {
            Y = i->first;
            flag = 1;
        }
    }
    if (Y > con_num && Y < image.size().height / 4)
        return (Y - con_num - 11) * slice_total < 0 ?
                0 : (Y - con_num - 11) * slice_total;
    else if (Y > con_num)
        return (Y - con_num) * slice_total;
    else
        return Y * 10;
}

int tclip(std::string source_path, std::string dest_path,
          int dest_width, int dest_height,
          std::string config_path)
{
    cv::Mat image;
    cv::Mat dest_image;
    cv::Size tmp_size;

    float ratio_width = 0;
    float ratio_height = 0;
    float ratio = 0;

    int clip_top = 0;
    int clip_bottom = 0;
    int clip_left = 0;
    int clip_right = 0;

    int result = 0;
    int param;

    if (source_path == "")
    {
        std::cerr << "you should specify the path of source." << std::endl;
        return 1;
    }
    if (dest_path == "")
    {
        std::cerr << "you should specify the path of destination." << std::endl;
        return 1;
    }

    image = cv::imread(source_path);
    if(!image.data)
    {
        printf("[error] do not load pic \n");
        return 1;
    }

    if (image.size().width * 3 <= image.size().height)
    {
        ratio = (float)dest_width / image.size().width;
        tmp_size = cv::Size((int)(image.size().width * ratio),
                            (int)(image.size().height * ratio));
        dest_image = cv::Mat(tmp_size, CV_32S);
        resize(image, dest_image, tmp_size);
        clip_top = 0;
        clip_bottom = dest_height - dest_image.size().height;
        clip_left = 0;
        clip_right = 0;
        dest_image.adjustROI(clip_top, clip_bottom, clip_left, clip_right);
        imwrite(dest_path, dest_image);
        return -1;
    }

    ratio = 300.0 / image.size().width;
    tmp_size = cv::Size((int)(image.size().width * ratio),
                        (int)(image.size().height * ratio));
    dest_image = cv::Mat(tmp_size, CV_32S);
    cv::resize(image, dest_image, tmp_size);

    result = detectFace(dest_image, config_path);

    if (result == -1)
        result = detectCharacter(dest_image);

    result = result == -1 ? -1 : (int)((float)result / ratio);

    ratio_width = (float)dest_width / image.size().width;
    ratio_height = (float)dest_height / image.size().height;

    if (ratio_width > ratio_height)
        ratio = ratio_width;
    else
        ratio = ratio_height;

    result = result == -1 ? -1 : (int)((float)result * ratio);

    tmp_size = cv::Size((int)(image.size().width * ratio),
                        (int)(image.size().height * ratio));
    dest_image = cv::Mat(tmp_size, CV_32S);
    cv::resize(image, dest_image, tmp_size);

    if (ratio_width > ratio_height)
    {
        if (result == -1)
        {
            clip_top = -((dest_image.size().height - dest_height) / 2);
            clip_bottom = clip_top;
        }
        else
        {
            if (dest_image.size().height - result >= dest_height)
            {
                clip_top = -result;
                clip_bottom = -(dest_image.size().height - result - dest_height);
            }
            else
            {
                clip_top = -(dest_image.size().height - dest_height);
            }
        }
    }
    else
    {
        clip_left = -((dest_image.size().width - dest_width) / 2);
        clip_right = clip_left;
    }

    dest_image.adjustROI(clip_top, clip_bottom, clip_left, clip_right);
    cv::imwrite(dest_path, dest_image);

    return 0;
}
