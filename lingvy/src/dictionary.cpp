#include "dictionary.h"

void Dictionary::AddWord(const std::string& word_) {
	if (word_.empty() || WordExists(word_)) return;

	std::string word = WordToLower(word_);
	Branch initial_branch(word[0]);
	auto root(m_initial_branches.find_first(initial_branch)); // Find initial letter of the word in dictionary

	if (root == m_initial_branches.end()) // Branch didn't appear in the list
		root = m_initial_branches.add(initial_branch);

	for (int i = 1; i < word.size(); i++)
		root = (*root).AddLetter(word[i]); // Add word letter by letter to the tree

	(*root).word_finisher() = true;	// Mark last letter as finisher for word

	m_word_count++;
}

bool Dictionary::WordExists(const std::string& word_) {
	if (word_.empty()) return false;

	std::string word = WordToLower(word_);
	Branch initial_branch(word[0]);
	auto root = m_initial_branches.find_first(initial_branch); // Find initial letter of the word in dictionary

	if (root == m_initial_branches.end()) return false;

	for (int i = 1; i < word.size(); i++) {	// Search for word letter by letter further into the tree
		root = (*root).FindNext(word[i]);
		if (root == ordered_list<Branch>::l_iterator(nullptr)) return false;
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
	for (int i = 0; i < temp_string.size(); i++)
		temp_string[i] = CharToLower(temp_string[i]);
	return temp_string;
}

bool Dictionary::SaveToFile(const std::string& file_name_) {
	if (file_name_.empty()) return false;

	m_output_file = std::fstream(file_name_, std::ios::out | std::ios::binary);
	if (!m_output_file.is_open()) return false;

	for (auto it = m_initial_branches.begin(); it != m_initial_branches.end(); it++) {
		m_current_word_stack.clear();
		RecursiveSaving(it);
	}

	m_output_file.close();
	return true;
}

void Dictionary::RecursiveSaving(ordered_list<Branch>::l_iterator root_) {
	m_current_word_stack += (*root_).letter();

	if ((*root_).word_finisher())
		m_output_file << m_current_word_stack << "\n";

	for (auto it = (*root_).branches().begin(); it != (*root_).branches().end(); it++)
		RecursiveSaving(it);

	m_current_word_stack.erase(m_current_word_stack.end() - 1);
}

bool Dictionary::LoadFromFile(const std::string& file_name_) {
	if (file_name_.empty()) return false;

	std::fstream input_file(file_name_, std::ios::in);
	if (!input_file.is_open()) return false;

	std::string word;
	while (input_file >> word)
		AddWord(word);

	input_file.close();
	return true;
}

bool Dictionary::GetFirstWord(std::string& word_) {
	if (m_initial_branches.empty()) return false;

	m_current_word_stack.clear();
	m_iteration_stack.clear();

	m_iteration_stack.push_front(m_initial_branches.begin());
	auto root = *m_iteration_stack.begin();

	while (root != ordered_list<Branch>::l_iterator(nullptr)) {
		m_current_word_stack += (*root).letter();
		m_iteration_stack.push_front(root);

		if ((*root).word_finisher()) {
			word_ = m_current_word_stack;
			m_iteration_stack.pop();
			return true;
		}

		root = (*root).branches().begin();
	}

	return false; 
}

bool Dictionary::GetNextWord(std::string& word_) {
	if (m_initial_branches.empty()) return false; // Return false if there are no words in dictionary

	auto root = *m_iteration_stack.begin(); // Current pos in tree
	auto root2 = (*root).branches().begin(); // Next branch (pos) in tree

	while (true) {
		if (root2 == ordered_list<Branch>::l_iterator(nullptr)) { // This is the end of tree branch
			m_iteration_stack.pop_front(); // Remove last branch from the stack

			if (m_iteration_stack.empty()) { // Got to the initial tree branch
				root = m_initial_branches.find_first(m_current_word_stack[m_current_word_stack.size() - 1]);
				root++;
				if (root == ordered_list<Branch>::l_iterator(nullptr)) // If there are no more initial branches
					return false;

				m_iteration_stack.push_front(root);
				root2 = (*root).branches().begin();
				m_current_word_stack = (*root).letter();
			}
			else {
				root = *m_iteration_stack.begin();

				// Set root2 the value of unvisited from root branch 
				root2 = (*root).branches().find_first(m_current_word_stack[m_current_word_stack.size() - 1]);
				root2++;

				m_current_word_stack.pop_back();
			}
		}
		else {
			m_current_word_stack += (*root2).letter();
			m_iteration_stack.push_front(root2);

			if ((*root2).word_finisher()) {
				word_ = m_current_word_stack;
				return true;
			}

			root2 = (*root2).branches().begin();
		}
	}

	return false;
}

std::pair<std::string, int> Dictionary::MakeCorrect(const std::string& word_) {
	Buffer buffer;
	Producer* prod = new Producer(buffer, this);
	Consumer* cons = new Consumer(buffer, this, word_);

	prod->Run(1);
	cons->Run(3);
	
	delete prod;
	delete cons;

	return buffer.PopResult();
}

int Dictionary::LDistance(const std::string& word_1_, const std::string& word_2_) {
	if (word_1_.size() > MAX_LDISTANCE_WORD_LENGTH || word_2_.size() > MAX_LDISTANCE_WORD_LENGTH) return -1;

	std::string temp_word_1 = " " + word_1_;
	std::string temp_word_2 = " " + word_2_;
	int d[MAX_LDISTANCE_WORD_LENGTH][MAX_LDISTANCE_WORD_LENGTH]; // An array for dynamic storage of temp. distances
	int subst = -1; // Cost for an substitution

	// Fill top-most and left-most rows with values from 1..sizes of words
	for (int i = 0; i < temp_word_1.size(); i++) d[i][0] = i;
	for (int i = 0; i < temp_word_2.size(); i++) d[0][i] = i;

	for (int i = 1; i < temp_word_1.size(); i++) {
		for (int j = 1; j < temp_word_2.size(); j++) {
			subst = (temp_word_1[i] == temp_word_2[j] ? 0 : 1);

			d[i][j] = std::min(
				std::min(
					d[i][j - 1] + 1,
					d[i - 1][j] + 1),
					d[i - 1][j - 1] + subst
			);
		}
	}

	return d[temp_word_1.size() - 1][temp_word_2.size() - 1];
}