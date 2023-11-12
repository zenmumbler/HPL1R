/*
 * 2023 by zenmumbler
 * This file is part of Rehatched
 */

#ifndef HPL_STL_HELPERS_H
#define HPL_STL_HELPERS_H

#include <cstdint>
#include <iterator>

namespace hpl {

	template <typename Derived>
	struct FullyComparableTrait {
		// requires operator== and operator<
		template <typename U>
		constexpr bool operator!=(const U& other) const {
			return !static_cast<const Derived*>(this)->operator==(other);
		}

		template <typename U>
		constexpr bool operator<=(const U& other) const {
			return static_cast<const Derived*>(this)->operator<(other) || static_cast<const Derived*>(this)->operator==(other);
		}

		template <typename U>
		constexpr bool operator>(const U& other) const {
			return !(static_cast<const Derived*>(this)->operator<(other) || static_cast<const Derived*>(this)->operator==(other));
		}

		template <typename U>
		constexpr bool operator>=(const U& other) const {
			return !static_cast<const Derived*>(this)->operator<(other);
		}

	protected:
		// traits may not be created directly
		FullyComparableTrait() = default;
	};

	//---------------------------------

	template <typename ValueType>
	class STLBufferIterator
	: public std::iterator<std::random_access_iterator_tag, ValueType>
	, public FullyComparableTrait<STLBufferIterator<ValueType>>
	{
	protected:
		uint8_t* position_ = nullptr;
		int strideBytes_ = 0;

	public:
		using ValueRef = ValueType&;
		using ConstValueRef = const ValueType&;

		constexpr STLBufferIterator() = default;

		constexpr STLBufferIterator(void* basePointer, int strideBytes)
		: position_{ static_cast<uint8_t*>(basePointer) }
		, strideBytes_{ strideBytes }
		{}

		constexpr ValueRef operator *() { return *(reinterpret_cast<ValueType*>(position_)); }
		constexpr ValueType* operator ->() { return reinterpret_cast<ValueType*>(position_); };

		constexpr ValueRef operator [](int index) {
			auto indexedPos = position_ + (strideBytes_	* index);
			return *reinterpret_cast<ValueType*>(indexedPos);
		}

		constexpr ConstValueRef operator [](int index) const {
			auto indexedPos = position_ + (strideBytes_ * index);
			return const_cast<ConstValueRef>(*reinterpret_cast<const ValueType*>(indexedPos));
		}

		const STLBufferIterator& operator ++() { position_ += strideBytes_; return *this; }
		STLBufferIterator operator ++(int) { auto ret = *this; position_ += strideBytes_; return ret; }

		const STLBufferIterator& operator --() { position_ -= strideBytes_; return *this; }
		STLBufferIterator operator --(int) { auto ret = *this; position_ -= strideBytes_; return ret; }

		constexpr bool operator ==(const STLBufferIterator& other) const { return position_ == other.position_; }
		constexpr bool operator <(const STLBufferIterator& other) const { return position_ < other.position_; }

		friend constexpr STLBufferIterator operator +(const STLBufferIterator& iter, int count) {
			auto ret = iter;
			ret.position_ += ret.strideBytes_ * count;
			return ret;
		}

		friend constexpr STLBufferIterator operator +(int count, const STLBufferIterator& iter) {
			auto ret = iter;
			ret.position_ += ret.strideBytes_ * count;
			return ret;
		}

		friend constexpr STLBufferIterator operator -(const STLBufferIterator& iter, int count) {
			auto ret = iter;
			ret.position_ -= ret.strideBytes_ * count;
			return ret;
		}

		friend STLBufferIterator& operator +=(STLBufferIterator& iter, int count) {
			iter.position_ += iter.strideBytes_ * count;
			return iter;
		}

		friend STLBufferIterator& operator -=(STLBufferIterator& iter, int count) {
			iter.position_ -= iter.strideBytes_ * count;
			return iter;
		}

		constexpr ptrdiff_t operator -(const STLBufferIterator& b) {
			return (position_ - b.position_) / static_cast<ptrdiff_t>(strideBytes_);
		}
	};

}

#endif /* HPL_STL_HELPERS_H */
