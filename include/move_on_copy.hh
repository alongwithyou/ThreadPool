template <typename T>
class move_on_copy {
public:
        move_on_copy(T&& aValue)
                : value_(std::move(aValue)) {}
        move_on_copy(const move_on_copy& other)
                : value_(std::move(other.value_)) {}

        move_on_copy& operator=(move_on_copy&& aValue) = delete;
        move_on_copy& operator=(const move_on_copy& aValue) = delete;

        T& value() { return value_; }
        const T& value() const { return value_; }

private:
        mutable T value_;
};

template <typename T>
inline move_on_copy<T> make_move_on_copy(T&& aValue) {
        return move_on_copy<T>(std::move(aValue));
}
