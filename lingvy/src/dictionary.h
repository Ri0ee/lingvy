#pragma once

#include <string>
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

	bool operator==(Branch& br1_) {
		if (m_letter == br1_.letter()) return true;
		return false;
	}
	
	char& letter() {
		return m_letter;
	}

	list<Branch>& branches() {
		return m_branches;
	}

	bool& word_finisher() {
		return m_word_finisher;
	}

	l_iterator<Branch> FindNext(char letter_) {
		return m_branches.find_first(letter_);
	}

	l_iterator<Branch> AddLetter(char letter_) {
		auto temp_iterator(m_branches.find_first(letter_));
		if (temp_iterator == m_branches.end()) {	// Adding new letter to the tree
			m_branches.push(letter_);
			return m_branches.tail();
		}
		return temp_iterator;
	}

	unsigned long long size() {
		unsigned long long temp_size = 1;
		for (auto it = m_branches.begin(); it != m_branches.end(); it++) {
			temp_size += (*it).size();
		}
		return temp_size;
	}

private:
	list<Branch> m_branches;
	char m_letter;
	bool m_word_finisher;
};

class Dictionary {
public:
	Dictionary() : m_initial_branches() {}

	~Dictionary() {}

	void AddWord(const std::string& word_);
	bool WordExists(const std::string& word_);

	unsigned long long UniqueLetterAmount() {
		unsigned long long temp_letter_counter = 0;
		for (auto it = m_initial_branches.begin(); it != m_initial_branches.end(); it++) {
			temp_letter_counter += (*it).size();
		}
		return temp_letter_counter;
	}

	int WordCount() {
		return m_word_count;
	}

private:
	char CharToLower(char letter_);
	std::string WordToLower(const std::string& word_);

	list<Branch> m_initial_branches;
	int m_word_count = 0;
};