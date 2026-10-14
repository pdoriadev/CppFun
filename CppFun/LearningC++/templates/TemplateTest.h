
// template class test
template <typename T>
class Number {
public:
	T number;

	Number(T newNum) {
		number = newNum;
	}	

  T getNumber();
};

// member function defined outside the class declaration
template <typename T>
T Number<T>::getNumber() {
	return number;
}
