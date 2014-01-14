#ifndef QTUPLE_H
#define QTUPLE_H

template <class Key>
class QTuple
{
public:
    /**
     * @brief QTuple
     *
     * Create empty tuple.
     */
    QTuple()
    {}

    /**
     * @brief QTuple
     * @param first
     * @param second
     *
     * Create tuple and initialise with first and second argument
     */
    QTuple(Key first, Key second)
    {
        mFirst = first;
        mSecond = second;
    }

    /**
     * @brief setFirst
     * @param first
     *
     * Set the first item
     */
    void setFirst(Key first)
    {
        mFirst = first;
    }

    /**
     * @brief setSecond
     * @param second
     *
     * Set the second item
     */
    void setSecond(Key second)
    {
        mSecond = second;
    }

    /**
     * @brief first
     * @return
     *
     * Get the first item
     */
    Key first()
    {
        return mFirst;
    }

    /**
     * @brief second
     * @return
     *
     * Get the second item
     */
    Key second()
    {
        return mSecond;
    }

    /**
     * @brief operator ==
     * @param t1
     * @param t2
     * @return if the tuples' data is equal
     */
    friend bool operator== (QTuple &t1, QTuple &t2)
    {
        return ((t1.first() == t2.first()) && (t1.second() == t2.second()));
    }

    /**
     * @brief operator !=
     * @param t1
     * @param t2
     * @return if the tuples' data is unequal
     */
    friend bool operator!= (QTuple &t1, QTuple &t2)
    {
        return ((t1.first() != t2.first()) || (t1.second() != t2.second()));
    }

private:
    Key mFirst;
    Key mSecond;
};

#endif // QTUPLE_H
