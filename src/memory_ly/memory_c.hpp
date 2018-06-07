/*
 * memory_c.hpp
 *
 *  Created on: Aug 17, 2014
 *      Author: Pietro Incardona
 */

#include <boost/multi_array.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/mpl/vector.hpp>
#include <array>

#include "util/boost_multi_array_openfpm.hpp"
#include "util/ct_array.hpp"
#include "memory_array.hpp"
#include "memory/memory.hpp"

#ifndef MEMORY_C_HPP_
#define MEMORY_C_HPP_

#define MEMORY_C_STANDARD 1
#define MEMORY_C_REDUCED  2

template<typename T, unsigned int impl = MEMORY_C_STANDARD, typename D = memory>
class memory_c
{

};

/*!
 * \brief This class is a container for the memory interface like HeapMemory CudaMemory
 *
 * It store the object used to allocate memory and a representation of this memory as an array of objects T
 *
 * It is mainly used by memory_conf to create the correct memory layout
 *
 * \see memory_traits_inte memory_traits_lin
 *
 */
template<typename T, typename D>
class memory_c<T,MEMORY_C_STANDARD,D>
{
	public:

	//! define T
	typedef memory_c<T> type;

	//! define a reference to T
	typedef T& reference;

	//! define T
	typedef T vtype;

	//! object that allocate memory like HeapMemory or CudaMemory
	memory * mem;

	//! object that represent the memory as an array of objects T
	memory_array<T> mem_r;

	/*! \brief This function set the object that allocate memory
	 *
	 * \param mem the memory object
	 *
	 */
	void setMemory(memory & mem)
	{
		if (this->mem != NULL)
		{
			this->mem->decRef();

			if (this->mem->ref() == 0 && &mem != this->mem)
				delete(this->mem);
		}
//			this->mem->decRef();
		mem.incRef();
		this->mem = &mem;
	}

	/*! \brief This function bind the memory_c to this memory_c as reference
	 *
	 * Bind ad reference it mean this this object does not create new memory but use the one from ref
	 * as a reference.
	 *
	 */
	bool bind_ref(const memory_c<T,MEMORY_C_STANDARD,D> & ref)
	{
	    mem = ref.mem;
	    mem->incRef();

	    //! we create the representation for the memory buffer
	    mem_r = ref.mem_r;

	    return true;
	}

	/*! \brief This function get the object that allocate memory
	 *
	 * \return memory object to allocate memory
	 *
	 */

	memory& getMemory()
	{
		return *this->mem;
	}

	/*! \brief Switch the pointer to device pointer
	 *
	 */
	void switchToDevicePtr()
	{
		mem_r.set_pointer(mem->getDevicePointer());
	}

	/*! \brief This function get the object that allocate memory
	 *
	 * \return memory object to allocate memory
	 *
	 */

	const memory& getMemory() const
	{
		return *this->mem;
	}

	/*! \brief This function allocate memory
	 *
	 */
	bool allocate(const size_t sz)
	{
		memory * mem = this->mem;

		//! We create a chunk of memory
	    mem->resize( sz*sizeof(T) );

	    //! we create the representation for this buffer
	    mem_r.initialize(mem->getPointer(),sz,mem->isInitialized());

	    return true;
	}

	/*! \brief It absorb the allocated object from another memory_c
	 *
	 * \param mem_c Memory object
	 *
	 */
/*	void move_copy(memory_c & mem_c)
	{
		// if mem_r is allocated delete it
		if (mem_r != NULL)
			delete(mem_r);

		//if mem is already allocated, deallocate it
		if (mem != NULL)
			delete(mem);

		// move the pointer
		mem = mem_c.mem;

		// Null the pointer to avoid double deallocation
		mem_c.mem = NULL;

		// move the pointer
		mem_r = mem_c.mem_r;

		// Null the pointer to avoid double deallocation
		mem_c.mem_r = NULL;
	}*/

	//! constructor
	memory_c():mem(NULL){}

	//! destructor
	~memory_c()
	{
		// deinitialixe mem_r
		mem_r.deinit();
		if (mem != NULL)
		{
			mem->decRef();

			if (mem->ref() == 0)
				delete(mem);
		}
	}

	/*! \brief swap the memory
	 *
	 * swap the memory between objects
	 *
	 */
	void swap(memory_c & mem_obj)
	{
		// Save on temporal

		void * mem_tmp = static_cast<void*>(mem);
		mem = mem_obj.mem;
		mem_obj.mem = static_cast<memory*>(mem_tmp);

		mem_obj.mem_r.swap(mem_r);
	}

};


