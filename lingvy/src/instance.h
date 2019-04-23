#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include "ordered_listy.h"
#include "time_manager.h"

#include "dictionary.h"

class Instance {
public:
	Instance(const std::string& exe_file_location_) {
		m_exe_file_location = exe_file_location_;
		m_program_directory = m_exe_file_location.substr(0, m_exe_file_location.find_last_of("\\"));
	}

	void SetInputFile(const std::string& input_file_name_) { // path to the dictionary text
		m_input_file_name = input_file_name_;
	}

	int Run();

private:
	std::string m_exe_file_location;
	std::string m_program_directory;
	std::string m_input_file_name;

	bool ReadData(Dictionary& dictionary_);
	bool SaveData(Dictionary& dictionary_);
	bool ReadDictionary(const std::string& dict_path_, Dictionary& dictionary_);
};