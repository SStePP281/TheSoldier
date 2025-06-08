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

    static EventSystem& GetInstance() {
        static EventSystem instance;
        return instance;
    }

    template <typename T>
    std::shared_ptr<EventHandler<T>> Subscribe(const std::string& event_name, EventHandler<T> handler) {
        auto event = GetOrCreateEvent<T>(event_name);
        auto handlerPtr = std::make_shared<EventHandler<T>>(handler);
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        event->handlers.push_back(handlerPtr);
        return handlerPtr;
    }

    template <typename T>
    void Unsubscribe(const std::string& event_name, const std::shared_ptr<EventHandler<T>>& handler) {
        auto event = GetEvent<T>(event_name);
        if (!event) return;

        std::lock_guard<std::recursive_mutex> lock(mutex_);
        auto& handlers = event->handlers;
        handlers.erase(std::remove_if(handlers.begin(), handlers.end(),
            [&handler](const std::shared_ptr<EventHandler<T>>& h) {
                return h == handler;
            }),
            handlers.end());
    }

    template <typename T>
    void Trigger(const std::string& event_name, const T& event_data) {
        auto event = GetEvent<T>(event_name);
        if (!event) { return; }

        std::lock_guard<std::recursive_mutex> lock(mutex_);
        for (const auto& handler : event->handlers) {
            (*handler)(event_data);
        }
    }
private:
    EventSystem() = default;
    ~EventSystem() = default;

    template <typename T>
    struct Event {
        std::vector<std::shared_ptr<EventHandler<T>>> handlers;
    };

    template <typename T>
    std::shared_ptr<Event<T>> GetOrCreateEvent(const std::string& event_name) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (events_.find(event_name) == events_.end()) {
            events_[event_name] = std::make_shared<Event<T>>();
        }
        return std::static_pointer_cast<Event<T>>(events_[event_name]);
    }

    template <typename T>
    std::shared_ptr<Event<T>> GetEvent(const std::string& event_name) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (events_.find(event_name) == events_.end()) {
            return nullptr;
        }
        return std::static_pointer_cast<Event<T>>(events_[event_name]);
    }

    std::unordered_map<std::string, std::shared_ptr<void>> events_;
    std::recursive_mutex mutex_;
};

#endif // !EVENT
