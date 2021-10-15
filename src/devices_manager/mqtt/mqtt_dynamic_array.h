#ifndef MQTT_DYNAMIC_ARRAY_H
#define MQTT_DYNAMIC_ARRAY_H

namespace aquabox {

namespace utils {
    template <typename T, typename SizeT = unsigned int>
    class DynamicArray {
    public:
        typedef T value_type;

        DynamicArray(SizeT capacity = 0)
        {
            if (capacity != 0) {
                m_data = new T[capacity];
                m_capacity = capacity;
            }
        }

        DynamicArray(const DynamicArray& src)
        {
            if (src.m_data != NULL) {
                m_data = new T[src.m_size];
                memcpy(m_data, src.m_data, src.m_size);
                m_size = src.m_size;
                m_capacity = m_size;
            }
        }
        ~DynamicArray()
        {
            if (m_data)
                delete[] m_data;
        }

        const T* begin() const
        {
            return m_data;
        }

        const T* end() const
        {
            return m_data + m_size;
        }

        const T* data() const
        {
            return m_data;
        }
        SizeT size() const
        {
            return m_size;
        }
        void push_back(const T& v)
        {
            if (m_size == m_capacity) {
                T* data = new T[SizeT(m_capacity * 1.5) + 1];
                ++m_capacity;
                memcpy(data, m_data, m_size);
                delete[] m_data;
                data = m_data;
            }

            m_data[m_size++] = v;
        }

    private:
        SizeT m_capacity = 0;
        SizeT m_size = 0;
        T* m_data = NULL;
    };
}
}

#endif // MQTT_DYNAMIC_ARRAY_H