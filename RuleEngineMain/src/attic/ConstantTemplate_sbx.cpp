#ifndef RULEENGINE_CONSTANTTEMPLATE_CPP_
#define RULEENGINE_CONSTANTTEMPLATE_CPP_

namespace sbx {

template <typename T>
class ConstantTemplate {
protected:
	T _value;
public:
	ConstantTemplate();
	ConstantTemplate(T);
	virtual ~ConstantTemplate() {}
	virtual T value() {
		return _value;
	}
};

template <typename T>
ConstantTemplate<T>::ConstantTemplate() {}

template <typename T>
ConstantTemplate<T>::ConstantTemplate(T _value) : _value( T(_value) ) {}


//virtual T Constant<T>::value() {
//	return _value;
//}



} // namespace sbx
#endif /* RULEENGINE_CONSTANTTEMPLATE_CPP_ */
