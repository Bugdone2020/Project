#include "Utilize.h"

int main(int argc, char *argv[]) {
  std::cout << "main program" << std::endl;

  // input processing
  int recursion_depth;       // recursion_depth
  std::string input_url;     // input url
  std::string path_log_file; // file for logs (not implemented)
  if (!input_processing(argc, argv, recursion_depth, input_url,path_log_file)) {
    return -1;
  }

  // Retrieving content from input url
  std::string Buffer; // buffer
  bool Buffer_status = false;
  std::pair<bool, std::string> pair_for_Buffer(Buffer_status, Buffer);
  pair_for_Buffer = get_content_from_input_url(input_url);
  if (!pair_for_Buffer.first) {
    std::cout << std::endl << "ERROR get_content_from_input_url" << std::endl;
    return -2;
  }

  std::vector<std::string> url_vector; // vector of url
  // Fill the vector with url from the content
  if (!search_all_url_in_content(pair_for_Buffer.second, url_vector)) {
    std::cout << std::endl << "ERROR search_all_url_in_content" << std::endl;
    return -3;
  }

  std::vector<std::string> http_url_vector;
  std::vector<std::string> https_same_domen_url_vector;
  std::vector<std::string> https_other_domen_url_vector;

  if (separation_ulr(url_vector, http_url_vector, https_same_domen_url_vector,
                     https_other_domen_url_vector, input_url)) {
    std::cout << std::endl << "----*RESULT*----" << std::endl;
    std::cout << std::endl << "http_url_vector :" << std::endl;
    for (size_t i = 0; i < http_url_vector.size(); i++)
      std::cout << http_url_vector[i] << std::endl;

    std::cout << std::endl << "https_same_domen_url_vector :" << std::endl;
    for (size_t i = 0; i < https_same_domen_url_vector.size(); i++)
      std::cout << https_same_domen_url_vector[i] << std::endl;

    std::cout << std::endl << "https_other_domen_url_vector :" << std::endl;
    for (size_t i = 0; i < https_other_domen_url_vector.size(); i++)
      std::cout << https_other_domen_url_vector[i] << std::endl;
  }

  return 0;
}
