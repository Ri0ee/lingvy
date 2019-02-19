#include "instance.h"

int Instance::Run() {
	Dictionary dictionary;

	ReadData(dictionary);

	if (!m_input_file_name.empty())
		if (!ReadDictionary(m_input_file_name, dictionary)) {
			std::cout << "Cannot open file " << m_input_file_name << "\n";
		}

	if (dictionary.WordExists("apple")) std::cout << "word apple exists in dictionary\n";

	system("PAUSE");
	SaveData(dictionary);
	return 0;
}

bool Instance::ReadDictionary(const std::string& dict_path_, Dictionary& dictionary_) {
	if (dict_path_.empty()) return false;

	std::ifstream input_file(dict_path_);
	if (!input_file.is_open()) return false;

	std::cout << "Reading data from " << dict_path_ << "\n";

	std::string word("");
	int word_counter = 0;
	while (input_file >> word) {
		dictionary_.AddWord(word);

		word_counter++;
		if (word_counter % 1000 == 0) std::cout << "Words read: " << word_counter << "\n";
	}

	input_file.close();
	return true;
}

void Instance::PrintDictionary(Dictionary& dictionary_) {
	//for (auto it = dictionary_.begin(); it != dictionary_.end(); it++) {
	//	std::cout << *it << "\n";
	//}
}

bool Instance::ReadData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";

	std::ifstream input_file(data_path);
	if (!input_file.is_open()) return false;

	std::string word("");
	while (input_file >> word) {
		dictionary_.AddWord(word);
	}
	
	input_file.close();
	return true;
}

bool Instance::SaveData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";

	std::ofstream output_file(data_path);
	if (!output_file.is_open()) return false;

	//for (auto it = dictionary_.begin(); it != dictionary_.end(); it++) {
	//	output_file << *it << " ";
	//}

	output_file.close();
	return true;
}