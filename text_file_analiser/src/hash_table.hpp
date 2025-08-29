#pragma once
#include <string>
#include <vector>
#include <functional>
#include <utility>
#include "linked_list.hpp"

template <typename K, typename V>
class HashTable {
    struct Entry {
        K key;
        V value;
    };

    std::vector<SinglyLinkedList<Entry>> buckets_;
    std::size_t sz_ = 0;
    float max_load_ = 0.75f;

    std::size_t index_for(const K& k) const {
        std::size_t h = std::hash<K>{}(k);
        return h % buckets_.size();
    }

    void rehash(std::size_t new_cap) {
        std::vector<SinglyLinkedList<Entry>> nb(new_cap);
        for (auto& lst : buckets_) {
            for (auto it = lst.begin(); it != lst.end(); ++it) {
                std::size_t idx = std::hash<K>{}((*it).key) % new_cap;
                nb[idx].push_front(Entry{(*it).key, (*it).value});
            }
        }
        buckets_.swap(nb);
    }

    void maybe_expand() {
        if (buckets_.empty()) { buckets_.resize(8); return; }
        float load = static_cast<float>(sz_) / static_cast<float>(buckets_.size());
        if (load > max_load_) rehash(buckets_.size() * 2);
    }

public:
    HashTable() { buckets_.resize(8); }
    std::size_t size() const { return sz_; }
    bool empty() const { return sz_ == 0; }

    bool contains(const K& key) const {
        std::size_t idx = index_for(key);
        for (auto it = const_cast<SinglyLinkedList<Entry>&>(buckets_[idx]).begin();
             it != const_cast<SinglyLinkedList<Entry>&>(buckets_[idx]).end(); ++it) {
            if ((*it).key == key) return true;
        }
        return false;
    }

    V* get(const K& key) {
        std::size_t idx = index_for(key);
        return buckets_[idx].find_if([&](Entry& e){ return e.key == key; }) ? 
               &buckets_[idx].find_if([&](Entry& e){ return e.key == key; })->value : nullptr;
    }

    // Insert or assign
    void put(const K& key, const V& val) {
        maybe_expand();
        std::size_t idx = index_for(key);
        if (auto* e = buckets_[idx].find_if([&](Entry& e){ return e.key == key; })) {
            e->value = val;
            return;
        }
        buckets_[idx].push_front(Entry{key, val});
        ++sz_;
    }

    // Increment (for counters)
    void increment(const K& key, const V& delta) {
        maybe_expand();
        std::size_t idx = index_for(key);
        if (auto* e = buckets_[idx].find_if([&](Entry& e){ return e.key == key; })) {
            e->value += delta;
        } else {
            buckets_[idx].push_front(Entry{key, delta});
            ++sz_;
        }
    }

    // Iterate all entries
    template <typename F>
    void for_each(F f) const {
        for (auto& lst : buckets_) {
            for (auto it = const_cast<SinglyLinkedList<Entry>&>(lst).begin();
                 it != const_cast<SinglyLinkedList<Entry>&>(lst).end(); ++it) {
                f((*it).key, (*it).value);
            }
        }
    }
};