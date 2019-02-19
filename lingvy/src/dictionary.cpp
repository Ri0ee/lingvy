#include "dictionary.h"

void Dictionary::AddWord(const std::string& word_) {
	if (word_.empty()) return;

	Branch initial_branch(word_[0]);
	auto temp_iterator(m_initial_branches.find_first(initial_branch));	// Search iterator

	if (temp_iterator == m_initial_branches.end()) {	// Branch didn't appear in the list
		m_initial_branches.push(initial_branch);
		temp_iterator = m_initial_branches.tail();
	}

	for (int i = 1; i < word_.size(); i++) {
		temp_iterator = (*temp_iterator).AddLetter(word_[i]);
	}
}

bool Dictionary::WordExists(const std::string& word_) {
	if (word_.empty()) return false;

	Branch initial_branch(word_[0]);
	auto temp_iterator = m_initial_branches.find_first(initial_branch);
	if (temp_iterator == m_initial_branches.end()) return false;

	for (int i = 1; i < word_.size(); i++) {
		temp_iterator = (*temp_iterator).FindNext(word_[i]);
		if (temp_iterator == l_iterator<Branch>(nullptr)) return false;
	}

	return true;
}
