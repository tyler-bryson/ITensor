//
// Distributed under the ITensor Library License, Version 1.2
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITENSOR_H
#define __ITENSOR_ITENSOR_H
#include "indexset.h"
#include "itdata/itdata.h"

namespace itensor {

//
// ITensor
//
class ITensor
    {
    public:

    using IndexT = Index;
    using IndexValT = IndexVal;
    using storage_ptr = PData;

    //
    // Constructors
    //

    //Construct Null ITensor, ITensor will evaluate to false in boolean context
    ITensor() { }

    //Construct rank 1 ITensor, all elements set to zero
    explicit
    ITensor(const Index& i1);

    //Construct rank 2 ITensor, all elements set to zero
    ITensor(const Index& i1,
            const Index& i2);

    //Construct rank n ITensor, all elements set to zero
    template <typename... Indices>
    ITensor(const Index& i1, 
            const Index& i2, 
            const Index& i3, 
            const Indices&... rest);

    //Construct rank 0 ITensor (scalar), value set to val
    //If val.imag()==0, only Real storage will be used
    explicit
    ITensor(Complex val);

    //Construct ITensor with diagonal elements set to z
    //(z can be Real argument too; convertible to Complex)
    template<typename... Inds>
    ITensor(Complex z, 
            const Index& i1,
            const Inds&... inds);

    //Construct diagonal ITensor,
    //diagonal elements given by VectorRef V
    template<typename... Inds>
    ITensor(const VectorRef& V,
            const Index& i1,
            const Inds&... inds);

    //Construct rank n ITensor, all
    //elements set to zero except the single
    //entry specified by the IndexVal args
    template <typename... IVals>
    explicit
    ITensor(const IndexVal& iv1, 
            const IVals&... rest);

    //
    // Accessor Methods
    //

    //Rank of this ITensor (number of indices)
    int 
    r() const { return is_.r(); }

    //Access index set
    const IndexSet&
    inds() const { return is_; }

    //evaluate to false if ITensor is default constructed
    explicit operator bool() const { return bool(store_); }

    template <typename... IndexVals>
    Real
    real(IndexVals&&... ivs) const;

    template <typename... IndexVals>
    Complex
    cplx(IndexVals&&... ivs) const;

    //Set element at location given by collection
    //of IndexVals. Will not switch storage
    //from Real to Complex unless val.imag()!=0 
    template<typename... IndexVals>
    void
    set(Complex val, const IndexVals&... ivs);

    //
    // Operators
    //

    //Contracting product
    //All matching Index pairs automatically contracted
    //Cji = \sum_{k,l} Akjl * Blki
    ITensor& 
    operator*=(const ITensor& other);

    // Contract with IndexVal
    // If iv = (J,n), Index J is fixed to it's nth
    // value and rank decreases by 1
    // (similar to summing against a Kronecker
    // delta tensor \delta_{J,n})
    ITensor& 
    operator*=(const IndexVal& iv) { return operator*=(ITensor(iv)); } 

    //Multiplication and division by scalar
    ITensor& 
    operator*=(Real fac);

    ITensor& 
    operator/=(Real fac);

    ITensor& 
    operator*=(Complex z);

    ITensor& 
    operator/=(Complex z);

    ITensor
    operator-() const;


    //Tensor addition and subtraction
    //Summands must have same Indices, in any order
    //Cijk = Aijk + Bkij
    ITensor& 
    operator+=(const ITensor& other);

    ITensor& 
    operator-=(const ITensor& other);

    //
    // Index Prime Level Methods
    //

    //Set primeLevel of Indices to zero
    ITensor& 
    noprime(IndexType type = All);

    //Set primeLevel of Index I to zero
    ITensor& 
    noprime(const Index& I);

    //Increase primeLevel of Indices by 1 (or optional amount inc)
    ITensor& 
    prime(int inc = 1);

    //Increase primeLevel of Indices by 1 (or optional amount inc)
    ITensor& 
    prime(IndexType type, int inc = 1);

    //Increase primeLevel of Index I by 1 (or optional amount inc)
    ITensor& 
    prime(const Index& I, int inc = 1);

    //Change all Indices having primeLevel plevold to have primeLevel plevnew
    ITensor& 
    mapprime(int plevold, int plevnew, IndexType type = All);

    //
    // Element Transformation Methods
    //

    //Set all elements to z. If z.imag()==0
    //(such as if z is automatically converted from a Real)
    //then storage will be real only.
    ITensor&
    fill(Complex z);

    //Call a function of the form f()->val once
    //for each element, assign result to each element.
    template <typename Func>
    ITensor&
    generate(Func&& f);

    //Apply a function of the form f(x)->y
    //to each element x, replacing it with y
    template <typename Func>
    ITensor&
    apply(Func&& f);

    //Apply a function of the form f(x)->void
    //to each element x.
    template <typename Func>
    const ITensor&
    visit(Func&& f) const;

    //
    // Complex number methods
    //

    ITensor&
    conj();

    private:

    void
    scaleOutNorm();

    void
    equalizeScales(ITensor& other);

    public:

    //
    // Developer / advanced methods
    //
    // The following methods should not
    // be needed for most user code.
    //

    //Construct by explicitly providing data members
    ITensor(IndexSet&& iset,
            NewData nd,
            LogNumber scale);

    //Provide indices from IndexSet
    explicit
    ITensor(const IndexSet& is);

    //Provide indices from an index set
    //and elements from a VectorRef
    ITensor(const IndexSet& is,
            const VectorRef& v);