/*!
 * \brief This class is a container for the memory interface like HeapMemory CudaMemory
 *
 * It store the object used to allocate memory and a representation of this memory as an array of objects T
 *
 * It is mainly used by memory_conf to create the correct memory layout
 *
 * \see memory_traits_inte memory_traits_lin
 *
 */
template<typename T>
class memory_c<T,MEMORY_C_REDUCED,memory>
{
	public:

	//! define T
	typedef memory_c<T> type;

	//! define a reference to T
	typedef T& reference;

	//! define T
	typedef T vtype;

	//! object that represent the memory as an array of objects T
	memory_array<T> mem_r;

	//! constructor
	memory_c() {}

	//! destructor
	~memory_c()
	{
	}
};

/*! \brief This class is a trick to indicate the compiler a specific
 *  specialization pattern
 *
 * In particular it say that a multidimensional array has been found and
 * need a special treatment, T is suppose to be a boost::mpl::vector of
 * unsigned int indicating each dimension. T has to be a type of known size at
 * compile time
 *
 */
template<typename T>
class multi_array
{
	typedef T type;
};

/*! \brief This class is a trick to indicate the compiler a specific
 *  specialization pattern
 *
 * This class is a trick to indicate the compiler a specific specialization
 * pattern, in particular it say that T is a key and
 * need a special treatment. T is suppose to be a boost::mpl::vector of
 * any type, (Actually does not have an application but is a generalization
 * of multi-array). T has to be a type of known size at compile time
 *
 */

template<typename T>
class key
{
	typedef T type;
};


/*! \brief this class multiply all the elements in a boost::mpl::vector excluding the first element
 *
 * \param T expecting a boost::mpl::vector
 *
 */
template<typename T, unsigned int N>
struct mult
{
	enum { value = mult<T,N-1>::value * boost::mpl::at<T,boost::mpl::int_<N>>::type::value };
};

template <typename T>
struct mult<T,1>
{
	enum { value = boost::mpl::at<T,boost::mpl::int_<1>>::type::value };
};

template<typename size_type, unsigned int dim>
static inline std::array<size_type ,dim> zero_dims()
{
	std::array<size_type ,dim> dimensions;

    // fill runtime, and the other dimensions
    for (size_t i = 0 ; i < dim ; i++)
    {dimensions[i] = 0;}

	return dimensions;
}

template<unsigned int dim, typename size_type>
struct array_ord
{
};

template<typename size_type>
struct array_ord<4,size_type>
{
	static constexpr size_type data[4] = {0,3,2,1};
};

template<typename size_type>
struct array_ord<3,size_type>
{
	static constexpr size_type data[3] = {0,2,1};
};

template<typename size_type>
struct array_ord<2,size_type>
{
	static constexpr size_type data[2] = {0,1};
};

template<unsigned int dim>
struct array_asc
{
};

template<>
struct array_asc<4>
{
	static constexpr bool data[4] = {true,true,true,true};
};

template<>
struct array_asc<3>
{
	static constexpr bool data[3] = {true,true,true};
};

template<>
struct array_asc<2>
{
	static constexpr bool data[2] = {true,true};
};


/*! \brief Specialization of memory_c for multi_array
 *
 * Specialization of memory_c for multi_array
 *
 * It is mainly used by memory_conf to create the correct layout
 *
 * \see memory_traits_inte memory_traits_lin
 *
 * \tparam T is suppose to be a boost::mpl::vector specifing at position 0 the type and at
 * position 1 to N the dimensions size of the multi_array
 *
 * \tparam D object that allocate memory
 *
 */
template<typename T, typename D>
class memory_c<multi_array<T>, MEMORY_C_STANDARD, D>
{
	/*! \brief In combination with generate_array is used to produce array at compile-time
	 *
	 * In combination with generate_array is used to produce at compile-time
	 * arrays like {0,N-1,.........0} used in boost::multi_array for index ordering
	 *
	 */
	template<size_t index,size_t N> struct ordering
	{
	    enum { value = (N-index) % N };
	};

	/*! \brief In combination with generate_array is used to produce array at compile-time
	 *
	 * In combination with generate_array is used to produce at compile-time
	 * arrays like {true,true,.........true} used in boost::multi_array to
	 * define ascending order
	 *
	 */
	template<size_t index,size_t N> struct ascending
	{
	    enum { value = true };
	};

	//! define boost::mpl::int_ without boost::mpl
	template<int S> using int_ = boost::mpl::int_<S>;

