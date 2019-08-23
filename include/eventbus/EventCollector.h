//
// Created by Dawid Drozd aka Gelldur on 18/10/16.
//

#pragma once

#include <memory>

#include "EventBus.h"

namespace Dexode
{

    class BusAttorney
    {
    public:
        BusAttorney(std::shared_ptr<EventBus> bus)
            : _bus(std::move(bus))
        {}

        /**
         * Notify all listeners for event
         *
         * @param event your event struct
         */
        template<typename Event>
        void notify(const Event& event) const
        {
            _bus->notify(event);
        }

        std::shared_ptr<EventBus> extract() const
        {
            return _bus;
        }

    private:
        std::shared_ptr<EventBus> _bus;
    };

    class EventCollector
    {
    public:
        EventCollector(const std::shared_ptr<EventBus>& bus);
        EventCollector(EventBus* bus);
        EventCollector(EventCollector const& other);
        EventCollector(EventCollector&& other);

        ~EventCollector();

        EventCollector& operator=(EventCollector const& other);
        EventCollector& operator=(EventCollector&& other);

        /**
         * Register listener for event.
         *
         * @tparam Event - type you want to listen for
         * @param callback - your callback to handle event
         */
        template<typename Event>
        void listen(const std::function<void(const Event&)>& callback)
        {
            if (!callback || !_bus)
            {
                return; // Skip such things
            }
            if (_token == 0)
            {
                _token = _bus->listen<Event>(callback);
            }
            else
            {
                _bus->listen<Event>(_token, callback);
            }
        }

        /* Be the only one to listen to this events, usefull when we have rentrant states
        */
        template<typename Event>
        void onlyListenerFor(const std::function<void(const Event&)>& callback)
        {
            if (!callback || !_bus)
            {
                return; // Skip such things
            }
            if (_token == 0)
            {
                _token = _bus->listen<Event>(callback);
            }
            else
            {
                _bus->unlisten<Event>(_token);
                _bus->listen<Event>(_token, callback);
            }
        }

        void unlistenAll();

        /**
         * @tparam Event - type you want to unlisten. @see Notiier::listen
         */
        template<typename Event>
        void unlisten()
        {
            if (_bus)
            {
                _bus->unlisten<Event>(_token);
            }
        }

        bool isUsing(const std::shared_ptr<EventBus>& bus) const;

        BusAttorney getBus() const;

    private:
        int _token = 0;
        std::shared_ptr<EventBus> _bus;
    };

} // namespace Dexode
