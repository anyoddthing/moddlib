//
// Created by Daniel Doubleday on 04/07/15.
// Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#ifndef UNITSYNTH_ALIGNEDBUFFER_HPP
#define UNITSYNTH_ALIGNEDBUFFER_HPP

#include <array>

#include "aot_simdlib/aot_simdlib.hpp"

#include "Types.hpp"

namespace moddlib
{
    /** RAII container for 16-byte aligned memory
     * Only supports OSes where malloc returns aligned memory.
     * FIXME: Would need specialization for Win32.
     */
    struct AlignedMemory
    {
        AlignedMemory(size_t size) :
            _heap(new float[size]), _size(size)
        {}
        
        float* data()
        {
            return _heap.get();
        }
        
        operator float*()
        {
            return _heap.get();
        }
        
        template <typename FunctionT>
        void forEach(FunctionT f)
        {
            for (size_t i = 0; i < size(); ++i)
            {
                f(i, _heap.get()[i]);
            }
        }
        
        size_t size()
        {
            return _size;
        }
        
    private:
        std::unique_ptr<float[]> _heap;
        size_t _size;
    };
    
    template <size_t Size>
    struct AlignedBuffer : NonCopyable
    {
        typedef float                                 value_type;
        typedef value_type&                           reference;
        typedef const value_type&                     const_reference;
        typedef value_type*                           iterator;
        typedef const value_type*                     const_iterator;
        typedef value_type*                           pointer;
        typedef const value_type*                     const_pointer;
        typedef std::reverse_iterator<iterator>       reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::integral_constant<size_t, Size>  size_type;

        AlignedBuffer() {}
        AlignedBuffer(AlignedBuffer&& other) : _buffer(std::move(other._buffer)){}
        AlignedBuffer& operator=(AlignedBuffer&& other)
        {
            _buffer = std::move(other._buffer);
            return *this;
        }

        AlignedBuffer(const AlignedBuffer&) = delete;
        AlignedBuffer& operator=(const AlignedBuffer&) = delete;

        static const size_t stride = 4;
        constexpr uint32_t size() { return Size; }

        operator const float*() const { return _buffer.data(); }
        operator float*() { return _buffer.data(); }

        float& operator [](size_t index) { return _buffer[index]; }

        iterator begin() noexcept { return _buffer.begin(); }
        const_iterator begin() const noexcept { return _buffer.begin(); }
        iterator end() noexcept { return _buffer.end(); }
        const_iterator end() const noexcept { return _buffer.end(); }

        reverse_iterator rbegin() noexcept {return reverse_iterator(end());}
        const_reverse_iterator rbegin() const noexcept {return const_reverse_iterator(end());}
        reverse_iterator rend() noexcept {return reverse_iterator(begin());}
        const_reverse_iterator rend() const noexcept {return const_reverse_iterator(begin());}

        const_iterator cbegin() const noexcept {return begin();}
        const_iterator cend() const noexcept {return end();}
        const_reverse_iterator crbegin() const noexcept {return rbegin();}
        const_reverse_iterator crend() const noexcept {return rend();}

        float* data() { return _buffer.data(); }
        const float* data() const { return _buffer.data(); }

        float& front() { return _buffer.front(); }

        float& back() { return _buffer.back(); }

        template <typename FunctionT>
        void forEach(FunctionT f)
        {
            int i = 0;
            std::for_each(begin(), end(), [&](reference val) {
                f(i, val);
                i++;
            });
        }

        void zero_mem()
        {
            simd::recipes<Size>::memset(data(), 0);
        }

        void fill(float value)
        {
            simd::recipes<Size>::memset(data(), value);
        }

        void copy(const float* src)
        {
            simd::recipes<Size>::memcopy(data(), src);
        }

        void add(const float* src)
        {
            simd::recipes<Size>::add(data(), data(), src);
        }
        
    private:
        alignas(16) std::array<float, Size> _buffer;
    };

    struct SampleBuffer : AlignedBuffer<8> {};
    
    struct SampleBuffers
    {
        static SampleBuffers& sharedBuffers()
        {
            return _instance;
        }

        const SampleBuffer& flatLine()
        {
            return _zeroBuffer;
        }

    private:
    
        SampleBuffers()
        {
            _zeroBuffer.zero_mem();
        }

        static const SampleBuffers& sampleBuffers()
        {
            static SampleBuffers buffers;
            return buffers;
        }

    private:
        static SampleBuffers _instance;

        SampleBuffer _zeroBuffer;

    };

}

#endif //UNITSYNTH_ALIGNEDBUFFER_HPP
