#include "instance.h"

int Instance::Run() {
	Dictionary dictionary;
	//ReadData(dictionary);

#ifdef _DEBUG
	m_input_file_name = m_program_directory + "\\inputfile.txt";
#endif // _DEBUG

	if (!m_input_file_name.empty())
		if (!ReadDictionary(m_input_file_name, dictionary)) {
			std::cout << "Cannot open file " << m_input_file_name << "\n";
		}

	unsigned long long unique_letter_amount = dictionary.UniqueLetterAmount();
	int total_sizeof = sizeof(Branch) + sizeof(list<Branch>);

	std::cout << "Sizeof Branch: " << sizeof(Branch) << "\n";
	std::cout << "Sizeof list<Branch>: " << sizeof(list<Branch>) << "\n";
	std::cout << "Total sizeof: " << total_sizeof << "\n";
	std::cout << "Amount of letters added: " << unique_letter_amount << "\n";

	std::cout << "Size of dictionary (bytes): " << unique_letter_amount * total_sizeof << "\n";
	std::cout << "Size of dictionary (kb): " << unique_letter_amount * total_sizeof / 1024 << "\n";
	std::cout << "Size of dictionary (mb): " << unique_letter_amount * total_sizeof / 1024 / 1024 << "\n";

	std::string word;
	do {
		std::cout << "Enter word to search: ";
		std::cin >> word;
		if (dictionary.WordExists(word)) 
			std::cout << "word \"" << word << "\" exists in dictionary\n";
	} while (word != "_");

	system("PAUSE");
	//SaveData(dictionary);
	return 0;
}

bool Instance::ReadDictionary(const std::string& dict_path_, Dictionary& dictionary_) {
	if (dict_path_.empty()) return false;

	std::ifstream input_file(dict_path_);
	if (!input_file.is_open()) return false;

	std::cout << "Reading data from " << dict_path_ << "\n";

	Time time_manager;
	time_manager.StartTimeMeasure();

	std::string word("");
	while (input_file >> word) {
		dictionary_.AddWord(word);
	}

	time_manager.EndTimeMeasure();
	std::cout << "Words loaded: " << dictionary_.WordCount() << "; Time taken: " << time_manager.GetTimeMeasure() << "\n";

	input_file.close();
	return true;
}

bool Instance::ReadData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";

	std::ifstream input_file(data_path);
	if (!input_file.is_open()) return false;

	std::string word;
	while (input_file >> word) {	// Read input file word by word
		dictionary_.AddWord(word);
	}
	
	input_file.close();
	return true;
}

bool Instance::SaveData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";

	std::ofstream output_file(data_path);
	if (!output_file.is_open()) return false;

	output_file.close();
	return true;
}