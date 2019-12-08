#include <algorithm>
#include <cassert>
#include <string>
#include <tuple>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

auto orb = cv::ORB::create();
auto matcher = cv::BFMatcher(cv::NORM_HAMMING);

std::tuple<cv::Mat, std::vector<cv::KeyPoint>, cv::Mat> extract_features(const cv::Mat&);
std::tuple<cv::Mat, std::vector<cv::KeyPoint>, std::vector<cv::KeyPoint>>
match_frames(
    const cv::Mat&, const cv::Mat&,
    const std::vector<cv::KeyPoint>, const std::vector<cv::KeyPoint>,
    const cv::Mat&, const cv::Mat&);

int main() {
    auto cap = cv::VideoCapture(0);
    cv::Mat current_frame, previous_frame;

    assert(cap.read(previous_frame));
    auto [previous_frame_grayscale, previous_keypoints, previous_descriptors] = extract_features(previous_frame);

    int k = -1;
    while (k == -1) {
        bool ret = cap.read(current_frame);
        if (ret) {
            auto [current_frame_grayscale, current_keypoints, current_descriptors] = extract_features(current_frame);

            auto [F, good_privious_keypoints, good_current_keypoints] = match_frames(
                previous_frame_grayscale, current_frame_grayscale,
                previous_keypoints, current_keypoints,
                previous_descriptors, current_descriptors);

            cv::drawKeypoints(current_frame, current_keypoints, current_frame);
            cv::imshow("", current_frame);
            k = cv::waitKey(1);
            current_frame_grayscale.copyTo(previous_frame_grayscale);
            previous_keypoints = current_keypoints;
            current_descriptors.copyTo(previous_descriptors);
        }
    }
    return 0;
}

std::tuple<cv::Mat, std::vector<cv::KeyPoint>, std::vector<cv::KeyPoint>>
match_frames(
        const cv::Mat& previous_frame, const cv::Mat& current_frame,
        const std::vector<cv::KeyPoint> previous_keypoints, const std::vector<cv::KeyPoint> current_keypoints,
        const cv::Mat& previous_descriptors, const cv::Mat& current_descriptors) {
    cv::Mat F;
    return std::make_tuple(F, previous_keypoints, current_keypoints);
}

std::tuple<cv::Mat, std::vector<cv::KeyPoint>, cv::Mat>
extract_features(const cv::Mat& frame) {
    cv::Mat gray;
    std::vector<cv::Point2f> pts;
    cv::Mat descriptors;
    std::vector<cv::KeyPoint> keypoints;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::goodFeaturesToTrack(gray, pts, 150, 0.01, 30);
    std::transform(pts.begin(), pts.end(), std::back_inserter(keypoints), [](auto pt){cv::KeyPoint kp; kp.pt=pt; return kp;});
    orb->compute(gray, keypoints, descriptors);
    return std::make_tuple(gray, keypoints, descriptors);
}
