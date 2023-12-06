#include "pch.h"

const struct iRuntimeClass iObject::classiObject =
{
    "iObject",
    sizeof(iObject),
    NULL,
    &iObject::GetBaseClass
};
