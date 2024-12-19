#include "ui_precompiled.h"
#include "kernel/ui_common.h"
#include "kernel/ui_main.h"

#include "as/asui.h"
#include "as/asui_local.h"
#include "as/asui_url.h"

#include <RmlUi/Core/Elements/ElementTabSet.h>
#include <RmlUi/Core/Elements/ElementForm.h>
#include "widgets/ui_image.h"


namespace ASUI {

// dummy class since ASBIND only can only bind unique classes 
// and AngelScript arrays are more like composite classes
class ASElementsArray : public CScriptArrayInterface {};
static asITypeInfo *elementsArrayType;

class ASStringsArray : public CScriptArrayInterface {};
static asITypeInfo *stringsArrayType;

//typedef Rml::Element Element;
//typedef Rml::ElementForm ElementForm;
//typedef Rml::ElementFormControl ElementFormControl;
//typedef Rml::ElementFormControlDataSelect ElementFormControlDataSelect;
//
////typedef ElementDataGrid ElementDataGrid;
////typedef ElementDataGridRow ElementDataGridRow;
//
//typedef Rml::ElementTabSet ElementTabSet;

//typedef WSWUI::ElementImage ElementImage;

}

//==========================================================

ASBIND_TYPE( Rml::ElementForm, ElementForm )
ASBIND_TYPE( Rml::ElementFormControl, ElementFormControl )
//ASBIND_TYPE( Rml::ElementFormControlDataSelect, ElementFormControlDataSelect )

//ASBIND_TYPE( Rml::ElementDataGrid, ElementDataGrid )
//ASBIND_TYPE( Rml::ElementDataGridRow, ElementDataGridRow )

ASBIND_TYPE( Rml::ElementTabSet, ElementTabSet )

ASBIND_TYPE( WSWUI::ElementImage, ElementImage )

// array of Element handlers
ASBIND_ARRAY_TYPE( ASUI::ASElementsArray, Element @ )
ASBIND_ARRAY_TYPE( ASUI::ASStringsArray, String @ )

//==============================================================

