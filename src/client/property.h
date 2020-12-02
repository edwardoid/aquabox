/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROPERTY_H
#define PROPERTY_H

#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include "log.h"
#include "enumerations.h"

#define property_name(name) m_ ## name

#define property(name, type) private:                                           \
                                type property_name(name);                       \
                             public:                                            \
                                inline void set ## name (const type & value)    \
                                {                                               \
                                    property_name(name) = value;                \
                                }                                               \
                                inline const type& get ## name () const        \
                                {                                               \
                                    return property_name(name);                 \
                                }                                               \

#define copyProperty(name) this -> property_name(name) = src.property_name(name);
#define moveProperty(name) this -> property_name(name) = std::move(src.property_name(name));


namespace aquabox
{
    class Convertable
    {
    public:
        virtual std::string getDebugName() const = 0;

        virtual bool parse(const nlohmann::json& json) = 0;
        virtual void json(nlohmann::json& json) const = 0;
    };

    template<typename T>
    class AbstractProperty: public Convertable
    {
    public:

        using ValueType = T;

        AbstractProperty(const char* name_, bool required_ = true)
            : name(name_), required(required_) {}

        T Value;
        std::string name;
        bool required;

        inline bool operator == (const AbstractProperty& src) const { return Value == src.Value; }
        inline operator T () { return Value; };
        inline operator T& () { return Value; };
        inline operator const T& () const { return Value; };
        inline T* operator -> () { return &Value; }
        inline const T* operator -> () const { return &Value; }
        inline T& operator * () { return Value; }
        inline const T& operator * () const { return Value; }
        std::string getDebugName() const override { return name; }
    };

    template<typename T>
    class EnumProperty: public AbstractProperty<T>
    {
    public:
        using ValueType = typename AbstractProperty<T>::ValueType;

        EnumProperty(const char* name, bool required = true)
            : AbstractProperty<T>(name, required) {}
        
        bool parse(const nlohmann::json& json) override
        {
            auto pos = json.find(AbstractProperty<T>::name);
            if (pos == json.end()) {
                if (AbstractProperty<T>::required) { LOG_ERROR << "Can't find property \"" << AbstractProperty<T>::name << "\""; }
                else { LOG_ERROR << "Can't find property \"" << AbstractProperty<T>::name << "\""; }
                return !AbstractProperty<T>::required;
            }

            if (!pos->is_string()) { LOG_ERROR << "Property \"" << AbstractProperty<T>::name << "\" must be a string"; }

            bool ok = false;
            AbstractProperty<T>::Value = Serializable<T>::convert((std::string)(*pos), &ok);
            return ok;
        }

        void json(nlohmann::json& json) const override
        {
            json[AbstractProperty<T>::name] = Serializable<T>::convert(AbstractProperty<T>::Value);
        }
    };

    template<typename T, nlohmann::detail::value_t MappedType>
    class PrimitiveProperty: public AbstractProperty<T>
    {
    public:

        using ValueType = typename AbstractProperty<T>::ValueType;

        PrimitiveProperty(const char* name, bool required = true)
            : AbstractProperty<T>(name, required) {}

        bool parse(const nlohmann::json& json) override
        {
            auto pos = json.find(AbstractProperty<T>::name);
            if (pos == json.end()) {
                if (AbstractProperty<T>::required) { LOG_ERROR << "Can't find property \"" << AbstractProperty<T>::name << "\""; }
                else { LOG_ERROR << "Can't find property \"" << AbstractProperty<T>::name << "\""; }
            
                return !AbstractProperty<T>::required;
            }

            if (pos->type() != MappedType) {
                LOG_ERROR << "Property \"" << AbstractProperty<T>::name << "\" must NOT have type " << pos->type_name();  
                return false;
            }
            AbstractProperty<T>::Value = *pos;
            return true; 
        }

        void json(nlohmann::json& json) const override { json[AbstractProperty<T>::name] = AbstractProperty<T>::Value; }

        const PrimitiveProperty& operator = (const T& v)
        {
            AbstractProperty<T>::Value = v;
            return *this;
        }
    };

    using BooleanProperty = PrimitiveProperty<bool, nlohmann::detail::value_t::boolean>;
    using NumberProperty = PrimitiveProperty<int64_t, nlohmann::detail::value_t::number_integer>;
    using UnsignedShortNumberProperty = PrimitiveProperty<uint8_t, nlohmann::detail::value_t::number_unsigned>;
    using UnsignedNumberProperty = PrimitiveProperty<uint64_t, nlohmann::detail::value_t::number_unsigned>;
    using FloatProperty = PrimitiveProperty<double, nlohmann::detail::value_t::number_float>;
    using StringProperty = PrimitiveProperty<std::string, nlohmann::detail::value_t::string>;
    
