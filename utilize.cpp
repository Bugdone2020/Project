#include "Utilize.h"

const char* cDepth{ "depth" }; 
const char* cHelp{ "help" };
const char* cUrl{ "url" };
const char* cLogPath{ "logpath" };

static bool check_one_url_from_cmd(std::string& input_url_f) //checks the correctness of the input url
{
	std::cmatch result;
	std::regex ex("(https)""(://)""([\\w\\-?=%.&;]+\\.\\w+)([\\w/\\-?=%.&;]+)");

	if (std::regex_match(input_url_f.c_str(), result, ex))
	{
		return true;
	}

	input_url_f = "error";
	return false;
}

static void parse_param(const po::options_description& desc, 
				const po::variables_map& vm , 
				std::string& input_url, 
				std::map<std::string, bool>& Map_arguments)
{
	if (vm.count(cHelp))
	{
		std::cout << desc << std::endl;
		Map_arguments[cHelp] = true;
	}

	if (vm.count(cDepth))
	{
		std::cout << "recursion depth: " << vm[cDepth].as<int>() << std::endl;
		Map_arguments[cDepth] = true;
	}

	if (vm.count(cUrl))
	{
		if(check_one_url_from_cmd(input_url))
			Map_arguments[cUrl] = true;
		std::cout << "input url: " << vm[cUrl].as<std::string>() << std::endl;
	}

	if (vm.count(cLogPath))
	{
		std::cout << "path to log file: " << vm[cLogPath].as<std::string>() << std::endl;
		Map_arguments[cLogPath] = true;
	}
}

bool input_processing(int argc,	//number of input arguments to main 
	char* argv[],				//input arguments to main
	int& recursion_depth,		//recursion depth
	std::string& input_url,		//input url
	std::string& path_log_file)	//file for logs (not implemented)
{
	
	std::cout << std::endl << "function \"input_processing\" in progress..." << std::endl;

	std::map<std::string, bool> Map_arguments;
	Map_arguments.emplace(cHelp, false);
	Map_arguments.emplace(cDepth, false);
	Map_arguments.emplace(cUrl, false);
	Map_arguments.emplace(cLogPath, false);

	po::options_description desc("Short description");
	desc.add_options()
		("help,h", "help me")
		("depth,d", po::value<int>(&recursion_depth)->default_value(1), "recursion depth")
		("url", po::value<std::string>(&input_url), "input url")
		("logpath,l", po::value<std::string>(&path_log_file), "path to log file");

	po::positional_options_description p;
	po::variables_map vm;
	try
	{
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm);
		parse_param(desc, vm, input_url, Map_arguments);
	}

	catch (po::error& e)
	{
		std::cout << e.what() << std::endl;
		std::cout << desc << std::endl;
		return false;
	}
	
	if (Map_arguments[cHelp])
		return false;
	if (!Map_arguments[cDepth])
	{
		std::cout << std::endl << "ERROR input_processing" << std::endl
		<< "invalid recursion depth" << std::endl;
		return false;
	}
	if (!Map_arguments[cUrl])
	{
		std::cout << std::endl << "ERROR input_processing" << std::endl
		<< "invalid input url" << std::endl;
		return false;
	}

	return true;
}

bool search_all_url_in_content(const std::string& subject, //buffer 
	std::vector<std::string>& url_vector_f) //vector all url
{
	std::cout << std::endl << "function \"search_all_url_in_content\" in progress..." << std::endl;

	try {
		std::regex re("(https|http)""(://)""([\\w\\-?=%.&;]+\\.\\w+)([\\w/\\-?=%.&;]+)");
		std::sregex_iterator next(subject.begin(), subject.end(), re);
		std::sregex_iterator end;
		while (next != end) {
			std::smatch match = *next;
			url_vector_f.push_back(match.str());// put the current url into the vector
			next++;
		}
	}
	catch (std::regex_error& e) {
		// Syntax error in the regular expression
		return false;
	}

	return true;
}

// function called by cURL to write received data to the buffer
static size_t curlWriteFunc(char* data, size_t size, size_t nmemb, std::string* buffer)
{
	size_t result = 0;

	if (buffer != nullptr && data != nullptr)
	{
		//places data from "data" in a memory block of size size * nmemb (apparently)
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}

	return result;//error
}

std::pair<bool, std::string> get_content_from_input_url(const std::string& test_string) //input url
{
	std::cout << std::endl << "function \"get_content_from_input_url\" in progress..." << std::endl;
	std::string curlBuffer; 
	bool Buffer_status = false;
	
	// buffer for saving text errors
	char curlErrorBuffer[CURL_ERROR_SIZE];
	CURL* curl = curl_easy_init(); // create a pointer to an object, easy initialization
	if (curl) {

		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrorBuffer);
		// set URL 
		curl_easy_setopt(curl, CURLOPT_URL, test_string.c_str());

		// function called by cURL to write received data
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlBuffer);

		// fulfill the request
		CURLcode curlResult = curl_easy_perform(curl);

		// session end 
		curl_easy_cleanup(curl);

		if (curlResult == CURLE_OK)
		{
			std::pair<bool, std::string> pair_for_Buffer(Buffer_status, curlBuffer);
			pair_for_Buffer.first = true;
			return pair_for_Buffer;
		}
		else {
			std::cout << "Error(" << curlResult << "): " << curlErrorBuffer << std::endl;
			std::pair<bool, std::string> pair_for_Buffer(Buffer_status, curlBuffer);
			return pair_for_Buffer;
		}

	}
	std::cout << "curl = false" << std::endl;
	std::pair<bool, std::string> pair_for_Buffer(Buffer_status, curlBuffer);
	pair_for_Buffer.first = false;
	return pair_for_Buffer;
}

bool separation_ulr(const std::vector<std::string>& url_vector_f,
	std::vector<std::string>& http_url_vector_f,
	std::vector<std::string>& https_same_domen_url_vector_f,
	std::vector<std::string>& https_other_domen_url_vector_f,
	const std::string& test_string)
{
	std::cmatch result1;
	std::cmatch result2;
	std::regex ex("(https|http)""(://)""([\\w\\-?=%.&;]+\\.\\w+)([\\w/\\-?=%.&;]+)");
	
	std::regex_match(test_string.c_str(), result2, ex);

	for (size_t pos = 0; pos < url_vector_f.size(); pos++)
	{
		if (std::regex_match(url_vector_f.at(pos).c_str(), result1, ex))
		{
			if (result1[1] != result2[1])
			{
				http_url_vector_f.push_back(url_vector_f.at(pos));
			}
			else
			{
				if (result1[3] == result2[3])
				{
					https_same_domen_url_vector_f.push_back(url_vector_f.at(pos));
				}
				else
				{
					https_other_domen_url_vector_f.push_back(url_vector_f.at(pos));
				}
			}
		}
	}
	return true;
}