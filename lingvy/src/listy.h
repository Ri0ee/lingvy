#pragma once

template<class TL>
class list {
public:

	class l_element {
	public:
		l_element() {};

		l_element(TL data_, l_element* next_element_ptr_ = nullptr) : m_data(data_), m_next_element_ptr(next_element_ptr_) {}

		l_element(l_element* next_element_ptr_) : m_next_element_ptr(next_element_ptr_) {}

		~l_element() {};

		l_element*& next() {
			return m_next_element_ptr;
		}

		TL& get_data() {
			return m_data;
		}

	private:
		TL m_data{};
		l_element* m_next_element_ptr = nullptr;
	};

	class l_iterator {
	public:
		l_iterator() {}

		l_iterator(const l_iterator& iterator_) {
			m_element_ptr = iterator_.element();
		}

		l_iterator(l_element* element_ptr_) : m_element_ptr(element_ptr_) {}

		~l_iterator() {}

		l_iterator& operator++() { // Prefix increment
			m_element_ptr = m_element_ptr->next();
			return *this;
		}

		l_iterator operator++(int) { // Postfix increment
			l_iterator result(*this);
			++(*this);
			return result;
		}

		TL& operator*() {
			return m_element_ptr->get_data();
		}

		bool operator==(const l_iterator& iterator_) {
			return iterator_.element() == m_element_ptr;
		}

		bool operator!=(const l_iterator& iterator_) {
			return iterator_.element() != m_element_ptr;
		}

		l_element* element() const {
			return m_element_ptr;
		}

		void swap_data(l_iterator& iterator_) {
			TL temp_data = iterator_.element()->get_data();
			iterator_.element()->get_data() = m_element_ptr->get_data();
			m_element_ptr->get_data() = temp_data;
		}

		void swap(l_iterator& iterator_) {
			l_element* temp_element_ptr = iterator_.element();
			iterator_.element() = m_element_ptr;
			m_element_ptr = temp_element_ptr;
		}

	private:
		l_element* m_element_ptr = nullptr;
	};

	// LIST
	
	list() {}

	list(list& list_) {
		for (auto it = list_.begin(); it != list_.end(); it++) 
			push(*it);
	}

	list(l_iterator begin_iterator_, l_iterator end_iterator_ = l_iterator(nullptr)) { 
		for (auto it = begin_iterator_; it != end_iterator_; it++)
			push(*it);
	}

	~list() {
		clear();
	}
	
	// Push data to the end of the list
	l_iterator push(TL data_) {
		if (m_head == nullptr)
			return push_front(data_);

		l_element* temp_element = new l_element(data_);
		tail().element()->next() = temp_element;
		
		return l_iterator(temp_element);
	}

	// Push data before the first element (head)
	l_iterator push_front(TL data_) {
		l_element* new_head = new l_element(data_, m_head);
		m_head = new_head;
		
		return l_iterator(m_head);
	}
	
	// Inserts one element into the list
	l_iterator push(unsigned int pos_, TL data_) {
		if (m_head == nullptr || pos_ <= 0)
			return push_front(data_);

		l_iterator prev = seek_before(pos_);
		prev.element()->next() = new l_element(data_, prev.element()->next());
		
		return l_iterator(prev.element()->next());
	}

	// Remove last element. Returns true if removed anything
	bool pop() {
		if (m_head == nullptr) return false;

		l_iterator ptail = pre_tail();
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

		l_element* temp_element = m_head->next();
		delete m_head;
		m_head = temp_element;

		return true;
	}

	// Removes one element from the list. Returns true if removed anything
	bool remove(unsigned int pos_) {
		if (m_head == nullptr) return false;
		if (pos_ == 0) return pop_front();
		if (pos_ >= size() - 1) return pop();

		l_iterator prev = seek_before(pos_);
		l_iterator after(prev.element()->next()->next());
		delete prev.element()->next();
		prev.element()->next() = after.element();

		return true;
	}

	// Removes all elements from the list
	void clear() {
		while (pop_front());
	}

	// Searches for the first appearance of data in list
	l_iterator find_first(TL data_) {
		for (auto it = begin(); it != end(); it++)
			if (*it == data_) return l_iterator(it);
		
		return end();
	}

	// Searches for the last appearance of data in list
	l_iterator find_last(TL data_) {
		l_iterator temp_iterator = end();
		for (auto it = begin(); it != end(); it++)
			if (*it == data_) temp_iterator = it;

		return temp_iterator;
	}

	// Searches for the element with minimal value
	l_iterator find_min(l_iterator begin_ = begin(), l_iterator end_ = end()) {
		l_iterator current_min(begin_);
		for (auto it = begin_; it != end_; it++)
			if (*current_min > *it)
				current_min = it;
				
		return current_min;
	}

	// Searches for the element with highest value
	l_iterator find_max(l_iterator begin_ = begin(), l_iterator end_ = end()) {
		l_iterator current_max(begin_);
		for (auto it = begin_; it != end_; it++)
			if (*current_max < *it)
				current_max = it;
			
		return current_max;
	}

	// Returns amount of elements in list
	unsigned int size() {
		unsigned int temp_size = 0;
		for (auto it = begin(); it != end(); it++)
			temp_size++;
		
		return temp_size;
	}

	// Returns iterator of the first element of the list
	l_iterator begin() {
		return l_iterator(m_head);
	}

	// Returns iterator signalling about the end for the list
	l_iterator end() {
		return l_iterator(nullptr);
	}

	// Returns iterator to the last element of the list
	l_iterator tail() {
		for (auto it = begin(); it != end(); it++)
			if (it.element()->next() == nullptr) return it;
		
		return l_iterator(nullptr);
	}

	// Returns iterator to the element that stays before last element
	l_iterator pre_tail() {
		for (auto it = begin(); it != end(); it++)
			if (it.element()->next() != nullptr) {
				if (it.element()->next()->next() == nullptr)
					return it;
			}
			else break;

		return l_iterator(nullptr);
	}

	// Returns distance between two iterators (amount of elements between them)
	int distance(l_iterator begin_iterator_, l_iterator dest_iterator_) {
		int counter = 0;
		for (auto it = begin_iterator_; it != end(); it++) {
			if (it == dest_iterator_) return counter;
			counter++;
		}

		return -1;
	}

private:
	l_element* m_head = nullptr; 

	l_iterator seek(unsigned int position_) {
		unsigned int current_position = 0;
		for (auto it = begin(); it != end() && current_position <= position_; it++) {
			if (current_position == position_)
				return it;
			current_position++;
		}
		return l_iterator(nullptr);
	}

	l_iterator seek_before(unsigned int position_) {
		return seek(position_ - 1);
	}

	l_iterator seek_after(unsigned int position_) {
		return seek(position_ + 1);
	}
};