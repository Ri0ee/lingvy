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
#include <vector>
#include <atomic>
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
		Buffer() {
			m_best_result = std::make_pair("", INT_MAX);
		}

		void Push(const std::string& word_) {
			std::unique_lock<std::mutex> locker(mu);
			cond.wait(locker, [this] {
				return !(m_data.size() >= m_max_size);
			});

			m_data.push_back(word_);

			locker.unlock();
			cond.notify_one();
		}

		bool Pop(std::string& word_) {
			std::unique_lock<std::mutex> locker(mu);
			cond.wait(locker, [this] {
				return !m_data.empty() || (m_done && m_data.empty());
			});

			if (m_done && m_data.empty()) {
				locker.unlock();
				cond.notify_one();
				return false;
			}

			word_ = m_data.front();
			m_data.pop_front();

			locker.unlock();
			cond.notify_one();
			return true;
		}

		void PushResult(const std::pair<std::string, int>& result_) {
			std::lock_guard<std::mutex> locker(res_mu);

			// No need to search further if we found word with distance of 1
			if (result_.second <= 1) {
				m_best_result = result_;
				m_should_finish = true;
				cond.notify_all();
				return;
			}

			if (result_.second <= m_best_result.second)
				m_best_result = result_;
		}

		std::pair<std::string, int> PopResult() {
			std::lock_guard<std::mutex> locker(res_mu);

			return m_best_result;
		}

		void SetDone() {
			m_done = true;
			cond.notify_all();
		}

		bool ShouldFinish() {
			return m_should_finish;
		}

	private:
		std::deque<std::string> m_data;
		const int m_max_size = 10;

		std::pair<std::string, int> m_best_result;

		bool m_done = false;
		bool m_should_finish = false;
		std::mutex mu;
		std::mutex res_mu;
		std::condition_variable cond;
	};

	class Producer {
	public:
		Producer(Buffer& buffer_, Dictionary* caller_) : m_buffer(buffer_), m_caller(caller_) {}
		~Producer() {
			for (int i = 0; i < m_threads.size(); i++) {
				m_threads[i]->join();
				delete m_threads[i];
			}
		}

		void Run(int thread_count_) {
			for (int i = 0; i < thread_count_; i++)
				m_threads.push_back(new std::thread(&Producer::Tick, this));
		}

		void Tick() {
			std::string word;
			bool status = m_caller->GetFirstWord(word);
			while (true) {
				if (status == false) {
					m_buffer.SetDone();
					break;
				}

				m_buffer.Push(word);
				status = m_caller->GetNextWord(word);
			}
		}

	private:
		Buffer& m_buffer;
		Dictionary* m_caller = nullptr;

		std::vector<std::thread*> m_threads;
	};

	class Consumer {
	public:
		Consumer(Buffer& buffer_, Dictionary* caller_, const std::string& dest_word_) : 
			m_buffer(buffer_), m_caller(caller_), m_dest_word(dest_word_) {}

		~Consumer() {
			for (int i = 0; i < m_threads.size(); i++) {
				m_threads[i]->join();
				delete m_threads[i];
			}
		}

		void Run(int thread_count_) {
			for (int i = 0; i < thread_count_; i++)
				m_threads.push_back(new std::thread(&Consumer::Tick, this));
		}

		void Tick() {
			std::string word;
			while (true) {
				if(!m_buffer.Pop(word))
					return;

				int result = m_caller->LDistance(m_dest_word, word);
				m_buffer.PushResult(std::make_pair(word, result));
			}
		}

	private:
		Buffer& m_buffer;
		Dictionary* m_caller = nullptr;

		std::string m_dest_word;
		std::vector<std::thread*> m_threads;
	};

	Dictionary() : m_initial_branches(), m_current_word_stack() {}

	~Dictionary() {}

	void AddWord(const std::string& word_);
	bool WordExists(const std::string& word_);
	bool SaveToFile(const std::string& file_name_);
	bool LoadFromFile(const std::string& file_name_);
	bool GetFirstWord(std::string& word_);
	bool GetNextWord(std::string& word_);
	std::pair<std::string, int> MakeCorrect(const std::string& word_);
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