namespace ASUI {

//
// EVENT

void PrebindEvent( ASInterface *as ) {
	ASBind::Class<Rml::Event, ASBind::class_nocount>( as->getEngine() );
}

static Rml::Element *Event_GetTargetElement( Rml::Event *self ) {
	Rml::Element *e = self->GetTargetElement();
	return e;
}

// String -> asstring_t*
static asstring_t *Event_GetType( Rml::Event *self ) {
	return ASSTR( self->GetType() );
}

static asstring_t *Event_GetParameterS( Rml::Event *self, const asstring_t &a, const asstring_t &b ) {
	Rml::String name = ASSTR( a );
	Rml::String default_value = ASSTR( b );
	return ASSTR( self->GetParameter( name, default_value ) );
}

static int Event_GetParameterI( Rml::Event *self, const asstring_t &a, const int default_value ) {
	Rml::String name = ASSTR( a );
	return self->GetParameter( name, default_value );
}

static unsigned Event_GetParameterU( Rml::Event *self, const asstring_t &a, const unsigned default_value ) {
	Rml::String name = ASSTR( a );
	return self->GetParameter( name, default_value );
}

static float Event_GetParameterF( Rml::Event *self, const asstring_t &a, const float default_value ) {
	Rml::String name = ASSTR( a );
	return self->GetParameter( name, default_value );
}

static bool Event_GetParameterB( Rml::Event *self, const asstring_t &a, const bool default_value ) {
	Rml::String name = ASSTR( a );
	return self->GetParameter( name, default_value );
}

static CScriptDictionaryInterface *Event_GetParameters( Rml::Event *self ) {
	CScriptDictionaryInterface *dict = UI_Main::Get()->getAS()->createDictionary();
	int stringObjectTypeId = UI_Main::Get()->getAS()->getStringObjectType()->GetTypeId();

	const Rml::Dictionary &parameters = self->GetParameters();

	Rml::String name;
	Rml::String value;
	for ( Rml::Dictionary::const_iterator it = parameters.begin(); it != parameters.end(); ++it ) {
		const std::string &val = it->second.Get<std::string>();
		dict->Set( *( ASSTR( it->first ) ), ASSTR( val ), stringObjectTypeId );
	}

	return dict;
}

static void Event_StopPropagation( Rml::Event *self ) {
	self->StopPropagation();
}

static int Event_GetPhase( Rml::Event *self ) {
	return int(self->GetPhase());
}

void BindEvent( ASInterface *as ) {
	ASBind::Enum( as->getEngine(), "eEventPhase" )
		( "EVENT_PHASE_CAPTURE", int(Rml::EventPhase::Capture) )
		( "EVENT_PHASE_TARGET",  int(Rml::EventPhase::Target) )
		( "EVENT_PHASE_BUBBLE", int(Rml::EventPhase::Bubble) )
	;

	ASBind::Enum( as->getEngine(), "eInputKey" )
		( "KI_ESCAPE", Rml::Input::KI_ESCAPE )
		( "KI_0", Rml::Input::KI_0 )
		( "KI_1", Rml::Input::KI_1 )
		( "KI_2", Rml::Input::KI_2 )
		( "KI_3", Rml::Input::KI_3 )
		( "KI_4", Rml::Input::KI_4 )
		( "KI_5", Rml::Input::KI_5 )
		( "KI_6", Rml::Input::KI_6 )
		( "KI_7", Rml::Input::KI_7 )
		( "KI_8", Rml::Input::KI_8 )
		( "KI_9", Rml::Input::KI_9 )
	;


	// reference (without factory)
	ASBind::GetClass<Rml::Event>( as->getEngine() )

	.method( &Event_GetType, "getType", true )
	.method( &Event_GetTargetElement, "getTarget", true )
	.method( &Event_GetParameterS, "getParameter", true )
	.method( &Event_GetParameterI, "getParameter", true )
	.method( &Event_GetParameterU, "getParameter", true )
	.method( &Event_GetParameterF, "getParameter", true )
	.method( &Event_GetParameterB, "getParameter", true )
	.method( &Event_GetParameters, "getParameters", true )
	.method( &Event_GetPhase, "getPhase", true )
	.method( &Event_StopPropagation, "stopPropagation", true )
	;
}

//==============================================================

//
// EVENT LISTENER

// EVENT LISTENER IS DANGEROUS, USES DUMMY REFERENCING!
void PrebindEventListener( ASInterface *as ) {
	ASBind::Class<Rml::EventListener, ASBind::class_nocount>( as->getEngine() )
	;
}

//==============================================================

//
// ELEMENT

// TODO: investigate if "self" here needs some reference counting tricks?

// ch : note that the ordering in these wrapping functions went like this:
// 	1) we need to wrap a few functions to handle reference-counting
// 	2) we need few wrapper functions to look-a-like jquery
//	3) we need to provide separate api for Form, Controls etc..
//	4) we need to convert all Rml::String to asstring_t*
// and thats why you have loads of misc functions in the end that use strings

// dummy funcdef
static void Element_EventListenerCallback( Rml::Element *elem, Rml::Event *event ) {
}


static Rml::Element *Element_Factory( void ) {
	Rml::ElementPtr eptr = Rml::Factory::InstanceElement( NULL, "*", "#element", Rml::XMLAttributes() );
	if( eptr == nullptr ) {
		return nullptr;
	}
	return eptr.get();
}

static Rml::Element *Element_Factory2( Rml::Element *parent ) {
	Rml::ElementPtr eptr = Rml::Factory::InstanceElement( parent, "*", "#element", Rml::XMLAttributes() );
	if( eptr == nullptr ) {
		return nullptr;
	}
	return eptr.get();
}

static Rml::Element *Element_FactoryRML( Rml::Element *parent, const asstring_t &rml ) {
	Rml::ElementPtr eptr = Rml::Factory::InstanceElement( parent, "*", "#element", Rml::XMLAttributes() );
	if( eptr == nullptr ) {
		return nullptr;
	}

	Rml::Element *e = eptr.get();
	e->SetInnerRML( ASSTR( rml ) );
	return e;
}

static Rml::EventListener *Element_AddEventListener( Rml::Element *elem, const asstring_t &event, asIScriptFunction *func ) {
	Rml::EventListener *listener = CreateScriptEventCaller( UI_Main::Get()->getAS(), func );
	elem->AddEventListener( ASSTR( event ), listener );
	if( func ) {
		func->Release();
	}
	return listener;
}

static void Element_RemoveEventListener( Rml::Element *elem, const asstring_t &event, Rml::EventListener *listener ) {
	elem->RemoveEventListener( ASSTR( event ), listener );
}

// CSS
static Rml::Element *Element_AddClass( Rml::Element *self, const asstring_t &c ) {
	self->SetClass( ASSTR( c ), true );
	return self;
}

static Rml::Element *Element_RemoveClass( Rml::Element *self, const asstring_t &c ) {
	self->SetClass( ASSTR( c ), false );
	return self;
}

static Rml::Element *Element_ToggleClass( Rml::Element *self, const asstring_t &c ) {
	Rml::String sc( ASSTR( c ) );
	bool set = self->IsClassSet( sc );
	self->SetClass( sc, !set );
	return self;
}

static Rml::Element *Element_SetCSS( Rml::Element *self, const asstring_t &prop, const asstring_t &value ) {
	if( !value.len ) {
		self->RemoveProperty( ASSTR( prop ) );
	} else {
		self->SetProperty( ASSTR( prop ), ASSTR( value ) );
	}
	return self;
}

static asstring_t *Element_GetCSS( Rml::Element *self, const asstring_t &name ) {
	const Rml::Property* prop = self->GetProperty( ASSTR( name ) );
	return ASSTR( prop ? prop->ToString() : "" );
}

// NODES
static Rml::Element *Element_GetParentNode( Rml::Element *self ) {
	Rml::Element *e = self->GetParentNode();
	return e;
}

static Rml::Element *Element_GetNextSibling( Rml::Element *self ) {
	Rml::Element *e = self->GetNextSibling();
	return e;
}

static Rml::Element *Element_GetPreviousSibling( Rml::Element *self ) {
	Rml::Element *e = self->GetPreviousSibling();
	return e;
}

static Rml::Element *Element_GetFirstChild( Rml::Element *self ) {
	Rml::Element *e = self->GetFirstChild();
	return e;
}

static Rml::Element *Element_GetLastChild( Rml::Element *self ) {
	Rml::Element *e = self->GetLastChild();
	return e;
}

static Rml::Element *Element_GetChild( Rml::Element *self, unsigned int index ) {
	Rml::Element *e = self->GetChild( index );
	return e;
}

// CONTENTS

static asstring_t *Element_GetInnerRML( Rml::Element *elem ) {
	Rml::String srml;
	elem->GetInnerRML( srml );
	return ASSTR( srml );
}

static void Element_SetInnerRML( Rml::Element *elem, const asstring_t &rml ) {
	elem->SetInnerRML( ASSTR( rml ) );
}

// TODO: wrap all other functions like this
static Rml::Element *Element_GetElementById( Rml::Element *elem, const asstring_t &id ) {
	Rml::Element *r = elem->GetElementById( ASSTR( id ) );
	return r;
}

static ASElementsArray *Element_GetElementsByTagName( Rml::Element *elem, const asstring_t &tag ) {
	Rml::ElementList elements;

	elem->GetElementsByTagName( elements, ASSTR( tag ) );

	CScriptArrayInterface *arr = UI_Main::Get()->getAS()->createArray( elements.size(), elementsArrayType );
	if( !arr ) {
		return NULL;
	}

	unsigned int n = 0;
	for( Rml::ElementList::iterator it = elements.begin(); it != elements.end(); ++it ) {
		Rml::Element *child = *it;
		*( (Rml::Element **)arr->At( n++ ) ) = child;
	}

	return static_cast<ASElementsArray *>( arr );
}

static ASElementsArray *Element_GetElementsByClassName( Rml::Element *elem, const asstring_t &tag ) {
	Rml::ElementList elements;

	elem->GetElementsByClassName( elements, ASSTR( tag ) );

	CScriptArrayInterface *arr = UI_Main::Get()->getAS()->createArray( elements.size(), elementsArrayType );
	if( !arr ) {
		return NULL;
	}

	unsigned int n = 0;
	for( Rml::ElementList::iterator it = elements.begin(); it != elements.end(); ++it ) {
		Rml::Element *child = *it;
		*( (Rml::Element **)arr->At( n++ ) ) = child;
	}

	return static_cast<ASElementsArray *>( arr );
}

static Rml::ElementDocument *Element_GetOwnerDocument( Rml::Element *elem ) {
	Rml::ElementDocument *d = elem->GetOwnerDocument();
	return d;
}

//
//
// NOW THE TEDIOUS PART OF WRAPPING REST OF THE FUNCTIONS USING Rml::String to use asstring_t* ...

static bool Element_SetProperty( Rml::Element *elem, const asstring_t &a, const asstring_t &b ) {
	return elem->SetProperty( ASSTR( a ), ASSTR( b ) );
}

static asstring_t *Element_GetProperty( Rml::Element *elem, const asstring_t &a ) {
	return ASSTR( elem->GetProperty<Rml::String>( ASSTR( a ) ) );
}

static void Element_RemoveProperty( Rml::Element *elem, const asstring_t &a ) {
	elem->RemoveProperty( ASSTR( a ) );
}

static void Element_SetClass( Rml::Element *elem, const asstring_t &a, bool b ) {
	elem->SetClass( ASSTR( a ), b );
}

static bool Element_IsClassSet( Rml::Element *elem, const asstring_t &a ) {
	return elem->IsClassSet( ASSTR( a ) );
}

static void Element_SetClassNames( Rml::Element *elem, const asstring_t &a ) {
	elem->SetClassNames( ASSTR( a ) );
}

static asstring_t *Element_GetClassNames( Rml::Element *elem ) {
	return ASSTR( elem->GetClassNames() );
}

static void Element_SetPseudoClass( Rml::Element *elem, const asstring_t &a, bool b ) {
	elem->SetPseudoClass( ASSTR( a ), b );
}

static bool Element_IsPseudoClassSet( Rml::Element *elem, const asstring_t &a ) {
	return elem->IsPseudoClassSet( ASSTR( a ) );
}

static Rml::Element *Element_SetAttributeS( Rml::Element *elem, const asstring_t &a, const asstring_t &b ) {
	elem->SetAttribute( ASSTR( a ), ASSTR( b ) );
	return elem;
}

static Rml::Element *Element_SetAttributeI( Rml::Element *elem, const asstring_t &a, const int b ) {
	elem->SetAttribute( ASSTR( a ), b );
	return elem;
}

static Rml::Element *Element_SetAttributeF( Rml::Element *elem, const asstring_t &a, const float b ) {
	elem->SetAttribute( ASSTR( a ), b );
	return elem;
}

static asstring_t *Element_GetAttributeS( Rml::Element *elem, const asstring_t &a, const asstring_t &b ) {
	return ASSTR( elem->GetAttribute<Rml::String>( ASSTR( a ), ASSTR( b ) ) );
}

static int Element_GetAttributeI( Rml::Element *elem, const asstring_t &a, const int b ) {
	return elem->GetAttribute<int>( ASSTR( a ), b );
}

static unsigned Element_GetAttributeU( Rml::Element *elem, const asstring_t &a, const unsigned b ) {
	return elem->GetAttribute<unsigned>( ASSTR( a ), b );
}

static float Element_GetAttributeF( Rml::Element *elem, const asstring_t &a, const float b ) {
	return elem->GetAttribute<float>( ASSTR( a ), b );
}

static bool Element_HasAttribute( Rml::Element *elem, const asstring_t &a ) {
	return elem->HasAttribute( ASSTR( a ) );
}

static void Element_RemoveAttribute( Rml::Element *elem, const asstring_t &a ) {
	elem->RemoveAttribute( ASSTR( a ) );
}

static asstring_t *Element_GetTagName( Rml::Element *elem ) {
	return ASSTR( elem->GetTagName() );
}

static asstring_t *Element_GetId( Rml::Element *elem ) {
	return ASSTR( elem->GetId() );
}

static void Element_SetId( Rml::Element *elem, const asstring_t &a ) {
	elem->SetId( ASSTR( a ) );
}

static float Element_GetContainingBlockWidth( Rml::Element *self ) {
	return self->GetContainingBlock().x;
}

static float Element_GetContainingBlockHeight( Rml::Element *self ) {
	return self->GetContainingBlock().y;
}

static float Element_ResolveNumericProperty( Rml::Element *self, const asstring_t &p ) {
	const Rml::Property* property = self->GetLocalProperty(Rml::String(ASSTR( p )));
	return self->ResolveLength(property->GetNumericValue());
}

//==============================================================

//
// FORM

static Rml::ElementForm *Element_CastToElementForm( Rml::Element *self ) {
	Rml::ElementForm *f = dynamic_cast<Rml::ElementForm *>( self );
	return f;
}

static Rml::Element *ElementForm_CastToElement( Rml::ElementForm *self ) {
	Rml::Element *e = dynamic_cast<Rml::Element *>( self );
	return e;
}

void ElementForm_Submit( Rml::ElementForm *self ) {
	self->Submit();
}

static void PreBindElementForm( ASInterface *as ) {
	ASBind::Class<Rml::ElementForm, ASBind::class_nocount>( as->getEngine() );
}

static void BindElementForm( ASInterface *as ) {
	asIScriptEngine *engine = as->getEngine();

	ASBind::GetClass<Rml::ElementForm>( engine )

	.method( &ElementForm_Submit, "submit", true )
	.refcast( &ElementForm_CastToElement, true, true )
	;

	// Cast behavior for the Element class
	ASBind::GetClass<Rml::Element>( engine )
	.refcast( &Element_CastToElementForm, true, true )
	;
}

//==============================================================

//
// TABSET

static Rml::ElementTabSet *Element_CastToElementTabSet( Rml::Element *self ) {
	Rml::ElementTabSet *f = dynamic_cast<Rml::ElementTabSet *>( self );
	return f;
}

static Rml::Element *ElementTabSet_CastToElement( Rml::ElementTabSet *self ) {
	Rml::Element *e = dynamic_cast<Rml::Element *>( self );
	return e;
}
/// Sets the specifed tab index's tab title RML.
static void ElementTabSet_SetTab( Rml::ElementTabSet *self, int tabIndex, const asstring_t & rml ) {
	self->SetTab( tabIndex, ASSTR( rml ) );
}

/// Sets the specifed tab index's tab panel RML.
static void ElementTabSet_SetPanel( Rml::ElementTabSet *self, int tabIndex, const asstring_t & rml ) {
	self->SetPanel( tabIndex, ASSTR( rml ) );
}

/// Set the specifed tab index's title element.
//static void ElementTabSet_SetTab( Rml::ElementTabSet *self, int tabIndex, Rml::Element *e ) {
//	self->SetTab( tabIndex, e );
//}

/// Set the specified tab index's body element.
//static void ElementTabSet_SetPanel( Rml::ElementTabSet *self, int tabIndex, Rml::Element *e ) {
//	assert(false);
//	//Rml::ElementPtr ele = Rml::ElementPtr(e);
//	//self->SetPanel( tabIndex, ele );
//}

/// Remove one of the tab set's panels and its corresponding tab.
static void ElementTabSet_RemoveTab( Rml::ElementTabSet *self, int tabIndex ) {
	self->RemoveTab( tabIndex );
}

/// Retrieve the number of tabs in the tabset.
static int ElementTabSet_GetNumTabs( Rml::ElementTabSet *self ) {
	return self->GetNumTabs();
}

/// Sets the currently active (visible) tab index.
static void ElementTabSet_SetActiveTab( Rml::ElementTabSet *self, int tabIndex ) {
	self->SetActiveTab( tabIndex );
}

/// Get the current active tab index.
static int ElementTabSet_GetActiveTab( Rml::ElementTabSet *self ) {
	return self->GetActiveTab();
}

static void PreBindElementTabSet( ASInterface *as ) {
	ASBind::Class<Rml::ElementTabSet, ASBind::class_nocount>( as->getEngine() );
}

static void BindElementTabSet( ASInterface *as ) {
	asIScriptEngine *engine = as->getEngine();

	ASBind::GetClass<Rml::ElementTabSet>( engine )


	.method( &ElementTabSet_RemoveTab, "removeTab", true )
	.constmethod( &ElementTabSet_GetNumTabs, "getNumTabs", true )
	.method( &ElementTabSet_SetActiveTab, "setActiveTab", true )
	.constmethod( &ElementTabSet_GetActiveTab, "getActiveTab", true )

	.refcast( &ElementTabSet_CastToElement, true, true )
	;

	// Cast behavior for the Element class
	ASBind::GetClass<Rml::Element>( engine )
	.refcast( &Element_CastToElementTabSet, true, true )
	;
}

//==============================================================

//
// DOCUMENT

static Rml::ElementDocument *Element_CastToElementDocument( Rml::Element *self ) {
	Rml::ElementDocument *d = dynamic_cast<Rml::ElementDocument *>( self );
	return d;
}

static Rml::Element *ElementDocument_CastToElement( Rml::ElementDocument *self ) {
	Rml::Element *e = dynamic_cast<Rml::Element *>( self );
	return e;
}

/// Returns URL of the current document.
static ASURL ElementDocument_GetURL( Rml::ElementDocument *self ) {
	return ASURL( self->GetSourceURL().c_str() );
}

/// Returns title of the current document.
static asstring_t *ElementDocument_GetTitle( Rml::ElementDocument *self ) {
	return ASSTR( self->GetTitle() );
}

/// Returns the BODY node of the current document.
static Rml::Element *ElementDocument_GetBody( Rml::ElementDocument *self ) {
	Rml::Element *e = dynamic_cast<Rml::Element *>( self );
	return e;
}

static void PreBindElementDocument( ASInterface *as ) {
	ASBind::Class<Rml::ElementDocument, ASBind::class_nocount>( as->getEngine() );
}

static void BindElementDocument( ASInterface *as ) {
	asIScriptEngine *engine = as->getEngine();

	ASBind::GetClass<Rml::ElementDocument>( engine )

	.constmethod( ElementDocument_GetURL, "get_URL", true )
	.constmethod( ElementDocument_GetTitle, "get_title", true )
	.constmethod( ElementDocument_GetBody, "get_body", true )

	.refcast( &ElementDocument_CastToElement, true, true )
	;

	// Cast behavior for the Element class
	ASBind::GetClass<Rml::Element>( engine )
	.refcast( &Element_CastToElementDocument, true, true )
	;
}

//==============================================================

//
// FORM CONTROLS

static Rml::ElementFormControl *Element_CastToElementFormControl( Rml::Element *self ) {
	Rml::ElementFormControl *f = dynamic_cast<Rml::ElementFormControl *>( self );
	return f;
}

static Rml::Element *ElementFormControl_CastToElement( Rml::ElementFormControl *self ) {
	Rml::Element *e = dynamic_cast<Rml::Element *>( self );
	return e;
}

static asstring_t *ElementFormControl_GetName( Rml::ElementFormControl *self ) {
	return ASSTR( self->GetName() );
}

static void ElementFormControl_SetName( Rml::ElementFormControl *self, const asstring_t &name ) {
	self->SetName( ASSTR( name ) );
}

static asstring_t *ElementFormControl_GetValue( Rml::ElementFormControl *self ) {
	return ASSTR( self->GetValue() );
}

static void ElementFormControl_SetValue( Rml::ElementFormControl *self, const asstring_t &value ) {
	self->SetValue( ASSTR( value ) );
}

static bool ElementFormControl_IsSubmitted( Rml::ElementFormControl *self ) {
	return self->IsSubmitted();
}

static bool ElementFormControl_IsDisabled( Rml::ElementFormControl *self ) {
	return self->IsDisabled();
}

static void ElementFormControl_SetDisabled( Rml::ElementFormControl *self, bool disable ) {
	self->SetDisabled( disable );
}

static void PreBindElementFormControl( ASInterface *as ) {
	ASBind::Class<Rml::ElementFormControl, ASBind::class_nocount>( as->getEngine() );
}

static void BindElementFormControl( ASInterface *as ) {
	asIScriptEngine *engine = as->getEngine();

	ASBind::GetClass<Rml::ElementFormControl>( engine )

	.constmethod( ElementFormControl_GetName, "get_name", true )
	.method( ElementFormControl_SetName, "set_name", true )
	.constmethod( ElementFormControl_GetValue, "get_value", true )
	.method( ElementFormControl_SetValue, "set_value", true )
	.constmethod( ElementFormControl_IsSubmitted, "get_submitted", true )
	.constmethod( ElementFormControl_IsDisabled, "get_disabled", true )
	.method( ElementFormControl_SetDisabled, "set_disabled", true )

	.refcast( &ElementFormControl_CastToElement, true, true )
	;

	// Cast behavior for the Element class
	ASBind::GetClass<Rml::Element>( engine )
	.refcast( &Element_CastToElementFormControl, true, true )
	;
}


//
// IMAGE

static WSWUI::ElementImage *Element_CastToElementImage( Rml::Element *self ) {
	WSWUI::ElementImage *f = dynamic_cast<WSWUI::ElementImage *>( self );
	return f;
}

static Rml::Element *ElementImage_CastToElement( WSWUI::ElementImage *self ) {
	Rml::Element *e = dynamic_cast<Rml::Element *>( self );
	return e;
}

static float ElementImage_GetWidth( WSWUI::ElementImage *self ) {
	Rml::Vector2f dimensions;
	self->GetIntrinsicDimensions( dimensions );
	return dimensions.x;
}

static float ElementImage_GetHeight( WSWUI::ElementImage *self ) {
	Rml::Vector2f dimensions;
	self->GetIntrinsicDimensions( dimensions );
	return dimensions.y;
}

static void PreBindElementImage( ASInterface *as ) {
	ASBind::Class<WSWUI::ElementImage, ASBind::class_nocount>( as->getEngine() );
}

static void BindElementImage( ASInterface *as ) {
	asIScriptEngine *engine = as->getEngine();

	ASBind::GetClass<WSWUI::ElementImage>( engine )

	.method( ElementImage_GetWidth, "get_width", true )
	.method( ElementImage_GetHeight, "get_height", true )

	.refcast( &ElementImage_CastToElement, true, true )
	;

	// Cast behavior for the Element class
	ASBind::GetClass<Rml::Element>( engine )
	.refcast( &Element_CastToElementImage, true, true )
	;
}

//==============================================================

//
//
// Bind

void PrebindElement( ASInterface *as ) {
	ASBind::Class<Rml::Element, ASBind::class_nocount>( as->getEngine() );

	PreBindElementDocument( as );

	PreBindElementForm( as );

	PreBindElementFormControl( as );

	PreBindElementTabSet( as );

	PreBindElementImage( as );
}

void BindElement( ASInterface *as ) {
	asIScriptEngine *engine = as->getEngine();

	ASBind::Global( as->getEngine() )

	// setTimeout and setInterval callback funcdefs
	.funcdef( &Element_EventListenerCallback, "DOMEventListenerCallback" )
	;

	// Elements are bound as reference types
	ASBind::GetClass<Rml::Element>( engine )
	.factory( &Element_Factory )
	.factory( &Element_Factory2 )
	.factory( &Element_FactoryRML )
	
	// css/style
	.method( &Element_SetProperty, "setProp", true )
	.method( &Element_GetProperty, "getProp", true )
	.method( &Element_RemoveProperty, "removeProp", true )

	// jquery-like
	.method( &Element_SetCSS, "css", true )         // css('prop', '') removes the property
	.method( &Element_GetCSS, "css", true )

	// classes TODO: make addClass, removeClass etc.. like in jQuery
	.method( &Element_SetClass, "setClass", true )
	.method( &Element_IsClassSet, "hasClass", true )
	.method( &Element_SetClassNames, "setClasses", true )
	.method( &Element_GetClassNames, "getClasses", true )
	.method( &Element_AddClass, "addClass", true )
	.method( &Element_RemoveClass, "removeClass", true )
	.method( &Element_ToggleClass, "toggleClass", true )
	.method( &Element_SetClass, "toggleClass", true )           // note alias to setClass
	// pseudo-classes
	.method( &Element_SetPseudoClass, "togglePseudo", true )
	.method( &Element_IsPseudoClassSet, "hasPseudo", true )

	// html attributes
	.method( &Element_SetAttributeS, "setAttr", true )
	.method( &Element_SetAttributeI, "setAttr", true )
	.method( &Element_SetAttributeF, "setAttr", true )
	.method( &Element_GetAttributeS, "getAttr", true )
	.method( &Element_GetAttributeI, "getAttr", true )
	.method( &Element_GetAttributeU, "getAttr", true )
	.method( &Element_GetAttributeF, "getAttr", true )
	.method( &Element_HasAttribute, "hasAttr", true )
	.method( &Element_RemoveAttribute, "removeAttr", true )
	.method( &Rml::Element::GetNumAttributes, "numAttr" )

	// dom
	.constmethod( &Element_GetTagName, "get_tagName", true )
	.constmethod( &Element_GetId, "get_id", true )
	.method( &Element_SetId, "set_id", true )

	.method( &Element_GetParentNode, "getParent", true )
	.method( &Element_GetNextSibling, "getNextSibling", true )
	.method( &Element_GetPreviousSibling, "getPrevSibling", true )
	.method( &Element_GetFirstChild, "firstChild", true )
	.method( &Element_GetLastChild, "lastChild", true )
	.method2( &Rml::Element::GetNumChildren, "uint getNumChildren( bool includeNonDomElements = false )" )
	.method( &Element_GetChild, "getChild", true )
	.constmethod( &Element_GetInnerRML, "getInnerRML", true )
	.method( &Element_SetInnerRML, "setInnerRML", true )

	.method( &Rml::Element::Focus, "focus" )
	.method( &Rml::Element::Blur, "unfocus" )
	.method( &Rml::Element::Click, "click" )
	.method( &Rml::Element::HasChildNodes, "hasChildren" )

	//.method( &Rml::Element::GetElementById, "getElementById", true ) //TODO:: missing
	//.method( &Rml::Element::GetElementsByTagName, "getElementsByTagName", true ) // TODO: missing 
	// .method( &Rml::Element::GetElementsByClassName, "getElementsByClassName", true ) //TODO: missing
	//.method( &Rml::Element::GetOwnerDocument, "get_ownerDocument", true ) // TODO: missing 

	.method2(Element_AddEventListener, "void addEventListener( const String &event, DOMEventListenerCallback @callback )", true )
	.method( Element_RemoveEventListener, "removeEventListener", true )

	.method( &Rml::Element::GetClientLeft, "clientLeft" )
	.method( &Rml::Element::GetClientTop, "clientTop" )
	.method( &Rml::Element::GetClientHeight, "clientHeight" )
	.method( &Rml::Element::GetClientWidth, "clientWidth" )

	.method( &Rml::Element::GetOffsetParent, "offsetParent" )
	.method( &Rml::Element::GetOffsetLeft, "offsetLeft" )
	.method( &Rml::Element::GetOffsetTop, "offsetTop" )
	.method( &Rml::Element::GetOffsetHeight, "offsetHeight" )
	.method( &Rml::Element::GetOffsetWidth, "offsetWidth" )

	.method( &Rml::Element::GetScrollLeft, "scrollLeft" )
	.method( &Rml::Element::SetScrollLeft, "scrollLeft" )
	.method( &Rml::Element::GetScrollTop, "scrollTop" )
	.method( &Rml::Element::SetScrollTop, "scrollTop" )
	.method( &Rml::Element::GetScrollHeight, "scrollHeight" )
	.method( &Rml::Element::GetScrollWidth, "scrollWidth" )

	.method( &Rml::Element::GetAbsoluteLeft, "absLeft" )
	.method( &Rml::Element::GetAbsoluteTop, "absTop" )

	.method( &Element_GetContainingBlockWidth, "containingBlockWith", true )
	.method( &Element_GetContainingBlockHeight, "containingBlockHeight", true )

	.method( &Element_ResolveNumericProperty, "resolveNumericProperty", true )
	;

	// cache type id for array<Element @>
	elementsArrayType = engine->GetTypeInfoByDecl(ASBind::typestr<ASElementsArray>());
	// cache type id for array<String @>
	stringsArrayType = engine->GetTypeInfoByDecl(ASBind::typestr<ASStringsArray>());

	// ElementDocument
	BindElementDocument( as );

	// ElementForm
	BindElementForm( as );

	// ElementFormControl
	BindElementFormControl( as );

	// ElementFormControlDataSelect
	//BindElementFormControlDataSelect( as );

	// ElementTabSet
	BindElementTabSet( as );

	// ElementImage
	BindElementImage( as );
}

}
