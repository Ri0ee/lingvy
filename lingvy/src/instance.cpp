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

	//unsigned long long unique_letter_amount = dictionary.UniqueLetterAmount();
	//int sizeof_branch = sizeof(Branch);
	//int sizeof_list_branch = sizeof(ordered_list<Branch>);
	//int total_sizeof = sizeof_branch + sizeof_list_branch;

	//std::cout << "Sizeof Branch: " << sizeof_branch << "\n";
	//std::cout << "Sizeof list<Branch>: " << sizeof_list_branch << "\n";
	//std::cout << "Total sizeof: " << total_sizeof << "\n";
	//std::cout << "Amount of letters added: " << unique_letter_amount << "\n";
	//std::cout << "Size of dictionary (mb): " <<	unique_letter_amount * total_sizeof / 1024 / 1024 << "\n";

	//std::string word;
	//do {
	//	std::cout << "Enter word to search: ";
	//	std::cin >> word;
	//	if (dictionary.WordExists(word)) 
	//		std::cout << "word \"" << word << "\" exists in dictionary\n";
	//} while (word != "_");

	std::string word;
	//dictionary.GetFirstWord(word);
	//std::cout << "First word: " << word << "\n";

	//int i = 0;
	//while (dictionary.GetNextWord(word) && i < 100) {
	//	std::cout << "Next word: " << word << "\n";
	//	i++;
	//}

	Time time_manager;
	time_manager.StartTimeMeasure();

	dictionary.GetFirstWord(word);
	while (dictionary.GetNextWord(word));

	time_manager.EndTimeMeasure();
	std::cout << "Iterating over " << dictionary.WordCount() << " words took " << time_manager.GetTimeMeasure() << " seconds\n";

	if (!SaveData(dictionary))
		std::cout << "Failed to save data\n";

	system("PAUSE");
	return 0;
}

bool Instance::ReadDictionary(const std::string& dict_path_, Dictionary& dictionary_) {
	if (dict_path_.empty()) return false;

	std::cout << "Reading data from " << dict_path_ << "\n";

	Time time_manager;
	time_manager.StartTimeMeasure();

	bool ret = dictionary_.LoadFromFile(dict_path_);

	time_manager.EndTimeMeasure();
	std::cout << "Reading dictionary data took " << time_manager.GetTimeMeasure() << " seconds; " << dictionary_.WordCount() << " words loaded\n";
	
	return ret;
}

bool Instance::ReadData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";
	
	return ReadDictionary(data_path, dictionary_);
}

bool Instance::SaveData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";

	std::cout << "Saving dictionary data...\n";

	Time time_manager;
	time_manager.StartTimeMeasure();

	bool ret = dictionary_.SaveToFile(data_path);

	time_manager.EndTimeMeasure();
	std::cout << "Saving dictionary data took " << time_manager.GetTimeMeasure() << " seconds\n";

	return ret;
}