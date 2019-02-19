#include "dictionary.h"

void Dictionary::AddWord(const std::string& word_) {
	if (word_.empty()) return;
	if (WordExists(word_)) return;

	std::string word = WordToLower(word_);
	Branch initial_branch(word[0]);
	auto temp_iterator(m_initial_branches.find_first(initial_branch));	// Search iterator

	if (temp_iterator == m_initial_branches.end()) {	// Branch didn't appear in the list
		m_initial_branches.push(initial_branch);
		temp_iterator = m_initial_branches.tail();
	}

	for (int i = 1; i < word.size(); i++) {
		temp_iterator = (*temp_iterator).AddLetter(word[i]);
	}
	(*temp_iterator).word_finisher() = true;
}

bool Dictionary::WordExists(const std::string& word_) {
	if (word_.empty()) return false;
	std::string word = WordToLower(word_);

	Branch initial_branch(word[0]);
	auto temp_iterator = m_initial_branches.find_first(initial_branch);
	if (temp_iterator == m_initial_branches.end()) return false;

	for (int i = 1; i < word.size(); i++) {
		temp_iterator = (*temp_iterator).FindNext(word[i]);
		if (temp_iterator == l_iterator<Branch>(nullptr)) return false;
	}

	return (*temp_iterator).word_finisher();
}

char Dictionary::ToLower(char letter_) {
	if (letter_ <= 'Z' && letter_ >= 'A')
		return letter_ - ('Z' - 'z');
	return letter_;
}

std::string Dictionary::WordToLower(const std::string& word_) {
	std::string temp_string(word_);
	for (int i = 0; i < temp_string.size(); i++) {
		temp_string[i] = ToLower(temp_string[i]);
	}
	return temp_string;
}
