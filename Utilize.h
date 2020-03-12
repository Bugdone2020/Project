#pragma once

#include <iostream>
#include <boost/program_options.hpp>
#include <algorithm>
#include <iterator>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <utility>
#include <regex>
#include <curl/curl.h>

namespace po = boost::program_options;

bool input_processing(int argc,	//number of input arguments to main
	char* argv[],				//input arguments to main
	int& recursion_depth,		//recursion depth
	std::string& input_url,		//input url
	std::string& path_log_file);//file for logs (not implemented)

std::pair<bool, std::string> get_content_from_input_url(const std::string& test_string);

bool search_all_url_in_content(const std::string& subject, //buffer 
	std::vector<std::string>& url_vector_f); //vector all url

bool separation_ulr(const std::vector<std::string>& url_vector_f,
	std::vector<std::string>& http_url_vector_f,
	std::vector<std::string>& https_same_domen_url_vector_f,
	std::vector<std::string>& https_other_domen_url_vector_f,
	const std::string& test_string);