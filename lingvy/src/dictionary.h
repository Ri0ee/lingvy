#pragma once

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include <string>
#include <fstream>
#include <algorithm>
#include <deque>
#include <thread>
#include <mutex>
#include "ordered_listy.h"
#include "listy.h"

#define MAX_LDISTANCE_WORD_LENGTH 100 // Memory consumed by LDistance will be MAX_LDISTANCE_WORD_LENGTH^2 * 4 bytes

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
	
	class Buffer {
	public:
		Buffer() {}

		void Push(const std::string& word_) {
			while (true) {
				std::unique_lock<std::mutex> locker(m);
				c.wait(locker, [this]() {
					return m_data.size() < m_max_size;
				});
				m_data.push_back(word_);
				locker.unlock();
				c.notify_all();
				return;
			}
		}

		std::string Pop(bool& finish_) {
			while (true) {
				std::unique_lock<std::mutex> locker(m);

				if (m_should_finish) {
					finish_ = true;
					locker.unlock();
					return std::string("");
				}

				c.wait(locker, [this]() {
					return m_data.size() > 0;
				});
				std::string back = m_data.back();
				m_data.pop_back();
				locker.unlock();
				c.notify_all();
				return back;
			}
		}

		void PushResult(const std::pair<std::string, int>& result_) {
			while (true) {
				std::unique_lock<std::mutex> locker(m);

				if (result_.second < m_best_distance) {
					m_best_distance = result_.second;
					m_best_word = result_.first;
				}

				locker.unlock();
				return;
			}
		}

		std::pair<std::string, int> PopResult() {
			while (true) {
				std::unique_lock<std::mutex> locker(m);

				std::pair<std::string, int> ret = std::make_pair(m_best_word, m_best_distance);

				locker.unlock();
				return ret;
			}
		}

		void Finish() {
			while (true) {
				std::unique_lock<std::mutex> locker(m);
				m_should_finish = true;
				locker.unlock();
				return;
			}
		}

	private:
		std::deque<std::string> m_data;
		int m_max_size = 10;

		std::string m_best_word;
		int m_best_distance = INT_MAX;

		bool m_should_finish = false;

		std::mutex m;
		std::condition_variable c;
	};

	class Producer {
	public:
		Producer(Buffer* buffer_, Dictionary* caller_) : m_buffer(buffer_), m_caller(caller_) {}

		void Run() {
			std::string word;
			m_caller->GetFirstWord(word);
			while (true) {
				m_buffer->Push(word);
				bool result = m_caller->GetNextWord(word);

				if (result == false) {
					m_buffer->Finish();
					break;
				}
			}
		}

	private:
		Buffer* m_buffer = nullptr;
		Dictionary* m_caller = nullptr;
	};

	class Consumer {
	public:
		Consumer(Buffer* buffer_, Dictionary* caller_, const std::string& dest_word_) : 
			m_buffer(buffer_), m_caller(caller_), m_dest_word(dest_word_) {}

		void Run() {
			while (true) {
				bool should_finish = false;
				std::string word = m_buffer->Pop(should_finish);
				
				if (should_finish == true)
					break;

				int result = m_caller->LDistance(m_dest_word, word);
				m_buffer->PushResult(std::make_pair(word, result));
			}
		}

	private:
		Buffer* m_buffer = nullptr;
		Dictionary* m_caller = nullptr;
		std::string m_dest_word;
	};

	Dictionary() : m_initial_branches(), m_current_word_stack() {}

	~Dictionary() {}

	void AddWord(const std::string& word_);
	bool WordExists(const std::string& word_);
	bool SaveToFile(const std::string& file_name_);
	bool LoadFromFile(const std::string& file_name_);
	bool GetFirstWord(std::string& word_);
	bool GetNextWord(std::string& word_);
	std::string MakeCorrect(const std::string& word_);
	int LDistance(const std::string& word_1_, const std::string& word_2_);

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

	std::string m_current_word_stack; // For saving to file
	std::fstream m_output_file; // For saving data to file

	list<ordered_list<Branch>::l_iterator> m_iteration_stack; // For itarting through tree
	std::string m_iteration_word_stack; // For iterating through tree
};