#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QString>
#include <cstddef>

template <typename T>
class Stack {
public:
    struct Node {
        T data;
        Node* next;
    };

    Stack() : top_(nullptr), size_(0) {}

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    ~Stack() { clear(); }

    void push(const T& value) {
        Node* node = new Node{ value, top_ };
        top_ = node;
        ++size_;
    }

    bool pop(T& out) {
        if (!top_) return false;
        Node* del = top_;
        out = del->data;
        top_ = del->next;
        delete del;
        --size_;
        return true;
    }

    bool pop() {
        T tmp;
        return pop(tmp);
    }

    bool top(T& out) const {
        if (!top_) return false;
        out = top_->data;
        return true;
    }

    bool empty() const { return top_ == nullptr; }
    int size() const { return size_; }

    void clear() {
        while (top_) {
            Node* del = top_;
            top_ = top_->next;
            delete del;
        }
        size_ = 0;
    }

    template <typename Fn>
    void forEach(Fn fn) const {
        Node* cur = top_;
        while (cur) {
            fn(cur->data);
            cur = cur->next;
        }
    }

private:
    Node* top_;
    int size_;
};

template <typename T>
class Queue {
public:
    struct Node {
        T data;
        Node* next;
    };

    Queue() : head_(nullptr), tail_(nullptr), size_(0) {}

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    ~Queue() { clear(); }

    void enqueue(const T& value) {
        Node* node = new Node{ value, nullptr };
        if (!head_) {
            head_ = tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    bool dequeue(T& out) {
        if (!head_) return false;
        Node* del = head_;
        out = del->data;
        head_ = del->next;
        if (!head_) tail_ = nullptr;
        delete del;
        --size_;
        return true;
    }

    bool front(T& out) const {
        if (!head_) return false;
        out = head_->data;
        return true;
    }

    bool empty() const { return head_ == nullptr; }
    int size() const { return size_; }

    void clear() {
        while (head_) {
            Node* del = head_;
            head_ = head_->next;
            delete del;
        }
        tail_ = nullptr;
        size_ = 0;
    }

    template <typename Fn>
    void forEach(Fn fn) const {
        Node* cur = head_;
        while (cur) {
            fn(cur->data);
            cur = cur->next;
        }
    }

    template <typename Fn>
    void forEachReversed(Fn fn) const {
        int n = size_;
        if (n == 0) return;
        T* buffer = new T[n];
        Node* cur = head_;
        int i = 0;
        while (cur) {
            buffer[i++] = cur->data;
            cur = cur->next;
        }
        for (int j = n - 1; j >= 0; --j) fn(buffer[j]);
        delete[] buffer;
    }

private:
    Node* head_;
    Node* tail_;
    int size_;
};

template <typename T>
class DoublyList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
    };

    DoublyList() : head_(nullptr), tail_(nullptr), size_(0) {}

    DoublyList(const DoublyList&) = delete;
    DoublyList& operator=(const DoublyList&) = delete;

    ~DoublyList() { clear(); }

    Node* pushBack(const T& value) {
        Node* node = new Node{ value, tail_, nullptr };
        if (!head_) {
            head_ = tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
        return node;
    }

    Node* pushFront(const T& value) {
        Node* node = new Node{ value, nullptr, head_ };
        if (!tail_) {
            head_ = tail_ = node;
        } else {
            head_->prev = node;
            head_ = node;
        }
        ++size_;
        return node;
    }

    void removeNode(Node* node) {
        if (!node) return;
        if (node->prev) node->prev->next = node->next;
        else head_ = node->next;
        if (node->next) node->next->prev = node->prev;
        else tail_ = node->prev;
        delete node;
        --size_;
    }

    Node* head() const { return head_; }
    Node* tail() const { return tail_; }
    int size() const { return size_; }
    bool empty() const { return head_ == nullptr; }

    void clear() {
        Node* cur = head_;
        while (cur) {
            Node* del = cur;
            cur = cur->next;
            delete del;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

    template <typename Fn>
    void forEach(Fn fn) const {
        Node* cur = head_;
        while (cur) {
            fn(cur->data);
            cur = cur->next;
        }
    }

    template <typename Fn>
    void forEachReversed(Fn fn) const {
        Node* cur = tail_;
        while (cur) {
            fn(cur->data);
            cur = cur->prev;
        }
    }

private:
    Node* head_;
    Node* tail_;
    int size_;
};

#endif
