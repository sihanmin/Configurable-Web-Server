// inspired by https://android.developreference.com/article/14503748/Positioning+an+image+within+an+image+using+GIL+from+Boost+in+C%2B%2B

#include "pic_modifier.h"
using namespace std;
using namespace boost::gil;

pic_modifier::pic_modifier()
{}

int pic_modifier::modify_pic(string path_to_1st_addon, string path_to_2nd_addon, string out_name, string out_path)
{
  if (out_path.back() != '/')
    out_path += '/';
  
  rgba8_image_t addon1;
  rgba8_image_t addon2;
  rgba8_image_t base;
  
  string base_img = "../t_rex/game/assets/default_100_percent/base.png";
  png_read_image(base_img, base);
  
  if (check_file_size(path_to_1st_addon) == 0)
    return 2;
  if (check_file_size(path_to_2nd_addon) == 0)
    return 2;

  try 
  {
    png_read_and_convert_image(path_to_1st_addon, addon1);
  }
  catch (ios_base::failure)
  {
    try 
    {
      jpeg_read_and_convert_image(path_to_1st_addon, addon1);
    }
    catch (ios_base::failure)
    {
      return 1;
    }
  }

  try 
  {
    png_read_and_convert_image(path_to_2nd_addon, addon2);
  }
  catch (ios_base::failure)
  {
    try 
    {
      jpeg_read_and_convert_image(path_to_2nd_addon, addon2);
    }
    catch (ios_base::failure)
    {
      return 1;
    }
  }

  
  for (size_t i = 0; i < 7; i++)
    copy_pixels(view(addon1), subimage_view(view(base), runner_x[i], runner_y[i], runner_dim[0], runner_dim[1]));
  for (size_t i = 0; i < 2; i++)
    copy_pixels(view(addon2), subimage_view(view(base), squat_x[i], squat_y[i], squat_dim[0], squat_dim[1]));

  png_write_view(out_path+out_name, const_view(base));
  return 0;
}

int pic_modifier::check_file_size(string path_to_file)
{
  streampos begin,end;
  ifstream myfile (path_to_file, ios::binary);
  begin = myfile.tellg();
  myfile.seekg (0, ios::end);
  end = myfile.tellg();
  myfile.close();
  return end-begin;
}