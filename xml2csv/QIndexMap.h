#ifndef QINDEXMAP_H
#define QINDEXMAP_H

#include <QMap>


template <class Key>
class QIndexMap
{
public:
    QIndexMap() {}

    /**
     * @brief add
     * @param key
     *
     * Adds the key and generates the index.
     */
    void add(Key key)
    {
        if(!map.contains(key))
        {
            map.insert(key, (uint)map.size());
        }
    }

    /**
     * @brief index
     * @param key
     * @return
     *
     * Get the index of the key.
     * First, if the key does not exist it is added.
     * The index is returned.
     */
    uint index(Key key)
    {
        add(key);

        return map.value(key);
    }

    /**
     * @brief key
     * @param index
     * @return
     *
     * Get the key with the specified index (value).
     * If the key does not exist an empty Key object is returned.
     */
    Key key(uint index)
    {
        Key result;

        return map.key(index, result);
    }

    /**
     * @brief clear
     * Removes all items from the map.
     */
    void clear()
    {
        map.clear();
    }

    /**
     * @brief size
     * @return
     *
     * Returns the number of (key, index) pairs.
     */
    uint size()
    {
        return map.size();
    }

private:
    QMap<Key, uint> map;
};

#endif // QINDEXMAP_H
