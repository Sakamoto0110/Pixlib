#include "PixAPI.h"
using namespace PixAPI;



// * * * * * * * * * * * * * * * * * * //
//                                     // 
//         Object functions            //
//                                     //
// * * * * * * * * * * * * * * * * * * //

#ifndef  OBJECT_H
#define OBJECT_H


OBJECT_t::OBJECT_t() {
    *this = 0;
    this->Type = 0;
    this->_valid = false;
}

bool OBJECT_t::isValid() {
    return OBJECT_t::_valid;
}

OBJECT_t& OBJECT_t::operator=(OBJECT_t obj) {
    switch (obj.Type)
    {
    case e_byte:  return *this = (BYTE)obj;
    case e_int:  return *this = (INT)obj;
    case e_uint:  return *this = (UINT)obj;
    case e_double:  return *this = (DOUBLE)obj;
    case e_lpvoid:  return *this = (LPVOID)obj;
    }
    return *this;
}

OBJECT_t& OBJECT_t::operator=(BYTE v) {
    this->_byte = v;
    this->Type = e_byte;
    this->_valid = true;
    return *this;
}

OBJECT_t& OBJECT_t::operator=(INT v) {
    this->_int = v;
    this->Type = e_int;
    this->_valid = true;
    return *this;
}

OBJECT_t& OBJECT_t::operator=(UINT v) {
    this->_uint = v;
    this->Type = e_uint;
    this->_valid = true;
    return *this;
}

OBJECT_t& OBJECT_t::operator=(DOUBLE v) {
    this->_double = v;
    this->Type = e_double;
    this->_valid = true;
    return *this;
}

OBJECT_t& OBJECT_t::operator=(LPVOID v) {
    this->_ptr = v;
    this->Type = e_lpvoid;
    this->_valid = true;
    return *this;
}

#endif // ! OBJECT_H



// * * * * * * * * * * * * * * * * * * //
//                                     // 
//       ObjectArray functions         //
//                                     //
// * * * * * * * * * * * * * * * * * * //

#ifndef OBJECT_ARR_H
#define OBJECT_ARR_H

OBJECT_ARR::OBJECT_ARR() {
    _max_size = 0;
    data = (Object*)calloc(_max_size, sizeof(Object));

}

OBJECT_ARR::OBJECT_ARR(int size) {
    _max_size = size;
    data = (Object*)calloc(_max_size, sizeof(Object));
}

OBJECT_ARR::OBJECT_ARR(std::initializer_list<Object> l) {
    _max_size = (int)l.size();
    data = (Object*)calloc(_max_size, sizeof(Object));
    if (data != NULL) {
        int i = 0;
        for (auto o = std::begin(l); o != std::end(l); ++o) {
            data[i++] = *o;
        }
    }
}


void PixAPI::objectArr_release(ObjectArr* arr) {
    free(arr->data);
}

void PixAPI::objectArr_resize(ObjectArr* arr, int i) {
    Object* temp = arr->data;
    if ((arr->data = (Object*)realloc(arr->data, sizeof(Object) * i)) != nullptr) {
        arr->_max_size = i;
        return;
    }
    free(temp);
}

int PixAPI::objectArr_count_valid(ObjectArr* arr) {
    int c = 0;
    for (int i = 0; i < arr->_max_size; i++) {
        if (arr->data[i].isValid()) {
            c++;
        }
    }
    return c;
}

int PixAPI::objectArr_first_valid_index(ObjectArr* arr) {
    for (int i = 0; i < arr->_max_size; i++) {
        if (arr->data[i].isValid()) {
            return i;
        }
    }
    return -1;
}

int PixAPI::objectArr_last_valid_index(ObjectArr* arr) {
    int c = -1;
    for (int i = 0; i < arr->_max_size; i++) {
        if (arr->data[i].isValid()) {
            c = i;
        }
    }
    return c;
}

Object* PixAPI::objectArr_peek(ObjectArr* arr, int i) {
    if (i < 0 || i >= arr->_max_size)
        throw std::out_of_range("Object index is out of bounds.");
    return &(arr->data[i]);
}

Object PixAPI::objectArr_get(ObjectArr* arr, int i) {
    if (i < 0 || i >= arr->_max_size)
        throw std::out_of_range("Object index is out of bounds.");
    return arr->data[i];
}






















#endif // !OBJECT_ARR_H



// * * * * * * * * * * * * * * * * * * //
//                                     // 
//       ObjectStack functions         //
//                                     //
// * * * * * * * * * * * * * * * * * * //

#ifndef OBJECT_STACK_H
#define OBJECT_STACK_H

int         PixAPI::stack_push(ObjectStack* s, Object o) {
    s->data[s->top++] = o;
    return s->top;
}

Object      PixAPI::stack_pop(ObjectStack* s) {
    return s->data[--(s->top)];
}

Object      PixAPI::stack_peek(ObjectStack* s) {
    return s->data[s->top - 1];
}

void        PixAPI::stack_clear(ObjectStack* s) {
    s->top = 0;
    delete(s->data);
}

void        PixAPI::stack_delete(ObjectStack* s) {
    s->top = 0;
    delete(s->data);
    delete s;
}

ObjectStack PixAPI::make_object_stack(int _size) {
    ObjectStack s;
    s.top = 0;
    s.size = _size;
    s.data = (Object*)malloc(sizeof(Object) * _size);
    return s;
}

#endif // !OBJECT_STACK_H




// * * * * * * * * * * * * * * * * * * //
//                                     // 
//         Functor functions           //
//                                     //
// * * * * * * * * * * * * * * * * * * //

#ifndef FUNCTOR_H
#define FUNCTOR_H



Functor::Functor() {
    _functorPtr = nullptr;
    _modelArr = (ObjectArr*)calloc(MAX_MODELS, sizeof(ObjectArr));
}

Object Functor::Invoke(int model_id) {
    ObjectStack* stack = &PixAPI::Obj_ArrToStack(&_modelArr[model_id]);
    Object o = _functorPtr->Invoke(*stack);
    stack_delete(stack);
    return o;
}

void Functor::registerModel(const INT_ObjectArr_Pair& kvp) const {
    _modelArr[kvp.key] = kvp.value;
}

void Functor::ClearModels() {
    for (int i = 0; i < MAX_MODELS; i++)
        PixAPI::objectArr_release(&_modelArr[i]);
}

const Functor& Functor::setFunctor(LPCallableObject _func) {
    if (_functorPtr != nullptr) {
        delete(_functorPtr);
    }
    _functorPtr = _func;
    return *this;
}




#endif // !FUNCTOR_H

// # # # # # # # # # # # # # # # # # # # # # # #  
// # # # # # # # # # # # # # # # # # # # # # # #  
// # # # # # # # # # # # # # # # # # # # # # # #  





ObjectStack& PixAPI::Obj_ArrToStack(ObjectArr* arr) {
    //int n = arr.count_valid();
    int n = PixAPI::objectArr_count_valid(arr);
    ObjectStack* stack = new ObjectStack;
    ObjectStack temp = PixAPI::make_object_stack(10);
    *stack = temp;
    Object* obj = nullptr;
    for (int i = 0; i < n; i++) {
        if ((obj = PixAPI::objectArr_peek(arr,i)) != nullptr && obj->isValid()) {
            stack_push(stack, *obj);
        }
    }
    return *stack;
}
