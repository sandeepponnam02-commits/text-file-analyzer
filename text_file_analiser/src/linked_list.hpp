// src/linked_list.hpp
#pragma once
#include <cstddef>
#include <utility>

template <typename T>
class SinglyLinkedList {
    struct Node {
        T value;
        Node* next;
        Node(const T& v, Node* n=nullptr): value(v), next(n) {}
        Node(T&& v, Node* n=nullptr): value(std::move(v)), next(n) {}
    };
    Node* head_ = nullptr;
    std::size_t size_ = 0;

public:
    SinglyLinkedList() = default;
    ~SinglyLinkedList() { clear(); }
    SinglyLinkedList(const SinglyLinkedList& o) {
        // copy preserving order
        Node* tail = nullptr;
        for (Node* p = o.head_; p; p = p->next) {
            Node* n = new Node(p->value);
            if (!head_) head_ = tail = n;
            else { tail->next = n; tail = n; }
        }
        size_ = o.size_;
    }
    SinglyLinkedList& operator=(SinglyLinkedList o) { swap(o); return *this; }
    SinglyLinkedList(SinglyLinkedList&& o) noexcept : head_(o.head_), size_(o.size_) {
        o.head_ = nullptr; o.size_ = 0;
    }

    void push_front(const T& v) { head_ = new Node(v, head_); ++size_; }
    void push_front(T&& v) { head_ = new Node(std::move(v), head_); ++size_; }

    template <typename Pred>
    T* find_if(Pred pred) {
        for (Node* p = head_; p; p = p->next) if (pred(p->value)) return &p->value;
        return nullptr;
    }

    template <typename Pred>
    bool remove_if(Pred pred) {
        Node* prev = nullptr; Node* cur = head_;
        while (cur) {
            if (pred(cur->value)) {
                if (prev) prev->next = cur->next; else head_ = cur->next;
                delete cur; --size_; return true;
            }
            prev = cur; cur = cur->next;
        }
        return false;
    }

    struct iterator {
        Node* p;
        T& operator*() { return p->value; }
        iterator& operator++(){ p = p->next; return *this; }
        bool operator!=(const iterator& o) const { return p != o.p; }
    };

    iterator begin() { return iterator{head_}; }
    iterator end() { return iterator{nullptr}; }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        Node* p = head_;
        while (p) { Node* n = p->next; delete p; p = n; }
        head_ = nullptr; size_ = 0;
    }

    void swap(SinglyLinkedList& o) noexcept {
        std::swap(head_, o.head_);
        std::swap(size_, o.size_);
    }
};