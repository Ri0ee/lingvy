#include "instance.h"

int Instance::Run() {
	Dictionary dictionary;
	ReadData(dictionary);

#ifdef _DEBUG
	m_input_file_name = m_program_directory + "\\inputfile.txt";
#endif // _DEBUG

	if (!m_input_file_name.empty())
		if (!ReadDictionary(m_input_file_name, dictionary))
			std::cout << "Cannot open file " << m_input_file_name << "\n";

	std::string word;
	do {
		std::cin >> word;

		if (word == "_") break;
		if (dictionary.WordExists(word))
			std::cout << "word \"" << word << "\" is correct\n";
		else
			std::cout << "word \"" << word << "\" is incorrect, correct one should be: \"" << dictionary.MakeCorrect(word).first << "\"\n";
	} while (true);

	if (!SaveData(dictionary))
		std::cout << "Failed to save data\n";

	system("PAUSE");
	return 0;
}

bool Instance::ReadDictionary(const std::string& dict_path_, Dictionary& dictionary_) {
	std::cout << "Reading data from " << dict_path_ << "\n";
	return dictionary_.LoadFromFile(dict_path_);;
}

bool Instance::ReadData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";
	return ReadDictionary(data_path, dictionary_);
}

bool Instance::SaveData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";
	return dictionary_.SaveToFile(data_path);
}