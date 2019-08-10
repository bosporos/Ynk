//
// file ynk/iterator/Iterator.h
// author Maximilien M. Cura
//

#ifndef __YNK_ITERATOR_ITERATOR
#define __YNK_ITERATOR_ITERATOR

namespcae Ynk::Iterator
{
    template <class T>
    struct Iterator
    {
        virtual Iterator & operator++ ()  = 0;
        virtual Iterator operator++ (int) = 0;
    };

    template <class T>
    struct InputIterator : public Iterator<T>
    {
        virtual bool operator== (Iterator<T> const &) = 0;
        virtual bool operator== (Iterator<T> rhs)
        {
            return this.operator== (rhs);
        }

        virtual T operator* ()    = 0;
        virtual T * operator-> () = 0;
    };

    template <class T>
    struct OutputIterator : public Iterator<T>
    {
        virtual T & operator* () = 0;
    };

    template <class T>
    struct ForwardIterator
        : public InputIterator<T>,
          public OutputIterator<T>
    {
        override virtual T & operator* () = 0;
        virtual T operator* const ()      = 0;
    };

    template <class T>
    struct BidirectionalIterator : public ForwardIterator<T>
    {
        virtual Iterator<T> & operator-- ()  = 0;
        virtual Iterator<T> operator-- (int) = 0;
    };

    template <class T>
    struct RandomAccessIterator : public BidirectionalIterator<T>
    {
        virtual RandomAccessIterator<T> && operator+ (isize) = 0;
        virtual RandomAccessIterator<T> && operator- (isize) = 0;
        virtual usize operator- (Iterator<T> const &)        = 0;
        virtual usize operator- (Iterator<T> rhs)
        {
            return this.operator- (rhs);
        }

        virtual bool operator< (Iterator<T> const &) = 0;
        virtual bool operator< (Iterator<T> rhs)
        {
            return this.operator< (rhs);
        }

        virtual bool operator> (Iterator<T> const &) = 0;
        virtual bool operator> (Iterator<T> rhs)
        {
            return this.operator> (rhs);
        }

        virtual Iterator<T> & operator+= (isize) = 0;
        virtual Iterator<T> & operator-= (isize) = 0;

        virtual Iterator<T> & operator-= (Iterator<T> const &) = 0;
        virtual Iterator<T> & operator-= (Iterator<T> rhs)
        {
            return this.operator-= (rhs);
        }

        virtual T operator[] (usize index) = 0;
    };
}

#endif /* !@__YNK_ITERATOR_ITERATOR */
