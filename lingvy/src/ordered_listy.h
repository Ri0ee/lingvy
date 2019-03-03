#pragma once

template<class TE>
class l_element {
public:
	l_element() {};

	l_element(TE data_, l_element<TE>* next_element_ptr_ = nullptr) : m_data(data_), m_next_element_ptr(next_element_ptr_) {}

	l_element(l_element<TE>* next_element_ptr_) : m_next_element_ptr(next_element_ptr_) {}

	~l_element() {};

	l_element<TE>*& next() {
		return m_next_element_ptr;
	}

	TE& get_data() {
		return m_data;
	}

private:
	TE m_data{};
	l_element<TE>* m_next_element_ptr = nullptr;
};

template<class TI>
class l_iterator {
public:
	l_iterator() {}

	l_iterator(const l_iterator<TI>& iterator_) {
		m_element_ptr = iterator_.element();
	}

	l_iterator(l_element<TI>* element_ptr_) : m_element_ptr(element_ptr_) {}

	~l_iterator() {}

	l_iterator<TI>& operator++() { // Prefix increment
		m_element_ptr = m_element_ptr->next();
		return *this;
	}

	l_iterator<TI> operator++(int) { // Postfix increment
		l_iterator<TI> result(*this);
		++(*this);
		return result;
	}

	TI& operator*() {
		return m_element_ptr->get_data();
	}

	bool operator==(const l_iterator<TI>& iterator_) {
		return iterator_.element() == m_element_ptr;
	}

	bool operator!=(const l_iterator<TI>& iterator_) {
		return iterator_.element() != m_element_ptr;
	}

	l_element<TI>* element() const {
		return m_element_ptr;
	}

	void swap_data(l_iterator<TI>& iterator_) {
		TI temp_data = iterator_.element()->get_data();
		iterator_.element()->get_data() = m_element_ptr->get_data();
		m_element_ptr->get_data() = temp_data;
	}

	void swap(l_iterator<TI>& iterator_) {
		l_element<TI>* temp_element_ptr = iterator_.element();
		iterator_.element() = m_element_ptr;
		m_element_ptr = temp_element_ptr;
	}

private:
	l_element<TI>* m_element_ptr = nullptr;
};

template<class TL>
class ordered_list {
public:
	ordered_list() {}

	ordered_list(ordered_list& list_) {
		for (auto it = list_.begin(); it != list_.end(); it++) 
			push(*it);
	}

	ordered_list(l_iterator<TL> begin_iterator_, l_iterator<TL> end_iterator_ = l_iterator<TL>(nullptr)) {
		for (auto it = begin_iterator_; it != end_iterator_; it++)
			push(*it);
	}

	~ordered_list() {
		clear(); // Free the memory used by all list elements
	}

	// Adds data in sorted way. Needs TL type to have ">" operator implemented
	l_iterator<TL> add(TL data_) {
		if (m_head == nullptr) // If the list is currently empty
			return push_front(data_);

		unsigned int current_pos = 0;
		for (auto it = begin(); it != end(); it++) {
			if (*it > data_) // This requires TL type to have comparsion operator 
				return insert(current_pos, data_);
				
			current_pos++;
		}

		return push(data_); // If we didn't find elements that have bigger value than ours does
	}

	// Remove last element. Returns true if removed anything
	bool pop() {
		if (m_head == nullptr) return false;

		l_iterator<TL> ptail = pre_tail();
		if (ptail.element() == nullptr) {
			pop_front();
		}
		else {
			delete ptail.element()->next();
			ptail.element()->next() = nullptr;
		}

		return true;
	}

	// Remove first element. Returns true if removed anything
	bool pop_front() {
		if (m_head == nullptr) return false;

		l_element<TL>* temp_element = m_head->next();
		delete m_head;
		m_head = temp_element;

		return true;
	}

	// Removes one element from the list. Returns true if removed anything
	bool remove(int pos_) {
		if (m_head == nullptr || pos_ < 0) return false;
		if (pos_ == 0) return pop_front();
		if (pos_ >= size() - 1) return pop();

		l_iterator<TL> prev = seek_before(pos_);
		l_iterator<TL> after(prev.element()->next()->next());
		delete prev.element()->next();
		prev.element()->next() = after.element();

		return true;
	}

