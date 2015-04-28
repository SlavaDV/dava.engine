#include "StringProperty.h"

StringProperty::StringProperty(const DAVA::String &propName, DAVA::UIControl *anObject, const Getter &aGetter, const Setter &aSetter, const StringProperty *sourceProperty, eCloneType cloneType)
    : ValueProperty(propName)
{
    object = SafeRetain(anObject);
    getter = aGetter;
    setter = aSetter;

    if (sourceProperty && setter != NULL && sourceProperty->GetValue() != DAVA::VariantType(getter(object)))
        setter(object, sourceProperty->GetValue().AsString());
}

StringProperty::~StringProperty()
{
    SafeRelease(object);
}

void StringProperty::Serialize(PackageSerializer *serializer) const
{

}

AbstractProperty::ePropertyType StringProperty::GetType() const
{
    return TYPE_VARIANT;
}

DAVA::VariantType StringProperty::GetValue() const
{
    if (getter != NULL)
    {
        return DAVA::VariantType(getter(object));
    }
    return DAVA::VariantType();
}

bool StringProperty::IsReadOnly() const
{
    return setter == NULL || ValueProperty::IsReadOnly();
}

void StringProperty::ApplyValue(const DAVA::VariantType &value)
{
    if (setter != NULL && value.GetType() == DAVA::VariantType::TYPE_STRING)
    {
        setter(object, value.AsString());
    }
}

