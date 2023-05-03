#ifndef __MATRIX_HPP_
#define __MATRIX_HPP_

#include <tuple>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <optional>


template<typename TValueType, TValueType defValue>
class Matrix {
private:

class Iterator;  

using const_iterator = Iterator;  
using SubStorage = std::unordered_map<size_t, TValueType>;
using Storage = std::unordered_map<size_t, SubStorage>;
using StorageIterator = std::optional<typename Storage::const_iterator>;
using SubStorageIterator = std::optional<typename SubStorage::const_iterator>;
Storage _store;

class Iterator {
private:
    friend class Matrix;
    using difference_type = ptrdiff_t;
    using value_type = std::tuple<TValueType, size_t, size_t>;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::input_iterator_tag;

    value_type _node;
    StorageIterator _storageEnd;
    StorageIterator _storageIterator;
    SubStorageIterator _subStorageIterator;

    Iterator(StorageIterator itStoreBegin, StorageIterator itStoreEnd):
        _storageEnd(itStoreEnd),
        _storageIterator(itStoreBegin)
        { 
            if (itStoreEnd != itStoreBegin) {
                _subStorageIterator = itStoreBegin.value()->second.begin();
            } else {
                _subStorageIterator = std::nullopt;
            }
        }   

public:
    Iterator(const Iterator &it):
        _storageIterator(it._storageIterator),
        _subStorageIterator(it._subStorageIterator)
        { }     

    bool operator==(const Iterator& other) const {
        return _storageIterator == other._storageIterator &&
                _subStorageIterator == other._subStorageIterator;
    }

    bool operator!=(const Iterator& other) const {
        return !(*this == other);
    }

    value_type operator*() {
        return value_type(
            _storageIterator.value()->first, 
            _subStorageIterator.value()->first, 
            _subStorageIterator.value()->second
        );
    }

    Iterator& operator++() {
         ++_subStorageIterator.value();
        if (_subStorageIterator.value() == _storageIterator.value()->second.end()) {
            ++_storageIterator.value();
            if (_storageIterator.value() != _storageEnd.value()) {
                _subStorageIterator = _storageIterator.value()->second.begin();
            } else {
                _subStorageIterator = std::nullopt;
            }
        }
        return *this;
    }    

};

    class MatrRow {

        class Value {
            private:
                size_t _index;
                MatrRow& _parent;
                friend MatrRow;

            size_t index() {
                return _index;
            }

            public:
                Value(MatrRow& parent, size_t index):        
                    _index(index),    
                    _parent(parent)
                {
                    
                }  

            operator TValueType() const { 
                return _parent.value(_index);
            }        

            Value& operator= (const TValueType &value) {
                _parent.update(_index, value);
                return *this;
            }   
        };

        private:
            using ColumsValue = std::unordered_map<size_t, Value>;
            Matrix& _parent;
            size_t _rowIndex;
            friend Matrix;

            size_t rowIndex() const {
                return _rowIndex;
            }

            void update(size_t columnIndex, const TValueType &value) {
                _parent.update(_rowIndex, columnIndex, value);
            }

            TValueType value(size_t columnIndex) {
                return _parent.value(_rowIndex, columnIndex);
            }

        public:
            MatrRow(Matrix& parent, size_t rowIndex): 
            _parent(parent),
            _rowIndex(rowIndex)
            {

            }

            Value operator[](std::size_t index) {
                return Value(*this, index);
            }
    };
    
    void addValue(size_t rowIndex, size_t columnIndex, const TValueType &value) {
        auto itRow = _store.find(rowIndex);
        if (itRow != _store.end()) {
            auto itValue = itRow->second.find(columnIndex);
            if (itValue != itRow->second.end()) {
                itValue->second = value;
            } else {
                itRow->second.emplace(columnIndex, value);
            }
        } else {
            _store.emplace(rowIndex, SubStorage{{columnIndex, value}});
        }
    }

    void removeValue(size_t rowIndex, size_t columnIndex, const TValueType &value) {
        auto itRow = _store.find(rowIndex);
        if (itRow != _store.end()) {
            itRow->second.erase(columnIndex);
            if (itRow->second.size() == 0) {
                _store.erase(itRow);
            }
        } 
    }

    void update(size_t rowIndex, size_t columnIndex, const TValueType &value) {    
        if (value != defValue) {
            addValue(rowIndex, columnIndex, value);
        } else {
            removeValue(rowIndex, columnIndex, value);
        }
    }

    TValueType value(size_t rowIndex, size_t columnIndex) const {
        auto itRow = _store.find(rowIndex);
        if (itRow != _store.end()) {
            auto itValue = itRow->second.find(columnIndex);
            if (itValue != itRow->second.end()) {
                return itValue->second;
            }
        }
        return defValue;
    }

public:

    MatrRow operator[](std::size_t index) {
        return MatrRow(*this, index);
    }

    size_t size() const  {
        std::size_t answer = 0;
        for (const auto& [index, row] : _store) {
            answer += row.size();
        }
        return answer;
    }

    const_iterator begin() const {
    	return const_iterator(_store.begin(), _store.end());
    }

    const_iterator end() const {
    	return const_iterator(_store.end(), _store.end());
    }

};


#endif