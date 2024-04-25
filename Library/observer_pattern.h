#ifndef OBSERVER_PATTERN_H
#define OBSERVER_PATTERN_H
#include <functional>
#include <list>
#include <cassert>

namespace observer_pattern {
template <class DataType>
class Observable;

template <class DataType>
class Observer {
public:
    using Connection = Observable<DataType>*;
    template <class SubscribeFunc, class NotifyFunc, class UnsubscribeFunc>
    Observer(SubscribeFunc&& subscribe_func, NotifyFunc&& notify_func,
             UnsubscribeFunc&& unsubscribe_func)
        : subscribe_func_(std::forward<SubscribeFunc>(subscribe_func)),
          notify_func_(std::forward<NotifyFunc>(notify_func)),
          unsubscribe_func_(std::forward<UnsubscribeFunc>(unsubscribe_func)) {
    }

    template <class UnifiedFunc>
    Observer(UnifiedFunc&& unified_func)
        : subscribe_func_(unified_func),
          notify_func_(unified_func),
          unsubscribe_func_(std::forward<UnifiedFunc>(unified_func)) {
    }

    Observer() = default;
    Observer(const Observer&) = delete;
    Observer(Observer&&) = delete;
    Observer& operator=(const Observer&) = delete;
    Observer& operator=(Observer&&) = delete;

    void Unsubscribe() {
        if (!IsConnected()) {
            return;
        }
        connection_->Disconnect(this);
        connection_ = nullptr;
    }

    void OnSubscribe(const DataType& data) const {
        subscribe_func_(data);
    }

    void OnNotify(const DataType& data) const {
        notify_func_(data);
    }

    void OnUnsubscribe(const DataType& data) const {
        unsubscribe_func_(data);
    }

    bool IsConnected() const {
        return connection_;
    };

    ~Observer() {
        Unsubscribe();
    }

private:
    friend class Observable<DataType>;

    static void DefaultOnCallFunc(const DataType&) {
    }

    std::function<void(const DataType&)> subscribe_func_ = DefaultOnCallFunc;
    std::function<void(const DataType&)> notify_func_ = DefaultOnCallFunc;
    std::function<void(const DataType&)> unsubscribe_func_ = DefaultOnCallFunc;
    Connection connection_ = nullptr;
};

template <class DataType>
class Observable {
public:
    using SubscriberPtr = Observer<DataType>*;

    template <class DataProducerType>
    Observable(DataProducerType&& data_producer)
        : data_producer_(std::forward<DataProducerType>(data_producer)) {
    }

    Observable() = default;
    Observable(const Observable&) = delete;
    Observable(Observable&&) = delete;
    Observable& operator=(const Observable&) = delete;
    Observable& operator=(Observable&&) = delete;

    bool Subscribe(SubscriberPtr consumer) {
        if (!consumer) {
            return false;
        }
        for (auto subscriber_ptr : subscribers_) {
            if (subscriber_ptr == consumer) {
                return false;
            }
        }
        subscribers_.push_back(consumer);
        consumer->connection_ = this;
        consumer->OnSubscribe(std::move(data_producer_()));
        return true;
    }

    void Notify() {
        DataType data = data_producer_();
        for (auto ptr : subscribers_) {
            ptr->OnNotify(data);
        }
    }

    ~Observable() {
        for (size_t i = subscribers_.size(); i != 0; i--) {
            subscribers_.back()->Unsubscribe();
        }
    }

    const DataType& GetData() {
        return data_producer_();
    }

private:
    friend class Observer<DataType>;

    static DataType DefaultDataProducer() {
        return {};
    }

    void Disconnect(SubscriberPtr ptr) {
        assert(ptr);
        ptr->OnUnsubscribe(data_producer_());
        subscribers_.remove(ptr);
    }

    std::function<DataType()> data_producer_ = DefaultDataProducer;
    std::list<SubscriberPtr> subscribers_;
};

template <>
class Observable<void>;

template <>
class Observer<void> {
public:
    using Connection = Observable<void>*;
    template <class SubscribeFunc, class NotifyFunc, class UnsubscribeFunc>
    Observer(SubscribeFunc&& subscribe_func, NotifyFunc&& notify_func,
             UnsubscribeFunc&& unsubscribe_func)
        : subscribe_func_(std::forward<SubscribeFunc>(subscribe_func)),
          notify_func_(std::forward<NotifyFunc>(notify_func)),
          unsubscribe_func_(std::forward<UnsubscribeFunc>(unsubscribe_func)) {
    }

    template <class UnifiedFunc>
    Observer(UnifiedFunc&& unified_func)
        : subscribe_func_(unified_func),
          notify_func_(unified_func),
          unsubscribe_func_(std::forward<UnifiedFunc>(unified_func)) {
    }

    Observer() = default;
    Observer(const Observer&) = delete;
    Observer(Observer&&) = delete;
    Observer& operator=(const Observer&) = delete;
    Observer& operator=(Observer&&) = delete;

    void Unsubscribe();

    void OnSubscribe() const {
        subscribe_func_();
    }

    void OnNotify() const {
        notify_func_();
    }

    void OnUnsubscribe() const {
        unsubscribe_func_();
    }

    bool IsConnected() const {
        return connection_;
    };

    ~Observer() {
        Unsubscribe();
    }

private:
    friend class Observable<void>;

    static void DefaultOnCallFunc() {
    }

    std::function<void()> subscribe_func_ = DefaultOnCallFunc;
    std::function<void()> notify_func_ = DefaultOnCallFunc;
    std::function<void()> unsubscribe_func_ = DefaultOnCallFunc;
    Connection connection_ = nullptr;
};

template <>
class Observable<void> {
public:
    using SubscriberPtr = Observer<void>*;

    Observable() = default;
    Observable(const Observable&) = delete;
    Observable(Observable&&) = delete;
    Observable& operator=(const Observable&) = delete;
    Observable& operator=(Observable&&) = delete;

    bool Subscribe(SubscriberPtr consumer) {
        if (!consumer) {
            return false;
        }
        for (auto subscriber_ptr : subscribers_) {
            if (subscriber_ptr == consumer) {
                return false;
            }
        }
        subscribers_.push_back(consumer);
        consumer->connection_ = this;
        consumer->OnSubscribe();
        return true;
    }

    void Notify() {
        for (auto ptr : subscribers_) {
            ptr->OnNotify();
        }
    }

    ~Observable() {
        for (size_t i = subscribers_.size(); i != 0; i--) {
            subscribers_.back()->Unsubscribe();
        }
    }

private:
    friend class Observer<void>;

    static void DefaultDataProducer() {
    }

    void Disconnect(SubscriberPtr ptr) {
        assert(ptr);
        ptr->OnUnsubscribe();
        subscribers_.remove(ptr);
    }

    std::list<SubscriberPtr> subscribers_;
};

inline void Observer<void>::Unsubscribe() {
    if (!IsConnected()) {
        return;
    }
    connection_->Disconnect(this);
    connection_ = nullptr;
}
}  // namespace observer_pattern
#endif  // OBSERVER_PATTERN_H