    ITensor(const IndexSet& is,
            const ITensor& t);

    //Scale factor, used internally for efficient scalar ops.
    //Mostly for developer use; not necessary to explicitly involve
    //scale factors in user-level ITensor operations.
    const LogNumber&
    scale() const { return scale_; }

    const ITData&
    data() const { return *store_; }

    void 
    scaleTo(const LogNumber& newscale);

    //
    // Deprecated methods
    //

    //Construct matrix-like rank 2 ITensor,
    //elements given by MatrixRef M
    ITensor(const Index& i1,
            const Index& i2,
            const MatrixRef& M);

    template <typename Callable> 
    ITensor&
    mapElems(Callable&& f)
        {
        return apply(std::forward<Callable>(f));
        }

    const IndexSet&
    indices() const { return inds(); }

    private:
    ////////////////////////
    IndexSet is_;
    LogNumber scale_;
    storage_ptr store_;
    ////////////////////////
    }; // class ITensor

std::ostream& 
operator<<(std::ostream & s, const ITensor& T);

ITensor inline
operator*(ITensor A, const ITensor& B) { A *= B; return A; }
ITensor inline
operator*(ITensor T, Real fac) { T *= fac; return T; }
ITensor inline
operator*(Real fac, ITensor T) { T *= fac; return T; }
ITensor inline
operator*(ITensor T, Complex fac) { T *= fac; return T; }
ITensor inline
operator*(Complex fac, ITensor T) { T *= fac; return T; }
ITensor inline
operator/(ITensor T, Real fac) { T /= fac; return T; }
ITensor inline
operator/(ITensor T, Complex fac) { T /= fac; return T; }
ITensor inline
operator+(ITensor A, const ITensor& B) { A += B; return A; }
ITensor inline
operator-(ITensor A, const ITensor& B) { A -= B; return A; }

ITensor inline
operator*(ITensor T, const IndexVal& iv) { T *= iv; return T; }
ITensor inline
operator*(const IndexVal& iv, const ITensor& t) { return (ITensor(iv) *= t); }

ITensor
combiner(std::vector<Index> inds);

template<typename... Inds>
ITensor
combiner(const Index& i1, const Inds&... inds)
    {
    return combiner(std::vector<Index>{i1,inds...});
    }

ITensor
delta(const Index& i1, const Index& i2);

//
// Define product of IndexVal iv1 = (I1,n1), iv2 = (I2,n2)
// (I1, I2 are Index objects; n1,n2 are type int)
// to be an ITensor T such that T(I1(n1),I2(n2)) == 1
//
// Useful for creating MPOs
//
ITensor inline
operator*(const IndexVal& iv1, const IndexVal& iv2) 
    { 
    ITensor t(iv1); 
    return (t *= iv2); 
    }
//
// Define product of IndexVal iv1 = (I1,n1) with a Real "val"
// to be an ITensor T such that T(I1(n1)) == val
//
// Useful for creating MPOs
//
ITensor inline
operator*(const IndexVal& iv1, Real val) 
    { 
    ITensor res(iv1); 
    res *= val; 
    return res; 
    }
ITensor inline
operator*(Real val, const IndexVal& iv) { return operator*(iv,val); }

//Return copy of ITensor with primeLevel of Index I increased by 1
//(or optional amount inc)
template <class Tensor, class IndexT>
Tensor
prime(Tensor A, const IndexT& I, int inc = 1)
    { 
    A.prime(I,inc); 
    return A; 
    }

//Return copy of ITensor with primeLevel of Index I set to zero
template <class Tensor, class IndexT>
Tensor
noprime(Tensor A, const IndexT& I)
    { 
    A.noprime(I); 
    return A; 
    }

template<class Tensor>
bool
hasindex(const Tensor& T, const typename Tensor::IndexT& I)
    {
    return detail::contains(T.inds(),I);
    }

ITensor
randomize(ITensor T, const Args& args = Global::args());

template <typename... Params>
ITensor
randIT(Params&&... params)
    {
    return randomize(ITensor(std::forward<Params>(params)...));
    }

template <typename... Indices>
ITensor
tieIndex(const ITensor& T,
         const Index& t0,
         const Index& t1,
         const Indices&... rest);

//Compute the norm of an ITensor.
//Thinking of elements as a vector, equivalent to sqrt(v*v).
//Result is equivalent to sqrt((T*T).real()) 
//[and similar for complex case] but computed much more efficiently.
Real 
norm(const ITensor& T);

ITensor
conj(ITensor T);

ITensor inline
dag(const ITensor& T) { return conj(T); }

bool
isComplex(const ITensor& T);

Real
sumels(const ITensor& T);

//
//Return copy of a tensor with primeLevels plev1 and plev2 swapped
//
//For example, if T has indices i,i' (like a matrix or a site
//operator) then swapPrime(T,0,1) will have indices i',i 
//i.e. the transpose of T.
//
template <class Tensor>
Tensor
swapPrime(Tensor T, int plev1, int plev2,
          IndexType type = All);

//Find index of tensor A (of optional type t) 
//which is shared with tensor B
template<class TensorA, class TensorB> typename 
TensorA::IndexT
commonIndex(const TensorA& A, const TensorB& B, IndexType t = All);

//Find index of tensor A (of optional type t) 
//which is NOT shared by tensor B
template<class TensorA, class TensorB> typename 
TensorA::IndexT
uniqueIndex(const TensorA& A, 
            const TensorB& B, 
            IndexType t);

}; //namespace itensor

//See file itensor.ih for template/inline method implementations
#include "itensor.ih"


#endif
