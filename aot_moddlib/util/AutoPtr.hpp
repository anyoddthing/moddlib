//
//  AutoPointer.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 12/28/17.
//  Copyright © 2017 Daniel Doubleday. All rights reserved.
//

#pragma once

namespace moddlib
{
    template <typename T>
    struct AutoPtr
    {
        AutoPtr(T* v) : tag(POINTER), ptr(v) {}
        
        AutoPtr(std::unique_ptr<T>&& v) : tag(UNIQUE_PTR), uPtr(std::move(v)) {}
        
        AutoPtr(std::shared_ptr<T> v) : tag(SHARED_PTR), sPtr(v) {}
        
        AutoPtr(AutoPtr&& other)
        {
            tag = other.tag;
            if (other.tag == UNIQUE_PTR)
            {
                std::swap(uPtr, other.uPtr);
            }
            else if (other.tag == SHARED_PTR)
            {
                std::swap(sPtr, other.sPtr);
            }
            else
            {
                std::swap(ptr, other.ptr);
                ptr = other.ptr;
            }
            
            other.tag = POINTER;
        }
        
        ~AutoPtr()
        {
            if (tag == UNIQUE_PTR)
            {
                uPtr.get_deleter()(uPtr.get());
            }
            else if (tag == SHARED_PTR)
            {
                sPtr.~shared_ptr<T>();
            }
        }
        
        enum {POINTER,UNIQUE_PTR,SHARED_PTR} tag;
        union
        {
            T* ptr;
            std::unique_ptr<T> uPtr;
            std::shared_ptr<T> sPtr;
        };
        
        T& operator*() const
        {
            if (tag == POINTER)
            {
                return *ptr;
            }
            else if (tag == UNIQUE_PTR)
            {
                return *uPtr;
            }
            else
            {
                return *sPtr;
            }
        }
    };
    
    template <typename T>
    AutoPtr<T> MakeAutoPtr(T* v)
    {
        return AutoPtr<T>(v);
    }
    
    template <typename T>
    AutoPtr<T> MakeAutoPtr(std::unique_ptr<T>&& v)
    {
        return AutoPtr<T>(std::move(v));
    }
}


