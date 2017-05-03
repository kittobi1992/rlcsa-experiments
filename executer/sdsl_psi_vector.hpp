/* sdsl - succinct data structures library
    Copyright (C) 2008 Simon Gog

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/ .
*/
/*! \file sdsl_psi_vector.hpp
   \brief sdsl_psi_vector.hpp contains the sdsl::sdsl_psi_vector class.
   \author Simon Gog
*/
#ifndef SDSL_PSI_VECTOR
#define SDSL_PSI_VECTOR

#include <vector>

#include <sdsl/int_vector.hpp>
#include <sdsl/enc_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/select_support.hpp>
#include <sdsl/coder.hpp>
#include <sdsl/iterators.hpp>

//! Namespace for the succinct data structure library.
namespace sdsl
{

//! A generic immutable space-saving vector class for unsigned integers.
/*! A vector v is stored more space-efficiently by self-delimiting coding
 *  the deltas v[i+1]-v[i] (v[-1]:=0). Space of the structure and random
 *  access time to it can be controlled by a sampling parameter t_dens.
 *
 *  \tparam t_coder  Self-delimiting coder.
 *  \tparam t_dens   Every t_dens-th element of v is sampled.
 *  \tparam t_width  Width of the int_vector used to store the samples and pointers.
 *  This class is a parameter of csa_sada.
 * @ingroup int_vector
 */
template <class t_vector=enc_vector<>>
class sdsl_psi_vector
{
  private:
    //static_assert(t_dens > 1 , "sdsl_psi_vector: sample density must be larger than `1`");
  public:
    typedef uint64_t value_type;
    typedef random_access_const_iterator<sdsl_psi_vector> iterator;
    typedef iterator const_iterator;
    typedef ptrdiff_t difference_type;
    typedef int_vector<>::size_type size_type;
    typedef iv_tag index_category;
    typedef typename sd_vector<>::rank_1_type rank_support;
    typedef typename sd_vector<>::select_1_type select_support;

  private:
    size_type m_size = 0; // number of vector elements
    size_type m_alphabet_size = 0;


    std::vector<t_vector> m_c;
    sd_vector<> m_alphabet_marker;
    rank_support m_alphabet_rank;
    select_support m_alphabet_select;

    void
    clear()
    {
        m_size = 0;
        m_alphabet_size = 0;
    }


  public:
    sdsl_psi_vector() = default;
    sdsl_psi_vector(const sdsl_psi_vector &) = default;
    sdsl_psi_vector(sdsl_psi_vector &&) = default;
    sdsl_psi_vector &operator=(const sdsl_psi_vector &) = default;
    sdsl_psi_vector &operator=(sdsl_psi_vector &&) = default;

    //! Constructor for a Container of unsigned integers.
    /*! \param c A container of unsigned integers.
          */
    template <class Container>
    sdsl_psi_vector(const Container &c);

    //! Constructor for an int_vector_buffer of unsigned integers.
    /*
            \param v_buf A int_vector_buf.
        */
    template <uint8_t int_width>
    sdsl_psi_vector(int_vector_buffer<int_width> &v_buf);

    //! Default Destructor
    ~sdsl_psi_vector() {}

    //! The number of elements in the sdsl_psi_vector.
    size_type size() const
    {
        return m_size;
    }

    //! Return the largest size that this container can ever have.
    static size_type max_size()
    {
        return int_vector<>::max_size() / 2;
    }

    //!    Returns if the sdsl_psi_vector is empty.
    bool empty() const
    {
        return 0 == m_size;
    }

    //! Swap method for sdsl_psi_vector
    void swap(sdsl_psi_vector &v);

    //! Iterator that points to the first element of the sdsl_psi_vector.
    const const_iterator begin() const
    {
        return const_iterator(this, 0);
    }

    //! Iterator that points to the position after the last element of the sdsl_psi_vector.
    const const_iterator end() const
    {
        return const_iterator(this, this->m_size);
    }

    //! operator[]
    /*! \param i Index. \f$ i \in [0..size()-1]\f$.
         */
    value_type operator[](size_type i) const;

    //! Serialize the sdsl_psi_vector to a stream.
    /*! \param out Out stream to write the data structure.
            \return The number of written bytes.
         */
    size_type serialize(std::ostream &out, structure_tree_node *v = nullptr, std::string name = "") const;

