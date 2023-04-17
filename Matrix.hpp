#ifndef __MATRIX_HPP_
#define __MATRIX_HPP_

#include <tuple>
#include <vector>
#include <algorithm>

template<typename TValueType, TValueType defValue>
class Matrix {
private:
    
    std::vector<size_t> _rows;
    std::vector<size_t> _columns;
    std::vector<TValueType> _values;
    
    std::vector<size_t>::iterator columnBegin(size_t row) {
        return _columns.begin() + _rows[row];
    }

    std::vector<size_t>::iterator columnEnd(size_t row) {
        return columnBegin(row) + (_rows[row + 1] - _rows[row]); 
    }

    size_t row(size_t valIdx) const {
        for (size_t i = 0; i < _rows.size(); ++i) {
            if (_rows[i] >= valIdx + 1) {
                return i-1;
            } 
        }
        return _rows.size() - 1;
    }

    size_t column(size_t valIdx) const {
        return _columns[valIdx];
    } 

    TValueType getValue(size_t row, size_t column) {
        if (row < _rows.size() - 1) {        
            auto iterIndex = std::find(columnBegin(row), columnEnd(row), column);
            if (iterIndex != columnEnd(row)) {
                auto valueIndex = std::distance(_columns.begin(), iterIndex);
                return _values[valueIndex];
            }
        }
        return defValue;
    }

    void addValue(TValueType value, size_t row, size_t column) {
        // добавляем новую строку
        if (row >= _rows.size() - 1) {
            _rows.resize(row + 2);
            _values.push_back(value);
            _columns.push_back(column);
            _rows[row + 1] = _values.size();
        } else {          
            auto insertAfter = std::find_if(columnBegin(row), columnEnd(row), [column](size_t& col) {
                return column < col;
            });
            auto position = (insertAfter != columnEnd(row))? std::distance(_columns.begin(), insertAfter): _rows[row + 1];
            if (_columns[position-1] != column) {
                _columns.insert(_columns.begin() + position, column);
                _values.insert(_values.begin() + position, value);
                std::for_each(_rows.begin() + 1 + row, _rows.end() - 1, [](size_t &count) { ++count; });
                _rows.back() = _values.size();
            } else {
                _values[position-1] = value;
            }
        }
    }

    void removeValue(size_t row, size_t column) {
        if (row >= _rows.size() - 1) {
            return;
        } else {
            auto insertAfter = std::find_if(columnBegin(row), columnEnd(row), [column](size_t& col) {
                return column == col;
            });
            if (insertAfter != columnEnd(row)) {
                auto position = std::distance(_columns.begin(), insertAfter);
                _columns.erase(_columns.begin() + position);
                _values.erase(_values.begin() + position);
                std::for_each(_rows.begin() + 1 + row, _rows.end() - 1, [](size_t &count) { --count; });
                _rows.back() = _values.size();
                auto eraseStart = std::find_if(_rows.rbegin(), _rows.rend() - 1, [](size_t& row){
                    return row != 0;
                });
                if (eraseStart.base() != _rows.end()) {
                    _rows.erase(eraseStart.base(), _rows.end());
                }    
            }
        }
    }

    void handleValue(TValueType value, size_t row, size_t column) {
        if (value != defValue) {
            addValue(value, row, column);
        } else {
            removeValue(row, column);
        }
    }

    class MatrRow {
    private:  
        size_t _row; 
        Matrix& _parent;
        friend class Matrix;
        
        class MatrValue {
        private:               
            size_t _column;
            MatrRow* _parent;
            TValueType _value;    
            friend class MatrRow;

            MatrValue(MatrRow* parent, size_t column, TValueType value): 
                _column(column),
                _parent(parent),
                _value(value)
            { } 

        public:
            MatrValue(TValueType value): 
                _value(value)
            { }  

            operator TValueType() const { 
                return _value; 
            }    

            MatrValue& operator= (const MatrValue &value) {
                _parent->handleValue(value, _column);
                _value = value;
                return *this;
            }
        };

        void handleValue(TValueType value, size_t column) {
            _parent.handleValue(value, _row, column);
        }

        TValueType getValue(size_t column) {
            return _parent.getValue(_row, column);
        }

        MatrRow(Matrix &parent, size_t row) : 
            _parent(parent),
            _row(row)
        { }

    public:
        MatrValue operator[](size_t column) {
            return MatrValue(this, column, getValue(column));
        }
    };

    class Iterator {
        private:
            friend class Matrix;
            using difference_type = ptrdiff_t;
            using value_type = std::tuple<TValueType, size_t, size_t>;
            using pointer = value_type*;
            using reference = value_type&;
            using iterator_category = std::input_iterator_tag;

            value_type _node;
            const Matrix& _parent;
            using ValueIter = typename std::vector<TValueType>::const_iterator;
    
            ValueIter _valuesBegin;
            ValueIter _valuesCurrent;
        
            Iterator(const Matrix& parent, ValueIter valueIter):
                _parent(parent),
                _valuesBegin(valueIter),
                _valuesCurrent(valueIter)
                { }   

        public:
            Iterator(const Iterator &it):
                _valuesCurrent(it._valuesCurrent)
                { }     

            bool operator!=(const Iterator& other) const {
    	        return _valuesCurrent != other._valuesCurrent;
            }

            bool operator==(const Iterator& other) const {
                return _valuesCurrent == other._valuesCurrent;
            }

            const value_type& operator*() {
                auto distance = std::distance(_valuesBegin, _valuesCurrent);
                std::get<0>(_node) = *_valuesCurrent;
                std::get<1>(_node) = _parent.row(distance);
                std::get<2>(_node) = _parent.column(distance);
                return _node;
            }

            Iterator& operator++() {
                ++_valuesCurrent;
                return *this;
            }    

    };

    using const_iterator = Iterator;   

public:
    Matrix(): 
        _rows(1, 0)
    {

    }

    size_t size() {
        return _values.size();
    }

    MatrRow operator[](int row) {
        return MatrRow(*this, row);
    }

    const_iterator begin() const {
    	return const_iterator(*this, _values.begin());
    }

    const_iterator end() const {
    	return const_iterator(*this, _values.end());
    }

};


#endif