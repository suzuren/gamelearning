
#ifndef __ENUM_CLASS_H_
#define __ENUM_CLASS_H_

enum class enum_class_value_t_1
{
	enum_class_value_1 = 3,
	enum_class_value_2,
	enum_class_value_3,
	enum_class_value_4,
};

enum class enum_class_value_t_2
{
	enum_class_value_1 = 17,
	enum_class_value_2,
	enum_class_value_3,
	enum_class_value_4,
};

class tets_class
{
public:
	tets_class() {};
	~tets_class() {};
	int i;
	void init()
	{
		i = 1;
	}
};

void test_enum_class()
{
	cout << "test_enum_class - t_1:" << static_cast<int>(enum_class_value_t_1::enum_class_value_1) << endl;
	cout << "test_enum_class - t_1:" << static_cast<int>(enum_class_value_t_1::enum_class_value_2) << endl;
	cout << "test_enum_class - t_1:" << static_cast<int>(enum_class_value_t_1::enum_class_value_3) << endl;
	cout << "test_enum_class - t_1:" << static_cast<int>(enum_class_value_t_1::enum_class_value_4) << endl;

	cout << "test_enum_class - t_2:" << static_cast<int>(enum_class_value_t_2::enum_class_value_1) << endl;
	cout << "test_enum_class - t_2:" << static_cast<int>(enum_class_value_t_2::enum_class_value_2) << endl;
	cout << "test_enum_class - t_2:" << static_cast<int>(enum_class_value_t_2::enum_class_value_3) << endl;
	cout << "test_enum_class - t_2:" << static_cast<int>(enum_class_value_t_2::enum_class_value_4) << endl;

	cout << "test_enum_class - t_1 - is_pod::value " << is_pod<enum_class_value_t_1>::value << endl;
	cout << "test_enum_class - t_2 - is_pod::value " << is_pod<enum_class_value_t_2>::value << endl;

	cout << "test_enum_class - tets_class - is_pod::value " << is_pod<tets_class>::value << endl;

}

#endif
