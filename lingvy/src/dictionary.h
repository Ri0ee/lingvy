#pragma once

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include <string>
#include <fstream>
#include <algorithm>
#include "ordered_listy.h"
#include "listy.h"

class Branch {
public:
	Branch() : m_letter(0), m_branches(), m_word_finisher(false) {}

	Branch(Branch& branch_) {
		m_branches = branch_.branches();
		m_letter = branch_.letter();
		m_word_finisher = branch_.word_finisher();
	}

	Branch(char letter_) : m_letter(letter_), m_branches(), m_word_finisher(false) {}

	~Branch() {}

	bool operator==(Branch& branch_) {
		if (m_letter == branch_.letter()) return true;
		return false;
	}

	bool operator>(Branch& branch_) { // Need this for ordered_listy to function well
		return m_letter > branch_.letter();
	}
	
	char& letter() {
		return m_letter;
	}

	ordered_list<Branch>& branches() {
		return m_branches;
	}

	bool& word_finisher() {
		return m_word_finisher;
	}

	ordered_list<Branch>::l_iterator FindNext(char letter_) {
		return m_branches.find_first(letter_);
	}

	ordered_list<Branch>::l_iterator AddLetter(char letter_) {
		auto temp_iterator(m_branches.find_first(letter_));
		if (temp_iterator == m_branches.end()) // Adding new letter to the tree
			return m_branches.add(letter_);
		return temp_iterator;
	}

	unsigned long long size() {
		unsigned long long temp_size = 1;
		for (auto it = m_branches.begin(); it != m_branches.end(); it++)
			temp_size += (*it).size();
		return temp_size;
	}

private:
	ordered_list<Branch> m_branches;
	char m_letter;
	bool m_word_finisher;
};

class Dictionary {
public:
	Dictionary() : m_initial_branches(), m_current_word_stack() {}

	~Dictionary() {}

	void AddWord(const std::string& word_);
	bool WordExists(const std::string& word_);
	bool SaveToFile(const std::string& file_name_);
	bool LoadFromFile(const std::string& file_name_);
	bool GetFirstWord(std::string& word_);
	bool GetNextWord(std::string& word_);
	std::string MakeCorrect(const std::string& word_);
	int LDistance();
	int RLDistance(const int i, const int j);
	int MDistance(const char s1, const char s2);

	unsigned long long UniqueLetterAmount() {
		unsigned long long temp_letter_counter = 0;
		for (auto it = m_initial_branches.begin(); it != m_initial_branches.end(); it++)
			temp_letter_counter += (*it).size();

		return temp_letter_counter;
	}

	int WordCount() {
		return m_word_count;
	}

private:
	char CharToLower(char letter_);
	std::string WordToLower(const std::string& word_);
	void RecursiveSaving(ordered_list<Branch>::l_iterator root_);

	ordered_list<Branch> m_initial_branches; // list of first word letters
	int m_word_count = 0; // Total count of words in dictionary

	std::string m_current_word_stack;
	std::fstream m_output_file;

	list<ordered_list<Branch>::l_iterator> m_iteration_stack;
	std::string m_iteration_word_stack;

	std::string m_s1;
	std::string m_s2;
};