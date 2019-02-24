#include "dictionary.h"

void Dictionary::AddWord(const std::string& word_) {
	if (word_.empty()) return;
	if (WordExists(word_)) return;

	std::string word = WordToLower(word_);
	Branch initial_branch(word[0]);
	auto root(m_initial_branches.find_first(initial_branch));	// Search iterator

	if (root == m_initial_branches.end()) {	// Branch didn't appear in the list
		m_initial_branches.push(initial_branch);
		root = m_initial_branches.tail();
	}

	for (int i = 1; i < word.size(); i++) {
		root = (*root).AddLetter(word[i]);	// Add word letter by letter to the tree
	}
	(*root).word_finisher() = true;	// Mark last letter as finisher for word

	m_word_count++;
}

bool Dictionary::WordExists(const std::string& word_) {
	if (word_.empty()) return false;

	std::string word = WordToLower(word_);
	Branch initial_branch(word[0]);
	auto root = m_initial_branches.find_first(initial_branch);	// Search iterator

	if (root == m_initial_branches.end()) return false;

	for (int i = 1; i < word.size(); i++) {	// Search for word letter by letter further into the tree
		root = (*root).FindNext(word[i]);
		if (root == l_iterator<Branch>(nullptr)) return false;
	}

	return (*root).word_finisher();
}

char Dictionary::CharToLower(char letter_) {
	if (letter_ <= 'Z' && letter_ >= 'A')
		return letter_ - ('Z' - 'z');
	return letter_;
}

std::string Dictionary::WordToLower(const std::string& word_) {
	std::string temp_string(word_);
	for (int i = 0; i < temp_string.size(); i++) {
		temp_string[i] = CharToLower(temp_string[i]);
	}
	return temp_string;
}