	// Removes all elements from the list
	void clear() {
		while (pop_front());
	}

	// Searches for the first appearance of data in list
	l_iterator<TL> find_first(TL data_) {
		for (auto it = begin(); it != end(); it++) {
			if (*it == data_) return l_iterator<TL>(it);
		}

		return end();
	}

	// Searches for the last appearance of data in list
	l_iterator<TL> find_last(TL data_) {
		l_iterator<TL> temp_iterator = end();
		for (auto it = begin(); it != end(); it++) {
			if (*it == data_) temp_iterator = it;
		}

		return temp_iterator;
	}

	// Searches for the element with minimal value
	l_iterator<TL> find_min(l_iterator<TL> begin_ = begin(), l_iterator<TL> end_ = end()) {
		l_iterator<TL> current_min(begin_);
		for (auto it = begin_; it != end_; it++) {
			if (*current_min > *it) {
				current_min = it;
			}
		}
		return current_min;
	}

	// Searches for the element with highest value
	l_iterator<TL> find_max(l_iterator<TL> begin_ = begin(), l_iterator<TL> end_ = end()) {
		l_iterator<TL> current_max(begin_);
		for (auto it = begin_; it != end_; it++) {
			if (*current_max < *it) {
				current_max = it;
			}
		}
		return current_max;
	}

	// Returns amount of elements in list
	unsigned int size() {
		unsigned int temp_size = 0;
		for (auto it = begin(); it != end(); it++) {
			temp_size++;
		}
		return temp_size;
	}

	// Returns iterator of the first element of the list
	l_iterator<TL> begin() {
		return l_iterator<TL>(m_head);
	}

	// Returns iterator signalling about the end for the list
	l_iterator<TL> end() {
		return l_iterator<TL>(nullptr);
	}

	// Returns iterator to the last element of the list
	l_iterator<TL> tail() {
		for (auto it = begin(); it != end(); it++) {
			if (it.element()->next() == nullptr) {
				return it;
			}
		}
		return l_iterator<TL>(nullptr);
	}

	// Returns iterator to the element that stays before last element
	l_iterator<TL> pre_tail() {
		for (auto it = begin(); it != end(); it++) {
			if (it.element()->next() != nullptr) {
				if (it.element()->next()->next() == nullptr) {
					return it;
				}
			}
			else break;
		}
		return l_iterator<TL>(nullptr);
	}

	// Returns distance between two iterators (amount of elements between them)
	int distance(l_iterator<TL> begin_iterator_, l_iterator<TL> dest_iterator_) {
		int counter = 0;
		for (auto it = begin_iterator_; it != end(); it++) {
			if (it == dest_iterator_) return counter;
			counter++;
		}

		return -1;
	}

private:
	l_element<TL>* m_head = nullptr; // Pointer to the first element of the list

	l_iterator<TL> seek(unsigned int position_) {
		unsigned int current_position = 0;
		for (auto it = begin(); it != end() && current_position <= position_; it++) {
			if (current_position == position_) {
				return it;
			}
			current_position++;
		}
		return l_iterator<TL>(nullptr);
	}

	l_iterator<TL> seek_before(unsigned int position_) {
		return seek(position_ - 1);
	}

	l_iterator<TL> seek_after(unsigned int position_) {
		return seek(position_ + 1);
	}

	// Push data to the end of the list
	l_iterator<TL> push(TL data_) {
		if (m_head == nullptr)
			return push_front(data_);

		l_element<TL>* temp_element = new l_element<TL>(data_);
		tail().element()->next() = temp_element;
		
		return l_iterator<TL>(temp_element);
	}

	// Push data before the first element (head)
	l_iterator<TL> push_front(TL data_) {
		l_element<TL>* new_head = new l_element<TL>(data_, m_head);
		m_head = new_head;
		
		return l_iterator<TL>(m_head);
	}

	// Inserts one element into the list
	l_iterator<TL> insert(unsigned int pos_, TL data_) {
		if (m_head == nullptr || pos_ <= 0)
			return push_front(data_);

		l_iterator<TL> prev = seek_before(pos_);
		prev.element()->next() = new l_element<TL>(data_, prev.element()->next());
		
		return l_iterator<TL>(prev.element()->next());
	}
};