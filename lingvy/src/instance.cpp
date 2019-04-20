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

	//double time_sum = 0;
	//Time time_manager;
	//for (int it = 0; it < 100; it++) {
	//	time_manager.StartTimeMeasure();
	//	auto correct = dictionary.MakeCorrect("thinkyoucanhbbng");
	//	time_manager.EndTimeMeasure();

	//	time_sum += time_manager.GetTimeMeasure();

	//	std::cout <<
	//		"corrected: " << correct.first <<
	//		"; distance: " << correct.second <<
	//		"; time: " << time_manager.GetTimeMeasure() << "\n";
	//}
	//std::cout << "average time: " << time_sum / 100 << "s\n";

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
	if (dict_path_.empty()) return false;

	std::cout << "Reading data from " << dict_path_ << "\n";

	//Time time_manager;
	//time_manager.StartTimeMeasure();

	bool ret = dictionary_.LoadFromFile(dict_path_);

	//time_manager.EndTimeMeasure();
	//std::cout << "Reading dictionary data took " << time_manager.GetTimeMeasure() << " seconds; " << dictionary_.WordCount() << " words loaded\n";
	
	return ret;
}

bool Instance::ReadData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";
	
	return ReadDictionary(data_path, dictionary_);
}

bool Instance::SaveData(Dictionary& dictionary_) {
	std::string data_path = m_program_directory + "\\dict.dat";

	std::cout << "Saving dictionary data...\n";

	//Time time_manager;
	//time_manager.StartTimeMeasure();

	bool ret = dictionary_.SaveToFile(data_path);

	//time_manager.EndTimeMeasure();
	//std::cout << "Saving dictionary data took " << time_manager.GetTimeMeasure() << " seconds\n";

	return ret;
}