	//! define the template vector size it give a number at compile time
	typedef typename boost::mpl::size<T> size_p;

	//! Define "at" meta function without boost::mpl
	template< typename S, unsigned int n> using at = boost::mpl::at<S,boost::mpl::int_<n> >;

	typedef typename at<T,0>::type base;

	//! define size_type
	typedef typename boost::multi_array_ref<base,size_p::value>::size_type size_type;

//#ifdef __NVCC__

	array_ord<size_p::value,typename boost::multi_array<T,size_p::value>::size_type> ord;

	array_asc<size_p::value> asc;

//#else

    //! we generate the ordering buffer ord::data = {0,N-1 ...... 1 }
//    typedef typename generate_array<typename boost::multi_array<T,size_p::value>::size_type,size_p::value, ordering>::result ord;

    // we generate the ascending buffer
//    typedef typename generate_array<bool,size_p::value, ascending>::result asc;

//#endif

	public:

	/*! \brief This function set the object that allocate memory
	 *
	 * \param mem the memory object
	 *
	 */

	void setMemory(memory & mem)
	{
		if (this->mem != NULL)
		{
			this->mem->decRef();

			if (this->mem->ref() == 0 && &mem != this->mem)
				delete(this->mem);
		}
		mem.incRef();
		this->mem = &mem;
	}

	/*! \brief This function get the object that allocate memory
	 *
	 * \return memory object to allocate memory
	 *
	 */

	memory& getMemory()
	{
		return *this->mem;
	}

	/*! \brief Switch the pointer to device pointer
	 *
	 */
	void switchToDevicePtr()
	{
		mem_r.set_pointer(mem->getDevicePointer());
	}

	/*! \brief This function bind the memory_c to this memory_c as reference
	 *
	 * Bind ad reference it mean this this object does not create new memory but use the one from ref
	 * as a reference.
	 *
	 */
	bool bind_ref(const memory_c<multi_array<T>, MEMORY_C_STANDARD, D> & ref)
	{
	    mem = ref.mem;
	    mem->incRef();

	    //! we create the representation for the memory buffer
	    mem_r = ref.mem_r;

	    return true;
	}

	/*! \brief This function allocate memory and associate the representation to mem_r
	 *
	 * This function allocate memory and associate the representation of that chunk of
	 * memory to mem_r
	 *
	 */
	bool allocate(const size_t sz)
	{
		memory * mem = this->mem;

		//! We create a chunk of memory
	    mem->resize( sz*mult<T,size_p::value-1>::value*sizeof(base) );

	    // We create an array dims from the boost::mpl::vector
	    typedef typename generate_array_vector<size_type,T>::result dims;

	    //! buffer to store the dimensions of the full multi_array buffer
	    std::array<size_type ,size_p::value> dimensions;

	    // fill runtime, and the other dimensions
	    dimensions[0] = sz;
	    for (int i = 0 ; i < size_p::value-1 ; i++)
	    {dimensions[i+1] = dims::data[i];}

	    boost::multi_array_ref_openfpm<base,size_p::value> tmp(static_cast<base *>(mem->getPointer()),dimensions,boost::general_storage_order<size_p::value>(boost::ofp_storage_order()));

	    //! we create the representation for the memory buffer
	    mem_r.swap(tmp);

	    return true;
	}

	//! define the type of the multi_array vector minus one of the original size
	//! basically we remove the index 0 of the multi_array
	typedef boost::multi_array<base,size_p::value> type;

	//! Reference to an object to allocate memory
	D * mem;

	//! object that represent the memory as a multi-dimensional array of objects T
	boost::multi_array_ref_openfpm<base,boost::mpl::size<T>::value> mem_r;

	//! constructor
	memory_c()
	:mem(NULL),
	 mem_r(static_cast<base *>(NULL),zero_dims<size_type,size_p::value>(),
		   boost::ofp_storage_order())
	{}

	//! destructor
	~memory_c()
	{
		if (mem != NULL)
		{
			mem->decRef();
			if (mem->ref() == 0)
				delete(mem);
		}
	}

	//! set the device memory interface, the object that allocate memory
	void set_mem(memory & mem)
	{
		this->mem = &mem;
	}

	/*! \brief swap the memory
	 *
	 * swap the memory between objects
	 *
	 */
	void swap(memory_c & mem_obj)
	{
		// Save on temporal

		void * mem_tmp = static_cast<void*>(mem);
		mem = mem_obj.mem;
		mem_obj.mem = static_cast<D*>(mem_tmp);

		mem_r.swap(mem_obj.mem_r);
	}
};

#endif

