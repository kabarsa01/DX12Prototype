#include "core/Class.h"
#include "core/ObjectBase.h"

std::map<size_t, std::shared_ptr<Class>> Class::classes { };

const Class & Class::Get(ObjectBase * InObject)
{
	return * GetClass(HashString{ typeid(*InObject).name() });
}

const HashString & Class::GetName() const
{
	return name;
}

bool Class::operator==(const Class & Other) const
{
	return this->name == Other.name;
}

bool Class::operator!=(const Class & Other) const
{
	return this->name != Other.name;
}

bool Class::operator<(const Class & Other) const
{
	return this->name < Other.name;
}

bool Class::operator>(const Class & Other) const
{
	return this->name > Other.name;
}

bool Class::operator<=(const Class & Other) const
{
	return this->name <= Other.name;
}

bool Class::operator>=(const Class & Other) const
{
	return this->name >= Other.name;
}

Class::Class()
	: name( HashString::NONE )
{
}

Class::Class(const std::string & InName)
	: name( InName )
{
}

Class::Class(const HashString & InName)
	: name( InName )
{
}

Class::Class(const Class & InClass)
	: name( InClass.name )
{
}

Class::~Class()
{
}

Class & Class::operator=(const Class & other)
{
	return *this;
}

const Class * Class::operator&() const
{
	return this;
}

std::shared_ptr<Class> Class::GetClass(const HashString & InName)
{
	if (classes.find(InName.GetHash()) == classes.end())
	{
		classes.insert(std::pair<size_t, std::shared_ptr<Class>>(InName.GetHash(), std::make_shared<Class>(InName)));
	}

	return classes.at(InName.GetHash());
}

std::shared_ptr<Class> Class::GetClass(ObjectBase * InObject)
{
	return GetClass(HashString{ typeid(*InObject).name() });
}
