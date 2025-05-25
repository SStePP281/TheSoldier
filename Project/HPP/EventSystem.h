#pragma once
#ifndef EVENT
#define EVENT

#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>

class EventSystem {
public:
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;

    template <typename T>
    using EventHandler = std::function<void(const T&)>;

    static EventSystem& getInstance()
    {
        static EventSystem instance;
        return instance;
    }

    template <typename T>
    std::shared_ptr<EventHandler<T>> subscribe(const std::string& eventName, EventHandler<T> handler)
    {
        auto event = getOrCreateEvent<T>(eventName);
        auto handlerPtr = std::make_shared<EventHandler<T>>(handler);
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        event->handlers.push_back(handlerPtr);
        return handlerPtr;
    }

    template <typename T>
    void unsubscribe(const std::string& eventName, const std::shared_ptr<EventHandler<T>>& handler)
    {
        auto event = getEvent<T>(eventName);
        if (!event) return;

        std::lock_guard<std::recursive_mutex> lock(mutex_);
        auto& handlers = event->handlers;
        handlers.erase(std::remove_if(handlers.begin(), handlers.end(),
            [&handler](const std::shared_ptr<EventHandler<T>>& h)
            {
                return h == handler;
            }),
            handlers.end());
    }

    template <typename T>
    void trigger(const std::string& eventName, const T& eventData)
    {
        auto event = getEvent<T>(eventName);
        if (!event) return;

        std::lock_guard<std::recursive_mutex> lock(mutex_);
        for (const auto& handler : event->handlers)
        {
            (*handler)(eventData);
        }
    }

private:
    EventSystem() = default;
    ~EventSystem() = default;

    template <typename T>
    struct Event
    {
        std::vector<std::shared_ptr<EventHandler<T>>> handlers;
    };

    std::unordered_map<std::string, std::shared_ptr<void>> events;
    std::recursive_mutex mutex_;

    template <typename T>
    std::shared_ptr<Event<T>> getOrCreateEvent(const std::string& eventName)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (events.find(eventName) == events.end())
        {
            events[eventName] = std::make_shared<Event<T>>();
        }
        return std::static_pointer_cast<Event<T>>(events[eventName]);
    }

    template <typename T>
    std::shared_ptr<Event<T>> getEvent(const std::string& eventName)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (events.find(eventName) == events.end())
        {
            return nullptr;
        }
        return std::static_pointer_cast<Event<T>>(events[eventName]);
    }
};

#endif // !EVENT