#pragma once
#include <stdlib.h>
#include <iostream>
#include <initializer_list>
#include <Windows.h>
#include <utility>



namespace PixAPI {

    typedef unsigned char BYTE;    
    typedef int INT;    
    typedef unsigned int UINT;
    typedef double DOUBLE;
    typedef void* LPVOID;    
    typedef unsigned char* LPBYTE;
    
    template<typename ... Tx> using _LPActionTx_t = void(*)(Tx...);
    template<typename Tout, typename ... Tx> using _LPFunctionTx_t = Tout(*)(Tx...);
    
    template<typename K, typename V>
    struct KV_Pair {
    public:
        K key;
        V value;
        KV_Pair(K _k, V _v) : key(_k), value(_v) {}
    };

   

#pragma region OBJECT_tt

    typedef struct OBJECT_t {        
        union {
            BYTE _byte;
            INT _int;
            UINT _uint;
            DOUBLE _double;
            LPVOID _ptr;
        };
        enum eType {
            e_nullobj = 0,
            e_byte,
            e_int,
            e_uint,
            e_double,
            e_lpvoid
        };            
    public:
        bool _valid;
        BYTE Type;
             
        OBJECT_t();     

        bool isValid();

        OBJECT_t& operator=(OBJECT_t obj);

        OBJECT_t& operator=(BYTE   v);
        OBJECT_t& operator=(INT    v);
        OBJECT_t& operator=(UINT   v);
        OBJECT_t& operator=(DOUBLE v);
        OBJECT_t& operator=(LPVOID v);
        
        // Implicit type casts
        // * * *
        inline OBJECT_t(BYTE v)   { *this = (BYTE)v;   }
        inline operator BYTE()    { return _byte;      }     
        // * * *
        inline OBJECT_t(INT v)    { *this = (INT)v;    }
        inline operator INT()     { return _int;       }    
        // * * *
        inline OBJECT_t(UINT v)   { *this = (UINT)v;   }
        inline operator UINT()    { return _uint;      } 
        // * * *
        inline OBJECT_t(DOUBLE v) { *this = (DOUBLE)v; }
        inline operator DOUBLE()  { return _double;    }   
        // * * *
        inline OBJECT_t(LPVOID v) { *this = (LPVOID)v; }
        inline operator LPVOID()  { return _ptr;       }
        // * * *
        template<typename T> inline operator T() { return *((T*)_ptr); }
        template<typename T> inline operator T*() { return (T*)_ptr; }
        // * * *


        

    }Object;
    
#pragma endregion
    
#pragma region OBJECT_ARR_t

    typedef struct OBJECT_ARR {
        Object* data;
        int _max_size;

        OBJECT_ARR();
        OBJECT_ARR(int size);
        OBJECT_ARR(std::initializer_list<Object> l);

        const OBJECT_ARR& operator +=(Object obj);
        const OBJECT_ARR& operator =(std::initializer_list<Object> l);

    }ObjectArr;
    using INT_ObjectArr_Pair = KV_Pair<INT, ObjectArr>;


    void objectArr_release(ObjectArr* arr);
    void objectArr_resize(ObjectArr* arr, int i);

    int objectArr_count_valid(ObjectArr* arr);
    int objectArr_first_valid_index(ObjectArr* arr);
    int objectArr_last_valid_index(ObjectArr* arr);

    Object* objectArr_peek(ObjectArr* arr, int i);
    Object objectArr_get(ObjectArr* arr, int i);

#pragma endregion

#pragma region OBJECT_STACK_t

    typedef struct OBJECT_STACK {
        int top;
        int size;
        Object* data;
    }ObjectStack;

    int stack_push(ObjectStack* s, Object o);
    Object stack_pop(ObjectStack* s);
    Object stack_peek(ObjectStack* s);
    void stack_clear(ObjectStack* s);
    void stack_delete(ObjectStack* s);
    ObjectStack make_object_stack(int _size);
    ObjectStack& Obj_ArrToStack(ObjectArr* arr);

#pragma endregion

    
    

    



    


 
        

    


    // Generic function container interface
    // Not binded to any function signature        
    typedef __interface CallableObject_t {
    public:
        inline virtual Object Invoke(ObjectStack stack) const = 0;

    }ICallableObject;
    typedef ICallableObject* LPCallableObject;



    // Generic function container
    // Binded to function signature with void return
    template<typename ... Tx>
    class Action : public ICallableObject {
    private:
        _LPActionTx_t<Tx...> ActionTxA;
    public:
        Action<Tx...>() : ActionTxA(nullptr) {}
        Action<Tx...>(const _LPActionTx_t<Tx...>& _func) : ActionTxA(_func) { }

        inline Object ICallableObject::Invoke(ObjectStack stack) const {
            ActionTxA((Tx...)(stack_pop(&stack))...);
            return (INT)0;
        }
        
        inline operator const _LPActionTx_t<Tx...>() { return ActionTxA; };
    };

    // Generic function container
    // Binded to function signature with Tout return
    template<typename Tout, typename ... Tx>
    class Function : public ICallableObject {
    private:
        _LPFunctionTx_t<Tout, Tx...> FunctionTxA;
    public:
        Function<Tout, Tx...>() : FunctionTxA(nullptr) { }
        Function<Tout, Tx...>(const _LPFunctionTx_t<Tout, Tx...>& _func) : FunctionTxA(_func) { }

        inline Object ICallableObject::Invoke(ObjectStack stack) const {
            return (Tout)FunctionTxA((Tx...)(stack_pop(&stack))...);
        }
       
        inline operator const _LPFunctionTx_t<Tout, Tx...>() { return FunctionTxA; };
    };

   

    // Wrapper class for Functors namespace to reduce visual polution caused by Functors namespace
    class Functor {
    private:
        ObjectArr* _modelArr;
        LPCallableObject _functorPtr;
    public:        
        const static int MAX_MODELS = 1024;
        Functor();
        
        Object Invoke(int model_id);
        void registerModel(const INT_ObjectArr_Pair& kvp) const;
        void ClearModels();        
        const Functor& setFunctor(LPCallableObject _func);


        const Functor& operator +=(const INT_ObjectArr_Pair& kvp) {
            this->registerModel(kvp);
            return *this;
        }

        template<typename...Tx>
        const Functor& operator =(const _LPActionTx_t<Tx...>& _func) {
            return setFunctor(new Action(_func));
            
        }

        template<typename Tout, typename...Tx>
        const Functor& operator =(const _LPFunctionTx_t<Tout, Tx...>& _func) {
            return setFunctor(new Function(_func));
        }

      
    };


}



    

