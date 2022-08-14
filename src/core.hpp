#include <memory>

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename... Args>
Scope<T> makeScope(Args &&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename... Args>
Ref<T> makeRef(Args &&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Weak = std::weak_ptr<T>;
