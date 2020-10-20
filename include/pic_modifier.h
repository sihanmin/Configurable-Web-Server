#ifndef PicModifier_h
#define PicModifier_h
#define _CRT_SECURE_NO_DEPRECATE
#define _SCL_SECURE_NO_WARNINGS
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class pic_modifier {
public:
  pic_modifier();
  enum subject_to_modify
  {
    runner,
    obstacles
  };

  int modify_pic(string path_to_addon, subject_to_modify element, string out_name="output.png", string out_path="../t_rex/game/assets/default_100_percent/") {};

  /// incorporates addon image to the base image
  /// normally returns 0
  /// if addon is neither jpeg nor png, returns 1
  /// if addon image is empty, returns 2
  int modify_pic(string path_to_1st_addon, string path_to_2nd_addon, string out_name="output.png", string out_path="../t_rex/game/assets/default_100_percent/");

private:
  int check_file_size(string path_to_file);
  unsigned int runner_x [7] = {42, 850, 894, 938, 982, 1026, 1070};
  unsigned int runner_y [7] = {1, 0, 0, 0, 0, 0, 0};
  unsigned int squat_x [2] = {1123, 1182};
  unsigned int squat_y [2] = {20, 20};
  unsigned int runner_dim [2] = {40,40};
  unsigned int squat_dim [2] = {48,20};
};

#endif