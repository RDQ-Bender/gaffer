//////////////////////////////////////////////////////////////////////////
//  
//  Copyright (c) 2011-2012, John Haddon. All rights reserved.
//  Copyright (c) 2011, Image Engine Design Inc. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//  
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//  
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  
//////////////////////////////////////////////////////////////////////////

#include "Gaffer/TypedObjectPlug.h"
#include "Gaffer/Action.h"

#include "OpenEXR/ImathFun.h"

#include "boost/bind.hpp"

using namespace Gaffer;

template<class T>
const IECore::RunTimeTyped::TypeDescription<TypedObjectPlug<T> > TypedObjectPlug<T>::g_typeDescription;

template<class T>
TypedObjectPlug<T>::TypedObjectPlug(
	const std::string &name,
	Direction direction,
	ConstValuePtr defaultValue,
	unsigned flags
)
	:	ValuePlug( name, direction, flags ),
		m_defaultValue( defaultValue ? defaultValue->copy() : 0 )
{
	typedStorage() = defaultValue ? defaultValue->copy() : 0;
}

template<class T>
TypedObjectPlug<T>::~TypedObjectPlug()
{
}

template<class T>
bool TypedObjectPlug<T>::acceptsInput( ConstPlugPtr input ) const
{
	if( !ValuePlug::acceptsInput( input ) )
	{
		return false;
	}
	if( input )
	{
		return input->isInstanceOf( staticTypeId() );
	}
	return true;
}

template<class T>
typename TypedObjectPlug<T>::ConstValuePtr TypedObjectPlug<T>::defaultValue() const
{
	return m_defaultValue;
}
		
template<class T>
void TypedObjectPlug<T>::setValue( ConstValuePtr value )
{
	// the other plug types only actually do anything if the new value is different than the old.
	// we don't do that here as it's assumed that the cost of testing large objects for equality
	// is greater than the cost of the additional callbacks invoked if the value is set to the same
	// again.
	Action::enact(
		this,
		boost::bind( &TypedObjectPlug<T>::setValueInternal, Ptr( this ), value ),
		boost::bind( &TypedObjectPlug<T>::setValueInternal, Ptr( this ), m_value )		
	);
}

template<class T>
void TypedObjectPlug<T>::setValueInternal( ConstValuePtr value )
{
	typedStorage() = value ? value->copy() : 0;
	valueSet();
}

template<class T>
typename TypedObjectPlug<T>::ConstValuePtr TypedObjectPlug<T>::getValue()
{
	return typedStorage( true );
}

template<class T>
void TypedObjectPlug<T>::setFromInput()
{
	setValue( getInput<TypedObjectPlug<T> >()->getValue() );
}

template<class T>
typename TypedObjectPlug<T>::ValuePtr &TypedObjectPlug<T>::typedStorage( bool update )
{
	return reinterpret_cast<ValuePtr &>( storage( update ) );
}

namespace Gaffer
{

IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( ObjectPlug, ObjectPlugTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( BoolVectorDataPlug, BoolVectorDataPlugTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( IntVectorDataPlug, IntVectorDataPlugTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( FloatVectorDataPlug, FloatVectorDataPlugTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( StringVectorDataPlug, StringVectorDataPlugTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( V3fVectorDataPlug, V3fVectorDataPlugTypeId )

}

// explicit instantiation
template class TypedObjectPlug<IECore::Object>;
template class TypedObjectPlug<IECore::BoolVectorData>;
template class TypedObjectPlug<IECore::IntVectorData>;
template class TypedObjectPlug<IECore::FloatVectorData>;
template class TypedObjectPlug<IECore::StringVectorData>;
template class TypedObjectPlug<IECore::V3fVectorData>;
