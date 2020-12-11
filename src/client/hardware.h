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

#pragma once

#include <memory>
#include <vector>
namespace aquabox
{
    using Bytes = std::vector<uint8_t>;
    using BytesCount = Bytes::size_type;
    using Offset = Bytes::size_type;

    enum class State
    {
        RegistrationRequest,
        Online,
        Oflfine
    };
    
    struct IO
    {
        virtual BytesCount send(const Bytes& data, const BytesCount count = 0) = 0;
        virtual BytesCount read(Bytes& data, const Offset = 0) = 0;
    };

    struct OnOff
    {
        virtual void setOn() = 0;
        virtual void setOff() = 0;
        virtual bool isOn() = 0;
    };

    template<typename T>
    struct Value
    {
        virtual bool set(T type) = 0;
        virtual T get() = 0;
    };
    class Hardware
    {
    public:
        static std::shared_ptr<Hardware> resolve(const std::string& address);
        virtual bool open(int flags) = 0;
        virtual bool close() = 0;

        template<class IFace>
        IFace* implementation()
        {
            return dynamic_cast<IFace*>(this);
        }

        template<typename IFace>
        bool implements() const
        {
            return implementation<IFace>() != nullptr;
        }
    };

    struct OnOffDevice: public Hardware, public OnOff {};

    struct FloatValueDevice: public Hardware, public Value<double> {};
}
