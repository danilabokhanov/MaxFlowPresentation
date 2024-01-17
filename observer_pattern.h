#ifndef OBSERVER_PATTERN_H
#define OBSERVER_PATTERN_H
#include <functional>
#include <list>
#include <stdexcept>

namespace observer_pattern {
template <class DataType>
class Observer {
public:
    using SubscriberPtr = Observer<DataType>*;
    using DataPtr = const DataType*;

    template <class SubscribeFunc, class NotifyFunc, class UnsubscribeFunc>
    Observer(SubscribeFunc&& subscribe_func, NotifyFunc&& notify_func,
             UnsubscribeFunc&& unsubscribe_func)
        : subscribe_func_(std::forward<SubscribeFunc>(subscribe_func)),
        notify_func_(std::forward<NotifyFunc>(notify_func)),
        unsubscribe_func_(std::forward<UnsubscribeFunc>(unsubscribe_func)) {
    }

    template <class UnifiedFunc>
    Observer(UnifiedFunc&& unified_func)
        : subscribe_func_(std::forward<UnifiedFunc>(unified_func)),
          notify_func_(std::forward<UnifiedFunc>(unified_func)),
          unsubscribe_func_(std::forward<UnifiedFunc>(unified_func)) {
    }

    Observer() = default;
    Observer(const Observer&) = delete;
    Observer(Observer&&) = delete;
    Observer& operator=(const Observer&) = delete;
    Observer& operator=(Observer&&) = delete;

    static void DefaultOnCallFunc(DataPtr) {
    }

    void Unsubscribe() {
        if (!IsConnected()) {
            return;
        }
        OnUnsubscribe((*connection_.data_producer_ptr)());
        connection_.subscribers_ptr->remove(this);
        connection_ = {nullptr, nullptr};
    }

    void OnSubscribe(DataPtr data) {
        subscribe_func_(data);
    }

    void OnNotify(DataPtr data) {
        notify_func_(data);
    }

    void OnUnsubscribe(DataPtr data) {
        unsubscribe_func_(data);
    }

    bool IsConnected() {
        return connection_.subscribers_ptr && connection_.data_producer_ptr;
    };

    ~Observer() {
        Unsubscribe();
    }

private:
    template <class W>
    friend class Subject;

    struct Connection {
        std::function<DataPtr()>* data_producer_ptr;
        std::list<SubscriberPtr>* subscribers_ptr;
    };

    std::function<void(DataPtr)> subscribe_func_ = DefaultOnCallFunc,
        notify_func_ = DefaultOnCallFunc,
        unsubscribe_func_ = DefaultOnCallFunc;
    Connection connection_ = {nullptr, nullptr};
};

template <class DataType>
class Subject {
public:
    template <class DataProducerType>
    Subject(DataProducerType&& data_producer)
        : data_producer_(std::forward<DataProducerType>(data_producer)) {
    }

    Subject() = default;
    Subject(const Subject&) = delete;
    Subject(Subject&&) = delete;
    Subject& operator=(const Subject&) = delete;
    Subject& operator=(Subject&&) = delete;

    void Subscribe(typename Observer<DataType>::SubscriberPtr consumer) {
        if (!consumer) {
            throw std::runtime_error("null pointer to subscriber");
        }
        if (!data_producer_) {
            throw std::runtime_error("empty data producer");
        }
        data_producer_();

        subscribers_.push_back(consumer);
        consumer->connection_ = {.data_producer_ptr = &data_producer_,
                                 .subscribers_ptr = &subscribers_};
        consumer->OnSubscribe(data_producer_());
    }

    void Notify() {
        for (auto& ptr : subscribers_) {
            ptr->OnNotify(data_producer_());
        }
    }

    ~Subject() {
        for (size_t i = subscribers_.size(); i != 0; i--) {
            subscribers_.back()->Unsubscribe();
        }
    }

    const DataType* GetData() {
        if (!data_producer_) {
            throw std::runtime_error("empty data producer");
        }
        return data_producer_();
    }

private:
    std::function<typename Observer<DataType>::DataPtr()> data_producer_;
    std::list<typename Observer<DataType>::SubscriberPtr> subscribers_;
};
}
#endif // OBSERVER_PATTERN_H