    //! Load the sdsl_psi_vector from a stream.
    void load(std::istream &in);


};

template <class t_vector>
inline typename sdsl_psi_vector<t_vector>::value_type
    sdsl_psi_vector<t_vector>::operator[](const size_type i) const
{
    size_type alphabet = m_alphabet_rank(i+1) - 1;
    size_type alphabet_start_pos = m_alphabet_select(alphabet+1);
    value_type val = m_c[alphabet][i - alphabet_start_pos];
    return val;
}

/*template<class t_int_vector, class t_bit_vector, uint32_t t_dens, uint8_t t_width>
inline typename sdsl_psi_vector<t_coder, t_dens,t_width>::value_type sdsl_psi_vector<t_coder, t_dens,t_width>::sample(const size_type i)const
{
    assert(i*get_sample_dens()+1 != 0);
    assert(i*get_sample_dens() < m_size);
    return m_sample_vals_and_pointer[i<<1];
}*/

template <class t_vector>
void sdsl_psi_vector<t_vector>::swap(sdsl_psi_vector<t_vector> &v)
{
    if (this != &v)
    {
        
    }
}

template <class t_vector>
template <class Container>
sdsl_psi_vector<t_vector>::sdsl_psi_vector(const Container &c)
{

    // clear bit_vectors
    clear();
    m_size = c.size();

    bit_vector alphabet_marker(m_size+1,0);
    alphabet_marker[0] = 1;
    alphabet_marker[m_size] = 1;
    m_alphabet_size = 1;
    for (size_type i = 1; i < m_size; ++i)
    {
        if(c[i] < c[i-1]) {
            alphabet_marker[i] = 1;
            m_alphabet_size++;
        }
    }
    m_alphabet_marker = sd_vector<>(alphabet_marker);
    m_alphabet_rank = rank_support(&m_alphabet_marker);
    m_alphabet_select = select_support(&m_alphabet_marker);

    m_c.assign(m_alphabet_size,t_vector());
    for (size_type i = 0; i < m_alphabet_size; ++i) {
        size_type start = m_alphabet_select(i+1);
        size_type end = m_alphabet_select(i+2);
        int_vector<> tmp(end-start,0);
        for (size_type j = start; j < end; ++j) {
            tmp[j - start] = c[j];
        }
        t_vector tmp_vec(tmp);
        m_c[i].swap(tmp_vec);
    }
}

template <class t_vector>
template <uint8_t int_width>
sdsl_psi_vector<t_vector>::sdsl_psi_vector(int_vector_buffer<int_width> &v_buf)
{
    // clear bit_vectors
    clear();
    m_size = v_buf.size();

    bit_vector alphabet_marker(m_size + 1, 0);
    alphabet_marker[0] = 1;
    alphabet_marker[m_size] = 1;
    m_alphabet_size = 1;
    for (size_type i = 1; i < m_size; ++i)
    {
        if (v_buf[i] < v_buf[i - 1])
        {
            alphabet_marker[i] = 1;
            m_alphabet_size++;
        }
    }
    m_alphabet_marker = sd_vector<>(alphabet_marker);
    m_alphabet_rank = rank_support(&m_alphabet_marker);
    m_alphabet_select = select_support(&m_alphabet_marker);

    m_c.resize(m_alphabet_size);
    for (size_type i = 0; i < m_alphabet_size; ++i)
    {
        size_type start = m_alphabet_select(i + 1);
        size_type end = m_alphabet_select(i + 2);
        int_vector<> tmp(end - start, 0);
        for (size_type j = start; j < end; ++j)
        {
            tmp[j - start] = v_buf[j];
        }
        m_c[i] = t_vector(tmp);
    }
}

template <class t_vector>
sdsl_psi_vector<>::size_type sdsl_psi_vector<t_vector>::serialize(std::ostream &out, structure_tree_node *v, std::string name) const
{
    structure_tree_node *child = structure_tree::add_child(v, name, util::class_name(*this));
    size_type written_bytes = 0;
    written_bytes += write_member(m_size, out, child, "size");
    written_bytes += write_member(m_alphabet_size, out, child, "size");
    written_bytes += serialize_vector(m_c,out,child,"psi");
    written_bytes += m_alphabet_marker.serialize(out, child, "alphabet marker");
    structure_tree::add_size(child, written_bytes);
    return written_bytes;
}

template <class t_vector>
void sdsl_psi_vector<t_vector>::load(std::istream &in)
{
    read_member(m_size, in);
    read_member(m_alphabet_size,in);
    m_c.resize(m_alphabet_size);
    load_vector(m_c, in);
    m_alphabet_marker.load(in);
    m_alphabet_rank = rank_support(&m_alphabet_marker);
    m_alphabet_select = select_support(&m_alphabet_marker);
}

} // end namespace sdsl
#endif