    template<typename T>
    struct ArrayProperty: public PrimitiveProperty<std::vector<T>, nlohmann::detail::value_t::array>
    {
        ArrayProperty(const char* name, bool required = true)
            : PrimitiveProperty<std::vector<T>, nlohmann::detail::value_t::array>(name, required)
        {}
    };

    class ClassProperty: public Convertable
    {
        ClassProperty(const ClassProperty&) = delete;
        ClassProperty(ClassProperty&&) = delete;
        const ClassProperty& operator = (const ClassProperty&) = delete;
    public:

        using ValueType = ClassProperty;

        ClassProperty(const char* name_ = "", std::initializer_list<Convertable*> dependencies = {}, bool required_ = true)
            : name(name_), Dependencies(dependencies), required(required_) {}

        bool parse(const nlohmann::json& json) override
        {
            LOG_DEBUG << "Parsing " << name;
            nlohmann::json obj;
            if (!name.empty() && name[0] != '-')
            {
                auto pos = json.find(name);
                if (pos == json.end()) {
                    if (required) { LOG_ERROR << "Can't find property \"" << name << "\""; }
                    else { LOG_ERROR << "Can't find property \"" << name << "\""; }
                    return !required;
                }

                if (!pos->is_object()) { LOG_ERROR << "Property \"" << name << "\" must be an object"; }

                obj = *pos;
            }
            else
            {
                obj = json;
            }

            bool ok = true;
            for(auto& d : Dependencies)
            {
                if(!d->parse(obj)) return false;
            }
            
            return true;
        }

        void json(nlohmann::json& json) const override
        {
            nlohmann::json obj = nlohmann::json::object();
            LOG_DEBUG << "Serializing " << name;
            for(auto d : Dependencies)
            {
                LOG_ERROR_IF(d == nullptr) << "Met null dependency!";
                if(d == nullptr)
                {
                    continue;
                }
                LOG_DEBUG << "Serializing " << d->getDebugName();
                d->json(obj);
            }
            if(!name.empty() && name[0] != '-')
                json[name] = obj;
            else
                json = obj;
            LOG_DEBUG << "Serialization done!";
        }

        virtual std::string getDebugName() const { return name; }

        std::string name;
        bool required;
        std::vector<Convertable*> Dependencies;
    
    protected:
        void copyData(const ClassProperty& src)
        {
            name = src.name;
            required = src.required;
        }

        template<class ChildT>
        void moveData(ChildT&& src)
        {
            name = std::move(src.name);
            required = std::move(src.required);
        }
    };

    template<typename ClassPropertyImpl>
    class ClassArrayProperty: public AbstractProperty<std::vector<ClassPropertyImpl>>
    {
    public:
        ClassArrayProperty(const char* name, bool required = true)
            : AbstractProperty<std::vector<ClassPropertyImpl>>(name, required) {}

        bool parse(const nlohmann::json& json) override
        {
            auto pos = json.find(AbstractProperty<std::vector<ClassPropertyImpl>>::name);
            if (pos == json.end()) {
                if (AbstractProperty<std::vector<ClassPropertyImpl>>::required) {
                    LOG_ERROR << "Can't find property \"" << AbstractProperty<std::vector<ClassPropertyImpl>>::name << "\"";
                }
                else {
                    LOG_ERROR << "Can't find property \"" << AbstractProperty<std::vector<ClassPropertyImpl>>::name << "\"";
                }
                return !AbstractProperty<std::vector<ClassPropertyImpl>>::required;
            }

            if (!pos->is_array()) {
                LOG_ERROR << "Property \"" << AbstractProperty<std::vector<ClassPropertyImpl>>::name << "\" must be an object";
                return false;
            }

            auto v = pos.value();
            AbstractProperty<std::vector<ClassPropertyImpl>>::Value.clear();
            for(size_t i = 0; i < v.size(); ++i)
            {
                ClassPropertyImpl c("");
                if(!c.parse(v[i])) {
                    return false;
                }

                AbstractProperty<std::vector<ClassPropertyImpl>>::Value.emplace_back(c);
            }
            
            return true;
        }

        void json(nlohmann::json& json) const override
        {
            std::vector<nlohmann::json> res;
            for(auto& c : AbstractProperty<std::vector<ClassPropertyImpl>>::Value)
            {
                nlohmann::json o;
                c.json(o);
                res.emplace_back(o);
            }
            
            json[AbstractProperty<std::vector<ClassPropertyImpl>>::name] = res;
        }
    };
} // namespace aquabox

#endif // PROPERTY_H